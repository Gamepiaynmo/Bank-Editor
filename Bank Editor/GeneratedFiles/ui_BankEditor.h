/********************************************************************************
** Form generated from reading UI file 'BankEditor.ui'
**
** Created by: Qt User Interface Compiler version 5.12.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BANKEDITOR_H
#define UI_BANKEDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BankEditorClass
{
public:
    QAction *action_Open;
    QAction *action_Close;
    QAction *action_Generate;
    QAction *action_Quit;
    QAction *action_AutoPlay;
    QAction *action_Export;
    QAction *action_Clear;
    QAction *action_About;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QTableWidget *table_bank;
    QMenuBar *menuBar;
    QMenu *menu_File;
    QMenu *menu_Edit;
    QMenu *menu_About;
    QToolBar *mainToolBar;

    void setupUi(QMainWindow *BankEditorClass)
    {
        if (BankEditorClass->objectName().isEmpty())
            BankEditorClass->setObjectName(QString::fromUtf8("BankEditorClass"));
        BankEditorClass->resize(965, 630);
        action_Open = new QAction(BankEditorClass);
        action_Open->setObjectName(QString::fromUtf8("action_Open"));
        action_Close = new QAction(BankEditorClass);
        action_Close->setObjectName(QString::fromUtf8("action_Close"));
        action_Generate = new QAction(BankEditorClass);
        action_Generate->setObjectName(QString::fromUtf8("action_Generate"));
        action_Quit = new QAction(BankEditorClass);
        action_Quit->setObjectName(QString::fromUtf8("action_Quit"));
        action_AutoPlay = new QAction(BankEditorClass);
        action_AutoPlay->setObjectName(QString::fromUtf8("action_AutoPlay"));
        action_AutoPlay->setCheckable(true);
        action_AutoPlay->setChecked(true);
        action_Export = new QAction(BankEditorClass);
        action_Export->setObjectName(QString::fromUtf8("action_Export"));
        action_Clear = new QAction(BankEditorClass);
        action_Clear->setObjectName(QString::fromUtf8("action_Clear"));
        action_About = new QAction(BankEditorClass);
        action_About->setObjectName(QString::fromUtf8("action_About"));
        centralWidget = new QWidget(BankEditorClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        table_bank = new QTableWidget(centralWidget);
        if (table_bank->columnCount() < 4)
            table_bank->setColumnCount(4);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        table_bank->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        table_bank->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        table_bank->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        table_bank->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        table_bank->setObjectName(QString::fromUtf8("table_bank"));
        QFont font;
        font.setFamily(QString::fromUtf8("Consolas"));
        table_bank->setFont(font);
        table_bank->setSelectionMode(QAbstractItemView::ExtendedSelection);
        table_bank->setSelectionBehavior(QAbstractItemView::SelectRows);

        verticalLayout->addWidget(table_bank);

        BankEditorClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(BankEditorClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 965, 26));
        menu_File = new QMenu(menuBar);
        menu_File->setObjectName(QString::fromUtf8("menu_File"));
        menu_Edit = new QMenu(menuBar);
        menu_Edit->setObjectName(QString::fromUtf8("menu_Edit"));
        menu_About = new QMenu(menuBar);
        menu_About->setObjectName(QString::fromUtf8("menu_About"));
        BankEditorClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(BankEditorClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        mainToolBar->setMovable(false);
        mainToolBar->setFloatable(false);
        BankEditorClass->addToolBar(Qt::TopToolBarArea, mainToolBar);

        menuBar->addAction(menu_File->menuAction());
        menuBar->addAction(menu_Edit->menuAction());
        menuBar->addAction(menu_About->menuAction());
        menu_File->addAction(action_Open);
        menu_File->addAction(action_Close);
        menu_File->addSeparator();
        menu_File->addAction(action_Generate);
        menu_File->addSeparator();
        menu_File->addAction(action_Quit);
        menu_Edit->addAction(action_AutoPlay);
        menu_Edit->addSeparator();
        menu_Edit->addAction(action_Export);
        menu_Edit->addAction(action_Clear);
        menu_About->addAction(action_About);
        mainToolBar->addAction(action_Open);
        mainToolBar->addAction(action_Generate);
        mainToolBar->addSeparator();
        mainToolBar->addAction(action_AutoPlay);
        mainToolBar->addAction(action_Export);
        mainToolBar->addAction(action_Clear);
        mainToolBar->addSeparator();
        mainToolBar->addAction(action_About);

        retranslateUi(BankEditorClass);

        QMetaObject::connectSlotsByName(BankEditorClass);
    } // setupUi

    void retranslateUi(QMainWindow *BankEditorClass)
    {
        BankEditorClass->setWindowTitle(QApplication::translate("BankEditorClass", "Bank \351\237\263\346\272\220\344\277\256\346\224\271\345\267\245\345\205\267", nullptr));
        action_Open->setText(QApplication::translate("BankEditorClass", "\346\211\223\345\274\200(&O)", nullptr));
#ifndef QT_NO_SHORTCUT
        action_Open->setShortcut(QApplication::translate("BankEditorClass", "Ctrl+O", nullptr));
#endif // QT_NO_SHORTCUT
        action_Close->setText(QApplication::translate("BankEditorClass", "\345\205\263\351\227\255(&C)", nullptr));
#ifndef QT_NO_SHORTCUT
        action_Close->setShortcut(QApplication::translate("BankEditorClass", "Ctrl+F4", nullptr));
#endif // QT_NO_SHORTCUT
        action_Generate->setText(QApplication::translate("BankEditorClass", "\347\224\237\346\210\220(&E)", nullptr));
#ifndef QT_NO_SHORTCUT
        action_Generate->setShortcut(QApplication::translate("BankEditorClass", "Ctrl+E", nullptr));
#endif // QT_NO_SHORTCUT
        action_Quit->setText(QApplication::translate("BankEditorClass", "\351\200\200\345\207\272(&Q)", nullptr));
        action_AutoPlay->setText(QApplication::translate("BankEditorClass", "\350\207\252\345\212\250\346\222\255\346\224\276(&P)", nullptr));
        action_Export->setText(QApplication::translate("BankEditorClass", "\345\257\274\345\207\272\351\200\211\345\256\232\351\241\271(&E)", nullptr));
        action_Clear->setText(QApplication::translate("BankEditorClass", "\346\270\205\351\231\244\351\200\211\345\256\232\351\241\271(&C)", nullptr));
        action_About->setText(QApplication::translate("BankEditorClass", "\345\205\263\344\272\216(&A)", nullptr));
        QTableWidgetItem *___qtablewidgetitem = table_bank->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("BankEditorClass", "\345\220\215\347\247\260", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = table_bank->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("BankEditorClass", "\351\225\277\345\272\246", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = table_bank->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("BankEditorClass", "\346\233\277\346\215\242", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = table_bank->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QApplication::translate("BankEditorClass", "\351\225\277\345\272\246", nullptr));
        menu_File->setTitle(QApplication::translate("BankEditorClass", "\346\226\207\344\273\266(&F)", nullptr));
        menu_Edit->setTitle(QApplication::translate("BankEditorClass", "\347\274\226\350\276\221(&E)", nullptr));
        menu_About->setTitle(QApplication::translate("BankEditorClass", "\345\205\263\344\272\216(&A)", nullptr));
    } // retranslateUi

};

namespace Ui {
    class BankEditorClass: public Ui_BankEditorClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BANKEDITOR_H
