/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_POINT_LIST_HPP_
#define _FWDATA_POINT_LIST_HPP_

#include <vector>

#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/Point.hpp"

fwCampAutoDeclareDataMacro((fwData)(PointList), FWDATA_API);

namespace fwData
{
/**
 * @class   PointList
 * @brief   This class defines a list of points
 * @see     ::fwData::Point
 * 
 * @date    2007-2009.
 */
class FWDATA_CLASS_API PointList : public Object
{

public :
    fwCoreClassDefinitionsWithFactoryMacro( (PointList)(::fwData::Object),
        (()), ::fwData::factory::New< PointList >) ;

    fwCampMakeFriendDataMacro((fwData)(PointList));

    typedef std::vector< ::fwData::Point::sptr > PointListContainer;


    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API PointList(::fwData::Object::Key key);

    /// Destructor
    FWDATA_API virtual ~PointList();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source );

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);

    /// Points container
    fwGettersSettersDocMacro(Points, vPoints, PointListContainer, a container of all points);

protected :

    //! Points container
    PointListContainer m_vPoints;

}; // end class PointList

} // end namespace fwData

#endif // _FWDATA_POINT_LIST_HPP_
