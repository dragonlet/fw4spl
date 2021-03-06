/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATA_EQUIPMENT_HPP__
#define __FWMEDDATA_EQUIPMENT_HPP__

#include "fwData/Object.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/macros.hpp"

#include "fwMedData/types.hpp"
#include "fwMedData/config.hpp"

fwCampAutoDeclareDataMacro((fwMedData)(Equipment), FWMEDDATA_API);

namespace fwMedData
{

/**
 * @class Equipment
 * Holds equipment information
 *
 * 
 */
class FWMEDDATA_CLASS_API Equipment : public ::fwData::Object
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (Equipment)(::fwData::Object), (()), ::fwData::factory::New< Equipment >) ;

    fwCampMakeFriendDataMacro((fwMedData)(Equipment));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWMEDDATA_API Equipment(::fwData::Object::Key key);

    /// Destructor
    FWMEDDATA_API virtual ~Equipment();

    /// Defines shallow copy
    FWMEDDATA_API void shallowCopy( const ::fwData::Object::csptr &_source );

    /// Defines deep copy
    FWMEDDATA_API void cachedDeepCopy( const ::fwData::Object::csptr &_source, DeepCopyCacheType &cache );

    /**
     * @name Getters / Setters
     * @{ */

    /**
     * @brief Institution where the equipment that produced the composite instances is located (0008,0080)
     * @{ */
    fwDataGetSetCRefMacro(InstitutionName, DicomValueType);
    /**  @} */

    /**  @} */

protected:

    /// Institution name
    DicomValueType m_attrInstitutionName;
};

}   //end namespace fwMedData

#endif // __FWMEDDATA_EQUIPMENT_HPP__


