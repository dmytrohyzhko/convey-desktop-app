#ifndef CONVEYWEBSOCKETSERVER_H
#define CONVEYWEBSOCKETSERVER_H

#include <QWebSocketServer>
#include <QHostAddress>
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>

#include <qhttpserver.h>
#include <qhttprequest.h>
#include <qhttpresponse.h>

class ConveyWebSocketServer : public QObject
{
    Q_OBJECT

private:
    QWebSocketServer *m_pWebSocketServer;
    QHttpServer *m_pHttpServer;
    QList<QWebSocket*> m_clients;

    QHash<QString, QByteArray> m_conveyScriptMap;

public:
    explicit ConveyWebSocketServer(QObject *parent = nullptr);

    void init();

public slots:
    void handleRequest(QHttpRequest *, QHttpResponse *);

    void onNewConnection();
    void onClosed();

    void processTextMessage(const QString &message);
    void socketDisconnected();

    void brodcast(QString message);

    void insertConveyScript(QString, QByteArray);

signals:
    void processMessage(QWebSocket*, QString, QString, QJsonObject);

private:
};

#endif // CONVEYWEBSOCKETSERVER_H
