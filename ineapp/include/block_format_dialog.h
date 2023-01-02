/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref BlockFormatDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef BLOCK_FORMAT_DIALOG_H
#define BLOCK_FORMAT_DIALOG_H

#include <QSet>
#include <QSharedPointer>

#include <eqt_programmatic_dialog.h>

#include <ld_justified_block_format.h>
#include <ld_unordered_list_paragraph_format.h>

#include "app_common.h"

class QWidget;
class QGroupBox;
class QComboBox;

namespace Ld {
    class PageFormat;
};

/**
 * Dialog used to adjust one or more block format instances.
 */
class APP_PUBLIC_API BlockFormatDialog:public EQt::ProgrammaticDialog {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        BlockFormatDialog(QWidget* parent = Q_NULLPTR);

        ~BlockFormatDialog() override;

        /**
         * Loads settings to be used by the dialog.
         *
         * \param[in] groupName The name of the settings group to use for the dialog.
         */
        void loadSettings(const QString& groupName) final;

        /**
         * Saves settings used by the dialog.
         *
         * \param[in] groupName The name of the settings group to use for the dialog.
         */
        void saveSettings(const QString& groupName) final;

        /**
         * Method you can use to enable or disable the auto-apply function.
         *
         * \param[in] nowAutoApply If true, auto-apply will be enabled.  If false, auto-apply will be disabled.
         */
        void setAutoApplyEnabled(bool nowAutoApply = true);

        /**
         * Method you can use to disable or enable the auto-apply function.
         *
         * \param[in] nowAutoApplyDisabled If true, auto-apply will be disabled.  If false, auto-apply will be enabled.
         */
        void setAutoApplyDisabled(bool nowAutoApplyDisabled = true);

        /**
         * Method you can use to determine if auto-apply is enabled.
         *
         * \return Returns true if auto-apply is enabled.  Returns false if auto-apply is disabled.
         */
        bool autoApplyEnabled() const;

        /**
         * Method you can use to determine if auto-apply is disabled.
         *
         * \return Returns true if auto-apply is disabled.  Returns false if auto-apply is enabled.
         */
        bool autoApplyDisabled() const;

        /**
         * Method you can use to set the default format to assume for the dialog.
         *
         * \param[in] newDefaultPageFormat The new default page type to assume when rendering this dialog.
         */
        void setDefaultPageFormat(QSharedPointer<Ld::PageFormat> newDefaultPageFormat);

        /**
         * Method you can use to determine the page format to assume for the dialog.
         *
         * \return Returns a shared pointer to the format assumed by the dialog.
         */
        QSharedPointer<Ld::PageFormat> defaultPageFormat() const;

        /**
         * Method you can use to determine if list settings are being presented.
         *
         * \return Returns true if list settings are being presented to the user.  Returns false if list settings are
         *         being hidden.
         */
        bool listSettingsVisible() const;

        /**
         * Method you can use to determine if list settings are being hidden.
         *
         * \return Returns true if list settings are being hidden from the user.  Returns false if list settings are
         *         being shown.
         */
        bool listSettingsHidden() const;

        /**
         * Method you can use to determine if list settings are modifiable.
         *
         * \return Returns true if the user is allowed to modify the list settings.  Returns false if the user is not
         *         able to modify the list settings.
         */
        bool listSettingsEnabled() const;

        /**
         * Method you can use to determine if list settings are not modifiable.
         *
         * \return Returns true if the user is not allowed to modify the list settings.  Returns false if the user is
         *         allowed to modify the list settings.
         */
        bool listSettingsDisabled() const;

        /**
         * Method you can use to determine if the diagram is showing a bullet/number block.
         *
         * \return Returns true if the paragraph diagram is showing a bullet/number block.  Returns false if the
         *         diagram is not showing a bullet/number block.
         */
        bool bulletNumberVisible() const;

        /**
         * Method you can use to determine if the diagram is hiding the bullet/number block.
         *
         * \return Returns true if the paragraph diagram is not showing a bullet/number block.  Returns false if the
         *         diagram is currently showing a bullet/number block.
         */
        bool bulletNumberHidden() const;

