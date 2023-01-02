/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ImageViewProxy class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef IMAGE_VIEW_PROXY_H
#define IMAGE_VIEW_PROXY_H

#include <QObject>
#include <QMap>
#include <QList>

#include <ld_image_format.h>

#include "update_format_command.h"
#include "view_widget.h"
#include "view_proxy.h"

class QKeyEvent;

namespace EQt {
    class ProgrammaticView;
    class ProgrammaticMainBWindow;
}

namespace Ld {
    class Element;
}

class ImageFormatDialog;

/**
 * View proxy used to manage the page formatting functions.  These functions include:
 *
 *     * Launching and managing the page format dialog
 *     * Managing the page format dock widget functions
 */
class ImageViewProxy:public ViewProxy {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] view   The view this class is proxying for.
         *
         * \param[in] window The main window that this view is within.
         */
        ImageViewProxy(EQt::ProgrammaticView* view, EQt::ProgrammaticMainWindow* window);

        ~ImageViewProxy() override;

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
        void insertImageRequested();

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
        void imageFormatDialogApplyUpdates(ImageFormatDialog* dialog);

        /**
         * Slot that is triggered when the rotate image button is clicked.
         */
        void rotateImageRequested();

        /**
         * Slot that is triggered when the user requests no horizontal scaling.
         */
        void widthNoScalingModeRequested();

        /**
         * Slot that is triggered when the user requests the horizontal axis maintain aspect ratio.
         */
        void widthMaintainAspectRatioRequested();

        /**
         * Slot that is triggered when the user requests the horizontal axis scale based on a percentage.
         */
        void widthPercentageScalingModeRequested();

        /**
         * Slot that is triggered when the user requests the horizontal axis scale to a specific size.
         */
        void widthDimensionScalingModeRequested();

        /**
         * Slot that is triggered when the user requests no vertical scaling.
         */
        void heightNoScalingModeRequested();

        /**
         * Slot that is triggered when the user requests the vertical axis maintain aspect ratio.
         */
        void heightMaintainAspectRatioRequested();

        /**
         * Slot that is triggered when the user requests the vertical axis scale based on a percentage.
         */
        void heightPercentageScalingModeRequested();

        /**
         * Slot that is triggered when the user requests the vertical axis scale to a specific size.
         */
        void heightDimensionScalingModeRequested();

        /**
         * Slot that is triggered when the horizontal percentage value changes.
         */
        void widthPercentageScalingChanged();

        /**
         * Slot that is triggered when the horizontal dimension value changes.
         */
        void widthDimensionScalingChanged();

        /**
         * Slot that is triggered when the vertical percentage value changes.
         */
        void heightPercentageScalingChanged();

        /**
         * Slot that is triggered when the vertical dimension value changes.
         */
        void heightDimensionScalingChanged();

    private:
        /**
         * Method that updates the character format dock state.
         *
         * \param[in] aggregationsByCapability A map of format aggregations by capability.
         */
        void updateImageFormatDockState(const Ld::AggregationsByCapability& aggregationsByCapability);

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
         * Method that updates the horizontal or vertical image scaling controls.
         *
         * \param[in] scalingModes                  A set holding all the scaling modes of interest.
         *
         * \param[in] values                        A set holding all the values of interest.
         *
         * \param[in] horizontal                    If true, we are adjusting horizontal settings.  If false, we are
         *                                          adjusting vertical  settings.
         *
         * \param[in] disableAspectRatioScalingMode If true, the aspect ratio scaling mode for this axis should be
         *                                          disabled.  If false, the aspect ratio scaling mode for this
         *                                          axis should be enabled.
         */
        void updateScalingControl(
            const QSet<Ld::ImageFormat::ImageScalingMode>& scalingModes,
            const QSet<float>&                             values,
            bool                                           horizontal,
            bool                                           disableAspectRatioScalingMode
        );

        /**
         * Method that is called to update the image dialog when the current aggregations are updated.
         *
         * \param[in] dialog The dialog instance to be updated.
         */
        void updateImageFormatDialog(ImageFormatDialog* dialog);

        /**
         * Method that disables all image format controls.
         */
        void disableFormatControls();

        /**
         * Method that restores focus back to the editor.
         */
        void restoreFocus();

        /**
         * Pointer to the currently active image format dialog.
         */
        ImageFormatDialog* currentImageFormatDialog;

        /**
         * The most recently reported aggregations by format capability.
         */
        Ld::AggregationsByCapability currentAggregationsByCapability;
};

#endif
