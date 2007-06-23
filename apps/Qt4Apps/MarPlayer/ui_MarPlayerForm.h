/********************************************************************************
** Form generated from reading ui file 'MarPlayerForm.ui'
**
** Created: Sat 23. Jun 10:51:12 2007
**      by: Qt User Interface Compiler version 4.3.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_MARPLAYERFORM_H
#define UI_MARPLAYERFORM_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QTimeEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_MarPlayerForm
{
public:
    QWidget *centralWidget;
    QVBoxLayout *vboxLayout;
    QGridLayout *gridLayout;
    QSlider *gainSlider;
    QComboBox *fileComboBox;
    QTimeEdit *posTimeEdit;
    QTimeEdit *durTimeEdit;
    QLabel *MarPlayerFormBanner_2_2_2;
    QHBoxLayout *hboxLayout;
    QPushButton *playButton;
    QPushButton *pauseButton;
    QPushButton *prevButton;
    QPushButton *nextButton;
    QSpacerItem *spacerItem;
    QSlider *positionSlider;

    void setupUi(QMainWindow *MarPlayerForm)
    {
    if (MarPlayerForm->objectName().isEmpty())
        MarPlayerForm->setObjectName(QString::fromUtf8("MarPlayerForm"));
    QSize size(517, 116);
    size = size.expandedTo(MarPlayerForm->minimumSizeHint());
    MarPlayerForm->resize(size);
    QPalette palette;
    QBrush brush(QColor(128, 128, 128, 255));
    brush.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
    QBrush brush1(QColor(207, 207, 207, 255));
    brush1.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::Button, brush1);
    QBrush brush2(QColor(255, 255, 255, 255));
    brush2.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::Light, brush2);
    QBrush brush3(QColor(238, 238, 238, 255));
    brush3.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::Midlight, brush3);
    QBrush brush4(QColor(103, 103, 103, 255));
    brush4.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::Dark, brush4);
    QBrush brush5(QColor(138, 138, 138, 255));
    brush5.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::Mid, brush5);
    palette.setBrush(QPalette::Active, QPalette::Text, brush);
    palette.setBrush(QPalette::Active, QPalette::BrightText, brush2);
    palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
    QBrush brush6(QColor(0, 0, 0, 255));
    brush6.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::Base, brush6);
    palette.setBrush(QPalette::Active, QPalette::Window, brush6);
    palette.setBrush(QPalette::Active, QPalette::Shadow, brush6);
    QBrush brush7(QColor(121, 125, 121, 255));
    brush7.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::Highlight, brush7);
    palette.setBrush(QPalette::Active, QPalette::HighlightedText, brush1);
    QBrush brush8(QColor(0, 0, 255, 255));
    brush8.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::Link, brush8);
    QBrush brush9(QColor(255, 0, 255, 255));
    brush9.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::LinkVisited, brush9);
    QBrush brush10(QColor(231, 231, 231, 255));
    brush10.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::AlternateBase, brush10);
    palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
    palette.setBrush(QPalette::Inactive, QPalette::Button, brush1);
    palette.setBrush(QPalette::Inactive, QPalette::Light, brush2);
    palette.setBrush(QPalette::Inactive, QPalette::Midlight, brush3);
    palette.setBrush(QPalette::Inactive, QPalette::Dark, brush4);
    palette.setBrush(QPalette::Inactive, QPalette::Mid, brush5);
    palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
    palette.setBrush(QPalette::Inactive, QPalette::BrightText, brush2);
    palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
    palette.setBrush(QPalette::Inactive, QPalette::Base, brush6);
    palette.setBrush(QPalette::Inactive, QPalette::Window, brush6);
    palette.setBrush(QPalette::Inactive, QPalette::Shadow, brush6);
    palette.setBrush(QPalette::Inactive, QPalette::Highlight, brush7);
    palette.setBrush(QPalette::Inactive, QPalette::HighlightedText, brush1);
    palette.setBrush(QPalette::Inactive, QPalette::Link, brush8);
    palette.setBrush(QPalette::Inactive, QPalette::LinkVisited, brush9);
    palette.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush10);
    palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush);
    palette.setBrush(QPalette::Disabled, QPalette::Button, brush1);
    palette.setBrush(QPalette::Disabled, QPalette::Light, brush2);
    palette.setBrush(QPalette::Disabled, QPalette::Midlight, brush3);
    palette.setBrush(QPalette::Disabled, QPalette::Dark, brush4);
    palette.setBrush(QPalette::Disabled, QPalette::Mid, brush5);
    palette.setBrush(QPalette::Disabled, QPalette::Text, brush);
    palette.setBrush(QPalette::Disabled, QPalette::BrightText, brush2);
    palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush);
    palette.setBrush(QPalette::Disabled, QPalette::Base, brush6);
    palette.setBrush(QPalette::Disabled, QPalette::Window, brush6);
    palette.setBrush(QPalette::Disabled, QPalette::Shadow, brush6);
    palette.setBrush(QPalette::Disabled, QPalette::Highlight, brush7);
    palette.setBrush(QPalette::Disabled, QPalette::HighlightedText, brush1);
    palette.setBrush(QPalette::Disabled, QPalette::Link, brush8);
    palette.setBrush(QPalette::Disabled, QPalette::LinkVisited, brush9);
    palette.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush10);
    MarPlayerForm->setPalette(palette);
    centralWidget = new QWidget(MarPlayerForm);
    centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
    vboxLayout = new QVBoxLayout(centralWidget);
#ifndef Q_OS_MAC
    vboxLayout->setSpacing(6);
#endif
    vboxLayout->setMargin(8);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    gridLayout = new QGridLayout();
#ifndef Q_OS_MAC
    gridLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
    gridLayout->setMargin(0);
#endif
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    gainSlider = new QSlider(centralWidget);
    gainSlider->setObjectName(QString::fromUtf8("gainSlider"));
    QPalette palette1;
    palette1.setBrush(QPalette::Active, QPalette::WindowText, brush6);
    QBrush brush11(QColor(255, 170, 0, 255));
    brush11.setStyle(Qt::SolidPattern);
    palette1.setBrush(QPalette::Active, QPalette::Button, brush11);
    QBrush brush12(QColor(255, 213, 127, 255));
    brush12.setStyle(Qt::SolidPattern);
    palette1.setBrush(QPalette::Active, QPalette::Light, brush12);
    QBrush brush13(QColor(255, 191, 63, 255));
    brush13.setStyle(Qt::SolidPattern);
    palette1.setBrush(QPalette::Active, QPalette::Midlight, brush13);
    QBrush brush14(QColor(127, 85, 0, 255));
    brush14.setStyle(Qt::SolidPattern);
    palette1.setBrush(QPalette::Active, QPalette::Dark, brush14);
    QBrush brush15(QColor(170, 113, 0, 255));
    brush15.setStyle(Qt::SolidPattern);
    palette1.setBrush(QPalette::Active, QPalette::Mid, brush15);
    palette1.setBrush(QPalette::Active, QPalette::Text, brush6);
    palette1.setBrush(QPalette::Active, QPalette::BrightText, brush2);
    palette1.setBrush(QPalette::Active, QPalette::ButtonText, brush6);
    palette1.setBrush(QPalette::Active, QPalette::Base, brush2);
    palette1.setBrush(QPalette::Active, QPalette::Window, brush11);
    palette1.setBrush(QPalette::Active, QPalette::Shadow, brush6);
    palette1.setBrush(QPalette::Active, QPalette::Highlight, brush7);
    palette1.setBrush(QPalette::Active, QPalette::HighlightedText, brush1);
    palette1.setBrush(QPalette::Active, QPalette::Link, brush8);
    palette1.setBrush(QPalette::Active, QPalette::LinkVisited, brush9);
    palette1.setBrush(QPalette::Active, QPalette::AlternateBase, brush13);
    palette1.setBrush(QPalette::Inactive, QPalette::WindowText, brush6);
    palette1.setBrush(QPalette::Inactive, QPalette::Button, brush11);
    palette1.setBrush(QPalette::Inactive, QPalette::Light, brush12);
    palette1.setBrush(QPalette::Inactive, QPalette::Midlight, brush13);
    palette1.setBrush(QPalette::Inactive, QPalette::Dark, brush14);
    palette1.setBrush(QPalette::Inactive, QPalette::Mid, brush15);
    palette1.setBrush(QPalette::Inactive, QPalette::Text, brush6);
    palette1.setBrush(QPalette::Inactive, QPalette::BrightText, brush2);
    palette1.setBrush(QPalette::Inactive, QPalette::ButtonText, brush6);
    palette1.setBrush(QPalette::Inactive, QPalette::Base, brush2);
    palette1.setBrush(QPalette::Inactive, QPalette::Window, brush11);
    palette1.setBrush(QPalette::Inactive, QPalette::Shadow, brush6);
    palette1.setBrush(QPalette::Inactive, QPalette::Highlight, brush7);
    palette1.setBrush(QPalette::Inactive, QPalette::HighlightedText, brush1);
    palette1.setBrush(QPalette::Inactive, QPalette::Link, brush8);
    palette1.setBrush(QPalette::Inactive, QPalette::LinkVisited, brush9);
    palette1.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush13);
    palette1.setBrush(QPalette::Disabled, QPalette::WindowText, brush14);
    palette1.setBrush(QPalette::Disabled, QPalette::Button, brush11);
    palette1.setBrush(QPalette::Disabled, QPalette::Light, brush12);
    palette1.setBrush(QPalette::Disabled, QPalette::Midlight, brush13);
    palette1.setBrush(QPalette::Disabled, QPalette::Dark, brush14);
    palette1.setBrush(QPalette::Disabled, QPalette::Mid, brush15);
    palette1.setBrush(QPalette::Disabled, QPalette::Text, brush14);
    palette1.setBrush(QPalette::Disabled, QPalette::BrightText, brush2);
    palette1.setBrush(QPalette::Disabled, QPalette::ButtonText, brush14);
    palette1.setBrush(QPalette::Disabled, QPalette::Base, brush11);
    palette1.setBrush(QPalette::Disabled, QPalette::Window, brush11);
    palette1.setBrush(QPalette::Disabled, QPalette::Shadow, brush6);
    palette1.setBrush(QPalette::Disabled, QPalette::Highlight, brush7);
    palette1.setBrush(QPalette::Disabled, QPalette::HighlightedText, brush1);
    palette1.setBrush(QPalette::Disabled, QPalette::Link, brush8);
    palette1.setBrush(QPalette::Disabled, QPalette::LinkVisited, brush9);
    palette1.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush13);
    gainSlider->setPalette(palette1);
    gainSlider->setSliderPosition(99);
    gainSlider->setOrientation(Qt::Vertical);

    gridLayout->addWidget(gainSlider, 0, 3, 3, 1);

    fileComboBox = new QComboBox(centralWidget);
    fileComboBox->setObjectName(QString::fromUtf8("fileComboBox"));
    QPalette palette2;
    palette2.setBrush(QPalette::Active, QPalette::WindowText, brush);
    palette2.setBrush(QPalette::Active, QPalette::Button, brush11);
    palette2.setBrush(QPalette::Active, QPalette::Light, brush2);
    palette2.setBrush(QPalette::Active, QPalette::Midlight, brush3);
    palette2.setBrush(QPalette::Active, QPalette::Dark, brush4);
    palette2.setBrush(QPalette::Active, QPalette::Mid, brush5);
    palette2.setBrush(QPalette::Active, QPalette::Text, brush);
    palette2.setBrush(QPalette::Active, QPalette::BrightText, brush2);
    palette2.setBrush(QPalette::Active, QPalette::ButtonText, brush);
    palette2.setBrush(QPalette::Active, QPalette::Base, brush6);
    palette2.setBrush(QPalette::Active, QPalette::Window, brush6);
    palette2.setBrush(QPalette::Active, QPalette::Shadow, brush6);
    palette2.setBrush(QPalette::Active, QPalette::Highlight, brush7);
    palette2.setBrush(QPalette::Active, QPalette::HighlightedText, brush1);
    palette2.setBrush(QPalette::Active, QPalette::Link, brush8);
    palette2.setBrush(QPalette::Active, QPalette::LinkVisited, brush9);
    palette2.setBrush(QPalette::Active, QPalette::AlternateBase, brush10);
    palette2.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
    palette2.setBrush(QPalette::Inactive, QPalette::Button, brush11);
    palette2.setBrush(QPalette::Inactive, QPalette::Light, brush2);
    palette2.setBrush(QPalette::Inactive, QPalette::Midlight, brush3);
    palette2.setBrush(QPalette::Inactive, QPalette::Dark, brush4);
    palette2.setBrush(QPalette::Inactive, QPalette::Mid, brush5);
    palette2.setBrush(QPalette::Inactive, QPalette::Text, brush);
    palette2.setBrush(QPalette::Inactive, QPalette::BrightText, brush2);
    palette2.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
    palette2.setBrush(QPalette::Inactive, QPalette::Base, brush6);
    palette2.setBrush(QPalette::Inactive, QPalette::Window, brush6);
    palette2.setBrush(QPalette::Inactive, QPalette::Shadow, brush6);
    palette2.setBrush(QPalette::Inactive, QPalette::Highlight, brush7);
    palette2.setBrush(QPalette::Inactive, QPalette::HighlightedText, brush1);
    palette2.setBrush(QPalette::Inactive, QPalette::Link, brush8);
    palette2.setBrush(QPalette::Inactive, QPalette::LinkVisited, brush9);
    palette2.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush10);
    palette2.setBrush(QPalette::Disabled, QPalette::WindowText, brush);
    palette2.setBrush(QPalette::Disabled, QPalette::Button, brush11);
    palette2.setBrush(QPalette::Disabled, QPalette::Light, brush2);
    palette2.setBrush(QPalette::Disabled, QPalette::Midlight, brush3);
    palette2.setBrush(QPalette::Disabled, QPalette::Dark, brush4);
    palette2.setBrush(QPalette::Disabled, QPalette::Mid, brush5);
    palette2.setBrush(QPalette::Disabled, QPalette::Text, brush);
    palette2.setBrush(QPalette::Disabled, QPalette::BrightText, brush2);
    palette2.setBrush(QPalette::Disabled, QPalette::ButtonText, brush);
    palette2.setBrush(QPalette::Disabled, QPalette::Base, brush6);
    palette2.setBrush(QPalette::Disabled, QPalette::Window, brush6);
    palette2.setBrush(QPalette::Disabled, QPalette::Shadow, brush6);
    palette2.setBrush(QPalette::Disabled, QPalette::Highlight, brush7);
    palette2.setBrush(QPalette::Disabled, QPalette::HighlightedText, brush1);
    palette2.setBrush(QPalette::Disabled, QPalette::Link, brush8);
    palette2.setBrush(QPalette::Disabled, QPalette::LinkVisited, brush9);
    palette2.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush10);
    fileComboBox->setPalette(palette2);

    gridLayout->addWidget(fileComboBox, 1, 1, 1, 1);

    posTimeEdit = new QTimeEdit(centralWidget);
    posTimeEdit->setObjectName(QString::fromUtf8("posTimeEdit"));
    QPalette palette3;
    palette3.setBrush(QPalette::Active, QPalette::WindowText, brush);
    palette3.setBrush(QPalette::Active, QPalette::Button, brush11);
    palette3.setBrush(QPalette::Active, QPalette::Light, brush2);
    palette3.setBrush(QPalette::Active, QPalette::Midlight, brush3);
    palette3.setBrush(QPalette::Active, QPalette::Dark, brush4);
    palette3.setBrush(QPalette::Active, QPalette::Mid, brush5);
    palette3.setBrush(QPalette::Active, QPalette::Text, brush11);
    palette3.setBrush(QPalette::Active, QPalette::BrightText, brush2);
    palette3.setBrush(QPalette::Active, QPalette::ButtonText, brush);
    palette3.setBrush(QPalette::Active, QPalette::Base, brush6);
    palette3.setBrush(QPalette::Active, QPalette::Window, brush6);
    palette3.setBrush(QPalette::Active, QPalette::Shadow, brush6);
    palette3.setBrush(QPalette::Active, QPalette::Highlight, brush7);
    palette3.setBrush(QPalette::Active, QPalette::HighlightedText, brush1);
    palette3.setBrush(QPalette::Active, QPalette::Link, brush8);
    palette3.setBrush(QPalette::Active, QPalette::LinkVisited, brush9);
    palette3.setBrush(QPalette::Active, QPalette::AlternateBase, brush10);
    palette3.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
    palette3.setBrush(QPalette::Inactive, QPalette::Button, brush11);
    palette3.setBrush(QPalette::Inactive, QPalette::Light, brush2);
    palette3.setBrush(QPalette::Inactive, QPalette::Midlight, brush3);
    palette3.setBrush(QPalette::Inactive, QPalette::Dark, brush4);
    palette3.setBrush(QPalette::Inactive, QPalette::Mid, brush5);
    palette3.setBrush(QPalette::Inactive, QPalette::Text, brush11);
    palette3.setBrush(QPalette::Inactive, QPalette::BrightText, brush2);
    palette3.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
    palette3.setBrush(QPalette::Inactive, QPalette::Base, brush6);
    palette3.setBrush(QPalette::Inactive, QPalette::Window, brush6);
    palette3.setBrush(QPalette::Inactive, QPalette::Shadow, brush6);
    palette3.setBrush(QPalette::Inactive, QPalette::Highlight, brush7);
    palette3.setBrush(QPalette::Inactive, QPalette::HighlightedText, brush1);
    palette3.setBrush(QPalette::Inactive, QPalette::Link, brush8);
    palette3.setBrush(QPalette::Inactive, QPalette::LinkVisited, brush9);
    palette3.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush10);
    palette3.setBrush(QPalette::Disabled, QPalette::WindowText, brush);
    palette3.setBrush(QPalette::Disabled, QPalette::Button, brush11);
    palette3.setBrush(QPalette::Disabled, QPalette::Light, brush2);
    palette3.setBrush(QPalette::Disabled, QPalette::Midlight, brush3);
    palette3.setBrush(QPalette::Disabled, QPalette::Dark, brush4);
    palette3.setBrush(QPalette::Disabled, QPalette::Mid, brush5);
    palette3.setBrush(QPalette::Disabled, QPalette::Text, brush);
    palette3.setBrush(QPalette::Disabled, QPalette::BrightText, brush2);
    palette3.setBrush(QPalette::Disabled, QPalette::ButtonText, brush);
    palette3.setBrush(QPalette::Disabled, QPalette::Base, brush6);
    palette3.setBrush(QPalette::Disabled, QPalette::Window, brush6);
    palette3.setBrush(QPalette::Disabled, QPalette::Shadow, brush6);
    palette3.setBrush(QPalette::Disabled, QPalette::Highlight, brush7);
    palette3.setBrush(QPalette::Disabled, QPalette::HighlightedText, brush1);
    palette3.setBrush(QPalette::Disabled, QPalette::Link, brush8);
    palette3.setBrush(QPalette::Disabled, QPalette::LinkVisited, brush9);
    palette3.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush10);
    posTimeEdit->setPalette(palette3);
    posTimeEdit->setFrame(false);
    posTimeEdit->setCurrentSection(QDateTimeEdit::HourSection);

    gridLayout->addWidget(posTimeEdit, 2, 2, 1, 1);

    durTimeEdit = new QTimeEdit(centralWidget);
    durTimeEdit->setObjectName(QString::fromUtf8("durTimeEdit"));
    QPalette palette4;
    palette4.setBrush(QPalette::Active, QPalette::WindowText, brush);
    palette4.setBrush(QPalette::Active, QPalette::Button, brush11);
    palette4.setBrush(QPalette::Active, QPalette::Light, brush2);
    palette4.setBrush(QPalette::Active, QPalette::Midlight, brush3);
    palette4.setBrush(QPalette::Active, QPalette::Dark, brush4);
    palette4.setBrush(QPalette::Active, QPalette::Mid, brush5);
    palette4.setBrush(QPalette::Active, QPalette::Text, brush11);
    palette4.setBrush(QPalette::Active, QPalette::BrightText, brush2);
    palette4.setBrush(QPalette::Active, QPalette::ButtonText, brush);
    palette4.setBrush(QPalette::Active, QPalette::Base, brush6);
    palette4.setBrush(QPalette::Active, QPalette::Window, brush6);
    palette4.setBrush(QPalette::Active, QPalette::Shadow, brush6);
    palette4.setBrush(QPalette::Active, QPalette::Highlight, brush7);
    palette4.setBrush(QPalette::Active, QPalette::HighlightedText, brush1);
    palette4.setBrush(QPalette::Active, QPalette::Link, brush8);
    palette4.setBrush(QPalette::Active, QPalette::LinkVisited, brush9);
    palette4.setBrush(QPalette::Active, QPalette::AlternateBase, brush10);
    palette4.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
    palette4.setBrush(QPalette::Inactive, QPalette::Button, brush11);
    palette4.setBrush(QPalette::Inactive, QPalette::Light, brush2);
    palette4.setBrush(QPalette::Inactive, QPalette::Midlight, brush3);
    palette4.setBrush(QPalette::Inactive, QPalette::Dark, brush4);
    palette4.setBrush(QPalette::Inactive, QPalette::Mid, brush5);
    palette4.setBrush(QPalette::Inactive, QPalette::Text, brush11);
    palette4.setBrush(QPalette::Inactive, QPalette::BrightText, brush2);
    palette4.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
    palette4.setBrush(QPalette::Inactive, QPalette::Base, brush6);
    palette4.setBrush(QPalette::Inactive, QPalette::Window, brush6);
    palette4.setBrush(QPalette::Inactive, QPalette::Shadow, brush6);
    palette4.setBrush(QPalette::Inactive, QPalette::Highlight, brush7);
    palette4.setBrush(QPalette::Inactive, QPalette::HighlightedText, brush1);
    palette4.setBrush(QPalette::Inactive, QPalette::Link, brush8);
    palette4.setBrush(QPalette::Inactive, QPalette::LinkVisited, brush9);
    palette4.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush10);
    palette4.setBrush(QPalette::Disabled, QPalette::WindowText, brush);
    palette4.setBrush(QPalette::Disabled, QPalette::Button, brush11);
    palette4.setBrush(QPalette::Disabled, QPalette::Light, brush2);
    palette4.setBrush(QPalette::Disabled, QPalette::Midlight, brush3);
    palette4.setBrush(QPalette::Disabled, QPalette::Dark, brush4);
    palette4.setBrush(QPalette::Disabled, QPalette::Mid, brush5);
    palette4.setBrush(QPalette::Disabled, QPalette::Text, brush);
    palette4.setBrush(QPalette::Disabled, QPalette::BrightText, brush2);
    palette4.setBrush(QPalette::Disabled, QPalette::ButtonText, brush);
    palette4.setBrush(QPalette::Disabled, QPalette::Base, brush6);
    palette4.setBrush(QPalette::Disabled, QPalette::Window, brush6);
    palette4.setBrush(QPalette::Disabled, QPalette::Shadow, brush6);
    palette4.setBrush(QPalette::Disabled, QPalette::Highlight, brush7);
    palette4.setBrush(QPalette::Disabled, QPalette::HighlightedText, brush1);
    palette4.setBrush(QPalette::Disabled, QPalette::Link, brush8);
    palette4.setBrush(QPalette::Disabled, QPalette::LinkVisited, brush9);
    palette4.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush10);
    durTimeEdit->setPalette(palette4);
    durTimeEdit->setFrame(false);
    durTimeEdit->setReadOnly(false);
    durTimeEdit->setCurrentSection(QDateTimeEdit::HourSection);

    gridLayout->addWidget(durTimeEdit, 1, 2, 1, 1);

    MarPlayerFormBanner_2_2_2 = new QLabel(centralWidget);
    MarPlayerFormBanner_2_2_2->setObjectName(QString::fromUtf8("MarPlayerFormBanner_2_2_2"));
    MarPlayerFormBanner_2_2_2->setPixmap(QPixmap(QString::fromUtf8(":/images/marsyasBanner.png")));

    gridLayout->addWidget(MarPlayerFormBanner_2_2_2, 1, 0, 2, 1);

    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(4);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    playButton = new QPushButton(centralWidget);
    playButton->setObjectName(QString::fromUtf8("playButton"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy.setHorizontalStretch(2);
    sizePolicy.setVerticalStretch(2);
    sizePolicy.setHeightForWidth(playButton->sizePolicy().hasHeightForWidth());
    playButton->setSizePolicy(sizePolicy);
    playButton->setMinimumSize(QSize(45, 27));
    QPalette palette5;
    palette5.setBrush(QPalette::Active, QPalette::WindowText, brush);
    palette5.setBrush(QPalette::Active, QPalette::Button, brush11);
    palette5.setBrush(QPalette::Active, QPalette::Light, brush2);
    palette5.setBrush(QPalette::Active, QPalette::Midlight, brush3);
    palette5.setBrush(QPalette::Active, QPalette::Dark, brush4);
    palette5.setBrush(QPalette::Active, QPalette::Mid, brush5);
    palette5.setBrush(QPalette::Active, QPalette::Text, brush);
    palette5.setBrush(QPalette::Active, QPalette::BrightText, brush2);
    palette5.setBrush(QPalette::Active, QPalette::ButtonText, brush);
    palette5.setBrush(QPalette::Active, QPalette::Base, brush6);
    palette5.setBrush(QPalette::Active, QPalette::Window, brush6);
    palette5.setBrush(QPalette::Active, QPalette::Shadow, brush6);
    palette5.setBrush(QPalette::Active, QPalette::Highlight, brush7);
    palette5.setBrush(QPalette::Active, QPalette::HighlightedText, brush1);
    palette5.setBrush(QPalette::Active, QPalette::Link, brush8);
    palette5.setBrush(QPalette::Active, QPalette::LinkVisited, brush9);
    palette5.setBrush(QPalette::Active, QPalette::AlternateBase, brush10);
    palette5.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
    palette5.setBrush(QPalette::Inactive, QPalette::Button, brush11);
    palette5.setBrush(QPalette::Inactive, QPalette::Light, brush2);
    palette5.setBrush(QPalette::Inactive, QPalette::Midlight, brush3);
    palette5.setBrush(QPalette::Inactive, QPalette::Dark, brush4);
    palette5.setBrush(QPalette::Inactive, QPalette::Mid, brush5);
    palette5.setBrush(QPalette::Inactive, QPalette::Text, brush);
    palette5.setBrush(QPalette::Inactive, QPalette::BrightText, brush2);
    palette5.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
    palette5.setBrush(QPalette::Inactive, QPalette::Base, brush6);
    palette5.setBrush(QPalette::Inactive, QPalette::Window, brush6);
    palette5.setBrush(QPalette::Inactive, QPalette::Shadow, brush6);
    palette5.setBrush(QPalette::Inactive, QPalette::Highlight, brush7);
    palette5.setBrush(QPalette::Inactive, QPalette::HighlightedText, brush1);
    palette5.setBrush(QPalette::Inactive, QPalette::Link, brush8);
    palette5.setBrush(QPalette::Inactive, QPalette::LinkVisited, brush9);
    palette5.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush10);
    palette5.setBrush(QPalette::Disabled, QPalette::WindowText, brush);
    palette5.setBrush(QPalette::Disabled, QPalette::Button, brush11);
    palette5.setBrush(QPalette::Disabled, QPalette::Light, brush2);
    palette5.setBrush(QPalette::Disabled, QPalette::Midlight, brush3);
    palette5.setBrush(QPalette::Disabled, QPalette::Dark, brush4);
    palette5.setBrush(QPalette::Disabled, QPalette::Mid, brush5);
    palette5.setBrush(QPalette::Disabled, QPalette::Text, brush);
    palette5.setBrush(QPalette::Disabled, QPalette::BrightText, brush2);
    palette5.setBrush(QPalette::Disabled, QPalette::ButtonText, brush);
    palette5.setBrush(QPalette::Disabled, QPalette::Base, brush6);
    palette5.setBrush(QPalette::Disabled, QPalette::Window, brush6);
    palette5.setBrush(QPalette::Disabled, QPalette::Shadow, brush6);
    palette5.setBrush(QPalette::Disabled, QPalette::Highlight, brush7);
    palette5.setBrush(QPalette::Disabled, QPalette::HighlightedText, brush1);
    palette5.setBrush(QPalette::Disabled, QPalette::Link, brush8);
    palette5.setBrush(QPalette::Disabled, QPalette::LinkVisited, brush9);
    palette5.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush10);
    playButton->setPalette(palette5);
    playButton->setIcon(QIcon(QString::fromUtf8(":/images/media-play.png")));

    hboxLayout->addWidget(playButton);

    pauseButton = new QPushButton(centralWidget);
    pauseButton->setObjectName(QString::fromUtf8("pauseButton"));
    sizePolicy.setHeightForWidth(pauseButton->sizePolicy().hasHeightForWidth());
    pauseButton->setSizePolicy(sizePolicy);
    pauseButton->setMinimumSize(QSize(45, 27));
    QPalette palette6;
    palette6.setBrush(QPalette::Active, QPalette::WindowText, brush);
    palette6.setBrush(QPalette::Active, QPalette::Button, brush11);
    palette6.setBrush(QPalette::Active, QPalette::Light, brush2);
    palette6.setBrush(QPalette::Active, QPalette::Midlight, brush3);
    palette6.setBrush(QPalette::Active, QPalette::Dark, brush4);
    palette6.setBrush(QPalette::Active, QPalette::Mid, brush5);
    palette6.setBrush(QPalette::Active, QPalette::Text, brush);
    palette6.setBrush(QPalette::Active, QPalette::BrightText, brush2);
    palette6.setBrush(QPalette::Active, QPalette::ButtonText, brush);
    palette6.setBrush(QPalette::Active, QPalette::Base, brush6);
    palette6.setBrush(QPalette::Active, QPalette::Window, brush6);
    palette6.setBrush(QPalette::Active, QPalette::Shadow, brush6);
    palette6.setBrush(QPalette::Active, QPalette::Highlight, brush7);
    palette6.setBrush(QPalette::Active, QPalette::HighlightedText, brush1);
    palette6.setBrush(QPalette::Active, QPalette::Link, brush8);
    palette6.setBrush(QPalette::Active, QPalette::LinkVisited, brush9);
    palette6.setBrush(QPalette::Active, QPalette::AlternateBase, brush10);
    palette6.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
    palette6.setBrush(QPalette::Inactive, QPalette::Button, brush11);
    palette6.setBrush(QPalette::Inactive, QPalette::Light, brush2);
    palette6.setBrush(QPalette::Inactive, QPalette::Midlight, brush3);
    palette6.setBrush(QPalette::Inactive, QPalette::Dark, brush4);
    palette6.setBrush(QPalette::Inactive, QPalette::Mid, brush5);
    palette6.setBrush(QPalette::Inactive, QPalette::Text, brush);
    palette6.setBrush(QPalette::Inactive, QPalette::BrightText, brush2);
    palette6.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
    palette6.setBrush(QPalette::Inactive, QPalette::Base, brush6);
    palette6.setBrush(QPalette::Inactive, QPalette::Window, brush6);
    palette6.setBrush(QPalette::Inactive, QPalette::Shadow, brush6);
    palette6.setBrush(QPalette::Inactive, QPalette::Highlight, brush7);
    palette6.setBrush(QPalette::Inactive, QPalette::HighlightedText, brush1);
    palette6.setBrush(QPalette::Inactive, QPalette::Link, brush8);
    palette6.setBrush(QPalette::Inactive, QPalette::LinkVisited, brush9);
    palette6.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush10);
    palette6.setBrush(QPalette::Disabled, QPalette::WindowText, brush);
    palette6.setBrush(QPalette::Disabled, QPalette::Button, brush11);
    palette6.setBrush(QPalette::Disabled, QPalette::Light, brush2);
    palette6.setBrush(QPalette::Disabled, QPalette::Midlight, brush3);
    palette6.setBrush(QPalette::Disabled, QPalette::Dark, brush4);
    palette6.setBrush(QPalette::Disabled, QPalette::Mid, brush5);
    palette6.setBrush(QPalette::Disabled, QPalette::Text, brush);
    palette6.setBrush(QPalette::Disabled, QPalette::BrightText, brush2);
    palette6.setBrush(QPalette::Disabled, QPalette::ButtonText, brush);
    palette6.setBrush(QPalette::Disabled, QPalette::Base, brush6);
    palette6.setBrush(QPalette::Disabled, QPalette::Window, brush6);
    palette6.setBrush(QPalette::Disabled, QPalette::Shadow, brush6);
    palette6.setBrush(QPalette::Disabled, QPalette::Highlight, brush7);
    palette6.setBrush(QPalette::Disabled, QPalette::HighlightedText, brush1);
    palette6.setBrush(QPalette::Disabled, QPalette::Link, brush8);
    palette6.setBrush(QPalette::Disabled, QPalette::LinkVisited, brush9);
    palette6.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush10);
    pauseButton->setPalette(palette6);
    pauseButton->setIcon(QIcon(QString::fromUtf8(":/images/media-pause.png")));

    hboxLayout->addWidget(pauseButton);

    prevButton = new QPushButton(centralWidget);
    prevButton->setObjectName(QString::fromUtf8("prevButton"));
    sizePolicy.setHeightForWidth(prevButton->sizePolicy().hasHeightForWidth());
    prevButton->setSizePolicy(sizePolicy);
    prevButton->setMinimumSize(QSize(45, 27));
    QPalette palette7;
    palette7.setBrush(QPalette::Active, QPalette::WindowText, brush);
    palette7.setBrush(QPalette::Active, QPalette::Button, brush11);
    palette7.setBrush(QPalette::Active, QPalette::Light, brush2);
    palette7.setBrush(QPalette::Active, QPalette::Midlight, brush3);
    palette7.setBrush(QPalette::Active, QPalette::Dark, brush4);
    palette7.setBrush(QPalette::Active, QPalette::Mid, brush5);
    palette7.setBrush(QPalette::Active, QPalette::Text, brush);
    palette7.setBrush(QPalette::Active, QPalette::BrightText, brush2);
    palette7.setBrush(QPalette::Active, QPalette::ButtonText, brush);
    palette7.setBrush(QPalette::Active, QPalette::Base, brush6);
    palette7.setBrush(QPalette::Active, QPalette::Window, brush6);
    palette7.setBrush(QPalette::Active, QPalette::Shadow, brush6);
    palette7.setBrush(QPalette::Active, QPalette::Highlight, brush7);
    palette7.setBrush(QPalette::Active, QPalette::HighlightedText, brush1);
    palette7.setBrush(QPalette::Active, QPalette::Link, brush8);
    palette7.setBrush(QPalette::Active, QPalette::LinkVisited, brush9);
    palette7.setBrush(QPalette::Active, QPalette::AlternateBase, brush10);
    palette7.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
    palette7.setBrush(QPalette::Inactive, QPalette::Button, brush11);
    palette7.setBrush(QPalette::Inactive, QPalette::Light, brush2);
    palette7.setBrush(QPalette::Inactive, QPalette::Midlight, brush3);
    palette7.setBrush(QPalette::Inactive, QPalette::Dark, brush4);
    palette7.setBrush(QPalette::Inactive, QPalette::Mid, brush5);
    palette7.setBrush(QPalette::Inactive, QPalette::Text, brush);
    palette7.setBrush(QPalette::Inactive, QPalette::BrightText, brush2);
    palette7.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
    palette7.setBrush(QPalette::Inactive, QPalette::Base, brush6);
    palette7.setBrush(QPalette::Inactive, QPalette::Window, brush6);
    palette7.setBrush(QPalette::Inactive, QPalette::Shadow, brush6);
    palette7.setBrush(QPalette::Inactive, QPalette::Highlight, brush7);
    palette7.setBrush(QPalette::Inactive, QPalette::HighlightedText, brush1);
    palette7.setBrush(QPalette::Inactive, QPalette::Link, brush8);
    palette7.setBrush(QPalette::Inactive, QPalette::LinkVisited, brush9);
    palette7.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush10);
    palette7.setBrush(QPalette::Disabled, QPalette::WindowText, brush);
    palette7.setBrush(QPalette::Disabled, QPalette::Button, brush11);
    palette7.setBrush(QPalette::Disabled, QPalette::Light, brush2);
    palette7.setBrush(QPalette::Disabled, QPalette::Midlight, brush3);
    palette7.setBrush(QPalette::Disabled, QPalette::Dark, brush4);
    palette7.setBrush(QPalette::Disabled, QPalette::Mid, brush5);
    palette7.setBrush(QPalette::Disabled, QPalette::Text, brush);
    palette7.setBrush(QPalette::Disabled, QPalette::BrightText, brush2);
    palette7.setBrush(QPalette::Disabled, QPalette::ButtonText, brush);
    palette7.setBrush(QPalette::Disabled, QPalette::Base, brush6);
    palette7.setBrush(QPalette::Disabled, QPalette::Window, brush6);
    palette7.setBrush(QPalette::Disabled, QPalette::Shadow, brush6);
    palette7.setBrush(QPalette::Disabled, QPalette::Highlight, brush7);
    palette7.setBrush(QPalette::Disabled, QPalette::HighlightedText, brush1);
    palette7.setBrush(QPalette::Disabled, QPalette::Link, brush8);
    palette7.setBrush(QPalette::Disabled, QPalette::LinkVisited, brush9);
    palette7.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush10);
    prevButton->setPalette(palette7);
    prevButton->setIcon(QIcon(QString::fromUtf8(":/images/media-prev.png")));

    hboxLayout->addWidget(prevButton);

    nextButton = new QPushButton(centralWidget);
    nextButton->setObjectName(QString::fromUtf8("nextButton"));
    sizePolicy.setHeightForWidth(nextButton->sizePolicy().hasHeightForWidth());
    nextButton->setSizePolicy(sizePolicy);
    nextButton->setMinimumSize(QSize(45, 27));
    QPalette palette8;
    palette8.setBrush(QPalette::Active, QPalette::WindowText, brush);
    palette8.setBrush(QPalette::Active, QPalette::Button, brush11);
    palette8.setBrush(QPalette::Active, QPalette::Light, brush2);
    palette8.setBrush(QPalette::Active, QPalette::Midlight, brush3);
    palette8.setBrush(QPalette::Active, QPalette::Dark, brush4);
    palette8.setBrush(QPalette::Active, QPalette::Mid, brush5);
    palette8.setBrush(QPalette::Active, QPalette::Text, brush);
    palette8.setBrush(QPalette::Active, QPalette::BrightText, brush2);
    palette8.setBrush(QPalette::Active, QPalette::ButtonText, brush);
    palette8.setBrush(QPalette::Active, QPalette::Base, brush6);
    palette8.setBrush(QPalette::Active, QPalette::Window, brush6);
    palette8.setBrush(QPalette::Active, QPalette::Shadow, brush6);
    palette8.setBrush(QPalette::Active, QPalette::Highlight, brush7);
    palette8.setBrush(QPalette::Active, QPalette::HighlightedText, brush1);
    palette8.setBrush(QPalette::Active, QPalette::Link, brush8);
    palette8.setBrush(QPalette::Active, QPalette::LinkVisited, brush9);
    palette8.setBrush(QPalette::Active, QPalette::AlternateBase, brush10);
    palette8.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
    palette8.setBrush(QPalette::Inactive, QPalette::Button, brush11);
    palette8.setBrush(QPalette::Inactive, QPalette::Light, brush2);
    palette8.setBrush(QPalette::Inactive, QPalette::Midlight, brush3);
    palette8.setBrush(QPalette::Inactive, QPalette::Dark, brush4);
    palette8.setBrush(QPalette::Inactive, QPalette::Mid, brush5);
    palette8.setBrush(QPalette::Inactive, QPalette::Text, brush);
    palette8.setBrush(QPalette::Inactive, QPalette::BrightText, brush2);
    palette8.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
    palette8.setBrush(QPalette::Inactive, QPalette::Base, brush6);
    palette8.setBrush(QPalette::Inactive, QPalette::Window, brush6);
    palette8.setBrush(QPalette::Inactive, QPalette::Shadow, brush6);
    palette8.setBrush(QPalette::Inactive, QPalette::Highlight, brush7);
    palette8.setBrush(QPalette::Inactive, QPalette::HighlightedText, brush1);
    palette8.setBrush(QPalette::Inactive, QPalette::Link, brush8);
    palette8.setBrush(QPalette::Inactive, QPalette::LinkVisited, brush9);
    palette8.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush10);
    palette8.setBrush(QPalette::Disabled, QPalette::WindowText, brush);
    palette8.setBrush(QPalette::Disabled, QPalette::Button, brush11);
    palette8.setBrush(QPalette::Disabled, QPalette::Light, brush2);
    palette8.setBrush(QPalette::Disabled, QPalette::Midlight, brush3);
    palette8.setBrush(QPalette::Disabled, QPalette::Dark, brush4);
    palette8.setBrush(QPalette::Disabled, QPalette::Mid, brush5);
    palette8.setBrush(QPalette::Disabled, QPalette::Text, brush);
    palette8.setBrush(QPalette::Disabled, QPalette::BrightText, brush2);
    palette8.setBrush(QPalette::Disabled, QPalette::ButtonText, brush);
    palette8.setBrush(QPalette::Disabled, QPalette::Base, brush6);
    palette8.setBrush(QPalette::Disabled, QPalette::Window, brush6);
    palette8.setBrush(QPalette::Disabled, QPalette::Shadow, brush6);
    palette8.setBrush(QPalette::Disabled, QPalette::Highlight, brush7);
    palette8.setBrush(QPalette::Disabled, QPalette::HighlightedText, brush1);
    palette8.setBrush(QPalette::Disabled, QPalette::Link, brush8);
    palette8.setBrush(QPalette::Disabled, QPalette::LinkVisited, brush9);
    palette8.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush10);
    nextButton->setPalette(palette8);
    nextButton->setIcon(QIcon(QString::fromUtf8(":/images/media-next.png")));

    hboxLayout->addWidget(nextButton);

    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem);


    gridLayout->addLayout(hboxLayout, 0, 0, 1, 3);

    positionSlider = new QSlider(centralWidget);
    positionSlider->setObjectName(QString::fromUtf8("positionSlider"));
    QPalette palette9;
    palette9.setBrush(QPalette::Active, QPalette::WindowText, brush6);
    palette9.setBrush(QPalette::Active, QPalette::Button, brush11);
    palette9.setBrush(QPalette::Active, QPalette::Light, brush12);
    palette9.setBrush(QPalette::Active, QPalette::Midlight, brush13);
    palette9.setBrush(QPalette::Active, QPalette::Dark, brush14);
    palette9.setBrush(QPalette::Active, QPalette::Mid, brush15);
    palette9.setBrush(QPalette::Active, QPalette::Text, brush6);
    palette9.setBrush(QPalette::Active, QPalette::BrightText, brush2);
    palette9.setBrush(QPalette::Active, QPalette::ButtonText, brush6);
    palette9.setBrush(QPalette::Active, QPalette::Base, brush2);
    palette9.setBrush(QPalette::Active, QPalette::Window, brush11);
    palette9.setBrush(QPalette::Active, QPalette::Shadow, brush6);
    palette9.setBrush(QPalette::Active, QPalette::Highlight, brush7);
    palette9.setBrush(QPalette::Active, QPalette::HighlightedText, brush1);
    palette9.setBrush(QPalette::Active, QPalette::Link, brush8);
    palette9.setBrush(QPalette::Active, QPalette::LinkVisited, brush9);
    palette9.setBrush(QPalette::Active, QPalette::AlternateBase, brush13);
    palette9.setBrush(QPalette::Inactive, QPalette::WindowText, brush6);
    palette9.setBrush(QPalette::Inactive, QPalette::Button, brush11);
    palette9.setBrush(QPalette::Inactive, QPalette::Light, brush12);
    palette9.setBrush(QPalette::Inactive, QPalette::Midlight, brush13);
    palette9.setBrush(QPalette::Inactive, QPalette::Dark, brush14);
    palette9.setBrush(QPalette::Inactive, QPalette::Mid, brush15);
    palette9.setBrush(QPalette::Inactive, QPalette::Text, brush6);
    palette9.setBrush(QPalette::Inactive, QPalette::BrightText, brush2);
    palette9.setBrush(QPalette::Inactive, QPalette::ButtonText, brush6);
    palette9.setBrush(QPalette::Inactive, QPalette::Base, brush2);
    palette9.setBrush(QPalette::Inactive, QPalette::Window, brush11);
    palette9.setBrush(QPalette::Inactive, QPalette::Shadow, brush6);
    palette9.setBrush(QPalette::Inactive, QPalette::Highlight, brush7);
    palette9.setBrush(QPalette::Inactive, QPalette::HighlightedText, brush1);
    palette9.setBrush(QPalette::Inactive, QPalette::Link, brush8);
    palette9.setBrush(QPalette::Inactive, QPalette::LinkVisited, brush9);
    palette9.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush13);
    palette9.setBrush(QPalette::Disabled, QPalette::WindowText, brush14);
    palette9.setBrush(QPalette::Disabled, QPalette::Button, brush11);
    palette9.setBrush(QPalette::Disabled, QPalette::Light, brush12);
    palette9.setBrush(QPalette::Disabled, QPalette::Midlight, brush13);
    palette9.setBrush(QPalette::Disabled, QPalette::Dark, brush14);
    palette9.setBrush(QPalette::Disabled, QPalette::Mid, brush15);
    palette9.setBrush(QPalette::Disabled, QPalette::Text, brush14);
    palette9.setBrush(QPalette::Disabled, QPalette::BrightText, brush2);
    palette9.setBrush(QPalette::Disabled, QPalette::ButtonText, brush14);
    palette9.setBrush(QPalette::Disabled, QPalette::Base, brush11);
    palette9.setBrush(QPalette::Disabled, QPalette::Window, brush11);
    palette9.setBrush(QPalette::Disabled, QPalette::Shadow, brush6);
    palette9.setBrush(QPalette::Disabled, QPalette::Highlight, brush7);
    palette9.setBrush(QPalette::Disabled, QPalette::HighlightedText, brush1);
    palette9.setBrush(QPalette::Disabled, QPalette::Link, brush8);
    palette9.setBrush(QPalette::Disabled, QPalette::LinkVisited, brush9);
    palette9.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush13);
    positionSlider->setPalette(palette9);
    positionSlider->setTracking(false);
    positionSlider->setOrientation(Qt::Horizontal);

    gridLayout->addWidget(positionSlider, 2, 1, 1, 1);


    vboxLayout->addLayout(gridLayout);

    MarPlayerForm->setCentralWidget(centralWidget);

    retranslateUi(MarPlayerForm);

    QMetaObject::connectSlotsByName(MarPlayerForm);
    } // setupUi

    void retranslateUi(QMainWindow *MarPlayerForm)
    {
    MarPlayerForm->setWindowTitle(QApplication::translate("MarPlayerForm", "MarPlayer", 0, QApplication::UnicodeUTF8));
    posTimeEdit->setDisplayFormat(QApplication::translate("MarPlayerForm", "h:mm:ss", 0, QApplication::UnicodeUTF8));
    durTimeEdit->setDisplayFormat(QApplication::translate("MarPlayerForm", "h:mm:ss", 0, QApplication::UnicodeUTF8));
    MarPlayerFormBanner_2_2_2->setText(QString());
    playButton->setText(QString());
    pauseButton->setText(QString());
    prevButton->setText(QString());
    nextButton->setText(QString());
    Q_UNUSED(MarPlayerForm);
    } // retranslateUi

};

namespace Ui {
    class MarPlayerForm: public Ui_MarPlayerForm {};
} // namespace Ui

#endif // UI_MARPLAYERFORM_H
