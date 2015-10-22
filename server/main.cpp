#include <QCoreApplication>
#include <QApplication>
#include <QObject>
#include <QString>
#include <QUrl>
#include <QIcon>
#include <QUdpSocket>
#include <QDesktopServices>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QCommandLineParser>
#include <QCommandLineOption>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

#   if !defined( _DEBUG )
        QApplication::setLibraryPaths(QStringList() << QApplication::applicationDirPath());
#   endif

    QApplication::setOrganizationName("Sparadon");
    QApplication::setOrganizationDomain("sparadon.com");
    QApplication::setApplicationName("2Desktop Server");

    QCommandLineOption optPort("port", "", "port");

    QCommandLineParser parser;
    parser.addOptions({optPort});
    parser.parse(QApplication::arguments());

    quint16 port = parser.value(optPort).toUShort() ? parser.value(optPort).toUShort() : 2711;

    QUdpSocket socket;
    while(!socket.bind(QHostAddress::Any, port)) port++;

    QObject::connect(&socket, &QUdpSocket::readyRead, [&socket]()
    {
        while (socket.hasPendingDatagrams())
        {
            QByteArray datagram;
            datagram.resize(socket.pendingDatagramSize());

            if(socket.readDatagram(datagram.data(), datagram.size()) > 0)
            {
                QUrl url(QString::fromUtf8(datagram));
                if(QDesktopServices::openUrl(url)) qInfo() << url;
                else qWarning() << url;
            }
        }
    });

    QMenu menu;

    QObject::connect(menu.addAction("Broadcast"), &QAction::triggered, [port](bool)
    {
        QByteArray datagram = QString("2Desktop").toUtf8();
        QUdpSocket().writeDatagram(datagram.data(), datagram.size(), QHostAddress::Broadcast, port);
    });

    QObject::connect(menu.addAction("Exit"), &QAction::triggered, [&app](bool) { app.quit(); });

    QSystemTrayIcon tray(QIcon(":/icon.ico"));
    tray.setToolTip(QApplication::applicationName() + " on port " + QString::number(port));
    tray.setContextMenu(&menu);
    tray.show();

    return app.exec();
}

