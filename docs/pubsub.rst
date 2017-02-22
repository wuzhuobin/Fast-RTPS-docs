Publisher-Subscriber Layer
==========================

*eprosima Fast RTPS* provides a high level Publisher-Subscriber Layer, which is a simple to use abstraction over the RTPS protocol.
By using this layer, you can code a straight-to-the-point application while letting the library take care of the lower level configuration.

How to use the Publisher-Subscriber Layer
-----------------------------------------

We are going to use the example built in the previous section to explain how this layer works.

The first step to create a :class:`Participant` instance, which will act as a container for the Publishers and
Subscribers our application needs. For this we use :class:`Domain`, a static class that manages RTPS entities.
We also need to pass a configuration structure for the Participant, which can be left in its default configuration for now:

.. code-block:: c++

   ParticipantAttributes Pparam; //Configuration structure
   Participant *mp_participant = Domain::createParticipant(Pparam);

The default configuration provides a basic working set of options with predefined ports for communications.
During this tutorial you will learn to tune *eProsima Fast RTPS*.

In order to use our topic, we have to register it within the :class:`Participant` using the code generated with *fastrtpsgen*. Once again, this is done by using the :class:`Domain` class:

.. code-block:: c++

   HelloWorldPubSubType m_type; //Auto-generated type from FastRTPSGen
   Domain::registerType(mp_participant, &m_type);

Once set up, we instantiate a :class:`Publisher` within our :class:`Participant`:

.. code-block:: c++

   PublisherAttributes Wparam; //Configuration structure
   PubListener m_listener; //Class that implements callbacks from the publisher
   Publisher *mp_publisher = Domain::createPublisher(mp_participant, Wparam, (PublisherListener *)&m_listener);

Once the :class:`Publisher` is functional, posting data is a simple process:

.. code-block:: c++

   HelloWorld m_Hello; //Auto-generated container class for topic data from FastRTPSGen
   m_Hello.msg("Hello there!"); // Add contents to the message
   mp_publisher->write((void *)&m_Hello); //Publish
	
The :class:`Publisher` has a set of optional callback functions that are triggered when events happen. An example is when a :class:`Subscriber` starts listening to our topic.

To implement these callbacks we create the class :class:`PubListener`, which inherits from the base class :class:`PublisherListener`.
We pass an instance to this class during the creation of the :class:`Publisher`.

.. code-block:: c++

    class PubListener : public PublisherListener
    {
        public PubListener(){};
        ~PubListener(){};
        void onPublicationmatched(Publisher* pub, MatchingInfo& info)
        {
            //Callback implementation. This is called each time the Publisher finds a Subscriber on the network that listens to the same topic.
        }
    } m_listener;

The :class:`Subscriber` creation and implementation is symmetric.

.. code-block:: c++

    SubscriberAttributes Rparam; //Configuration structure
    SubListener m_listener; //Class that implements callbacks from the Subscriber
    Subscriber *mp_subscriber = Domain::createSubscriber(mp_participant,Rparam,(SubsciberListener*)&m_listener);

Incoming messages are processed within the callback that is called when a new message is received:

.. code-block c++

    class SubListener: public SubscriberListener
    {
    public:
        SubListener(){};
        ~SubListener(){};
        HelloWorld m_Hello; //Storage for incoming messages
        SampleInfo_t m_info; //Auxiliary structure with meta-data on the message
        void onNewDataMessage(Subscriber * sub)
        {
            if(sub->takeNextData((void*)&m_Hello, &m_info))
                if(m_info.sampleKind == ALIVE)
                    std::cout << "New message: " << m_Hello.msg() << std::endl;
        }
    }

Participant Configuration
-------------------------

The :class:`Participant` is configured via the :class:`ParticipantAttributes` structure. We will now go over the most common configuration options.

Setting the name and Domain of the Participant
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The name of the :class:`Participant`, which forms part of the meta-data of the RTPS protocol, can be changed from within the :class:`ParticipantAttributes`:

.. code-block:: c++

   Pparam.setName("my_participant");

Publishers and Subscribers can only talk to each other if their Participants belong to the same DomainId. To set the Domain:

.. code-block:: c++

    Pparam.rtps.builtin.domainId = 80;

Defining Input and Output channels
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In the RTPS Standard, input-output channels (sockets) are defined as Locators. Locators in *eprosima Fast RTPS* are enclosed as type :class:`Locator_t`, which has the following fields:

* Kind: Defines the protocol. *eProsima Fast RTPS* currently supports UDPv4 or UDPv6
* Port: Port as an UDP/IP port.
* Address: Maps to IP address.

You can specify a default list of channels for the Publishers and Subscribers by passing lists of Locators to the configuration structure

.. code-block:: c++

    Locator_t loc1.set_IP4_address(127,0,0,1);
    loc1.port = 22222;
    Pparam.rtps.defaultUnicastLocatorList.push_back(loc1); //Input for Unicast messages
    Locator_t loc2.set_IP4_address(127,0,0,1);
    loc2.port = 33333;
    Pparam.rtps.defaultMulticastLocatorList.push_back(loc2); //Input for Multicast messages
    Locator_t loc3.set_IP4_address(127,0,0,1);
    loc3.port = 44444;
    Pparam.rtps.defaultOutLocatorList.push_back(loc3); //Output Channels

If no Locators are specified by the user *eprosima Fast RTPS* will calculate a minimalistic set to ensure correct behaviour. 

Publisher and Subscriber Configuration
--------------------------------------

Publishers and Subscribers inherit traits from the configuration of their Participant. You can override these traits by
providing different values at their configuration structures. For example, you can specify a different set of
Locators for a Publisher to use:

