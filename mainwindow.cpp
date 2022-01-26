#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "device.h"
#include "QMessageBox"
#include "sysinfoapi.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mqttclient = new QMqttClient();
    recovery_state();
    if(ui->autocon->checkState()==Qt::CheckState::Checked)
    {
        InitMqtt();
    }
    systemTray = new QSystemTrayIcon(this);
    systemTray->setIcon(QIcon(":/icon/icon.png"));
    systemTray->setToolTip("shut off");
    systemTray->show();

    connect(systemTray,&QSystemTrayIcon::activated,this,&MainWindow::ActivationReason);

    ui->timeshow->setText(__DATE__ "\n" __TIME__);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mqttclient;

    delete settings;
}


void MainWindow::ActivationReason(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
     case QSystemTrayIcon::DoubleClick:
        this->showNormal();
        break;
     case QSystemTrayIcon::Unknown:
     case QSystemTrayIcon::Context:
     case QSystemTrayIcon::Trigger:
     case QSystemTrayIcon::MiddleClick:
          break;
    }
}

void MainWindow::hideEvent(QHideEvent  *event)
{
    if(systemTray->isVisible())
    {
        this->hide(); //隐藏窗口
        event->ignore(); //忽略事件
    }
}

void MainWindow::recovery_state()
{
    settings = new QSettings("./state.ini", QSettings::IniFormat);
    ui->autocon->setCheckState(Qt::CheckState(settings->value("autocon").toInt()));
    ui->clientID->setText(settings->value("clientID").toString());
    ui->topic->setText(settings->value("topic").toString());
}

void MainWindow::InitMqtt()
{
    if(mqttclient == nullptr)
            return;
    ClientId = ui->clientID->text();
    topic = ui->topic->text();
    if(ClientId.isEmpty() || topic.isEmpty())
    {
        QMessageBox::information(this, "Error","输入错误！");
        return;
    }
//    mqttclient->setClientId("62a63a43a400ec0d755959d849ce0547");
    mqttclient->setClientId(ClientId);
    mqttclient->setHostname("bemfa.com");
    mqttclient->setPort(9501);
    mqttclient->connectToHost();


    connect(mqttclient, &QMqttClient::connected, this, &MainWindow::qt_connected);
    connect(mqttclient, &QMqttClient::disconnected, this, &MainWindow::connect_lost);

}

void MainWindow::TryConnectMqtt()
{

    if(mqttclient == nullptr)
            return;
    ClientId = ui->clientID->text();
    topic = ui->topic->text();
    if(ClientId.isEmpty() || topic.isEmpty())
    {
        QMessageBox::information(this, "Error","输入错误！");
        return;
    }
    mqttclient->setClientId(ClientId);
    mqttclient->setHostname("bemfa.com");
    mqttclient->setPort(9501);

    mqttclient->disconnectFromHost();
    mqttclient->cleanSession();
//    mqttclient->disconnect();
    mqttclient->connectToHost();
}

void MainWindow::timeout()
{
    delete timer;
    TryConnectMqtt();
}

void MainWindow::connect_lost()
{
    timer = new QTimer;
//    timer->setInterval(5000*timeoutcount);
    timeoutcount++;
    if(timeoutcount >= 10)
        timeoutcount = 1;
    timer->start(5000*timeoutcount);
    ui->Log_Text->append("连接丢失 正在重连......");
    connect(timer, &QTimer::timeout, this, &MainWindow::timeout);

}

void MainWindow::data_analyze(const char *data, int len)
{
    if(len > 2)
    {
        if(stricmp(data,"off")==0)
        {
            ui->Log_Text->append("system(\"shutdown -h\")");
            system("shutdown -h");
        }
    }
}

void MainWindow::messageReceived(const QByteArray &message, const QMqttTopicName &topic)
{
    QString rec = topic.name()+":"+message;
    ui->Log_Text->append(rec);
    if(topic.name().compare(this->topic.toStdString().c_str())==0)
    {
        data_analyze(message.toStdString().c_str(),message.length());
    }
}

void MainWindow::qt_connected()
{
    timeoutcount = 0;

    mqttclient->subscribe(QMqttTopicFilter(topic),0);

    disconnect(mqttclient, &QMqttClient::messageReceived, this, &MainWindow::messageReceived);   //先取消连接，不然会有多次消息接收
    connect(mqttclient, &QMqttClient::messageReceived, this, &MainWindow::messageReceived);
    ui->connect2server->setDisabled(true);
    save_state();
}



void MainWindow::Update_LogText(QString message)
{
    ui->Log_Text->append(message);
}



void MainWindow::on_connect2server_clicked()
{
    InitMqtt();
}

void MainWindow::save_state()
{
    settings->setValue("autocon",ui->autocon->checkState());
    settings->setValue("clientID",ui->clientID->text());
    settings->setValue("topic",ui->topic->text());
}

void MainWindow::on_autocon_stateChanged(int arg1)
{
//    save_state();
    (void)arg1;
}


void MainWindow::on_clientID_editingFinished()
{
//    save_state();
}


void MainWindow::on_topic_editingFinished()
{
//    save_state();
}



