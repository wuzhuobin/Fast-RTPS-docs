// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*! 
 * @file TestTypeObject.h
 * This header file contains the declaration of the described types in the IDL file.
 *
 * This file was generated by the tool gen.
 */

#ifndef _TEST_TYPE_OBJECT_H_
#define _TEST_TYPE_OBJECT_H_


#include <fastrtps/types/TypeObject.h>
#include <map>

#if defined(_WIN32)
#if defined(EPROSIMA_USER_DLL_EXPORT)
#define eProsima_user_DllExport __declspec( dllexport )
#else
#define eProsima_user_DllExport
#endif
#else
#define eProsima_user_DllExport
#endif

#if defined(_WIN32)
#if defined(EPROSIMA_USER_DLL_EXPORT)
#if defined(Test_SOURCE)
#define Test_DllAPI __declspec( dllexport )
#else
#define Test_DllAPI __declspec( dllimport )
#endif // Test_SOURCE
#else
#define Test_DllAPI
#endif
#else
#define Test_DllAPI
#endif // _WIN32

using namespace eprosima::fastrtps::types;

class TestTypeFactory
{
public:
    TestTypeFactory();
    ~TestTypeFactory();

    eProsima_user_DllExport void registerTypes();

    eProsima_user_DllExport const TypeIdentifier* GetTypeIdentifier(const std::string &basic_type_name);
    eProsima_user_DllExport const TypeObject* GetTypeObject(const std::string &basic_type_name);

    eProsima_user_DllExport const TypeIdentifier* GetMyEnumIdentifier();
    eProsima_user_DllExport const TypeObject* GetMyEnumObject();

    eProsima_user_DllExport const TypeIdentifier* GetMyAliasEnumIdentifier();
    eProsima_user_DllExport const TypeObject* GetMyAliasEnumObject();

    eProsima_user_DllExport const TypeIdentifier* GetMyAliasEnum2Identifier();
    eProsima_user_DllExport const TypeObject* GetMyAliasEnum2Object();

    eProsima_user_DllExport const TypeIdentifier* GetMyAliasEnum3Identifier();
    eProsima_user_DllExport const TypeObject* GetMyAliasEnum3Object();

    eProsima_user_DllExport const TypeIdentifier* GetBasicStructIdentifier();
    eProsima_user_DllExport const TypeObject* GetBasicStructObject();

    eProsima_user_DllExport const TypeIdentifier* GetMyOctetArray500Identifier();
    eProsima_user_DllExport const TypeObject* GetMyOctetArray500Object();

    eProsima_user_DllExport const TypeIdentifier* GetBSAlias5Identifier();
    eProsima_user_DllExport const TypeObject* GetBSAlias5Object();

    eProsima_user_DllExport const TypeIdentifier* GetMA3Identifier();
    eProsima_user_DllExport const TypeObject* GetMA3Object();

    eProsima_user_DllExport const TypeIdentifier* GetMyMiniArrayIdentifier();
    eProsima_user_DllExport const TypeObject* GetMyMiniArrayObject();

    eProsima_user_DllExport const TypeIdentifier* GetMySequenceLongIdentifier();
    eProsima_user_DllExport const TypeObject* GetMySequenceLongObject();

    eProsima_user_DllExport const TypeIdentifier* GetComplexStructIdentifier();
    eProsima_user_DllExport const TypeObject* GetComplexStructObject();

    eProsima_user_DllExport const TypeIdentifier* GetMyUnionIdentifier();
    eProsima_user_DllExport const TypeObject* GetMyUnionObject();

    eProsima_user_DllExport const TypeIdentifier* GetMyUnion2Identifier();
    eProsima_user_DllExport const TypeObject* GetMyUnion2Object();

    eProsima_user_DllExport const TypeIdentifier* GetCompleteStructIdentifier();
    eProsima_user_DllExport const TypeObject* GetCompleteStructObject();


private:
    std::map<std::string, std::string> m_Aliases;
};

#endif // _TEST_TYPE_OBJECT_H_