.. code-block:: c++

    Locator_t my_locator;
    //Initialize the Locator
    SubAttr.unicastLocatorList.push_back(my_locator);

Setting the name and data type of the topic
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The topic name and data type are used as meta-data to determine whether Publishers and Subscribers can exchange messages.

.. code-block:: c++

    // Topic name and type for Publisher
    Wparam.topic.topicDataType = "HelloWorldType"	
    Wparam.topic.topicName = "HelloWorldTopic"
    // Topic name and type for Subscriber
    Rparam.topic.topicName = "HelloWorldTopic"
    Rparam.topic.topicDataType = "HelloWorldType"

Reliability Kind
^^^^^^^^^^^^^^^^

The RTPS standard defines two behaviour modes for message delivery:

* Best-Effort (default): Messages are sent without arrival confirmation from the receiver (subscriber). It is fast, but messages can be lost.
* Reliable: The sender agent (publisher) expects arrival confirmation from the receiver (subscriber). It is slower, but prevents data loss.

You can specify which mode you want to use in the publisher or subscriber parameters:

.. code-block:: c++

    Wparam.qos.m_reliability.kind = RELIABLE_RELIABILITY_QOS; //Set the publisher to Realiable Mode
    Rparam.qos.m_reliability.kind = BEST_EFFORT_RELIABILITY_QOS; //Set the subscriber to Best Effort

Keep in mind that different reliability mode configurations will make a Publisher and a Subscriber incompatible and unable to talk to each other. Read more about this in the [Built-In Prococols](#built-in-procotols) section.

Configuring sample storage
^^^^^^^^^^^^^^^^^^^^^^^^^^

*eProsima Fast RTPS* provides two policies for sample storage: 

* Keep-All (Default): Store all samples in memory.
* Keep-Last: Store samples up to a maximum *depth*. When this limit is reached, they start to become overwritten. 

The mode is changed the following way:

.. code-block:: c++

    Wparam.topic.historyQos.kind = KEEP_LAST;

The depth of the stored samples in keep-last mode is changes in the following way:

.. code-block:: c++
    
    Wparam.topic.historyQos.depth = 5; //changes the maximum number of stored samples

The durability configuration of the endpoint defines how it behaves regarding samples that existed on the topic before a Subscriber joins

* Volatile: Past samples are ignored, a joining Subscriber receives samples generated after the moment it matches.
* Transient Local (Default): When a new Subscriber joins, its History is filled with past samples.

To set the durability mode:

.. code-block:: c++
	
	Rparam.qos.m_durability.kind = VOLATILE_DURABILITY_QOS; 
	
It is also possible to control the maximum size of the History:

.. code-blocks:: c++

	Rparam.topic.resourceLimitsQos.max_samples = 200; // Sets the History to hold a maximum of 200 samples

Additional Concepts
-------------------

Built-In Protocols
^^^^^^^^^^^^^^^^^^

Before a Publisher and a Subscriber can exchange messages, they must be matched. The matching process is performed by the built-in protocols.

The RTPS Standard defines two built-in protocols that are used by Endpoints to gain information about other elements in the network

* Participant Discovery Protocol (PDP): Used by the Participant to gain knowledge of other Participants in the network.
* Endpoint Discovery Protocol (EDP): Used to gain knowledge of the endpoints (Publishers and Subscribers) a remote Participant has.

When a local and a remote endpoint have the same topic name, data type and have compatible configuration they are matched and data posted by Publisher is delivered to the Subscriber.
When a Publisher posts data it is sent to all of its matching Subscribers.  This includes the exchange arrival confirmation messages from both parties in the case of Reliable Mode.

As an user, you can actually interact with the way the Built-in protocols behave. To learn more, go to the [Advanced Topics](#Advanced Topics) section.

Using message meta-data
^^^^^^^^^^^^^^^^^^^^^^^

When a message is taken from the Subscriber, an auxiliary :class:`SampleInfo_t` structure instance is also returned.

.. code-block:: c++

    HelloWorld m_Hello;
    SampleInfo_t m_info;
    sub->takeNextData((void*)&m_Hello, &m_info);

This :class:`SampleInfo_t` structure contains meta-data on the incoming message:

* sampleKind: type of the sample, as defined by the RTPS Standard. Healthy messages from a topic are always ALIVE.
* WriterGUID: Signature of the sender (Publisher) the message comes from.
* OwnershipStrength: When several senders are writing the same data, this field can be used to determine which data is more reliable.
* SourceTimestamp: A timestamp on the sender side that indicates the moment the sample was encapsulated and sent.

This meta-data can be used to implement filters:

.. code-block:: c++

    if((m_info->sampleKind == ALIVE)& (m_info->OwnershipStrength > 25 ){
        //Process data
   }

Defining callbacks
^^^^^^^^^^^^^^^^^^

As we saw in the example, both the :class:`Publisher` and :class:`Subscriber` have a set of callbacks you can use
in your application. These callbacks are to be implemented within classes that derive from
:class:`SubscriberListener` or :class:`PublisherListener`. The following table gathers information about
the possible callbacks that can be implemented in both cases:

        +-----------------------+-----------+------------+
        |      Callback         | Publisher | Subscriber |
        +=======================+===========+============+
        |   onNewDataMessage    |     N     |      Y     |
        +-----------------------+-----------+------------+
        | onSubscriptionMatched |     N     |      Y     |
        +-----------------------+-----------+------------+
        | onPublicationMatched  |     Y     |      N     |
        +-----------------------+-----------+------------+
