/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIPYTHON_CONFIG_HPP_
#define _UIPYTHON_CONFIG_HPP_


#ifdef _WIN32
    #pragma warning( disable : 4251 )


    #ifdef UIPYTHON_EXPORTS
    #define UIPYTHON_API __declspec(dllexport)
    #else
    #define UIPYTHON_API __declspec(dllimport)
    #endif

    #define UIPYTHON_CLASS_API
    #pragma warning(disable: 4290)

#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)

    #ifdef UIPYTHON_EXPORTS
    #define UIPYTHON_API __attribute__ ((visibility("default")))
    #define UIPYTHON_CLASS_API __attribute__ ((visibility("default")))
    #else
    #define UIPYTHON_API __attribute__ ((visibility("hidden")))
    #define UIPYTHON_CLASS_API __attribute__ ((visibility("hidden")))
    #endif

#else

    #define UIPYTHON_API
    #define UIPYTHON_CLASS_API

#endif

#endif // #ifndef _UIPYTHON_CONFIG_HPP_
