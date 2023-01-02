/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref BulletNumberPopupDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QDialog>
#include <QPoint>
#include <QSet>
#include <QHash>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QPushButton>

#include <ld_unordered_list_paragraph_format.h>
#include <ld_ordered_list_paragraph_format.h>

#include "application.h"
#include "unicode_marshaller.h"
#include "bullet_number_popup_dialog.h"

BulletNumberPopupDialog::BulletNumberPopupDialog(QWidget* parent):QDialog(parent, Qt::Popup) {
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    QGroupBox* bulletGroupBox = new QGroupBox(tr("Bullet Styles"));
    mainLayout->addWidget(bulletGroupBox);

    QGridLayout* bulletLayout = new QGridLayout;
    bulletGroupBox->setLayout(bulletLayout);

    QList<Ld::UnorderedListParagraphFormat::StandardBullet> standardBullets =
        Ld::UnorderedListParagraphFormat::standardBullets();

    unsigned bulletsPerLine = static_cast<unsigned>((standardBullets.size() + 1) / 2);
    bulletsPerLine += (bulletsPerLine % 2);

    unsigned row    = 0;
    unsigned column = 0;

    QSet<QToolButton*> styleToolButtons;

    for (unsigned bulletIndex=0 ; bulletIndex<static_cast<unsigned>(standardBullets.size()) ; ++bulletIndex) {
        const Ld::UnorderedListParagraphFormat::StandardBullet& bulletData  = standardBullets.at(bulletIndex);
        QString                                                 bullet      = bulletData.bullet();
        QString                                                 description = bulletData.description();

        QToolButton* bulletToolButton = new QToolButton;
        bulletToolButton->setText(bullet);
        bulletToolButton->setToolTip(tr("Click to make an ordered list using %1s (%2)").arg(description).arg(bullet));
        bulletToolButton->setWhatsThis(
            tr(
               "You can use this button to turn this paragraph or selected paragraphs into an unordered or "
               "non-numbered list using %1s (%2) to mark each paragraph."
            ).arg(description).arg(bullet)
        );
        bulletButtons.insert(bulletData.unicodeValue(), bulletToolButton);

        bulletLayout->addWidget(bulletToolButton, row, column);
        styleToolButtons << bulletToolButton;

        ++column;
        if (column >= bulletsPerLine) {
            column = 0;
            ++row;
        }
    }

    QHBoxLayout* numberPaddingLayout = new QHBoxLayout;

    mainLayout->addLayout(numberPaddingLayout);
    numberPaddingLayout->addStretch();

    QGroupBox* numberGroupBox = new QGroupBox(tr("Number Styles"));
    numberPaddingLayout->addWidget(numberGroupBox);

    numberPaddingLayout->addStretch();

    QHBoxLayout* numberLayout = new QHBoxLayout;
    numberGroupBox->setLayout(numberLayout);

    numberStyleToolButton = new QToolButton;
    numberStyleToolButton->setText(tr("1."));
    numberStyleToolButton->setToolTip(tr("Click to make an ordered list.  Each paragraph will be numbered."));
    numberStyleToolButton->setWhatsThis(
        tr(
           "You can click this button to turn this paragraph or selected paragraphs into an ordered list.  Each "
           "paragraph will be consecutively numbered."
        )
    );
    numberLayout->addWidget(numberStyleToolButton);

    styleToolButtons << numberStyleToolButton;

    upperCaseStyleToolButton = new QToolButton;
    upperCaseStyleToolButton->setText(tr("A."));
    upperCaseStyleToolButton->setToolTip(
      tr(
         "Click to make an ordered list.  Each paragraph will be marked using upper case letters, in alphabetical "
          "order."
        )
    );
    upperCaseStyleToolButton->setWhatsThis(
        tr(
           "You can click this button to turn this paragraph or selected paragraphs into an ordered list.  Each"
           "paragraph will be consecutively marked using upper case letters in alphabetical order.  If needed, "
           "multiple letters will be used on each paragraph to uniquely mark large numbers consecutive paragraphs."
          )
    );
    numberLayout->addWidget(upperCaseStyleToolButton);

    styleToolButtons << upperCaseStyleToolButton;

    lowerCaseStyleToolButton = new QToolButton;
    lowerCaseStyleToolButton->setText(tr("a."));
    lowerCaseStyleToolButton->setToolTip(
      tr(
         "Click to make an ordered list.  Each paragraph will be marked using lower case letters, in alphabetical "
          "order."
        )
    );
    lowerCaseStyleToolButton->setWhatsThis(
        tr(
           "You can click this button to turn this paragraph or selected paragraphs into an ordered list.  Each"
           "paragraph will be consecutively marked using lower case letters in alphabetical order.  If needed, "
           "multiple letters will be used on each paragraph to uniquely mark large numbers consecutive paragraphs."
          )
    );
    numberLayout->addWidget(lowerCaseStyleToolButton);

    styleToolButtons << lowerCaseStyleToolButton;

    romanUpperStyleToolButton = new QToolButton;
    romanUpperStyleToolButton->setText(tr("IV."));
    romanUpperStyleToolButton->setToolTip(
      tr("Click to make an ordered list.  Each paragraph will be marked using roman numerals.")
    );
    romanUpperStyleToolButton->setWhatsThis(
        tr(
           "You can click this button to turn this paragraph or selected paragraphs into an ordered list.  Each"
           "paragraph will be consecutively marked using roman numerals.  Note that values will be limited to 3999 or"
           "\"MMMCMXCIX\"."
          )
    );
    numberLayout->addWidget(romanUpperStyleToolButton);

    styleToolButtons << romanUpperStyleToolButton;

    romanLowerStyleToolButton = new QToolButton;
    romanLowerStyleToolButton->setText(tr("iv."));
    romanLowerStyleToolButton->setToolTip(
      tr("Click to make an ordered list.  Each paragraph will be marked using lower case roman numerals.")
    );
    romanLowerStyleToolButton->setWhatsThis(
        tr(
           "You can click this button to turn this paragraph or selected paragraphs into an ordered list.  Each"
           "paragraph will be consecutively marked using lower case roman numerals.  Note that values will be limited "
           "to 3999 or \"mmmcmxcix\"."
          )
    );
    numberLayout->addWidget(romanLowerStyleToolButton);

    styleToolButtons << romanLowerStyleToolButton;

    removeFromListPushButton = new QPushButton(tr("Remove List Formatting"));
    mainLayout->addWidget(removeFromListPushButton);

    QFont buttonFont = Application::font();
    QFontMetrics fontMetrics(buttonFont);
    unsigned     maximumSide = static_cast<unsigned>(fontMetrics.height());
    for (  QSet<QToolButton*>::const_iterator buttonIterator    = styleToolButtons.constBegin(),
                                              buttonEndIterator = styleToolButtons.constEnd()
         ; buttonIterator != buttonEndIterator
         ; ++buttonIterator
        ) {
        QToolButton* button    = *buttonIterator;
        unsigned     textWidth = fontMetrics.horizontalAdvance(button->text());
        if (textWidth > maximumSide) {
            maximumSide = textWidth;
        }
    }

    QSize buttonSize(1.5 * maximumSide, 1.5 * maximumSide);
    for (  QSet<QToolButton*>::const_iterator buttonIterator    = styleToolButtons.constBegin(),
                                              buttonEndIterator = styleToolButtons.constEnd()
         ; buttonIterator != buttonEndIterator
         ; ++buttonIterator
        ) {
        QToolButton* button = *buttonIterator;
        button->setCheckable(true);
        button->setFont(buttonFont);
        button->setFixedSize(buttonSize);
    }

    for (  QHash<UnicodeValue, QToolButton*>::const_iterator bulletButtonIterator    = bulletButtons.constBegin(),
                                                             bulletButtonEndIterator = bulletButtons.constEnd()
         ; bulletButtonIterator != bulletButtonEndIterator
         ; ++bulletButtonIterator
        ) {
        UnicodeValue unicodeValue = bulletButtonIterator.key();
        QToolButton* bulletButton = bulletButtonIterator.value();

        UnicodeMarshaller* marshaller = new UnicodeMarshaller(unicodeValue, this);
        connect(bulletButton, &QToolButton::clicked, marshaller, &UnicodeMarshaller::trigger);
        connect(
            marshaller,
            static_cast<void (UnicodeMarshaller::*)(UnicodeValue)>(&UnicodeMarshaller::triggered),
            this,
            &BulletNumberPopupDialog::bulletButtonClicked
        );
    }

    connect(numberStyleToolButton, &QToolButton::clicked, this, &BulletNumberPopupDialog::numberListStyleSelected);
    connect(
        upperCaseStyleToolButton,
        &QToolButton::clicked,
        this,
        &BulletNumberPopupDialog::upperCaseLetterListStyleSelected
    );
    connect(
        lowerCaseStyleToolButton,
        &QToolButton::clicked,
        this,
        &BulletNumberPopupDialog::lowerCaseLetterListStyleSelected
    );
    connect(
        romanUpperStyleToolButton,
        &QToolButton::clicked,
        this,
        &BulletNumberPopupDialog::upperCaseRomanNumberListStyleSelected
    );
    connect(
        romanLowerStyleToolButton,
        &QToolButton::clicked,
        this,
        &BulletNumberPopupDialog::lowerCaseRomanNumberListStyleSelected
    );
    connect(
        removeFromListPushButton,
        &QPushButton::clicked,
        this,
        &BulletNumberPopupDialog::removeFromListButtonClicked
    );
}


