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

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

#   if !defined( _DEBUG )
        QApplication::setLibraryPaths(QStringList() << QApplication::applicationDirPath());
#   endif

    QApplication::setOrganizationName("Sparadon");
    QApplication::setOrganizationDomain("sparadon.com");
    QApplication::setApplicationName("2Desktop Server");

    QMenu menu;
    QObject::connect
    (
        menu.addAction(QIcon::fromTheme("application-exit"), "Exit 2Desktop"),
        &QAction::triggered,
        [&app](bool) { app.quit(); }
    );

    QSystemTrayIcon tray(QIcon(":/icon.ico"));
    tray.setToolTip(QApplication::applicationName());
    tray.setContextMenu(&menu);
    tray.show();

    QUdpSocket socket;
    socket.bind(QHostAddress::Any, 2711);

    QObject::connect(&socket, &QUdpSocket::readyRead, [&socket]()
    {
        while (socket.hasPendingDatagrams())
        {
            QByteArray datagram;
            datagram.resize(socket.pendingDatagramSize());

            if(socket.readDatagram(datagram.data(), datagram.size()) > 0)
            {
                QUrl url(QString::fromUtf8(datagram));
                qDebug() << url;
                QDesktopServices::openUrl(url);
            }
        }
    });

    return app.exec();
}

