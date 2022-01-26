#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "device.h"
#include "QString"
#include "deviceinfo.h"
#include "QListWidgetItem"
#include "QtMqtt/QtMqtt"
#include "QSettings"
#include "QSystemTrayIcon"
#include "QHideEvent"
#include "QTimer"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    Device *bluetoothdev;
    ServiceInfo *curservice;
    void InitMqtt();
    void data_analyze(const char *data, int len);
    void recovery_state();
    void TryConnectMqtt();
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void hideEvent(QHideEvent  *event);

private:
    void save_state();



private slots:

    void Update_LogText(QString message);
    void qt_connected();
    void messageReceived(const QByteArray &message, const QMqttTopicName &topic );
    void connect_lost();

    void on_connect2server_clicked();

    void on_autocon_stateChanged(int arg1);

    void on_clientID_editingFinished();

    void on_topic_editingFinished();

    void ActivationReason(QSystemTrayIcon::ActivationReason reason);
    void timeout();


private:
    Ui::MainWindow *ui;
    QMqttClient *mqttclient;
    QString topic;
    QString ClientId;
    QSettings *settings;
    QSystemTrayIcon *systemTray;
    QTimer *timer;
    unsigned int timeoutcount;
};
#endif // MAINWINDOW_H
