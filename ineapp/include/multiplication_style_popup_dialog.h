/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref MultiplicationStylePopupDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef MULTIPLICATION_STYLE_POPUP_DIALOG_H
#define MULTIPLICATION_STYLE_POPUP_DIALOG_H

#include <QDialog>
#include <QPoint>
#include <QSet>
#include <QHash>
#include <QFont>

#include <ld_multiplication_operator_format.h>

#include "app_common.h"

class QWidget;
class QToolButton;
class QCheckBox;
class QFocusEvent;

/**
 * Class that provides a small popup dialog you can use to select a multiplication symbol.
 */
class APP_PUBLIC_API MultiplicationStylePopupDialog:public QDialog {
    Q_OBJECT

    public:
        /**
         * Type used to represent a multiplication symbol style.
         */
        typedef Ld::MultiplicationOperatorFormat::MultiplicationSymbol MultiplicationSymbol;

        /**
         * Constructor.
         *
         * \param[in] parent The parent object for this menu.
         */
        MultiplicationStylePopupDialog(QWidget* parent = Q_NULLPTR);

        ~MultiplicationStylePopupDialog() override;

    signals:
        /**
         * Signal that is emitted when the user selects a new multiplication symbol.
         *
         * \param[out] newMultiplicationSymbol The newly selected multiplication symbol.
         */
        void multiplicationSymbolSelected(MultiplicationSymbol newMultiplicationSymbol);

        /**
         * Signal that is emitted when the dialog loses focus.
         */
        void focusLost();

    public slots:
        /**
         * Slot you can use to set the multiplication symbol.
         *
         * \param[in] multiplicationSymbol The current multiplication symbol.
         */
        void setMultiplicationSymbol(MultiplicationSymbol multiplicationSymbol);

        /**
         * Slot you can use to set the multiplication symbols.
         *
         * \param[in] multiplicationSymbols The current multiplication symbols.
         */
        void setMultiplicationSymbol(const QSet<MultiplicationSymbol>& multiplicationSymbols);

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
         * Slot that is triggered when the "space" button is clicked.
         */
        void spaceButtonClicked();

        /**
         * Slot that is triggered when the "dot" button is clicked.
         */
        void dotButtonClicked();

        /**
         * Slot that is triggered when the "cross" button is clicked.
         */
        void crossButtonClicked();

        /**
         * Slot that is triggered when the "circle" button is clicked.
         */
        void circleButtonClicked();

    private:
        /**
         * The current default tool button.
         */
        QToolButton* currentDefaultToolButton;

        /**
         * The current "space" tool button.
         */
        QToolButton* currentSpaceToolButton;

        /**
         * The current "dot" tool button.
         */
        QToolButton* currentDotToolButton;

        /**
         * The current "cross" tool button.
         */
        QToolButton* currentCrossToolButton;

        /**
         * The current "circle" tool button.
         */
        QToolButton* currentCircleToolButton;
};

#endif
