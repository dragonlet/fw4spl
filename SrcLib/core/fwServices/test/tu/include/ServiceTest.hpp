/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWSERVICES_TEST_TU_SERVICETEST_HPP_
#define _FWSERVICES_TEST_TU_SERVICETEST_HPP_

#include <cppunit/extensions/HelperMacros.h>
#include <fwRuntime/EConfigurationElement.hpp>

namespace fwServices
{
namespace ut
{

/**
 * @brief Test many methods to create service (with uuid, with configuration,...).
 * And test service methods (start, stop and update).
 */
class ServiceTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( ServiceTest );
    CPPUNIT_TEST( testServiceCreation );
    CPPUNIT_TEST( testServiceCreationWithTemplateMethods );
    CPPUNIT_TEST( testServiceCreationWithUUID );
    CPPUNIT_TEST( testStartStopUpdate );
    CPPUNIT_TEST( testCommunication );
    CPPUNIT_TEST_SUITE_END();


public:
    // interface
    void setUp();
    void tearDown();


    // fonctions de tests

    /// Test add and remove service
    void testServiceCreation();

    /// Test add and remove service using template methods
    void testServiceCreationWithTemplateMethods();

    /// Test add and remove service with UUID
    void testServiceCreationWithUUID();

    /// test start/stop/update service
    void testStartStopUpdate();

    /// test sending/receiving message
    void testCommunication();

private:
    // create a configurationElement to build a service
    ::fwRuntime::EConfigurationElement::sptr buildServiceConfig();

    // create a configurationElement to build an object with services
    ::fwRuntime::ConfigurationElement::sptr buildObjectConfig();
};


/**
 * @brief Service interface for test
 */
class TestService : public ::fwServices::IService
{

public :

    fwCoreServiceClassDefinitionsMacro ( (TestService)(::fwServices::IService) ) ;
    TestService() throw()
    :   m_isUpdated(false),
        m_isUpdatedMessage(false)
        {};

    virtual ~TestService() throw() {};

    virtual void configuring() throw( ::fwTools::Failed ) {};
    virtual void starting() throw(::fwTools::Failed) {};
    virtual void stopping() throw(::fwTools::Failed) {};
    virtual void updating() throw(::fwTools::Failed) {};
    virtual void info(std::ostream &_sstream ) {_sstream << "TestService" ;};

    /// return true if the service is updated with update() method
    bool getIsUpdated() { return m_isUpdated; };

    /// return true if the service is updated with update(msg) method
    bool getIsUpdatedMessage() { return m_isUpdatedMessage; };

protected:
    bool m_isUpdated;
    bool m_isUpdatedMessage;
};


/**
 * @brief Service implementation for test
 */
class TestServiceImplementation : public TestService
{

public :
    fwCoreServiceClassDefinitionsMacro ( (TestServiceImplementation)(::fwServices::ut::TestService) ) ;
    TestServiceImplementation() throw() {};
    virtual ~TestServiceImplementation() throw() {};

    virtual void configuring() throw( ::fwTools::Failed ) {};
    virtual void starting() throw(::fwTools::Failed) {};
    virtual void stopping() throw(::fwTools::Failed) {};
    virtual void updating() throw(::fwTools::Failed)
    {
        m_isUpdated = true;
    };

    virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
    {
        m_isUpdatedMessage = true;
    };

    virtual void info(std::ostream &_sstream ) {_sstream << "TestServiceImplementation" ;};
};

} //namespace ut
} //namespace fwServices

#endif
