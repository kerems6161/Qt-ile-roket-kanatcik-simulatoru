#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , udpCommunicationStopped(false)  // Başlangıçta UDP iletişimi aktif
{
    ui->setupUi(this);

    udpSocket = new QUdpSocket(this);  // QUdpSocket başlatılıyor (UDP üzerinden veri gönderip almak için)

    // Butonları ve işlemleri birbirine bağlayın
    connect(ui->pushButton1, &QPushButton::clicked, this, &MainWindow::sendDataToPython);  // pushButton1'e tıklandığında veri gönderecek fonksiyonu bağla
    connect(ui->pushButton2, &QPushButton::clicked, this, &MainWindow::stopUdpCommunication);  // pushButton2'ye tıklandığında UDP bağlantısını durduracak fonksiyonu bağla
    connect(udpSocket, &QUdpSocket::readyRead, this, &MainWindow::processPendingDatagrams);    // UDP soketi veri geldiğinde 'processPendingDatagrams' fonksiyonunu çalıştıracak

    // 8081 portundan dinlemeye başla
    udpSocket->bind(QHostAddress::LocalHost, 8081);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sendDataToPython()
{
    // Eğer soket kapatıldıysa tekrar bağla
    if (udpCommunicationStopped) {
        // Eski soketi kapat
        udpSocket->abort();  // Mevcut bağlantıyı kesin

        // Yeni bağlantıyı başlat
        udpSocket->bind(QHostAddress::LocalHost, 8081);  // Soketi tekrar bind et
        udpCommunicationStopped = false;  // İletişim aktif
        ui->textBrowser4->append("UDP communication restarted.");
    }

    // Arayüzden girilen verileri textEdit alanlarından al
    int ktsSendPeriod = ui->textEdit1->toPlainText().toInt();  // KTS gönderim periyodu textEdit1'den alınıyor
    int sinusPeriod = ui->textEdit2->toPlainText().toInt();    // Sinüs periyodu textEdit2'den alınıyor

    // UDP mesajı oluşturuluyor ve 8080 portundan Python'a gönderiliyor
    QByteArray datagram = QString("%1,%2").arg(ktsSendPeriod).arg(sinusPeriod).toUtf8();  // Veriler string'e çevrilip UTF-8 formatında byte array'e dönüştürülüyor
    udpSocket->writeDatagram(datagram, QHostAddress::LocalHost, 8080);  // Mesaj localhost üzerinden 8080 portuna gönderiliyor

    // Mesaj arayüzde textBrowser4'e yazdırılıyor
    ui->textBrowser4->append("Sent: " + QString(datagram));  // Gönderilen mesajın arayüzde görünmesi sağlanıyor
}

void MainWindow::stopUdpCommunication()
{
    // UDP bağlantısını kapatmak için soketi durdur
    udpSocket->close();  // QUdpSocket bağlantısı kapatılıyor, artık veri gönderimi/alımı yapılmayacak
    udpCommunicationStopped = true;  // UDP iletişiminin durdurulduğunu belirtiyoruz
    ui->textBrowser4->append("UDP communication stopped.");  // Bağlantının kapatıldığı arayüzde belirtiliyor

    // 8080 portuna "UDP communication stopped." mesajı gönderme
    QUdpSocket udpSocketSender;
    QByteArray message = "UDP communication stopped.";  // Gönderilecek mesaj
    udpSocketSender.writeDatagram(message, QHostAddress::LocalHost, 8080);  // Mesajı 8080 portuna gönder

    // Eğer bir hata varsa, bu hatayı textBrowser4'e yazdır
    if (udpSocketSender.error() != QAbstractSocket::SocketError::UnknownSocketError) {
        ui->textBrowser4->append("Error sending stop message: " + udpSocketSender.errorString());
    }
}

void MainWindow::processPendingDatagrams()
{
    // Gelen UDP mesajlarını işleme al
    while (udpSocket->hasPendingDatagrams()) {  // UDP soketinde bekleyen veri var mı kontrol ediliyor
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());  // Gelen veri boyutu kadar buffer ayarlanıyor
        udpSocket->readDatagram(datagram.data(), datagram.size());  // Veri UDP soketinden okunuyor

        QString receivedData = QString::fromUtf8(datagram);  // Gelen veri UTF-8 formatında string'e dönüştürülüyor
        ui->textBrowser2->append("Received: " + receivedData);  // Gelen mesajı kullanıcıya gösteriyoruz

        // Mesajın işlenme gecikmesini alıyoruz
        int messageDelay = ui->textEdit3->toPlainText().toInt();  // Gecikme süresi textEdit3'da belirtilen değerden alınıyor (milisaniye cinsinden)

        // Mesajı gecikme süresi kadar bekletip sonra Python'a gönderiyoruz
        QTimer::singleShot(messageDelay * 1000, this, [=]() {  // Belirtilen gecikme süresi sonunda mesaj işlenecek (milisaniye cinsinden)
            // Gelen mesajı textBrowser3'te güncellenmiş açı değeri olarak göster
            ui->textBrowser3->append("Güncellenen değer: " + receivedData);  // Gecikme sonrası işlenip gönderilen mesajı arayüzde göster

            // Mesajı UDP üzerinden 8080 portuna gönder
            udpSocket->writeDatagram(receivedData.toUtf8(), QHostAddress::LocalHost, 8080);  // Mesaj tekrar Python'a 8080 portundan gönderiliyor
        });
    }
}



