#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void sendDataToPython();
    void stopUdpCommunication();
    void processPendingDatagrams();

private:
    Ui::MainWindow *ui;
    QUdpSocket *udpSocket;
    bool udpCommunicationStopped;  // UDP iletişiminin durdurulup durdurulmadığını belirten bayrak
};

#endif // MAINWINDOW_H





/*
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTimer>
#include <QHostAddress>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void startUdpCommunication();
    void stopUdpCommunication();
    void processPendingDatagrams();
    void sendDataToPython();
    //void sendDataToPythonOnce();  // Bu satırı ekleyin

private:
    Ui::MainWindow *ui;
    QUdpSocket *udpSocket;
    QTimer *timer;
    int ktsSendPeriod;
    int sinusPeriod;
    int messageDelay;
};
#endif // MAINWINDOW_H

*/