BulletNumberPopupDialog::~BulletNumberPopupDialog() {}


void BulletNumberPopupDialog::setListFont(const QFont& listFont) {
    QFontMetrics fontMetrics(listFont);

    for (  QHash<UnicodeValue, QToolButton*>::const_iterator buttonIterator = bulletButtons.constBegin(),
                                                             buttonEndIterator = bulletButtons.constEnd()
         ; buttonIterator != buttonEndIterator
         ; ++buttonIterator
        ) {
        UnicodeValue unicodeValue = buttonIterator.key();
        QToolButton* button       = buttonIterator.value();

        button->setFont(listFont);
        button->setVisible(fontMetrics.inFont(QChar(unicodeValue)));
    }

    numberStyleToolButton->setFont(listFont);
    upperCaseStyleToolButton->setFont(listFont);
    lowerCaseStyleToolButton->setFont(listFont);
    romanUpperStyleToolButton->setFont(listFont);
    romanLowerStyleToolButton->setFont(listFont);
}

void BulletNumberPopupDialog::setSelectedBulletStyles(const QSet<QString>& bulletStyles) {
    for (  QHash<UnicodeValue, QToolButton*>::const_iterator buttonIterator = bulletButtons.constBegin(),
                                                             buttonEndIterator = bulletButtons.constEnd()
         ; buttonIterator != buttonEndIterator
         ; ++buttonIterator
        ) {
        UnicodeValue unicodeValue = buttonIterator.key();
        QToolButton* button       = buttonIterator.value();
        QString      bulletString = QString(QChar(unicodeValue));
        button->setChecked(bulletStyles.contains(bulletString));
    }
}


