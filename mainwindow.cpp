#include "mainwindow.h"
#include "ui_mainwindow.h"

QJsonValue MainWindow::QJS2QJsonValue (const QJSValue& value) {
    if (value.isBool ()) {
        return (QJsonValue (value.toBool ()));
    } else if (value.isDate ()) {
        // https://bugreports.qt.io/browse/QTBUG-59235
#if (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
        return (QJsonValue (value.toDateTime().toString(Qt::ISODateWithMs)));
#else
        return (QJsonValue (value.toDateTime().toString(Qt::ISODate)));
#endif
    } else if (value.isNull ()) {
        return (QJsonValue (QJsonValue::Null));
    } else if (value.isNumber ()) {
        return (QJsonValue (value.toNumber()));
    } else if (value.isString ()) {
        return (QJsonValue (value.toString()));
    } else if (value.isArray ()) {
        QJsonArray jsonArray;
        int length = value.property("length").toInt ();
        for (int i = 0; i < length; i++) {
            jsonArray.append (MainWindow::QJS2QJsonValue (value.property (i)));
        }
        return (jsonArray);
    } else if (value.isObject ()) {
        QJsonObject jsonObject;
        QJSValueIterator i (value);
        while (i.hasNext ()) {
            i.next ();
            jsonObject.insert (i.name(), MainWindow::QJS2QJsonValue (i.value ()));
        }
        return (jsonObject);
    } else {
        return (QJsonValue (QJsonValue::Undefined));
    }
}

QJsonDocument MainWindow::QJS2QJsonDocument (const QJSValue& value) {
    if (value.isArray ()) {
        return (QJsonDocument (MainWindow::QJS2QJsonValue(value).toArray ()));
    } else if (value.isObject ()) {
        return (QJsonDocument (MainWindow::QJS2QJsonValue(value).toObject ()));
    } else {
        return (QJsonDocument ());
    }
}

QString MainWindow::QJS2QString (const QJSValue& value) {
    if (value.isBool ()) {
        if (value.toBool ()) {
            return ("true");
        } else {
            return ("false");
        }
    } else if (value.isCallable ()) {
        return ("function () { /* Native code */ }");
    } else if (value.isDate ()) {
        // https://bugreports.qt.io/browse/QTBUG-59235
#if (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
        return (value.toDateTime().toString(Qt::ISODateWithMs));
#else
        return (value.toDateTime().toString(Qt::ISODate));
#endif
    } else if (value.isNull ()) {
        return ("null");
    } else if (value.isNumber ()) {
        return (QString::number (value.toNumber()));
    } else if (value.isRegExp ()) {
        QRegExp regExp (value.toVariant().toRegExp());
        return (QString("QRegExp: CaseSensitivity=%1 , PatternSyntax=%2 , Pattern='%3'").arg(regExp.caseSensitivity()).arg(regExp.patternSyntax()).arg(regExp.pattern()));
    } else if (value.isString ()) {
        return (value.toString());
    } else if (value.isUndefined ()) {
        return ("undefined");
    } else {
        QJsonDocument jsonDocument (MainWindow::QJS2QJsonDocument(value));
        if (jsonDocument.isNull ()) {
            return ("(conversion error)");
        } else {
            return (QString::fromUtf8(jsonDocument.toJson(QJsonDocument::Compact)));
        }
    }
}

QTextEdit* MainWindow::outputText () {
    return (this->ui->txtOutput);
}

MainWindow::MainWindow (QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow),
    jsEngine (new QJSEngine (this)) {
    ui->setupUi(this);
    QObject::connect (this->ui->btnEvaluate, &QPushButton::clicked, this, &MainWindow::btnEvaluate_clicked);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::btnEvaluate_clicked () {
    QJSValue jsValue = this->jsEngine->evaluate (this->ui->txtCode->toPlainText(), this->windowTitle());
    if (jsValue.isError ()) {
        qInfo() << QString("Javascript exception at %1:%2: %3: %4")
            .arg(jsValue.property("fileName").toString())
            .arg(jsValue.property("lineNumber").toInt())
            .arg(jsValue.property("name").toString())
            .arg(jsValue.property("message").toString());
        qDebug() << jsValue.property("stack").toString().toLocal8Bit().constData();
    } else if (! jsValue.isUndefined ()) {
        QString strValue;
        if (jsValue.isArray() || jsValue.isObject()) {
            strValue = QString::fromUtf8(MainWindow::QJS2QJsonDocument(jsValue).toJson(QJsonDocument::Compact));
        } else {
            strValue = jsValue.toString();
        }
        if (jsValue.isArray()) {
            qDebug() << QString("Evaluation returned an Array: '%1'").arg(strValue);
        } else if (jsValue.isObject()) {
            qDebug() << QString("Evaluation returned an Object of class '%1': '%2'").arg(jsValue.property("constructor").property("name").toString()).arg(strValue);
        } else {
            qDebug() << QString("Evaluation returned: '%1'").arg(strValue);
        }
    }
    this->ui->txtCode->setFocus();
}