        /**
         * Method you can use to obtain the current justification.
         *
         * \return Returns the current justification mode.
         */
        Ld::JustifiedBlockFormat::Justification justification() const;

        /**
         * Method you can use to obtain the current left indentation value.
         *
         * \return Returns the paragraph left indentation value.
         */
        float leftIndentation() const;

        /**
         * Method you can use to obtain the current list indentation value.
         *
         * \return Returns the paragraph list indentation value.
         */
        float listIndentation() const;

        /**
         * Method you can use to obtain the current right indentation value.
         *
         * \return Returns the paragraph right indentation value.
         */
        float rightIndentation() const;

        /**
         * Method you can use to obtain the current top spacing value.
         *
         * \return Returns the paragraph top spacing.
         */
        float topSpacing() const;

        /**
         * Method you can use to obtain the current bottom spacing value.
         *
         * \return Returns the paragraph bottom spacing.
         */
        float bottomSpacing() const;

        /**
         * Method you can use to obtain the current first line left indentation value.
         *
         * \return Returns the first line left indentation value.
         */
        float firstLineIndentation() const;

        /**
         * Method you can use to obtain the currently displayed line spacing.
         *
         * \return Returns the current line spacing.  A value of 1.0 indicates single space, a value of 1.5
         *         indicates 1 1/2 line spacing.  A value of 2.0 indicates double spacing.
         */
        float lineSpacing() const;

        /**
         * Method that determines if the left indentation value is cleared.
         *
         * \return Returns true if the parameter is cleared.  Returns false if the paramter is set.
         */
        bool isLeftIndentationCleared() const;

        /**
         * Method that determines if the list indentation value is cleared.
         *
         * \return Returns true if the parameter is cleared.  Returns false if the paramter is set.
         */
        bool isListIndentationCleared() const;

        /**
         * Method that determines if the right indentation value is cleared.
         *
         * \return Returns true if the parameter is cleared.  Returns false if the paramter is set.
         */
        bool isRightIndentationCleared() const;

        /**
         * Method that determines if the top spacing value is cleared.
         *
         * \return Returns true if the parameter is cleared.  Returns false if the paramter is set.
         */
        bool isTopSpacingCleared() const;

        /**
         * Method that determines if the bottom spacing value is cleared.
         *
         * \return Returns true if the parameter is cleared.  Returns false if the paramter is set.
         */
        bool isBottomSpacingCleared() const;

        /**
         * Method that determines if the first line left indentation value is cleared.
         *
         * \return Returns true if the parameter is cleared.  Returns false if the paramter is set.
         */
        bool isFirstLineIndentationCleared() const;

        /**
         * Method that determines if the line spacing value is cleared.
         *
         * \return Returns true if the parameter is cleared.  Returns false if the paramter is set.
         */
        bool isLineSpacingCleared() const;

        /**
         * Method that determines if the left indentation value is set.
         *
         * \return Returns true if the parameter is set.  Returns false if the paramter is cleared.
         */
        bool isLeftIndentationSet() const;

        /**
         * Method that determines if the lost indentation value is set.
         *
         * \return Returns true if the parameter is set.  Returns false if the paramter is cleared.
         */
        bool isListIndentationSet() const;

        /**
         * Method that determines if the right indentation value is set.
         *
         * \return Returns true if the parameter is set.  Returns false if the paramter is cleared.
         */
        bool isRightIndentationSet() const;

        /**
         * Method that determines if the top spacing value is set.
         *
         * \return Returns true if the parameter is set.  Returns false if the paramter is cleared.
         */
        bool isTopSpacingSet() const;

        /**
         * Method that determines if the bottom spacing value is set.
         *
         * \return Returns true if the parameter is set.  Returns false if the paramter is cleared.
         */
        bool isBottomSpacingSet() const;

        /**
         * Method that determines if the first line left indentation value is set.
         *
         * \return Returns true if the parameter is set.  Returns false if the paramter is cleared.
         */
        bool isFirstLineIndentationSet() const;

