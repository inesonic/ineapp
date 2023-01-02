/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref LogicalNotStylePopupDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef LOGICAL_NOT_STYLE_POPUP_DIALOG_H
#define LOGICAL_NOT_STYLE_POPUP_DIALOG_H

#include <QDialog>
#include <QPoint>
#include <QSet>
#include <QHash>
#include <QFont>

#include <ld_logical_unary_not_operator_format.h>

#include "app_common.h"

class QWidget;
class QToolButton;
class QCheckBox;
class QFocusEvent;

/**
 * Class that provides a small popup dialog you can use to select a logical not symbol.
 */
class APP_PUBLIC_API LogicalNotStylePopupDialog:public QDialog {
    Q_OBJECT

    public:
        /**
         * Type used to represent a logical not symbol style.
         */
        typedef Ld::LogicalUnaryNotOperatorFormat::LogicalNotSymbol LogicalNotSymbol;

        /**
         * Constructor.
         *
         * \param[in] parent The parent object for this menu.
         */
        LogicalNotStylePopupDialog(QWidget* parent = Q_NULLPTR);

        ~LogicalNotStylePopupDialog() override;

    signals:
        /**
         * Signal that is emitted when the user selects a new logical not symbol.
         *
         * \param[out] newLogicalNotSymbol The newly selected logical not symbol.
         */
        void logicalNotSymbolSelected(LogicalNotSymbol newLogicalNotSymbol);

        /**
         * Signal that is emitted when the dialog loses focus.
         */
        void focusLost();

    public slots:
        /**
         * Slot you can use to set the logical not symbol.
         *
         * \param[in] LogicalNotSymbol The current logical not symbol.
         */
        void setLogicalNotSymbol(LogicalNotSymbol LogicalNotSymbol);

        /**
         * Slot you can use to set the logical not symbols.
         *
         * \param[in] LogicalNotSymbols The current logical not symbols.
         */
        void setLogicalNotSymbol(const QSet<LogicalNotSymbol>& LogicalNotSymbols);

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
         * Slot that is triggered when the "negation" button is clicked.
         */
        void negationButtonClicked();

        /**
         * Slot that is triggered when the "overbar" button is clicked.
         */
        void overbarButtonClicked();

        /**
         * Slot that is triggered when the "tilde" button is clicked.
         */
        void tildeButtonClicked();

    private:
        /**
         * The current default tool button.
         */
        QToolButton* currentDefaultToolButton;

        /**
         * The current "negation" tool button.
         */
        QToolButton* currentNegationToolButton;

        /**
         * The current "overbar" tool button.
         */
        QToolButton* currentOverbarToolButton;

        /**
         * The current "tilde" tool button.
         */
        QToolButton* currentTildeToolButton;
};

#endif
