/********************************************************************************
** Form generated from reading UI file 'mainupdatewindow.ui'
**
** Created by: Qt User Interface Compiler version 5.0.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINUPDATEWINDOW_H
#define UI_MAINUPDATEWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainUpdateWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QLabel *ProgressLabel;
    QProgressBar *DownloadProgress;
    QLabel *label;
    QPushButton *SkipUpdaterButton;

    void setupUi(QMainWindow *MainUpdateWindow)
    {
        if (MainUpdateWindow->objectName().isEmpty())
            MainUpdateWindow->setObjectName(QStringLiteral("MainUpdateWindow"));
        MainUpdateWindow->resize(461, 106);
        centralwidget = new QWidget(MainUpdateWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        ProgressLabel = new QLabel(centralwidget);
        ProgressLabel->setObjectName(QStringLiteral("ProgressLabel"));

        verticalLayout->addWidget(ProgressLabel);

        DownloadProgress = new QProgressBar(centralwidget);
        DownloadProgress->setObjectName(QStringLiteral("DownloadProgress"));
        DownloadProgress->setContextMenuPolicy(Qt::PreventContextMenu);
        DownloadProgress->setValue(100);
        DownloadProgress->setAlignment(Qt::AlignCenter);
        DownloadProgress->setInvertedAppearance(false);

        verticalLayout->addWidget(DownloadProgress);

        label = new QLabel(centralwidget);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout->addWidget(label);

        SkipUpdaterButton = new QPushButton(centralwidget);
        SkipUpdaterButton->setObjectName(QStringLiteral("SkipUpdaterButton"));

        verticalLayout->addWidget(SkipUpdaterButton);

        MainUpdateWindow->setCentralWidget(centralwidget);

        retranslateUi(MainUpdateWindow);

        QMetaObject::connectSlotsByName(MainUpdateWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainUpdateWindow)
    {
        MainUpdateWindow->setWindowTitle(QApplication::translate("MainUpdateWindow", "ABR Updater", 0));
        ProgressLabel->setText(QApplication::translate("MainUpdateWindow", "Checking for Updates ...", 0));
        label->setText(QApplication::translate("MainUpdateWindow", "Note: If the progress bar % goes blank DO NOT SKIP, it is still downloading the files", 0));
        SkipUpdaterButton->setText(QApplication::translate("MainUpdateWindow", "Skip", 0));
    } // retranslateUi

};

namespace Ui {
    class MainUpdateWindow: public Ui_MainUpdateWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINUPDATEWINDOW_H