        /**
         * Method that determines if the line spacing value is set.
         *
         * \return Returns true if the parameter is set.  Returns false if the paramter is cleared.
         */
        bool isLineSpacingSet() const;

        /**
         * Method that can be called to populate the dialog's widgets.
         */
        void populate() final;

    signals:
        /**
         * Signal that is emitted when the Apply button is clicked on Windows or when any update occurs on MacOS.
         *
         * \param[out] thisDialog A pointer to this dialog.  You can use this parameter to obtain the current values.
         */
        void apply(BlockFormatDialog* thisDialog);

    public slots:
        /**
         * Slot you can use to show/hide list settings.
         *
         * \param[in] nowVisible If true, list controls and settings will be visible.  If false, list controls and
         *                       settings will be hidden.
         */
        void setListSettingsVisible(bool nowVisible = true);

        /**
         * Slot you can use to hide/show list settings.
         *
         * \param[in] nowHidden If true, list controls and settings will be hidden.  If false, list controls and
         *                      settings will be bisible.
         */
        void setListSettingsHidden(bool nowHidden = true);

        /**
         * Slot you can use to enable or disable the list setting controls.
         *
         * \param[in] nowEnabled If true, the list settings controls will be enabled.  If false, list settings controls
         *                       will be disabled.
         */
        void setListSettingsEnabled(bool nowEnabled = true);

        /**
         * Slot you can use to disable or enable the list settings controls.
         *
         * \param[in] nowDisabled If true, the list settings controls will be disabled.  If false, the list settings
         *                        controls will be enabled.
         */
        void setListSettingsDisabled(bool nowDisabled = true);

        /**
         * Slot you can use to show the bullet/number block in the diagram.
         *
         * \param[in] nowVisible If true, the bullet/number block will be visible.  If false, the bullet/number block
         *                       will be hidden.
         */
        void setBulletNumberVisible(bool nowVisible = true);

        /**
         * Slot you can use to show the bullet/number block in the diagram.
         *
         * \param[in] nowHidden If true, the bullet/number block will be hidden.  If false, the bullet/number block
         *                      will be visible.
         */
        void setBulletNumberHidden(bool nowHidden = true);

        /**
         * Slot you can use to set the justification.
         *
         * \param[in] newJustification The new justification mode.
         */
        void setJustification(Ld::JustifiedBlockFormat::Justification newJustification);

        /**
         * Slot you can use to set one or multiple justification settings at once.
         *
         * \param[in] newJustification The new justification mode.
         */
        void setJustification(const QSet<Ld::JustifiedBlockFormat::Justification>& newJustification);

        /**
         * Slot you can use to set the justification to left justified.
         */
        void setLeftJustified();

        /**
         * Slot you can use to set the justification to centered.
         */
        void setCentered();

        /**
         * Slot you can use to set the justification to right justified.
         */
        void setRightJustified();

        /**
         * Slot you can use to set the justification to fully justified.
         */
        void setJustified();

        /**
         * Slot you can use to set the new left indentation.
         *
         * \param[in] newLeftIndentation The new left indentation value.
         */
        void setLeftIndentation(float newLeftIndentation);

        /**
         * Slot you can use to set the new list indentation.
         *
         * \param[in] newListIndentation The new list indentation value.
         */
        void setListIndentation(float newListIndentation);

        /**
         * Slot you can use to set the new right indentation.
         *
         * \param[in] newRightIndentation The new right indentation value.
         */
        void setRightIndentation(float newRightIndentation);

        /**
         * Slot you can use to set the new top spacing.
         *
         * \param[in] newTopSpacing The top spacing value.
         */
        void setTopSpacing(float newTopSpacing);

        /**
         * Slot you can use to set the new bottom spacing.
         *
         * \param[in] newBottomSpacing The bottom spacing value.
         */
        void setBottomSpacing(float newBottomSpacing);

        /**
         * Slot you can use to set the first line left indentation value.
         *
         * \param[in] newFirstLineIndentation The new first line indentation value.
         */
        void setFirstLineIndentation(float newFirstLineIndentation);

