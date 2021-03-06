/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_VIDEO_HPP_
#define _FWDATA_VIDEO_HPP_

#include <boost/cstdint.hpp>

#include <fwCore/mt/types.hpp>

#include "fwData/Object.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/Camera.hpp"
#include "fwData/TransformationMatrix3D.hpp"

fwCampAutoDeclareDataMacro((fwData)(Video), FWDATA_API);

namespace fwData
{

/**
 * @brief   This class focuses on video
 * @note    This version is done for test purposes (augmented reality and visualization) and will have to be accurately design
 * @note    Information to be stored here does not necessarily concern video buffer.
 * 
 * @date    2007-2009.
 * @todo    implement appropriate API
 */
class FWDATA_CLASS_API Video : public Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Video)(::fwData::Object), (()), ::fwData::factory::New< Video >) ;

    fwCampMakeFriendDataMacro((fwData)(Video));

    typedef ::boost::uint8_t VideoType;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Video(::fwData::Object::Key key);

    /**
     * @brief destructor
     */
    FWDATA_API virtual ~Video() throw();

    /// @brief Get the buffer size along X axis
    ::boost::uint32_t  getXSize() const { return m_ui32XSize; };

    /// @brief Get the buffer size along Y axis
    ::boost::uint32_t  getYSize() const { return m_ui32YSize; };

    /// @brief Get the number of bits per pixel
    VideoType  getBPP() const { return m_ui8BPP; };

    /// @brief Get the address of the image buffer
    VideoType  * getImageBuffer() const { return m_pImageBuffer; };

    /// @brief Get the camera associated with the video
    ::fwData::Camera::sptr getCamera() const { return m_camera; };

    /// @brief Return True if the data is available
    FWDATA_API bool &dataAvailable();

    /// @brief Get value of the last modification
    FWDATA_API ::boost::uint64_t getLastModified();

    /**
     * @brief Increments the count of modification
     */
    FWDATA_API void Modified();

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);

    //! The buffer size along X axis
    ::boost::uint32_t  m_ui32XSize;

    //! The buffer size along Y axis
    ::boost::uint32_t  m_ui32YSize;

    //! The number of bit by pixel
    VideoType  m_ui8BPP;

    //! The address of the image buffer
    VideoType  * m_pImageBuffer;

protected :

    /// Flag if the video is available
    bool m_dataAvailable;

    /// Value of the last modification (incremented by Modified() method
    ::boost::uint64_t m_lastModified;

    /// Camera
    ::fwData::Camera::sptr m_camera;
};

} // namespace fwData


#endif // _FWDATA_VIDEO_HPP_
