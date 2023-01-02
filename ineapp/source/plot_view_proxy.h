/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlotViewProxy class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLOT_VIEW_PROXY_H
#define PLOT_VIEW_PROXY_H

#include <QObject>
#include <QMap>
#include <QList>

#include <ld_image_format.h>
#include <ld_aggregations_by_capability.h>

#include "update_format_command.h"
#include "insert_element_command.h"
#include "view_proxy.h"

class QKeyEvent;

namespace EQt {
    class ProgrammaticView;
    class ProgrammaticMainBWindow;
}

namespace Ld {
    class Element;
    class PlotElement;
    class PlotFormat;
}

class PlotSettingsWidget;
class PlotFormatDialog;

/**
 * View proxy used to manage the plot formatting and insertion functions.
 */
class PlotViewProxy:public ViewProxy {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] view   The view this class is proxying for.
         *
         * \param[in] window The main window that this view is within.
         */
        PlotViewProxy(EQt::ProgrammaticView* view, EQt::ProgrammaticMainWindow* window);

        ~PlotViewProxy() override;

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
         * Slot that is triggered when the insert image dialog is requested.
         */
        void insertPlotRequested();

        /**
         * Slot that is triggered when the format image dialog is requested.
         */
        void formatDialogRequested();

        /**
         * Slot that is triggered when the page format dialog Apply button is pressed or when auto-update is enabled
         * and a change occurs within the dialog.
         *
         * \param[in] dialog A pointer to the dialog reporting the updates.
         */
        void plotFormatDialogApplyUpdates(PlotFormatDialog* dialog);

    private:
        /**
         * Method that connects slots to the current view.
         */
        void connectSignals();

        /**
         * Method that disconnects slots from the current view.
         */
        void disconnectSignals();


        /**
         * Method that updates a plot format dialog based on the currently reported format data.
         *
         * \param[in] plotFormatDialog The dialog to be updated.
         */
        void updatePlotFormatDialog(PlotFormatDialog* plotFormatDialog);

        /**
         * Method that uypdates a Ld::PlotElement and Ld::PlotFormat from the contents of a \ref PlotSettingsWidget
         * instance.
         *
         * \param[in] plotElement        The plot element to be updated.
         *
         * \param[in] plotFormat         The plot format to be updated.
         *
         * \param[in] plotSettingsWidget The plot settings widget holding the values to be udpated.
         *
         * \return Returns true if the element has been updated.  Returns false if the element is untouched.
         */
        static bool updatePlotElementAndFormat(
            QSharedPointer<Ld::PlotElement> plotElement,
            QSharedPointer<Ld::PlotFormat>  plotFormat,
            const PlotSettingsWidget*       plotSettingsWidget
        );

        /**
         * Method you can call to insert a new element.  This version allows you to specify where the cursor should
         * be placed after inserting the element.
         *
         * \param[in] newElement          The new element to be inserted.
         *
         * \param[in] replaceElement      If true, the currently selected element should be replaced.
         *
         * \param[in] finalCursorPosition The desired final cursor position.
         */
        void insertElement(
            Ld::ElementPointer                        newElement,
            bool                                      replaceElement,
            InsertElementCommand::FinalCursorPosition finalCursorPosition
        );

        /**
         * Method that issues an \ref UpdateFormatCommand to the view.
         *
         * \param[in] formatsByElement A map of the formats to be updated, keyed by the element that needs updating.
         */
        void issueUpdateFormatCommand(const UpdateFormatCommand::FormatsByElement& formatsByElement);

        /**
         * Method that restores focus back to the editor.
         */
        void restoreFocus();

        /**
         * The most recently reported aggregations by format capability.
         */
        Ld::AggregationsByCapability currentAggregationsByCapability;

        /**
         * Pointer to the currently active plot format dialog.
         */
        PlotFormatDialog* currentPlotFormatDialog;
};

#endif
