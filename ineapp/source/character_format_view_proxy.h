/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref CharacterFormatViewProxy class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef CHARACTER_FORMAT_VIEW_PROXY_H
#define CHARACTER_FORMAT_VIEW_PROXY_H

#include <QObject>
#include <QMap>
#include <QList>

#include <ld_format_structures.h>
#include <ld_font_format.h>
#include <ld_character_format.h>

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

class CharacterFormatDialog;

/**
 * View proxy used to manage the character formatting functions.  These functions include:
 *
 *     * Basic character format menu items
 *     * Basic character formatting dock widget functions.
 */
class CharacterFormatViewProxy:public ViewProxy {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] view   The view this class is proxying for.
         *
         * \param[in] window The main window that this view is within.
         */
        CharacterFormatViewProxy(EQt::ProgrammaticView* view, EQt::ProgrammaticMainWindow* window);

        ~CharacterFormatViewProxy() override;

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

        /**
         * Method that is called to perform binding of static actions to slots used to perform those actions.  This
         * method is called once each time a new view is created.
         */
        void bind() final;

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
         * Slot that is triggered when the font family is changed.
         */
        void fontFamilyChanged();

        /**
         * Slot that is triggered when the font size is changed.
         */
        void fontSizeChanged();

        /**
         * Slot that is triggered when the user clicks the bold tool button.
         */
        void boldToolButtonClicked();

        /**
         * Slot that is triggered when the user selects italics.
         */
        void italicsSelected();

        /**
         * Slot that is triggered when the user selects underline.
         */
        void underlineSelected();

        /**
         * Slot that is triggered when the user selects overline.
         */
        void overlineSelected();

        /**
         * Slot that is triggered when the user selects strikethrough.
         */
        void strikethroughSelected();

        /**
         * Slot that is triggered when the user selects subscript.  This slot will also restore the normal baseline if
         * all the fonts in the aggregation indicate subscript.
         */
        void subscriptSelected();

        /**
         * Slot that is triggered when the user selects superscript.  This slot will also restore the normal baseline
         * if all the fonts in the aggregation indicate superscript.
         */
        void superscriptSelected();

        /**
         * Slot that is triggered when the user selects normal baseline.
         */
        void normalBaselineSelected();

        /**
         * Slot that is triggered when the user selects a new font color.
         */
        void fontColorSelected();

        /**
         * Slot that is triggered when the user selects a new font background color.
         */
        void fontBackgroundColorSelected();

        /**
         * Slot that is triggered when the user requests the character format dialog.
         */
        void characterFormatDialogRequested();

        /**
         * Slot that is triggered when the character format dialog Apply button is pressed or when auto-update is
         * enabled and a change occurs within the dialog.
         *
         * \param[in] dialog A pointer to the dialog reporting the updates.
         */
        void characterFormatDialogApplyUpdates(CharacterFormatDialog* dialog);

    private:
        /**
         * Type used to represent a list of font formats.
         */
        typedef QList<QSharedPointer<Ld::FontFormat>> CharacterFormats;

        /**
         * Method that updates the character format dock state.
         *
         * \param[in] aggregationsByCapability A map of format aggregations by capability.
         */
        void updateCharacterFormatDockState(const Ld::AggregationsByCapability& aggregationsByCapability);

        /**
         * Method that is called to connect signals.
         */
        void connectSignals();

        /**
         * Method that is called to disconnect signals.
         */
        void disconnectSignals();

        /**
         * Method that is called to update font weights.
         *
         * \param[in] newFontWeight The new font weight to apply.
         */
        void applyFontWeight(Ld::FontFormat::Weight newFontWeight);

        /**
         * Method that is called to apply a new font position.
         *
         * \param[in] newPosition The new font position.
         */
        void applyPosition(Ld::CharacterFormat::Position newPosition);

        /**
         * Method that obtains a single format to be edited when a cached format is required.
         *
         * \return Returns a shared pointer to a cached format that can be modified.  A null pointer is returned if the
         *         cursor state indicates that we need to directly update formats through
         *         CharacterFormatViewProxy::processUpdates.
         */
        QSharedPointer<Ld::FontFormat> getCachableFormat() const;

        /**
         * Method that updates the cached format.
         *
         * \param[in] format The format to be cached in the view for new text entry events.
         */
        void setCachedFormat(QSharedPointer<Ld::FontFormat> format);

        /**
         * Method that processes a map of updated formats, by element.  This method is not expected to be called if
         * the cursor points into text in a element that can be split at the cursor position.
         *
         * \param[in] formatsByElement A map of formats to assign to each element.  Note that an empty map is allowed
         *                             and will simply cause this call to return, performing no actions.
         */
        void processUpdates(const UpdateFormatCommand::FormatsByElement& formatsByElement);

        /**
         * Method that issues an Ld::UpdatePageFormatCommand to the view.
         *
         * \param[in] formatsByElement A map of the formats to be updated, keyed by the element that needs updating.
         */
        void issueUpdateFormatCommand(const UpdateFormatCommand::FormatsByElement& formatsByElement);

        /**
         * Method that is called to update a single format from a dialog.
         *
         * \param[in] dialog The dialog instance to use to update the format.
         *
         * \param[in] format The format to be updated.
         *
         * \return Returns true if the format was actually updated.  Returns false if no updates were performed.
         */
        static bool updateFormatFromDialog(CharacterFormatDialog* dialog, Ld::FormatPointer format);

        /**
         * Method that is called to update a character format dialog using the current aggregation data.
         *
         * \param[in] dialog The dialog that should be updated.
         */
        void updateCharacterFormatDialog(CharacterFormatDialog* dialog);

        /**
         * Method that restores focus back to the editor.
         */
        void restoreFocus();

        /**
         * Pointer to the currently active character format dialog.
         */
        CharacterFormatDialog* currentCharacterFormatDialog;

        /**
         * The most recently reported aggregations by format capability.
         */
        Ld::AggregationsByCapability currentAggregationsByCapability;
};

#endif
