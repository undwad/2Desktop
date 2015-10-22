#include <QList>
#include <QApplication>
#include <QThread>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QStandardPaths>
#include <QElapsedTimer>
#include <QDebug>

#include "install.h"

void copyRecursively(const QString& sourceFilePath, const QString& targetFilePath)
{
    QFileInfo sourceFileInfo(sourceFilePath);
    if (sourceFileInfo.isDir())
    {
        QDir targetDir(targetFilePath);
        if (!targetDir.exists())
            if(!targetDir.mkpath(targetFilePath))
            {
                qWarning() << "mkdir" << targetFilePath << "failed";
                return;
            }
        QStringList fileNames = QDir(sourceFilePath).entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
        foreach (const QString &fileName, fileNames)
            copyRecursively(sourceFilePath + '/' + fileName, targetFilePath + '/' + fileName);
    }
    else if(!QFile::copy(sourceFilePath, targetFilePath))
        qWarning() << "copy" << sourceFilePath << "to" << targetFilePath << "failed";
}

int install(int argc, char *argv[])
{
    const QString programDir = perMachineProgramDir();
    if(QDir(programDir) != QDir(QApplication::applicationDirPath()))
    {
        if(isRunningUnderAdmin())
        {
            qDebug() << "INSTALLING";

            const QString exeName = QFileInfo(QApplication::applicationFilePath()).fileName();
            const QString exePath = programDir + '/' + exeName;

            if(QFile(exePath).exists())
            {
                killProcess(exePath);
                QElapsedTimer timer;
                timer.start();
                while(timer.elapsed() < 30000 && !QFile(exePath).remove())
                    QThread::sleep(1);
            }

            copyRecursively(QApplication::applicationDirPath(), programDir);

            createExeLink
            (
                exePath, "", programDir, QApplication::applicationName(),
                QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + '/' + QFileInfo(QApplication::applicationFilePath()).baseName() + ".lnk"
            );

            registerAutoRun(exePath);

            executeCommand(exePath, "", programDir);
        }
        else
        {
            QStringList args = QApplication::arguments();
            args.pop_front();
            executeCommandUnderAdmin(QApplication::applicationFilePath(), args.join(' '), QApplication::applicationDirPath());
        }
    }

    return 0;
}

