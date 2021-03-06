/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_TU_CONFIGPARSERTEST_HPP_
#define _FWCOMED_TU_CONFIGPARSERTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>

#include <fwCore/base.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/ObjectMsg.hpp>

#include <fwComEd/CompositeMsg.hpp>
#include <fwComEd/ImageMsg.hpp>

#include <fwRuntime/EConfigurationElement.hpp>


namespace fwComEd
{
namespace ut
{

/**
 * @brief   Test ProcessObject building from a ConfigurationElement and composite message.
 * @see     ICompositeParser
 * @see     CompositeEditor
 * @see     CompositeMsg
 */
class ConfigParserTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( ConfigParserTest );
    CPPUNIT_TEST( testObjectCreationWithConfig );
    CPPUNIT_TEST( testBuildComposite );
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp();
    void tearDown();

    /// test object with services creation from a configuration
    void testObjectCreationWithConfig();

    /// Test the composite building
    void testBuildComposite();

private:
    /// Create a configurationElement to build an object
    ::fwRuntime::ConfigurationElement::sptr buildObjectConfig();

    /// Create a configurationElement to build the composite
    ::fwRuntime::ConfigurationElement::sptr buildCompositeConfig();
};


/**
 * @brief   Service type for test.
 */
class TestService : public ::fwServices::IService
{
public :
    fwCoreServiceClassDefinitionsMacro ( (TestService)(::fwServices::IService) ) ;
    TestService() throw()
    :   m_isUpdated(false),
        m_isUpdatedMessage(false)
        {}

    virtual ~TestService() throw() {}

    /// return true if the service is updated with updating() method
    bool getIsUpdated() { return m_isUpdated; }

    /// return true if the service is updated with updating(msg) method
    bool getIsUpdatedMessage() { return m_isUpdatedMessage; }

    /// return the message receiving in updating(msg) method
    ::fwServices::ObjectMsg::sptr getMessage() { return m_compoMsg; }

    const std::string & getMessageEvent(){return m_messageEvent;}


protected:
    virtual void configuring() throw( ::fwTools::Failed ) {}
    virtual void starting() throw(::fwTools::Failed) {}
    virtual void stopping() throw(::fwTools::Failed) {}
    virtual void updating() throw(::fwTools::Failed) {}
    virtual void info( std::ostream &_sstream ) {_sstream << "TestService" ;}

    bool m_isUpdated;
    bool m_isUpdatedMessage;
    std::string m_messageEvent;
    ::fwServices::ObjectMsg::sptr m_compoMsg;
};

/**
 * @brief   Test service implementation for composite
 */
class TestServiceImplementationComposite : public TestService
{

public :
    fwCoreServiceClassDefinitionsMacro ( (TestServiceImplementationComposite)(::fwComEd::ut::TestService) ) ;
    TestServiceImplementationComposite() throw() {};
    virtual ~TestServiceImplementationComposite() throw() {};

    virtual void configuring() throw( ::fwTools::Failed ) {};
    virtual void starting() throw(::fwTools::Failed) {};
    virtual void stopping() throw(::fwTools::Failed) {
        SLM_TRACE_FUNC();
    };
    virtual void updating() throw(::fwTools::Failed) { m_isUpdated = true; };
    virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
    {
        ::fwComEd::CompositeMsg::csptr compositeMessage = ::fwComEd::CompositeMsg::dynamicConstCast( _msg );
        m_messageEvent.clear();

        if (compositeMessage)
        {
            if( compositeMessage->hasEvent(::fwComEd::CompositeMsg::ADDED_KEYS))
            {
                m_messageEvent += ::fwComEd::CompositeMsg::ADDED_KEYS;
            }
            if( compositeMessage->hasEvent(::fwComEd::CompositeMsg::CHANGED_KEYS))
            {
                m_messageEvent += ::fwComEd::CompositeMsg::CHANGED_KEYS;
            }
            if( compositeMessage->hasEvent(::fwComEd::CompositeMsg::REMOVED_KEYS))
            {
                m_messageEvent += ::fwComEd::CompositeMsg::REMOVED_KEYS;
            }

            m_isUpdatedMessage = true;
            m_compoMsg = ::boost::const_pointer_cast< ::fwServices::ObjectMsg >( _msg ) ;
        }
    };
};

/**
 * @brief   Test service implementation for image
 */
class TestServiceImplementationImage : public TestService
{

public :
    fwCoreServiceClassDefinitionsMacro ( (TestServiceImplementationImage)(::fwComEd::ut::TestService) ) ;
    TestServiceImplementationImage() throw() {};
    virtual ~TestServiceImplementationImage() throw() {};

    virtual void configuring() throw( ::fwTools::Failed ) {};
    virtual void starting() throw(::fwTools::Failed) {};
    virtual void stopping() throw(::fwTools::Failed) {};
    virtual void updating() throw(::fwTools::Failed) { m_isUpdated = true; };
    virtual void receiving( ::boost::shared_ptr< const ::fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed)
    {
        ::fwComEd::ImageMsg::csptr imageMessage = ::fwComEd::ImageMsg::dynamicConstCast( _msg );
        if (imageMessage && imageMessage->hasEvent(::fwComEd::ImageMsg::SLICE_INDEX))
        {
            // if receiving a imageMsg : tag service is updated
            m_isUpdatedMessage = true;
            m_compoMsg = ::boost::const_pointer_cast< ::fwServices::ObjectMsg >( _msg ) ;
        }
    };
};

} //namespace ut
} //namespace fwComEd

#endif // _FWCOMED_TU_CONFIGPARSERTEST_HPP_
