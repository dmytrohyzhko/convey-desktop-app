#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <ConveyBrowserEngine.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    ConveyWebSocketServer server;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void processMessage(QWebSocket*, QString, QString, QJsonObject);
    void sendMessage(QString);

private slots:
    void on_btnGetWindows_clicked();

    void on_btnGetPageSource_clicked();

    void on_btnOpenNewWindow_clicked();

    void on_btnEnterValue_clicked();

    void on_btnClickElement_clicked();

    void on_btnEvaluateScript_clicked();

    void on_btnMonitorFor_clicked();

    void on_btnSend_clicked();

    void on_btnStopMonitorFor_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
