#include "ConveyBrowserEngine.h"
#include <QUrlQuery>

#define CONVEY_WEBSOCKET_PORT 15898
#define CONVEY_HTTPSERVER_PORT 15899

ConveyWebSocketServer::ConveyWebSocketServer(QObject *parent) : QObject(parent),
    m_pWebSocketServer(new QWebSocketServer(QString('convey websocket server'), QWebSocketServer::NonSecureMode, this)),
    m_pHttpServer(new QHttpServer(this))
{

}

void ConveyWebSocketServer::init()
{
    if (m_pWebSocketServer->listen(QHostAddress::Any, CONVEY_WEBSOCKET_PORT)) {
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection, this, &ConveyWebSocketServer::onNewConnection);
        connect(m_pWebSocketServer, &QWebSocketServer::closed, this, &ConveyWebSocketServer::onClosed);
    }

    if (m_pHttpServer->listen(QHostAddress::Any, CONVEY_HTTPSERVER_PORT)) {
        connect(m_pHttpServer, &QHttpServer::newRequest, this, &ConveyWebSocketServer::handleRequest);
    }
}

void ConveyWebSocketServer::onNewConnection()
{
    QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();

    connect(pSocket, &QWebSocket::textMessageReceived, this, &ConveyWebSocketServer::processTextMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &ConveyWebSocketServer::socketDisconnected);

    m_clients << pSocket;

    qDebug() << "newConnection:" << pSocket;

//    pSocket->sendTextMessage("{ \"type\": \"windows\" }");
    emit processMessage(pSocket, "Extension connected", "", QJsonObject());
}

void ConveyWebSocketServer::onClosed()
{

}

void ConveyWebSocketServer::insertConveyScript(QString id, QByteArray script)
{
    m_conveyScriptMap.insert(id, script);
}

void ConveyWebSocketServer::handleRequest(QHttpRequest *request, QHttpResponse *response)
{
    // Handle the GET request
    if (request->method() == QHttpRequest::HTTP_GET) {

        QUrlQuery urlQuery(request->url());
        QString queryId = urlQuery.queryItemValue("id");

        // Respond with a simple message
        response->setHeader("Content-Type", "text/plain");
        response->writeHead(QHttpResponse::STATUS_OK);

        if(m_conveyScriptMap.keys().contains(queryId)) {
            response->end(m_conveyScriptMap.value(queryId));
        }
    } else {
        // Respond with an error for unsupported methods
        response->writeHead(QHttpResponse::STATUS_METHOD_NOT_ALLOWED);
        response->end("Method not allowed");
    }
}

void ConveyWebSocketServer::processTextMessage(const QString &message)
{
    QWebSocket *senderSocket = qobject_cast<QWebSocket *>(sender());
    qDebug() << "Text message received from client: " << message;

    // Parse the JSON message
    QJsonDocument jsonDoc = QJsonDocument::fromJson(message.toUtf8());
    if (!jsonDoc.isNull() && jsonDoc.isObject()) {

        try {
            QJsonObject jsonObject = jsonDoc.object();
            const QString type = jsonObject["type"].toString();
            const QJsonObject data = jsonObject["data"].toObject();

            emit processMessage(senderSocket,  jsonDoc.toJson(QJsonDocument::Indented), type, data);

        }  catch (...) {
            qDebug() << "Invalid MESSAGE format!";
        }

    } else {
        qDebug() << "Invalid JSON format!";
    }
}

void ConveyWebSocketServer::socketDisconnected()
{
    QWebSocket *pSocket = qobject_cast<QWebSocket *>(sender());

    qDebug() << "socketDisconnected:" << pSocket;
    emit processMessage(pSocket, "Extension disconnected", "", QJsonObject());

    if (pSocket) {
        m_clients.removeAll(pSocket);
        pSocket->deleteLater();
    }
}

void ConveyWebSocketServer::brodcast(QString message)
{
    foreach(QWebSocket* socket, m_clients)
        socket->sendTextMessage(message);
}
