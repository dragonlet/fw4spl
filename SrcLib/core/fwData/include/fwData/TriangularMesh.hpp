/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_TRIANGULARMESH_HPP_
#define _FWDATA_TRIANGULARMESH_HPP_

#include <vector>

#include <boost/function.hpp>

#include <fwMath/IntrasecTypes.hpp>

#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/factory/new.hpp"

fwCampAutoDeclareDataMacro((fwData)(TriangularMesh), FWDATA_API);
namespace fwData
{
/**
 * @class     TriangularMesh
 * @brief     This class defines a mesh.
 *
 * A mesh is represented by a container of 3D points and a container of cells.
 * A cell contains indices of the three points related to a triangular cell.
 *
 * 
 * @date    2007-2009.
 */
class FWDATA_CLASS_API TriangularMesh : public Object
{
public :
    fwCoreClassDefinitionsWithFactoryMacro( (TriangularMesh)(::fwData::Object), (()), ::fwData::factory::New< TriangularMesh >) ;

    fwCampMakeFriendDataMacro((fwData)(TriangularMesh));

    /// 3D point container
    typedef std::vector< std::vector< float > >         PointContainer ;
    typedef const std::vector< std::vector< float > >   ConstPointContainer ;

    /// A cell contains indices of the three points related to a triangular cell
    typedef std::vector< std::vector< int > >           CellContainer ;
    typedef const std::vector< std::vector< int > >     ConstCellContainer ;

    /**
     * Typedef using for pointer function
     */
    typedef ::boost::function<void      ( ::fwData::TriangularMesh*, int, float, float, float ) > set1ValueFunc;
    typedef ::boost::function<double*   ( ::fwData::TriangularMesh*, int)  > get1ValueFunc;
    typedef ::boost::function<void      ( ::fwData::TriangularMesh*, int, int, int, int ) > set1IndexFunc;
    typedef ::boost::function<const int ( ::fwData::TriangularMesh* ) > get1IndexFunc;
    typedef ::boost::function<void      ( ::fwData::TriangularMesh*, int, float, float, float ) > set1NormalFunc;
    typedef ::boost::function<const int ( ::fwData::TriangularMesh* ) > get1NormalFunc;
    typedef ::boost::function<const int ( ::fwData::TriangularMesh* ) > getNumPointsFunc;
    typedef ::boost::function<const int ( ::fwData::TriangularMesh* ) > getNumCellsFunc;


    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API TriangularMesh(::fwData::Object::Key key);

    /**
     * @brief destructor
     */
    FWDATA_API virtual ~TriangularMesh() ;

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source );

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);

    /**
     * @brief returns editable point container
     */
    FWDATA_API PointContainer &points() ;

    /**
     * @brief returns point container
     */
    FWDATA_API ConstPointContainer &points() const;

    /**
     * @brief returns editable cell container
     */
    FWDATA_API CellContainer &cells() ;

    /**
     * @brief returns cell container
     */
    FWDATA_API ConstCellContainer &cells() const;

    /**
     * Clear the point container
     */
    FWDATA_API void clearPoints();

    /**
     * Clear the cell container
     */
    FWDATA_API void clearCells();

    FWDATA_API void setOneVectorInPointList(int _iIndex, float _fX, float _fY, float _fZ);

    FWDATA_API void setOneIndexInIndexList(int _iIndex, int _p1, int _p2, int _p3);

    FWDATA_API double* getOneVectorFromPointList(int _pt) const;

    FWDATA_API int getOneIndexFromIndexList(void) const;

    FWDATA_API void setOneVectorInNormalList(int x, float, float, float);

    FWDATA_API int getOneIndexFromNormalList(void) const;

    FWDATA_API size_t getNumPoints(void) const ;

    FWDATA_API size_t getNumCells(void) const ;

protected :

    PointContainer m_points ;
    CellContainer  m_cells ;

} ;

}

#endif //_FWDATA_TRIANGULARMESH_HPP_
