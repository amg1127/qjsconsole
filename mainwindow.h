#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDateTime>
#include <QJSEngine>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJSValue>
#include <QJSValueIterator>
#include <QTextEdit>
#include <QtDebug>
#include <QWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT
private:
    static QJsonValue QJS2QJsonValue (const QJSValue& value);
    static QJsonDocument QJS2QJsonDocument (const QJSValue& value);
    static QString QJS2QString (const QJSValue& value);
    Ui::MainWindow* ui;
    QJSEngine* jsEngine;
public:
    QTextEdit* outputText();
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void btnEvaluate_clicked ();
};

#endif // MAINWINDOW_H