void BulletNumberPopupDialog::setSelectedNumberStyles(const QSet<NumberStyle>& numberStyles) {
    numberStyleToolButton->setChecked(numberStyles.contains(NumberStyle::NUMBER));
    upperCaseStyleToolButton->setChecked(numberStyles.contains(NumberStyle::UPPER_CASE));
    lowerCaseStyleToolButton->setChecked(numberStyles.contains(NumberStyle::LOWER_CASE));
    romanUpperStyleToolButton->setChecked(numberStyles.contains(NumberStyle::ROMAN_UPPER_CASE));
    romanLowerStyleToolButton->setChecked(numberStyles.contains(NumberStyle::ROMAN_LOWER_CASE));
}


void BulletNumberPopupDialog::setRemoveListStyleButtonVisible(bool nowVisible) {
    removeFromListPushButton->setVisible(nowVisible);
}


void BulletNumberPopupDialog::setRemoveListStyleButtonHidden(bool nowHidden) {
    setRemoveListStyleButtonVisible(!nowHidden);
}


void BulletNumberPopupDialog::focusOutEvent(QFocusEvent*) {
    QSet<QWidget*> focusChildren;
    for (  QHash<UnicodeValue, QToolButton*>::const_iterator buttonIterator = bulletButtons.constBegin(),
                                                             buttonEndIterator = bulletButtons.constEnd()
         ; buttonIterator != buttonEndIterator
         ; ++buttonIterator
        ) {
        focusChildren << buttonIterator.value();
    }

    focusChildren << numberStyleToolButton;
    focusChildren << upperCaseStyleToolButton;
    focusChildren << lowerCaseStyleToolButton;
    focusChildren << romanUpperStyleToolButton;
    focusChildren << romanLowerStyleToolButton;
    focusChildren << removeFromListPushButton;

    QWidget* focusWidget = Application::focusWidget();
    if (!focusChildren.contains(focusWidget)) {
        emit focusLost();
    }
}


void BulletNumberPopupDialog::bulletButtonClicked(UnicodeValue newUnicodeValue) {
    emit bulletSelected(QString(QChar(newUnicodeValue)));
}


void BulletNumberPopupDialog::numberListStyleSelected() {
    emit numberStyleSelected(NumberStyle::NUMBER);
}


void BulletNumberPopupDialog::upperCaseLetterListStyleSelected() {
    emit numberStyleSelected(NumberStyle::UPPER_CASE);
}


void BulletNumberPopupDialog::lowerCaseLetterListStyleSelected() {
    emit numberStyleSelected(NumberStyle::LOWER_CASE);
}


void BulletNumberPopupDialog::upperCaseRomanNumberListStyleSelected() {
    emit numberStyleSelected(NumberStyle::ROMAN_UPPER_CASE);
}


void BulletNumberPopupDialog::lowerCaseRomanNumberListStyleSelected() {
    emit numberStyleSelected(NumberStyle::ROMAN_LOWER_CASE);
}


void BulletNumberPopupDialog::removeFromListButtonClicked() {
    emit noListStyleSelected();
}
