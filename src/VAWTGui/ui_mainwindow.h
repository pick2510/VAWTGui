/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *label;
    QPushButton *btnStart;
    QPushButton *btnClose;
    QCheckBox *chkLogging;
    QLabel *label_3;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *lblDelay;
    QSpinBox *spbDelay;
    QLabel *label_8;
    QTextEdit *txtDisp;
    QPushButton *btnPath;
    QLabel *label_6;
    QCheckBox *chkMaxon;
    QLabel *label_7;
    QCheckBox *chkTorque;
    QLineEdit *txtMayumi;
    QLabel *label_4;
    QLabel *label_5;
    QLineEdit *txtAnemo;
    QPushButton *btnCancel;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(818, 341);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        label = new QLabel(centralwidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(530, 140, 121, 21));
        btnStart = new QPushButton(centralwidget);
        btnStart->setObjectName(QStringLiteral("btnStart"));
        btnStart->setGeometry(QRect(44, 261, 110, 41));
        btnClose = new QPushButton(centralwidget);
        btnClose->setObjectName(QStringLiteral("btnClose"));
        btnClose->setGeometry(QRect(290, 261, 110, 41));
        chkLogging = new QCheckBox(centralwidget);
        chkLogging->setObjectName(QStringLiteral("chkLogging"));
        chkLogging->setGeometry(QRect(174, 20, 85, 21));
        chkLogging->setChecked(true);
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(79, 100, 201, 21));
        layoutWidget = new QWidget(centralwidget);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(32, 33, 121, 50));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        lblDelay = new QLabel(layoutWidget);
        lblDelay->setObjectName(QStringLiteral("lblDelay"));
        lblDelay->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        verticalLayout->addWidget(lblDelay);

        spbDelay = new QSpinBox(layoutWidget);
        spbDelay->setObjectName(QStringLiteral("spbDelay"));
        spbDelay->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        spbDelay->setMinimum(100);
        spbDelay->setMaximum(40000);
        spbDelay->setSingleStep(10);
        spbDelay->setValue(100);

        verticalLayout->addWidget(spbDelay);

        label_8 = new QLabel(centralwidget);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(420, 160, 351, 151));
        label_8->setPixmap(QPixmap(QString::fromUtf8(":/images/HSR.png")));
        label_8->setScaledContents(false);
        txtDisp = new QTextEdit(centralwidget);
        txtDisp->setObjectName(QStringLiteral("txtDisp"));
        txtDisp->setGeometry(QRect(370, 20, 411, 111));
        btnPath = new QPushButton(centralwidget);
        btnPath->setObjectName(QStringLiteral("btnPath"));
        btnPath->setGeometry(QRect(170, 50, 110, 41));
        label_6 = new QLabel(centralwidget);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(30, 128, 121, 20));
        label_6->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        chkMaxon = new QCheckBox(centralwidget);
        chkMaxon->setObjectName(QStringLiteral("chkMaxon"));
        chkMaxon->setGeometry(QRect(160, 130, 16, 16));
        chkMaxon->setChecked(true);
        label_7 = new QLabel(centralwidget);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(49, 149, 101, 20));
        label_7->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        chkTorque = new QCheckBox(centralwidget);
        chkTorque->setObjectName(QStringLiteral("chkTorque"));
        chkTorque->setGeometry(QRect(160, 151, 16, 16));
        chkTorque->setChecked(true);
        txtMayumi = new QLineEdit(centralwidget);
        txtMayumi->setObjectName(QStringLiteral("txtMayumi"));
        txtMayumi->setGeometry(QRect(160, 172, 108, 22));
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(30, 174, 121, 20));
        label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_5 = new QLabel(centralwidget);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(60, 202, 91, 20));
        label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        txtAnemo = new QLineEdit(centralwidget);
        txtAnemo->setObjectName(QStringLiteral("txtAnemo"));
        txtAnemo->setGeometry(QRect(160, 200, 108, 22));
        btnCancel = new QPushButton(centralwidget);
        btnCancel->setObjectName(QStringLiteral("btnCancel"));
        btnCancel->setEnabled(false);
        btnCancel->setGeometry(QRect(169, 260, 110, 41));
        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "VAWT Testfacility", 0));
        label->setText(QApplication::translate("MainWindow", "VAWT Testfacility", 0));
        btnStart->setText(QApplication::translate("MainWindow", "Start", 0));
        btnClose->setText(QApplication::translate("MainWindow", "Close", 0));
        chkLogging->setText(QApplication::translate("MainWindow", "Logging", 0));
        label_3->setText(QApplication::translate("MainWindow", "Peripherals (empty for none)", 0));
        lblDelay->setText(QApplication::translate("MainWindow", "Delay Messloop", 0));
        label_8->setText(QString());
        txtDisp->setHtml(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'.SF NS Text'; font-size:13pt;\"><br /></p></body></html>", 0));
        btnPath->setText(QApplication::translate("MainWindow", "Path", 0));
        label_6->setText(QApplication::translate("MainWindow", " Maxon Encoder", 0));
        chkMaxon->setText(QString());
        label_7->setText(QApplication::translate("MainWindow", "Torque Sensor", 0));
        chkTorque->setText(QString());
        txtMayumi->setText(QApplication::translate("MainWindow", "/dev/mayumi", 0));
        label_4->setText(QApplication::translate("MainWindow", "Mayumi DC Load", 0));
        label_5->setText(QApplication::translate("MainWindow", "Anemometer", 0));
        txtAnemo->setText(QApplication::translate("MainWindow", "/dev/anemo", 0));
        btnCancel->setText(QApplication::translate("MainWindow", "Cancel", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
