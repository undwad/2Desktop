#include <QCoreApplication>
#include <QApplication>
#include <QObject>
#include <QString>
#include <QUrl>
#include <QIcon>
#include <QWebSocket>
#include <QWebSocketServer>
#include <QNetworkInterface>
#include <QDesktopServices>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStringList>

#include "install.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

#   if !defined( _DEBUG )
        QApplication::setLibraryPaths(QStringList() << QApplication::applicationDirPath());
#   endif

    QApplication::setApplicationName("2Desktop Server");

    QCommandLineOption optPort("port", "", "port");
    QCommandLineOption optInstall("install");

    QCommandLineParser parser;
    parser.addOptions({optPort, optInstall});
    parser.parse(QApplication::arguments());

    if(parser.isSet(optInstall)) return install("--port 2711");
    else
    {
        quint16 port = parser.value(optPort).toUShort() ? parser.value(optPort).toUShort() : 2711;

        QWebSocketServer server("2desktop", QWebSocketServer::NonSecureMode);

        while(!server.listen(QHostAddress::AnyIPv4, port)) port++;

        QObject::connect(&server, &QWebSocketServer::newConnection, [&server]()
        {
            QObject::connect(server.nextPendingConnection(), &QWebSocket::textMessageReceived, [](QString message)
            {
                QUrl url(message);
                if(QDesktopServices::openUrl(url)) qInfo() << url;
                else qWarning() << url;
            });
        });

        QStringList hosts;
        foreach (const QHostAddress &address, QNetworkInterface::allAddresses())
            if (QAbstractSocket::IPv4Protocol == address.protocol() && QHostAddress(QHostAddress::LocalHost) != address)
                 hosts << address.toString();

        QIcon icon(":/icon.ico");
        QSystemTrayIcon tray(icon);

        tray.setToolTip(QString("%1\nHosts: %2\nPort: %3\n")
            .arg(QApplication::applicationName()+ ' ' + __DATE__ + ' ' + __TIME__)
            .arg(hosts.join(','))
            .arg(port));

        QMenu menu;
        QObject::connect(menu.addAction(icon, "Exit"), &QAction::triggered, [&app](bool) { app.quit(); });
        tray.setContextMenu(&menu);
        tray.show();

        return app.exec();
    }
}

