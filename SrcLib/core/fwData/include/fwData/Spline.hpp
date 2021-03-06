/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_SPLINE_HPP_
#define _FWDATA_SPLINE_HPP_

#include <vector>
#include <boost/cstdint.hpp>

#include "fwData/config.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/Color.hpp"

namespace fwData
{
/**
 * @class   Spline
 * @brief   This class defines a spline object.
 * 
 * @date    2007-2009.
 */
class FWDATA_CLASS_API Spline : public Object
{
public :
    fwCoreClassDefinitionsWithFactoryMacro( (Spline)(::fwData::Object), (()), ::fwData::factory::New< Spline >) ;

    /**
     * @struct point
     * @brief Defines a 3D %point for a spline.
     */
    class point
    {
    public:

        int id;                      /**< @brief Identifier */
        double p[3];                 /**< @brief Point coordinates */
        double normal[3];            /**< @brief Normal coordinates*/
        ::fwData::Color::sptr c;     /**< @brief Point color*/
        bool isVisible;              /**< @brief Flag if %point is visible */

        point &operator=( const point & _point )
        {
            this->id = _point.id;
            this->p[0] = _point.p[0];
            this->p[1] = _point.p[1];
            this->p[2] = _point.p[2];
            this->normal[0] = _point.normal[0];
            this->normal[1] = _point.normal[1];
            this->normal[2] = _point.normal[2];
            this->isVisible = _point.isVisible;
            c = ::fwData::Object::copy( _point.c );
            return(*this);
        };
    };

    /// 3D %point container
    typedef std::vector< point > Points ;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Spline(::fwData::Object::Key key);

    /**
     * @brief destructor
     */
    FWDATA_API virtual ~Spline() ;

    /**
     * @brief returns editable point container
     */
    FWDATA_API Points &points() ;

    fwGettersSettersDocMacro(Radius, radius, double, radius value);

    fwGettersSettersDocMacro(NbSides, nbSides, int, number of slices);

    fwGettersSettersDocMacro(IdSpline, idSpline, int, spline identifier);

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);

protected :

    /// Points container
    Points      m_points ;
    double      m_radius;
    int         m_nbSides;
    int         m_idSpline;

} ;

}

#endif // _FWDATA_SPLINE_HPP_
