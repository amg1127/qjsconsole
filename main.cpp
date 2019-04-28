#include "mainwindow.h"

#include <iostream>

#include <QApplication>
#include <QTextEdit>

QTextEdit* outputText = Q_NULLPTR;

void messageHandlerFunction (QtMsgType, const QMessageLogContext&, const QString&);

int main (int argc, char *argv[]) {
    qInstallMessageHandler (messageHandlerFunction);

    QApplication a(argc, argv);
    MainWindow w;
    outputText = w.outputText ();
    w.show ();
    w.setWindowState (Qt::WindowMaximized | Qt::WindowActive);
    return a.exec();
}

void messageHandlerFunction (QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    QString prefix;
    switch (type) {
    case QtFatalMsg:
        prefix = "FATAL" ; break;
    case QtCriticalMsg:
        prefix = "ERROR" ; break;
    case QtWarningMsg:
        prefix = "WARNING" ; break;
    case QtInfoMsg:
        prefix = "INFO" ; break;
    case QtDebugMsg:
        prefix = "DEBUG" ; break;
    default:
        prefix = "UNKNOWN";
    }
    QString msgLineContext (QString(" (%1:%2%3)").arg((context.file) ? ((context.file[0]) ? context.file : "(unknown)") : "(unknown)").arg((context.line) ? context.line : 0).arg((context.function) ? ((context.function[0]) ? (QString(", %1").arg(context.function)) : "") : ""));
    QString dateTimeFormat ("yyyy-MM-dd'T'HH:mm:ss.zzzt");
    QStringList msgLines (msg.split(QRegExp("[\\r\\n]+"), QString::SkipEmptyParts));
    for (QStringList::const_iterator msgLine = msgLines.constBegin(); msgLine != msgLines.constEnd(); msgLine++) {
        QString msgTransform (*msgLine);
        while ((! msgTransform.isEmpty ()) && msgTransform.right(1).at(0).isSpace()) {
            msgTransform.chop(1);
        }
        if (msgTransform.left(1) == "\"")
            msgTransform.remove (0, 1);
        if (msgTransform.right(1) == "\"")
            msgTransform.chop(1);
        QString msgLineFormatted (QString("[%1] %2: %3").arg(QDateTime::currentDateTime().toString(dateTimeFormat)).arg(prefix).arg(msgTransform.trimmed() + msgLineContext));
        std::cerr << msgLineFormatted.toLocal8Bit().constData() << std::endl;
        if (outputText != Q_NULLPTR) {
            outputText->append (msgLineFormatted);
        }
    }
    if (type == QtFatalMsg) {
        abort ();
    }
}