/*
 *
 * #include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , udpCommunicationStopped(false)  // Başlangıçta UDP iletişimi aktif
{
    ui->setupUi(this);

    udpSocket = new QUdpSocket(this);  // QUdpSocket başlatılıyor (UDP üzerinden veri gönderip almak için)

    // Butonları ve işlemleri birbirine bağlayın
    connect(ui->pushButton1, &QPushButton::clicked, this, &MainWindow::sendDataToPython);  // pushButton1'e tıklandığında veri gönderecek fonksiyonu bağla
    connect(ui->pushButton2, &QPushButton::clicked, this, &MainWindow::stopUdpCommunication);  // pushButton2'ye tıklandığında UDP bağlantısını durduracak fonksiyonu bağla
    connect(udpSocket, &QUdpSocket::readyRead, this, &MainWindow::processPendingDatagrams);    // UDP soketi veri geldiğinde 'processPendingDatagrams' fonksiyonunu çalıştıracak

    // 8081 portundan dinlemeye başla
    udpSocket->bind(QHostAddress::LocalHost, 8081);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sendDataToPython()
{
    // Eğer soket kapatıldıysa tekrar bağla
    if (udpCommunicationStopped) {
        // Eski soketi kapat
        udpSocket->abort();  // Mevcut bağlantıyı kesin

        // Yeni bağlantıyı başlat
        udpSocket->bind(QHostAddress::LocalHost, 8081);  // Soketi tekrar bind et
        udpCommunicationStopped = false;  // İletişim aktif
        ui->textBrowser4->append("UDP communication restarted.");
    }

    // Arayüzden girilen verileri textEdit alanlarından al
    int ktsSendPeriod = ui->textEdit1->toPlainText().toInt();  // KTS gönderim periyodu textEdit1'den alınıyor
    int sinusPeriod = ui->textEdit2->toPlainText().toInt();    // Sinüs periyodu textEdit2'den alınıyor

    // UDP mesajı oluşturuluyor ve 8080 portundan Python'a gönderiliyor
    QByteArray datagram = QString("%1,%2").arg(ktsSendPeriod).arg(sinusPeriod).toUtf8();  // Veriler string'e çevrilip UTF-8 formatında byte array'e dönüştürülüyor
    udpSocket->writeDatagram(datagram, QHostAddress::LocalHost, 8080);  // Mesaj localhost üzerinden 8080 portuna gönderiliyor

    // Mesaj arayüzde textBrowser4'e yazdırılıyor
    ui->textBrowser4->append("Sent: " + QString(datagram));  // Gönderilen mesajın arayüzde görünmesi sağlanıyor
}

void MainWindow::stopUdpCommunication()
{
    // UDP bağlantısını kapatmak için soketi durdur
    udpSocket->close();  // QUdpSocket bağlantısı kapatılıyor, artık veri gönderimi/alımı yapılmayacak
    udpCommunicationStopped = true;  // UDP iletişiminin durdurulduğunu belirtiyoruz
    ui->textBrowser4->append("UDP communication stopped.");  // Bağlantının kapatıldığı arayüzde belirtiliyor

    // 8080 portuna "UDP communication stopped." mesajı gönderme
    QUdpSocket udpSocketSender;
    QByteArray message = "UDP communication stopped.";  // Gönderilecek mesaj
    udpSocketSender.writeDatagram(message, QHostAddress::LocalHost, 8080);  // Mesajı 8080 portuna gönder

    // Eğer bir hata varsa, bu hatayı textBrowser4'e yazdır
    if (udpSocketSender.error() != QAbstractSocket::SocketError::UnknownSocketError) {
        ui->textBrowser4->append("Error sending stop message: " + udpSocketSender.errorString());
    }
}

void MainWindow::processPendingDatagrams()
{
    // Gelen UDP mesajlarını işleme al
    while (udpSocket->hasPendingDatagrams()) {  // UDP soketinde bekleyen veri var mı kontrol ediliyor
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());  // Gelen veri boyutu kadar buffer ayarlanıyor
        udpSocket->readDatagram(datagram.data(), datagram.size());  // Veri UDP soketinden okunuyor

        QString receivedData = QString::fromUtf8(datagram);  // Gelen veri UTF-8 formatında string'e dönüştürülüyor
        ui->textBrowser2->append("Received: " + receivedData);  // Gelen mesajı kullanıcıya gösteriyoruz

        // Mesajın işlenme gecikmesini alıyoruz
        int messageDelay = ui->textEdit3->toPlainText().toInt();  // Gecikme süresi textEdit3'da belirtilen değerden alınıyor (milisaniye cinsinden)

        // Mesajı gecikme süresi kadar bekletip sonra Python'a gönderiyoruz
        QTimer::singleShot(messageDelay * 1000, this, [=]() {  // Belirtilen gecikme süresi sonunda mesaj işlenecek (milisaniye cinsinden)
            // Gelen mesajı textBrowser3'te güncellenmiş açı değeri olarak göster
            ui->textBrowser3->append("Güncellenen değer: " + receivedData);  // Gecikme sonrası işlenip gönderilen mesajı arayüzde göster

            // Mesajı UDP üzerinden 8080 portuna gönder
            udpSocket->writeDatagram(receivedData.toUtf8(), QHostAddress::LocalHost, 8080);  // Mesaj tekrar Python'a 8080 portundan gönderiliyor
        });
    }
}




--------------------------------------------------------------------------------





#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    udpSocket = new QUdpSocket(this);  // QUdpSocket başlatılıyor (UDP üzerinden veri gönderip almak için)

    // Butonları ve işlemleri birbirine bağlayın
    connect(ui->pushButton1, &QPushButton::clicked, this, &MainWindow::sendDataToPython);  // pushButton1'e tıklandığında veri gönderecek fonksiyonu bağla
    connect(ui->pushButton2, &QPushButton::clicked, this, &MainWindow::stopUdpCommunication);  // pushButton2'ye tıklandığında UDP bağlantısını durduracak fonksiyonu bağla
    connect(udpSocket, &QUdpSocket::readyRead, this, &MainWindow::processPendingDatagrams);    // UDP soketi veri geldiğinde 'processPendingDatagrams' fonksiyonunu çalıştıracak

    // 8081 portundan dinlemeye başla
    udpSocket->bind(QHostAddress::LocalHost, 8081);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sendDataToPython()
{
    // Arayüzden girilen verileri textEdit alanlarından al
    int ktsSendPeriod = ui->textEdit1->toPlainText().toInt();  // KTS gönderim periyodu textEdit1'den alınıyor
    int sinusPeriod = ui->textEdit2->toPlainText().toInt();    // Sinüs periyodu textEdit2'den alınıyor

    // UDP mesajı oluşturuluyor ve 8080 portundan Python'a gönderiliyor
    QByteArray datagram = QString("%1,%2").arg(ktsSendPeriod).arg(sinusPeriod).toUtf8();  // Veriler string'e çevrilip UTF-8 formatında byte array'e dönüştürülüyor
    udpSocket->writeDatagram(datagram, QHostAddress::LocalHost, 8080);  // Mesaj localhost üzerinden 8080 portuna gönderiliyor

    // Mesaj arayüzde textBrowser4'e yazdırılıyor
    ui->textBrowser4->append("Sent: " + QString(datagram));  // Gönderilen mesajın arayüzde görünmesi sağlanıyor
}

void MainWindow::stopUdpCommunication()
{
    // UDP bağlantısını kapatmak için socket'i durdur
    udpSocket->close();  // QUdpSocket bağlantısı kapatılıyor, artık veri gönderimi/alımı yapılmayacak
    ui->textBrowser4->append("UDP communication stopped.");  // Bağlantının kapatıldığı arayüzde belirtiliyor

    // 8080 portuna "UDP communication stopped." mesajı gönderme
    QUdpSocket udpSocketSender;
    QByteArray message = "UDP communication stopped.";  // Gönderilecek mesaj
    udpSocketSender.writeDatagram(message, QHostAddress::LocalHost, 8080);  // Mesajı 8080 portuna gönder

    // Eğer bir hata varsa, bu hatayı textBrowser4'e yazdır
    if (udpSocketSender.error() != QAbstractSocket::SocketError::UnknownSocketError) {
        ui->textBrowser4->append("Error sending stop message: " + udpSocketSender.errorString());
    }
}


void MainWindow::processPendingDatagrams()
{
    // Gelen UDP mesajlarını işleme al
    while (udpSocket->hasPendingDatagrams()) {  // UDP soketinde bekleyen veri var mı kontrol ediliyor
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());  // Gelen veri boyutu kadar buffer ayarlanıyor
        udpSocket->readDatagram(datagram.data(), datagram.size());  // Veri UDP soketinden okunuyor

        QString receivedData = QString::fromUtf8(datagram);  // Gelen veri UTF-8 formatında string'e dönüştürülüyor
        ui->textBrowser2->append("Received: " + receivedData);  // Gelen mesajı kullanıcıya gösteriyoruz

        // Mesajın işlenme gecikmesini alıyoruz
        int messageDelay = ui->textEdit3->toPlainText().toInt();  // Gecikme süresi textEdit3'da belirtilen değerden alınıyor (saniye cinsinden)

        // Mesajı gecikme süresi kadar bekletip sonra Python'a gönderiyoruz
        QTimer::singleShot(messageDelay * 1000, this, [=]() {  // Belirtilen gecikme süresi sonunda mesaj işlenecek (milisaniye cinsinden)
            // Gelen mesajı textBrowser3'te güncellenmiş açı değeri olarak göster
            ui->textBrowser3->append("güncellenen değer: " + receivedData);  // Gecikme sonrası işlenip gönderilen mesajı arayüzde göster

            // Mesajı UDP üzerinden 8080 portuna gönder
            udpSocket->writeDatagram(receivedData.toUtf8(), QHostAddress::LocalHost, 8080);  // Mesaj tekrar Python'a 8080 portundan gönderiliyor
        });
    }
}

*/
