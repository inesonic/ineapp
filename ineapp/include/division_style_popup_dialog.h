/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref DivisionStylePopupDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef DIVISION_STYLE_POPUP_DIALOG_H
#define DIVISION_STYLE_POPUP_DIALOG_H

#include <QDialog>
#include <QPoint>
#include <QSet>
#include <QHash>
#include <QFont>

#include <ld_division_operator_format.h>

#include "app_common.h"

class QWidget;
class QToolButton;
class QCheckBox;
class QFocusEvent;

/**
 * Class that provides a small popup dialog you can use to select a division symbol.
 */
class APP_PUBLIC_API DivisionStylePopupDialog:public QDialog {
    Q_OBJECT

    public:
        /**
         * Type used to represent a division symbol style.
         */
        typedef Ld::DivisionOperatorFormat::DivisionSymbol DivisionSymbol;

        /**
         * Constructor.
         *
         * \param[in] parent The parent object for this menu.
         */
        DivisionStylePopupDialog(QWidget* parent = Q_NULLPTR);

        ~DivisionStylePopupDialog() override;

    signals:
        /**
         * Signal that is emitted when the user selects a new division symbol.
         *
         * \param[out] newDivisionSymbol The newly selected division symbol.
         */
        void divisionSymbolSelected(DivisionSymbol newDivisionSymbol);

        /**
         * Signal that is emitted when the dialog loses focus.
         */
        void focusLost();

    public slots:
        /**
         * Slot you can use to set the division symbol.
         *
         * \param[in] divisionSymbol The current division symbol.
         */
        void setDivisionSymbol(DivisionSymbol divisionSymbol);

        /**
         * Slot you can use to set the division symbols.
         *
         * \param[in] divisionSymbols The current division symbols.
         */
        void setDivisionSymbol(const QSet<DivisionSymbol>& divisionSymbols);

    protected:
        /**
         * Method that is called when the dialog loses focus.
         *
         * \param[in] event The event that triggered the call to this method.
         */
        void focusOutEvent(QFocusEvent* event) override;

    private slots:
        /**
         * Slot that is triggered when the default button is clicked.
         */
        void defaultButtonClicked();

        /**
         * Slot that is triggered when the "obelus" button is clicked.
         */
        void obelusButtonClicked();

        /**
         * Slot that is triggered when the "fraction" button is clicked.
         */
        void fractionButtonClicked();

    private:
        /**
         * The current default tool button.
         */
        QToolButton* currentDefaultToolButton;

        /**
         * The current "obelus" tool button.
         */
        QToolButton* currentObelusToolButton;

        /**
         * The current "fraction" tool button.
         */
        QToolButton* currentFractionToolButton;
};

#endif
