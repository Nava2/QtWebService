#include "QWebServiceConfig.h"

#include "router/QWebRoute.h"
#include "router/QWebRouter.h"

QWebServiceConfig::QWebServiceConfig() :
    m_handlers(),
    m_specialHandlers(),
    m_404(nullptr),
    m_factory(new QWebRouteFactory()) {

    // initialize the handler QHash
#define HANDLER_INIT( TYPE ) \
    m_handlers[ QWebService::HttpMethod::TYPE ] = QSet<QWebServiceConfig::Key::Ptr>()

    HANDLER_INIT(HTTP_GET);
    HANDLER_INIT(HTTP_DELETE);
    HANDLER_INIT(HTTP_POST);
    HANDLER_INIT(HTTP_PUT);

#undef HANDLER_INIT

}

QWebServiceConfig::~QWebServiceConfig() {
    delete m_factory;
}

QWebService* QWebServiceConfig::build(QObject* parent) const
{
    typedef QPair<QWebRoute::Ptr, QWebService::RouteFunction> RouteHandler;

    QHash<QWebService::HttpMethod, QList<RouteHandler> > handlerTable;

    // keep a buffer of already used routes to use the same path to diff handler
    QHash<QString, QWebRoute::Ptr> routeBuff;

    for (const QWebService::HttpMethod method : m_handlers.keys()) {
        QList<RouteHandler> handlers;

        for (QWebServiceConfig::Key::Ptr route : m_handlers[method]) {
//            if (route == "/") {
//                rootHandler[method] = rhandles[route];

//                continue;
//            }

            const QString str = route->strRep;
            QWebRoute::Ptr routeObj = routeBuff.contains(str) ? routeBuff[str] : QWebRoute::Ptr();
            if (!routeObj) {
                if (route->isPath) {
                    routeObj = m_factory->create(route->path);
                } else {
                    routeObj = m_factory->create(route->reg);
                }

                // TODO CHECK FOR ERRORS

                routeBuff[str] = routeObj;
            }

            handlers += RouteHandler(routeObj, route->func);
        }

        // handlers now has all of the route objs
        handlerTable.insert(method, handlers);
    }

    // handlerTable is now populated minimizing QHttpRoute instances

    QWebService::RouteFunction fourohfour = this->m_404;
    if (!fourohfour) {
        fourohfour = QWebRouter::DEFAULT_404;
    }

    // we let the QHttpRouter ctor setup the parent relationships
    auto router = new QWebRouter(handlerTable, fourohfour);

    auto server = new QHttpServer();
    QObject::connect(server, &QHttpServer::newRequest, router, &QWebRouter::handleRoute );

    auto service = new QWebService(server, router, parent);
    router->setWebService(service);

    router->setParent(service);
    server->setParent(service);

    // for all of the special handlers, set their parent to the new router
    for (auto ptr : this->m_specialHandlers) {
        ptr->setParent(router);
    }

    return service;
}



QWebServiceConfig& QWebServiceConfig::fourohfour(QWebService::RouteFunction func)
{
    this->m_404 = func;

    return *this;
}
