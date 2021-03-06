/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */



#ifndef _FWTHREAD_CONFIG_HPP_
#define _FWTHREAD_CONFIG_HPP_

#ifdef _WIN32

    #ifdef FWTHREAD_EXPORTS
    #define FWTHREAD_API __declspec(dllexport)
    #else
    #define FWTHREAD_API __declspec(dllimport)
    #endif

    #define FWTHREAD_CLASS_API

    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef FWTHREAD_EXPORTS
    #define FWTHREAD_API __attribute__ ((visibility("default")))
    #define FWTHREAD_CLASS_API __attribute__ ((visibility("default")))
    #else
    //#define FWTHREAD_API __attribute__ ((visibility("hidden")))
    //#define FWTHREAD_CLASS_API __attribute__ ((visibility("hidden")))
    #define FWTHREAD_API
    #define FWTHREAD_CLASS_API
    #endif

#else

    #define FWTHREAD_API
    #define FWTHREAD_CLASS_API

#endif

#endif //_FWTHREAD_CONFIG_HPP_
