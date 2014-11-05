#pragma once
#ifndef QTWEBSERVICEFWD_H
#define QTWEBSERVICEFWD_H

// forward classes
class QWebService;
class QWebServiceConfig;

class QWebMiddleWare;
class QWebMiddleWareRegistrar;

class QWebRouter;
class QWebRoute;
class QWebRouteFactory;
class QWebRequest;
class QWebResponse;

// Define to export or import depending if we are building or using the library.
// QTWEBAPPLICATION_EXPORT should only be defined when building.
#if defined(QTWEBSERVICE_EXPORT)
#define QTWEBSERVICE_API Q_DECL_EXPORT
#else
#define QTWEBSERVICE_API Q_DECL_IMPORT
#endif


#endif // QTWEBSERVICEFWD_H
