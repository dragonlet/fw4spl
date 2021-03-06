/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_PORT_HPP_
#define _FWDATA_PORT_HPP_

#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/factory/new.hpp"

fwCampAutoDeclareDataMacro((fwData)(Port), FWDATA_API);

namespace fwData
{
/**
 * @class   Port
 * @brief   A port is defined by an identifier and a type.
 * 
 * @date    2007-2009.
 */
class FWDATA_CLASS_API Port : public ::fwData::Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Port)(::fwData::Object), (()), ::fwData::factory::New< Port >) ;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Port(::fwData::Object::Key key);

    /// Destructor
    FWDATA_API virtual ~Port();

    fwGettersSettersDocMacro(Identifier, identifier, std::string,the port identifier (example : "ID_SIZEX", "THRESHOLD_LOW") );

    fwGettersSettersDocMacro(Type, type, std::string, the port type (example : "Image", "Mesh", a %data className));

    fwCampMakeFriendDataMacro((fwData)(Port));

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source );

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);

protected :

    //! port identifier
    std::string  m_identifier; // example "ID_SIZEX", "THRESHOLD_LOW"
    //! port type
    std::string  m_type ; // example "Image, Mesh" un className de data
};

}

#endif // _FWDATA_PORT_HPP_
