#include <QCoreApplication>

#include <QFileInfo>
#include <QDateTime>
#include <QThread>
#include <QRegExp>
#include <QFile>

#include <unistd.h>
#include <stdio.h>

#include "genericuitest.h"

void __msgOutput(QtMsgType type, const QMessageLogContext &ctx, const QString &msg)
{
    QByteArray streamData;
    {
        QTextStream textStream(&streamData, QIODevice::WriteOnly);
        textStream << '[';
        textStream << QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate);
        textStream << "] [";
        {
            QThread* thread = QThread::currentThread();
            QByteArray name = thread->objectName().toLocal8Bit();
            if(name.isEmpty())
                name = thread->metaObject()->className();
            textStream << name;
            textStream << ' ';
            textStream << thread;
        }
        textStream << "] [";

        switch(type) {
            case QtDebugMsg:
                textStream << "DEBUG";
            break;

            case QtWarningMsg:
                textStream << "WARNING";
            break;

            case QtCriticalMsg:
                textStream << "CRITICAL";
            break;

            case QtFatalMsg:
                textStream << "FATAL";
            break;

            default:
                textStream << "UNKNOWN";
            break;
        }

        textStream << "] ";

        QString func(ctx.function);
        if(func.isEmpty())
            func = "anonymous";
        else {
            QRegExp methodName("[\\s^]([\\w]+[\\w\\d\\-_]*(::[\\w]+[\\w\\d\\-_]*)?)\\(");
            if(methodName.indexIn(ctx.function) > -1)
                func = methodName.cap(1);
        }
        textStream << '[';
        textStream << func;
        textStream << "] ";

        QString file(ctx.file);
        if(file.isEmpty())
            file = "Unknown";

        textStream << file << ':' << ctx.line << '\n';
        textStream << msg << "\n";
    }


    // Ensure messages don't get merged due to threading
    static QFile log(QFileInfo("output.log").absoluteFilePath());
    if(!log.isOpen()) {
        endwin();
        printf("Redirecting log to %s", log.fileName().toLocal8Bit().data());
        fflush(stdout);

        log.open(QFile::WriteOnly);
        sleep(5);
        refresh();
    }
    log.write(streamData);
    log.flush();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qInstallMessageHandler(__msgOutput);
    qDebug() << "Started";

    new GenericUITest();
    return a.exec();
}
