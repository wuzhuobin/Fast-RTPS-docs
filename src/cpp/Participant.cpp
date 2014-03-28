/*************************************************************************
 * Copyright (c) 2013 eProsima. All rights reserved.
 *
 * This copy of FastCdr is licensed to you under the terms described in the
 * EPROSIMARTPS_LIBRARY_LICENSE file included in this distribution.
 *
 *************************************************************************/

/**
 * Participant.cpp
 *
 *  Created on: Feb 25, 2014
 *      Author: Gonzalo Rodriguez Canosa
 *      email:  gonzalorodriguez@eprosima.com
 *      		grcanosa@gmail.com
 */

#include "eprosimartps/Participant.h"

#include "eprosimartps/writer/StatelessWriter.h"
#include "eprosimartps/reader/StatelessReader.h"
#include "eprosimartps/writer/StatefulWriter.h"
#include "eprosimartps/reader/RTPSReader.h"
#include "eprosimartps/writer/RTPSWriter.h"

#include "eprosimartps/dds/DomainParticipant.h"



namespace eprosima {
namespace rtps {


Participant::Participant(const ParticipantParams_t& PParam):
		m_defaultUnicastLocatorList(PParam.defaultUnicastLocatorList),
		m_defaultMulticastLocatorList(PParam.defaultMulticastLocatorList),
		m_endpointToListenThreadSemaphore(new boost::interprocess::interprocess_semaphore(0)),
		IdCounter(0)
{
	Locator_t loc;
	loc.port = PParam.defaultSendPort;
	m_send_thr.initSend(loc);

	m_event_thr.init_thread();

	// Create Unique GUID
	dds::DomainParticipant *dp;
	dp = dds::DomainParticipant::getInstance();
	uint32_t ID = dp->getNewId();
	int pid;
	#if defined(_WIN32)
		pid = (int)_getpid();
	#else
		pid = (int)getpid();
	#endif
	//cout << "PID: " << pid << " ID:"<< ID << endl;

	m_guid.guidPrefix.value[0] = m_send_thr.m_sendLocator.address[12];
	m_guid.guidPrefix.value[1] = m_send_thr.m_sendLocator.address[13];
	m_guid.guidPrefix.value[2] = m_send_thr.m_sendLocator.address[14];
	m_guid.guidPrefix.value[3] = m_send_thr.m_sendLocator.address[15];
	m_guid.guidPrefix.value[4] = ((octet*)&pid)[0];
	m_guid.guidPrefix.value[5] = ((octet*)&pid)[1];
	m_guid.guidPrefix.value[6] = ((octet*)&pid)[2];
	m_guid.guidPrefix.value[7] = ((octet*)&pid)[3];
	m_guid.guidPrefix.value[8] = ((octet*)&ID)[0];
	m_guid.guidPrefix.value[9] = ((octet*)&ID)[1];
	m_guid.guidPrefix.value[10] = ((octet*)&ID)[2];
	m_guid.guidPrefix.value[11] = ((octet*)&ID)[3];
	m_guid.entityId = ENTITYID_PARTICIPANT;
	std::stringstream ss;
		for(int i =0;i<12;i++)
			ss << (int)m_guid.guidPrefix.value[i] << ".";
		pInfo("Participant created with guidPrefix: " <<ss.str()<< endl);
}



Participant::~Participant()
{

	//Destruct threads:
	for(std::vector<ThreadListen*>::iterator it=m_threadListenList.begin();
			it!=m_threadListenList.end();++it)
		(*it)->~ThreadListen();
	for(std::vector<RTPSReader*>::iterator it=m_readerList.begin();
			it!=m_readerList.end();++it)
		delete(*it);
	for(std::vector<RTPSWriter*>::iterator it=m_writerList.begin();
			it!=m_writerList.end();++it)
		delete(*it);
}

bool Participant::createStatelessWriter(StatelessWriter** SW_out,const WriterParams_t& Wparam,uint32_t payload_size)
{
	pDebugInfo("Creating Stateless Writer"<<endl);
	StatelessWriter* SWriter = new StatelessWriter(&Wparam,payload_size);
	pDebugInfo("Finished Writer creation"<<endl);
	//Check if locator lists are empty:
	if(SWriter->unicastLocatorList.empty())
		SWriter->unicastLocatorList = m_defaultUnicastLocatorList;
	if(SWriter->unicastLocatorList.empty())
		SWriter->multicastLocatorList = m_defaultMulticastLocatorList;
	//Assign participant pointer
	SWriter->participant = this;
	//Assign GUID
	SWriter->guid.guidPrefix = m_guid.guidPrefix;
	SWriter->init_header();

	if(SWriter->topicKind == NO_KEY)
		SWriter->guid.entityId.value[3] = 0x03;
	else if(SWriter->topicKind == WITH_KEY)
		SWriter->guid.entityId.value[3] = 0x02;
	IdCounter++;
	octet* c = (octet*)&IdCounter;
	SWriter->guid.entityId.value[2] = c[0];
	SWriter->guid.entityId.value[1] = c[1];
	SWriter->guid.entityId.value[0] = c[2];
	//Look for receiving threads that are already listening to this writer receiving addresses.
	assignEnpointToListenThreads((Endpoint*)SWriter,'W');
	//Wait until the thread is correctly created

m_writerList.push_back((RTPSWriter*)SWriter);

	*SW_out = SWriter;
	pDebugInfo("Finished Writer initialization"<<endl);
	return true;
}

bool Participant::createStatefulWriter(StatefulWriter** SFW_out,const WriterParams_t& Wparam,uint32_t payload_size) {

	StatefulWriter* SFWriter = new StatefulWriter(&Wparam, payload_size);
	//Check if locator lists are empty:
	if(SFWriter->unicastLocatorList.empty())
		SFWriter->unicastLocatorList = m_defaultUnicastLocatorList;
	if(SFWriter->unicastLocatorList.empty())
		SFWriter->multicastLocatorList = m_defaultMulticastLocatorList;
	//Assign participant pointer
	SFWriter->participant = this;
	//Assign GUID
	SFWriter->guid.guidPrefix = m_guid.guidPrefix;
	SFWriter->init_header();

	if(SFWriter->topicKind == NO_KEY)
		SFWriter->guid.entityId.value[3] = 0x03;
	else if(SFWriter->topicKind == WITH_KEY)
		SFWriter->guid.entityId.value[3] = 0x02;
	IdCounter++;
	octet* c = (octet*)&IdCounter;
	SFWriter->guid.entityId.value[2] = c[0];
	SFWriter->guid.entityId.value[1] = c[1];
	SFWriter->guid.entityId.value[0] = c[2];
	//Look for receiving threads that are already listening to this writer receiving addresses.
	assignEnpointToListenThreads((Endpoint*)SFWriter,'W');
	//Wait until the thread is correctly created

	m_writerList.push_back((RTPSWriter*)SFWriter);

	*SFW_out = SFWriter;
	return true;
}




bool Participant::createStatelessReader(StatelessReader** SR_out,
		const ReaderParams_t& RParam,uint32_t payload_size)
{
	StatelessReader* SReader = new StatelessReader(&RParam, payload_size);
	//If NO UNICAST
	if(SReader->unicastLocatorList.empty())
		SReader->unicastLocatorList = m_defaultUnicastLocatorList;
	//IF NO MULTICAST
	if(SReader->multicastLocatorList.empty())
		SReader->multicastLocatorList = m_defaultMulticastLocatorList;


	//Assign participant pointer
	SReader->participant = this;
	//Assign GUID
	SReader->guid.guidPrefix = m_guid.guidPrefix;
	if(SReader->topicKind == NO_KEY)
		SReader->guid.entityId.value[3] = 0x04;
	else if(SReader->topicKind == WITH_KEY)
		SReader->guid.entityId.value[3] = 0x07;
	IdCounter++;
	octet* c = (octet*)&IdCounter;
	SReader->guid.entityId.value[2] = c[3];
	SReader->guid.entityId.value[1] = c[2];
	SReader->guid.entityId.value[0] = c[1];
	//Look for receiving threads that are already listening to this writer receiving addreesses.

	assignEnpointToListenThreads((Endpoint*)SReader,'R');

	m_readerList.push_back((RTPSReader*)SReader);

	*SR_out = SReader;
	return true;



}

inline void addEndpoint(ThreadListen* th,Endpoint* end,char type)
{
	if(type == 'W')
		th->m_assoc_writers.push_back((RTPSWriter*)end);
	else if(type =='R')
		th->m_assoc_readers.push_back((RTPSReader*)end);
}


bool Participant::assignEnpointToListenThreads(Endpoint* endpoint, char type) {
	if(type !='R' && type!='W')
		throw ERR_PARTICIPANT_INCORRECT_ENDPOINT_TYPE;

	std::vector<ThreadListen*>::iterator thit;
	std::vector<Locator_t>::iterator locit_th;
	std::vector<Locator_t>::iterator locit_e;
	bool assigned = false;

	for(locit_e = endpoint->unicastLocatorList.begin();locit_e!=endpoint->unicastLocatorList.end();++locit_e)
	{
		assigned = false;
		for(thit=m_threadListenList.begin();
				thit!=m_threadListenList.end();++thit)
		{
			for(locit_th = (*thit)->m_locList.begin();
					locit_th != (*thit)->m_locList.end();++locit_th)
			{
				if((*locit_th).port == (*locit_e).port) //Found a match, assign to this thread
				{
					addEndpoint(*thit,endpoint,type);
					assigned = true;
				}
			}
		}
		if(!assigned) //Create new listen thread
		{
			ThreadListen* thListen = NULL;
			addNewListenThread(*locit_e,&thListen); //Add new listen thread to participant
			m_endpointToListenThreadSemaphore->wait();
			addEndpoint(thListen,endpoint,type); //add endpoint to that listen thread
			assigned = true;
		}
	}
	for(locit_e = endpoint->multicastLocatorList.begin();locit_e!=endpoint->multicastLocatorList.end();++locit_e)
	{
		//FIXME: in multicast the IP is important, change this.
		assigned = false;
		for(thit=m_threadListenList.begin();
				thit!=m_threadListenList.end();++thit)
		{
			for(locit_th = (*thit)->m_locList.begin();
					locit_th != (*thit)->m_locList.end();++locit_th)
			{
				if((*locit_th).port == (*locit_e).port) //Found a match, assign to this thread
				{
					addEndpoint((*thit),endpoint,type);
					assigned = true;
				}
			}
		}
		if(!assigned) //Create new listen thread
		{
			ThreadListen* thListen = NULL;
			addNewListenThread(*locit_e,&thListen); //Add new listen thread to participant
			m_endpointToListenThreadSemaphore->wait();
			addEndpoint(thListen,endpoint,type);   //add Endpoint to that listen thread
			assigned = true;
		}
	}
	return true;
}

bool Participant::addNewListenThread(Locator_t& loc,ThreadListen** thlisten_in) {
	*thlisten_in = new ThreadListen();
	(*thlisten_in)->m_locList.push_back(loc);
	(*thlisten_in)->m_participant_ptr = this;
	m_threadListenList.push_back(*thlisten_in);
	(*thlisten_in)->init_thread();

	return true;
}

bool Participant::removeEndpoint(Endpoint* end){
	bool found = false;
	char type = 'W';
	for(std::vector<RTPSWriter*>::iterator wit=m_writerList.begin();
			wit!=m_writerList.end();++wit)
	{
		if((*wit)->guid == end->guid) //Found it
		{
			m_writerList.erase(wit);
			found = true;
			break;
		}
	}
	if(!found)
	{
		for(std::vector<RTPSReader*>::iterator rit=m_readerList.begin()
				;rit!=m_readerList.end();++rit)
		{
			if((*rit)->guid == end->guid) //Found it
			{
				m_readerList.erase(rit);
				found = true;
				type = 'R';
				break;
			}
		}
	}
	if(!found)
		return false;
	//Remove it from threadListenList
	std::vector<ThreadListen*>::iterator thit;
	for(thit=m_threadListenList.begin();
			thit!=m_threadListenList.end();thit++)
	{
		if(type == 'W')
		{
			for(std::vector<RTPSWriter*>::iterator wit = (*thit)->m_assoc_writers.begin();
					wit!=(*thit)->m_assoc_writers.end();++wit)
			{
				if((*wit)->guid == end->guid)
				{
					(*thit)->m_assoc_writers.erase(wit);
					if((*thit)->m_assoc_writers.empty() && (*thit)->m_assoc_readers.empty())
						m_threadListenList.erase(thit);

				}
			}
		}
		else if(type == 'R')
		{
			for(std::vector<RTPSReader*>::iterator rit = (*thit)->m_assoc_readers.begin();rit!=(*thit)->m_assoc_readers.end();++rit)
			{
				if((*rit)->guid == end->guid)
				{
					(*thit)->m_assoc_readers.erase(rit);
					if((*thit)->m_assoc_readers.empty() && (*thit)->m_assoc_writers.empty())
						m_threadListenList.erase(thit);

				}
			}
		}
	}
	return true;
}


} /* namespace rtps */
} /* namespace eprosima */


