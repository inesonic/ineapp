/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ValueFieldViewProxy class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef VALUE_FIELD_VIEW_PROXY_H
#define VALUE_FIELD_VIEW_PROXY_H

#include <QObject>

#include <ld_element_structures.h>
#include <ld_complex_data_type_format.h>

#include "page_list.h"
#include "update_format_command.h"
#include "view_widget.h"
#include "view_proxy.h"

class QKeyEvent;

namespace EQt {
    class ProgrammaticView;
    class ProgrammaticMainBWindow;
}

class NumericValueFormatDialog;
class InsertElementCommand;

/**
 * View proxy used to manage the basic math functions.
 */
class ValueFieldViewProxy:public ViewProxy {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] view   The view this class is proxying for.
         *
         * \param[in] window The main window that this view is within.
         */
        ValueFieldViewProxy(EQt::ProgrammaticView* view, EQt::ProgrammaticMainWindow* window);

        ~ValueFieldViewProxy() override;

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
         * Slot that is triggered when the insert value field button is clicked.
         */
        void insertValueFieldRequested();

        /**
         * Slot that is triggered whe the format value field button is clicked.
         */
        void formatValueFieldRequested();

        /**
         * Slot that is triggered when the numeric value format dialog's apply button is clicked.
         *
         * \param[in] dialog The dialog containing the updates to be applied.
         */
        void formatValueFieldDialogApplyUpdates(NumericValueFormatDialog* dialog);

        /**
         * Slot that is triggered when the user changes the number of integer digits.
         */
        void integerNumberDigitsChanged();

        /**
         * Slot that is triggered when the user changes the integer base.
         */
        void integerBaseChanged();

        /**
         * Slot that is triggered when the integer number style is changed.
         */
        void integerNumberStyleChanged();

        /**
         * Slot that is triggered when the real/complex number precision is changed.
         */
        void realNumberPrecisionChanged();

        /**
         * Slot that is trigered when the real/complex number style is changed.
         */
        void realNumberStyleChanged();

        /**
         * Slot that is triggered when the real/complex number multiplier is changed.
         */
        void realNumberMultiplierChanged();

        /**
         * Slot that is triggered when the imaginary unit "i" is selected.
         */
        void complexImaginaryUnitISelected();

        /**
         * Slot that is triggered when the imaginary unit "j" is selected.
         */
        void complexImaginaryUnitJSelected();

    private:
        /**
         * Method that is called to connect signals.
         */
        void connectSignals();

        /**
         * Method that is called to disconnect signals.
         */
        void disconnectSignals();

        /**
         * Method that is called to update the dock state.
         *
         * \param[in] aggregationsByCapability The current aggregations by format capability.
         */
        void updateValueFieldDockState(const Ld::AggregationsByCapability& aggregationsByCapability);

        /**
         * Method that updates a numerical value field dialog based on the current aggregation.
         *
         * \param[in] numericalValueFormatDialog The numerical value format dialog to be updated.
         */
        void updateNumericValueFormatDialog(NumericValueFormatDialog* numericalValueFormatDialog);

        /**
         * Method that updates the value field imaginary unit.
         *
         * \param[in] newValue The new value.
         */
        void updateImaginaryUnit(Ld::ComplexDataTypeFormat::ImaginaryUnit newValue);

        /**
         * Method that issues an \ref UpdateFormatCommand to the view.
         *
         * \param[in] formatsByElement A map of the formats to be updated, keyed by the element that needs updating.
         */
        void issueUpdateFormatCommand(const UpdateFormatCommand::FormatsByElement& formatsByElement);

        /**
         * Method you can call to insert a new element.
         *
         * \param[in] newElement The new element to be inserted.
         */
        void insertElement(Ld::ElementPointer newElement);

        /**
         * Method that restores focus back to the editor.
         */
        void restoreFocus();

        /**
         * The most recently reported aggregations by format capability.
         */
        Ld::AggregationsByCapability currentAggregationsByCapability;

        /**
         * Pointer to the currently active image format dialog.
         */
        NumericValueFormatDialog* currentNumericFormatDialog;
};

#endif
