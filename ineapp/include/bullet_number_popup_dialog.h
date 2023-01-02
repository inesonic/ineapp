/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref BulletNumberPopupDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef BULLET_NUMBER_POPUP_DIALOG_H
#define BULLET_NUMBER_POPUP_DIALOG_H

#include <QDialog>
#include <QPoint>
#include <QSet>
#include <QHash>
#include <QFont>

#include <ld_unordered_list_paragraph_format.h>
#include <ld_ordered_list_paragraph_format.h>

#include "app_common.h"

class QWidget;
class QToolButton;
class QPushButton;
class QFocusEvent;

/**
 * Class that provides a small popup dialog you can use to select bullet/number styles.  The dialog also includes a
 * bullon to disable list formatting.
 */
class APP_PUBLIC_API BulletNumberPopupDialog:public QDialog {
    Q_OBJECT

    public:
        /**
         * Type used to represent number styles.
         */
        typedef Ld::OrderedListParagraphFormat::Style NumberStyle;

        /**
         * Constructor.
         *
         * \param[in] parent The parent object for this menu.
         */
        BulletNumberPopupDialog(QWidget* parent = Q_NULLPTR);

        ~BulletNumberPopupDialog() override;

    signals:
        /**
         * Signal that is emitted when the user selects a new bullet style.
         *
         * \param[out] newBullet The selected bullet style.
         */
        void bulletSelected(const QString& newBullet);

        /**
         * Signal that is emtited when the user selects a new numbering style.
         *
         * \param[out] newNumberStyle The selected numbering style.
         */
        void numberStyleSelected(NumberStyle newNumberStyle);

        /**
         * Signal that is emitted when the user selects no list style.
         */
        void noListStyleSelected();

        /**
         * Signal that is emitted when the dialog loses focus.
         */
        void focusLost();

    public slots:
        /**
         * Slot you can use to set the font used to render bullets and numbers.  This method will also hide buttons for
         * bullets that can not be rendered with the specified font.
         *
         * \param[in] listFont The font used to render bullets and numbers.
         */
        void setListFont(const QFont& listFont);

        /**
         * Slot you can use to select which bullet styles are shown as enabled.
         *
         * \param[in] bulletStyles A set of every selected bullet.
         */
        void setSelectedBulletStyles(const QSet<QString>& bulletStyles);

        /**
         * Slot you can use to select which number styles are shown as enabled.
         *
         * \param[in] numberStyles A set of every selected number style.
         */
        void setSelectedNumberStyles(const QSet<NumberStyle>& numberStyles);

        /**
         * Slot you can use to show or hide the remove list style button.
         *
         * \param[in] nowVisible If true, the list style button will be shown.  If false, the list style button will be
         *                       hidden.
         */
        void setRemoveListStyleButtonVisible(bool nowVisible = true);

        /**
         * Slot you can use to hide or show the remove list style button.
         *
         * \param[in] nowHidden If true, the list style button will be hidden.  If false, the list style button will be
         *                      shown.
         */
        void setRemoveListStyleButtonHidden(bool nowHidden = true);

    protected:
        /**
         * Method that is called when the dialog loses focus.
         *
         * \param[in] event The event that triggered the call to this method.
         */
        void focusOutEvent(QFocusEvent* event) override;

    private:
        /**
         * Type used to represent a Unicode bullet style.
         */
        typedef Ld::UnorderedListParagraphFormat::StandardBullet::UnicodeValue UnicodeValue;

    private slots:
        /**
         * Slot that intercepts and forwards a bullet button click.
         *
         * \param[in] newUnicodeValue The unicode value of the clicked button.
         */
        void bulletButtonClicked(UnicodeValue newUnicodeValue);

        /**
         * Slot that is triggered when the user selects the number ordered list style.
         */
        void numberListStyleSelected();

        /**
         * Slot that is triggered when the user selects the upper case letter list style.
         */
        void upperCaseLetterListStyleSelected();

        /**
         * Slot that is triggered when the user selects the lower case letter style.
         */
        void lowerCaseLetterListStyleSelected();

        /**
         * Slot that is triggered when the user selects the upper-case roman number style.
         */
        void upperCaseRomanNumberListStyleSelected();

        /**
         * Slot that is triggered when the user selects the lower-case roman number style.
         */
        void lowerCaseRomanNumberListStyleSelected();

        /**
         * Slot that is triggered when the user selects the remove from list button.
         */
        void removeFromListButtonClicked();

    private:
        /**
         * Hash of bullet buttons by unicode value.
         */
        QHash<UnicodeValue, QToolButton*> bulletButtons;

        /**
         * The number style tool button.
         */
        QToolButton* numberStyleToolButton;

        /**
         * Upper case style tool button.
         */
        QToolButton* upperCaseStyleToolButton;

        /**
         * Lower case style tool button.
         */
        QToolButton* lowerCaseStyleToolButton;

        /**
         * Upper case roman numeral style tool button.
         */
        QToolButton* romanUpperStyleToolButton;

        /**
         * Lower case roman numeral style tool button.
         */
        QToolButton* romanLowerStyleToolButton;

        /**
         * The remove from list button.
         */
        QPushButton* removeFromListPushButton;
};

#endif