        /**
         * Slot you can use to set the line spacing.
         *
         * \param[in] newLineSpacing The new line spacing value.  A value of 1.0 indicates single spacing.  A value
         *                           of 1.5 indicates 1 1/2 line spacing.  A value of 2.0 indicates double spacing.
         */
        void setLineSpacing(float newLineSpacing);

        /**
         * Slot you can use to clear the left indentation value.
         */
        void clearLeftIndentation();

        /**
         * SLot you can use to clear the list indentation.
         */
        void clearListIndentation();

        /**
         * Slot you can use to clear the right indentation value.
         */
        void clearRightIndentation();

        /**
         * Slot you can use to clear the top spacing value.
         */
        void clearTopSpacing();

        /**
         * Slot you can use to clear the bottom spacing value.
         */
        void clearBottomSpacing();

        /**
         * Slot you can use to clear the first line left indentation value.
         */
        void clearFirstLineIndentation();

        /**
         * Slot you can use to clear the line spacing value.
         */
        void clearLineSpacing();

    protected:
        /**
         * Performs binding of signals to slots.  This method will be executed by the
         * EQt::ProgrammaticDialog::runBuilders method.
         *
         * You can overload this method if you want to bind signals to slots prior to the proxy classes.
         */
        void bind() final;

    private slots:
        /**
         * Slot that is triggered when the left aligned tool button is clicked.
         */
        void leftAlignedButtonClicked();

        /**
         * Slot that is triggered when the centered button is clicked.
         */
        void centeredButtonClicked();

        /**
         * Slot that is triggered when the right aligned tool button is clicked.
         */
        void rightAlignedButtonClicked();

        /**
         * Slot that is triggered when the justified button is clicked.
         */
        void justifiedButtonClicked();

        /**
         * Slot that is triggered when the apply button is clicked.
         */
        void applyClicked();

        /**
         * Slot that is triggered when a value changes.  The slot checks for updates and adjusts the dialog
         * accordingly.
         */
        void updateDialog();

    private:
        /**
         * The minimum allowed dialog width.  Included to keep notes from being squished excessively.
         */
        static const unsigned dialogMinimumWidth;

        /**
         * The size of the alignment buttons.
         */
        static const unsigned alignmentToolButtonSize;

        /**
         * Value used to flag a cleared/unused margin setting.
         */
        static const float invalidMarginSetting;

        /**
         * Called from the constructor to configure this dialog.
         */
        void configureWidget();

        /**
         * Method that is called to check if values are different from the input values.
         *
         * \return Returns true if one or more values are different.  Returns false if all values match the inputs.
         */
        bool valuesChanged() const;

        /**
         * Flag that indicates if auto-apply is enabled.
         */
        bool currentAutoApplyEnabled;

        /**
         * The page format used to draw margin.
         */
        QSharedPointer<Ld::PageFormat> inputPageFormat;

        /**
         * Current input justification setting.
         */
        QSet<Ld::JustifiedBlockFormat::Justification> inputJustifications;

        /**
         * Current input left indentation settings.  A negative value indicates that the parameter was not set or was
         * cleared.
         */
        float inputLeftIndentation;

        /**
         * Current input list indentation settings.  A negative value indicates that the parameter was not set or was
         * cleared.
         */
        float inputListIndentation;

        /**
         * Current input right indentation settings.  A negative value indicates that the parameter was not set or was
         * cleared.
         */
        float inputRightIndentation;

        /**
         * Current input top spacing setting.  A negative value indicates that the parameter was not set or was
         * cleared.
         */
        float inputTopSpacing;

        /**
         * Current input bottom spacing setting.    A negative value indicates that the parameter was not set or was
         * cleared.
         */
        float inputBottomSpacing;

        /**
         * Current input first line indentation setting.  A negative value indicates that the parameter was not set or
         * was cleared.
         */
        float inputFirstLineIndentation;

        /**
         * Current input line spacing setting.  A negative value indicates that the parameter was not set or was
         * cleared.
         */
        float inputLineSpacing;
};

#endif
