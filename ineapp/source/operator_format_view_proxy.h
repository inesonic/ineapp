/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref OperatorFormatViewProxy class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef OPERATOR_FORMAT_VIEW_PROXY_H
#define OPERATOR_FORMAT_VIEW_PROXY_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QSharedPointer>

#include <ld_image_format.h>

#include "update_format_command.h"
#include "parenthesis_style_popup_dialog.h"
#include "multiplication_style_popup_dialog.h"
#include "division_style_popup_dialog.h"
#include "logical_not_style_popup_dialog.h"
#include "view_widget.h"
#include "math_view_proxy_base.h"

class QKeyEvent;
class QDialog;

namespace EQt {
    class ProgrammaticView;
    class ProgrammaticMainBWindow;
}

namespace Ld {
    class Element;
    class ElementWithGridChildren;
}

class OperatorFormatDialog;
class BraceConditionalConfigurationDialog;

/**
 * View proxy used to manage the operator format functions.
 */
class OperatorFormatViewProxy:public MathViewProxyBase {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] view   The view this class is proxying for.
         *
         * \param[in] window The main window that this view is within.
         */
        OperatorFormatViewProxy(EQt::ProgrammaticView* view, EQt::ProgrammaticMainWindow* window);

        ~OperatorFormatViewProxy() override;

        /**
         * Method that is called each time the user interface wants this proxy to be controlled.  The method should
         * bind slots or actions from the main window to slots in this class.
         */
        void bindDynamic() final;

        /**
         * Method that is called each time the user interface no longer wants this proxy to be controlled by the
         * main window.  The method should unbind slots or actions from this class.
         */
        void unbindDynamic() final;

    private slots:
        /**
         * Slot that is triggered whenever the element stack for the current view changes.
         *
         * \param[in] aggregationsByCapabilityWithAncestors    A database of aggregations organized by capability.
         *                                                     This version includes ancestors.
         *
         * \param[in] aggregationsByCapabilityWithoutAncestors A database of aggregations organized by capability.
         *                                                     This version excludes any ancestors.
         */
        void formatsUnderCursorChanged(
            const Ld::AggregationsByCapability& aggregationsByCapabilityWithAncestors,
            const Ld::AggregationsByCapability& aggregationsByCapabilityWithoutAncestors
        );

        /**
         * Slot that is triggered when the element stack changes.
         */
        void elementStackChanged();

        /**
         * Slot that is triggered when the user requests the parenthesis popup dialog using the horizontal dock
         * parenthesis style button.
         */
        void horizontalDockParenthesisStyleButtonClicked();

        /**
         * Slot that is triggered when the user requests the parenthesis popup dialog using the vertical dock
         * parenthesis style button.
         */
        void verticalDockParenthesisStyleButtonClicked();

        /**
         * Slot that is triggered when the user requests the multiplication symbol style button is clicked.
         */
        void horizontalDockMultiplicationSymbolStyleButtonClicked();

        /**
         * Slot that is triggered when the user requests the multiplication symbol style button is clicked.
         */
        void verticalDockMultiplicationSymbolStyleButtonClicked();

        /**
         * Slot that is triggered when the user requests the division symbol style button is clicked.
         */
        void horizontalDockDivisionSymbolStyleButtonClicked();

        /**
         * Slot that is triggered when the user requests the division symbol style button is clicked.
         */
        void verticalDockDivisionSymbolStyleButtonClicked();

        /**
         * Slot that is triggered when the user requests the logical not symbol style button is clicked.
         */
        void horizontalDockLogicalNotSymbolStyleButtonClicked();

        /**
         * Slot that is triggered when the user requests the logical not symbol style button is clicked.
         */
        void verticalDockLogicalNotSymbolStyleButtonClicked();

        /**
         * Slot that is triggered when the user requests the operator format dialog.
         */
        void operatorFormatDialogRequested();

        /**
         * Slot that is triggered when the user requests the brace conditional format dialog.
         */
        void braceConditionalFormatDialogRequested();

        /**
         * Slot that is triggered when the matrix insert rows button clicked.
         */
        void matrixInsertRowsButtonClicked();

        /**
         * Slot that is triggered when the matrix insert columns button clicked.
         */
        void matrixInsertColumnsButtonClicked();

        /**
         * Slot that is triggered when the matrix remove row button clicked.
         */
        void matrixRemoveRowButtonClicked();

        /**
         * Slot that is triggered when the matrix remove column button clicked.
         */
        void matrixRemoveColumnButtonClicked();

        /**
         * Slot that is triggered when operator format dialog updates need to be applied.
         *
         * \param[in] dialog Pointer to the dialog containing the desired updates.
         */
        void applyOperatorFormatDialogUpdates(OperatorFormatDialog* dialog);

        /**
         * Slot that is triggered when brace conditional operator dialog updates need to be applied.
         *
         * \param[in] dialog Pointer to the dialog containing the desired updates.
         */
        void applyBraceConditionalOperatorFormatDialogUpdates(BraceConditionalConfigurationDialog* dialog);

        /**
         * Slot that is triggered when the user selects a new parenthesis style.
         *
         * \param[in] newParenthesisStyle The newly selected parenthesis style.
         */
        void parenthesisStyleSelected(ParenthesisStylePopupDialog::ParenthesisStyle newParenthesisStyle);

        /**
         * Slot that is triggered when the user checks or uncheckes the force parenthesis checkbox.
         *
         * \param[in] nowChecked If true, the force parenthesis button is now checked.
         */
        void forceParenthesisButtonChecked(bool nowChecked);

        /**
         * Signal that is emitted when the user selects a new parameter parenthesis style.
         *
         * \param[out] newParenthesisStyle The newly selected parenthesis style.
         */
        void parameterParenthesisStyleSelected(ParenthesisStylePopupDialog::ParenthesisStyle newParenthesisStyle);

        /**
         * Signal that is emitted when the user checks or unchecks the force parameter parenthesis checkbox.
         *
         * \param[out] nowChecked If true, the force parameter parenthesis button is now checked.
         */
        void forceParameterParenthesisButtonChecked(bool nowChecked);

        /**
         * Slot that is triggered when the parenthesis style popup dialog loses focus.
         */
        void parenthesisStyleDialogFocusLost();

        /**
         * Slot that is triggered when the user selects a new multiplication symbol.
         *
         * \param[out] newMultiplicationSymbol The newly selected multiplication symbol.
         */
        void multiplicationSymbolSelected(
            MultiplicationStylePopupDialog::MultiplicationSymbol newMultiplicationSymbol
        );

        /**
         * Slot that is triggered when the multiplication symbol popup dialog loses focus.
         */
        void multiplicationSymbolFocusLost();

        /**
         * Slot that is triggered when the user selects a new division symbol.
         *
         * \param[out] newDivisionSymbol The newly selected division symbol.
         */
        void divisionSymbolSelected(DivisionStylePopupDialog::DivisionSymbol newDivisionSymbol);

        /**
         * Slot that is triggered when the division symbol popup dialog loses focus.
         */
        void divisionSymbolFocusLost();

        /**
         * Slot that is triggered when the user selects a new logical not symbol.
         *
         * \param[out] newLogicalNotSymbol The newly selected logical not symbol.
         */
        void logicalNotSymbolSelected(LogicalNotStylePopupDialog::LogicalNotSymbol newLogicalNotSymbol);

        /**
         * Slot that is triggered when the logical not symbol popup dialog loses focus.
         */
        void logicalNotSymbolFocusLost();

    private:
        /**
         * Method that updates the operator format dock state.
         *
         * \param[in] aggregationsByCapability A map of format aggregations by capability.
         */
        void updateOperatorFormatDockState(const Ld::AggregationsByCapability& aggregationsByCapability);

        /**
         * Method that updates the operator format dialog.
         *
         * \param[in] aggregationsByCapability A map of format aggregations by capability.
         *
         * \param[in] operatorFormatDialog     The operator format dialog to be updated.
         */
        void updateOperatorFormatDialog(
            const Ld::AggregationsByCapability& aggregationsByCapability,
            OperatorFormatDialog*               operatorFormatDialog
        );

        /**
         * Method that updates the brace conditional operator format dialog.
         *
         * \param[in] aggregationsByCapability             A map of format aggregations by capability.
         *
         * \param[in] braceConditionalOperatorFormatDialog The brace conditional operator format dialog to be updated.
         */
        void updateBraceConditionalOperatorFormatDialog(
            const Ld::AggregationsByCapability&  aggregationsByCapability,
            BraceConditionalConfigurationDialog* braceConditionalOperatorFormatDialog
        );

        /**
         * Method that issues an \ref UpdateFormatCommand to the view.
         *
         * \param[in] formatsByElement A map of the formats to be updated, keyed by the element that needs updating.
         */
        void issueUpdateFormatCommand(const UpdateFormatCommand::FormatsByElement& formatsByElement);

        /**
         * Method that connects slots to the current view.
         */
        void connectSignals();

        /**
         * Method that disconnects slots from the current view.
         */
        void disconnectSignals();

        /**
         * Method that restores focus back to the editor.
         */
        void restoreFocus();

        /**
         * Method that displays a popup dialog.
         *
         * \param[in] button       The button used to locate the dialog.
         *
         * \param[in] dialogWindow The window to be positioned.
         */
        void displayDialog(const QToolButton* button, QDialog* dialogWindow);

        /**
         * Method that obtains the grid element and location in the grid element currently pointed to by the cursor.
         *
         * \param[in]  elementName The type name of the element we're searching for.
         *
         * \param[out] rowIndex    The zero based row index into the grid element.
         *
         * \param[out] columnIndex The zero based column index into the grid element.
         *
         * \return Returns a shared pointer to the grid element of interest.
         */
        QSharedPointer<Ld::ElementWithGridChildren> locationInGridElement(
            const QString& elementName,
            unsigned long& rowIndex,
            unsigned long& columnIndex
        ) const;

        /**
         * Pointer to the currently active operator format dialog.
         */
        OperatorFormatDialog* currentOperatorFormatDialog;

        /**
         * Pointer to the currently active brace conditional configuration dialog.
         */
        BraceConditionalConfigurationDialog* currentBraceConditionalConfigurationDialog;

        /**
         * Pointer to the current parenthesis style popup dialog.
         */
        ParenthesisStylePopupDialog* currentParenthesisStylePopupDialog;

        /**
         * Pointer to the current multiplication style popup dialog.
         */
        MultiplicationStylePopupDialog* currentMultiplicationStylePopupDialog;

        /**
         * Pointer to the current division style popup dialog.
         */
        DivisionStylePopupDialog* currentDivisionStylePopupDialog;

        /**
         * Pointer to the current logical not style popup dialog.
         */
        LogicalNotStylePopupDialog* currentLogicalNotStylePopupDialog;

        /**
         * The most recently reported aggregations by format capability.
         */
        Ld::AggregationsByCapability currentAggregationsByCapability;
};

#endif
