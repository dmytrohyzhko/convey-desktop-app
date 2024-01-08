#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    server.init();
    connect(&server, &ConveyWebSocketServer::processMessage, this, &MainWindow::processMessage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QByteArray customPercentEncode(const QString &input) {
    QByteArray result;
    for (const QChar &ch : input) {
        if (ch == '\'' || ch == '"') {
            result += '%' + QByteArray::number(static_cast<uchar>(ch.toLatin1()), 16).toUpper();
        } else {
            result += ch;
        }
    }
    return result;
}


void MainWindow::processMessage(QWebSocket *pSender, QString message, QString type, QJsonObject data)
{
    ui->textEdit->append(">>>>>>>>>>>>>>>>>>>>>>>>>>");
    ui->textEdit->append(message);

    if(ui->textEdit->verticalScrollBar()) ui->textEdit->verticalScrollBar()->setValue(ui->textEdit->verticalScrollBar()->maximum());
}

void MainWindow::sendMessage(QString message) {
    ui->textEdit->append("<<<<<<<<<<<<<<<<<<<<<<<<<<");
    ui->textEdit->append(message);
    server.brodcast(message);

    if(ui->textEdit->verticalScrollBar()) ui->textEdit->verticalScrollBar()->setValue(ui->textEdit->verticalScrollBar()->maximum());
}

void MainWindow::on_btnGetWindows_clicked()
{
    sendMessage("{ \"type\": \"windows\" }");
}


void MainWindow::on_btnGetPageSource_clicked()
{
    QString tabId = ui->lineEditTabId->text();
    if(tabId.isEmpty()) return;

    sendMessage(QString("{ \"type\": \"pageSource\", \"data\": { \"tabId\": %1 } }").arg(tabId));
}


void MainWindow::on_btnOpenNewWindow_clicked()
{
    QString parentWindowId = ui->lineEditParentWindowId->text();
    QString url = ui->lineEditUrl->text();

    if(parentWindowId.isEmpty()) return;

    sendMessage(QString("{ \"type\": \"newWindow\", \"data\": { \"parentWindowId\": %1, \"url\": \"%2\" } }").arg(parentWindowId).arg(url));

}


void MainWindow::on_btnEnterValue_clicked()
{
    QString tabId = ui->lineEditTabId_2->text();
    QString selector = customPercentEncode(ui->lineEditSelector->text());
    QString value = ui->lineEditValue->text();

    qDebug() << selector;

    if(tabId.isEmpty()) return;
    if(selector.isEmpty()) return;

    sendMessage(QString("{ \"type\": \"enterValue\", \"data\": { \"tabId\": %1, \"selector\": \"%2\", \"value\": \"%3\" } }").arg(tabId).arg(selector).arg(value));
}


void MainWindow::on_btnClickElement_clicked()
{
    QString tabId = ui->lineEditTabId_3->text();
    QString selector = customPercentEncode(ui->lineEditSelector_2->text());

    qDebug() << selector;

    if(tabId.isEmpty()) return;
    if(selector.isEmpty()) return;

    sendMessage(QString("{ \"type\": \"clickElement\", \"data\": { \"tabId\": %1, \"selector\": \"%2\" } }").arg(tabId).arg(selector));
}

#include <QUuid>

void MainWindow::on_btnEvaluateScript_clicked()
{
    QString tabId = ui->lineEditTabId_4->text();
    QString script = ui->plainTextEdit->toPlainText();

    if(tabId.isEmpty()) return;
    if(script.isEmpty()) return;

    QUuid uuid = QUuid::createUuid();
    QString messageId = QString("evaluateScript") + uuid.toString();

    QString addtionalCode =
        "(async() => {\n"
        "var messageId = '" + messageId + "'" + "\n"
        "try {" + "\n"
        "  const ret = main();" + "\n"
        "  await window.postMessage({" + "\n"
        "    type: 'evaluateScript'," + "\n"
        "    messageId: messageId," + "\n"
        "    status: true," + "\n"
        "    data: ret," + "\n"
        "  }, '*');" + "\n"
        "} catch (error) {" + "\n"
        "  console.log(error);" + "\n"
        "  await window.postMessage({" + "\n"
        "    type: 'evaluateScript'," + "\n"
        "    messageId: messageId," + "\n"
        "    status: false," + "\n"
        "    data: null," + "\n"
        "    message: error?.message ?? \"Something went wrong\"," + "\n"
        "  }, '*');" + "\n"
        "}})()";

    server.insertConveyScript(messageId, (script + addtionalCode).toLocal8Bit());

    sendMessage(QString("{ \"type\": \"evaluateScript\", \"data\": { \"tabId\": %1, \"scriptId\": \"%2\" } }").arg(tabId).arg(messageId));
}


void MainWindow::on_btnMonitorFor_clicked()
{
    QString monitorId = ui->lineEditTabId_11->text();
    QString condition = ui->plainTextEdit_3->toPlainText();
    QString script = ui->plainTextEdit_2->toPlainText();

    if(monitorId.isEmpty()) return;
    if(script.isEmpty()) return;
    if(condition.isEmpty()) return;

    QUuid uuid = QUuid::createUuid();
    QString scriptId = QString("monitorFor") + uuid.toString();

    QString addtionalCode =
            "var timer = setInterval(async () => {\n"
            "   var scriptId = '" + scriptId + "';" + "\n"
            "   var monitorId = '" + monitorId + "';" + "\n"
            "   if(condition()) {" + "\n"
            "      clearInterval(timer);" + "\n"
            "      try {" + "\n"
            "      const ret = main();" + "\n"
            "      await window.postMessage({" + "\n"
            "          type: 'monitorFor'," + "\n"
            "          scriptId: scriptId," + "\n"
            "          monitorId: monitorId," + "\n"
            "          status: true," + "\n"
            "          data: ret," + "\n"
            "      }, '*');" + "\n"
            "      } catch (error) {" + "\n"
            "          console.log(error);" + "\n"
            "          await window.postMessage({" + "\n"
            "              type: 'monitorFor'," + "\n"
            "              scriptId: scriptId," + "\n"
            "              monitorId: monitorId," + "\n"
            "              status: false," + "\n"
            "              data: null," + "\n"
            "              message: error?.message ?? \"Something went wrong\"," + "\n"
            "          }, '*');" + "\n"
            "      }" + "\n"
            "   }" + "\n"
            "}, 1000)";

    server.insertConveyScript(scriptId, (condition + "\n" + script + "\n" + addtionalCode).toLocal8Bit());

    sendMessage(QString("{ \"type\": \"monitorFor\", \"data\": { \"monitorId\": %1, \"scriptId\": \"%2\" } }").arg(monitorId).arg(scriptId));
}


void MainWindow::on_btnSend_clicked()
{
    QString code = ui->plainTextEdit_4->toPlainText();
    sendMessage(code);
}

