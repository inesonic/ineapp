/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref TableFormatDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef TABLE_FORMAT_DIALOG_H
#define TABLE_FORMAT_DIALOG_H

#include <QSet>
#include <QSharedPointer>
#include <QColor>
#include <QSize>

#include <cstdint>

#include <eqt_programmatic_dialog.h>

#include <ld_format.h>
#include <ld_table_frame_format.h>
#include <ld_table_line_settings.h>
#include <ld_table_column_width.h>

#include "app_common.h"

class QWidget;

namespace Ld {
    class BlockFormat;
    class PageFormat;
};

/**
 * Dialog used to obtain settings for a newly inserted table or to modify existing table settings.
 */
class APP_PUBLIC_API TableFormatDialog:public EQt::ProgrammaticDialog {
    Q_OBJECT

    public:
        /**
         * Type used to represent a line style.
         */
        typedef Ld::TableLineSettings::Style LineStyle;

        /**
         * Type used to represent the column width settings.
         */
        typedef Ld::TableColumnWidth::WidthType WidthType;

        /**
         * Type used to represent a column width.
         */
        typedef Ld::TableColumnWidth ColumnWidth;

        /**
         * Type used to represent the table justification.
         */
        typedef Ld::TableFrameFormat::Justification Justification;

        /**
         * Value holding the default table justification value.
         */
        static const Justification defaultJustification;

        /**
         * Value holding the default left indentation value.
         */
        static const float defaultLeftIndentation;

        /**
         * Value holding true if the default left indentation value is valid.
         */
        static const bool defaultLeftIndentationValid;

        /**
         * Value holding the default right indentation value.
         */
        static const float defaultRightIndentation;

        /**
         * Value holding true if the default right indentation value is valid.
         */
        static const bool defaultRightIndentationValid;

        /**
         * Value holding the default top spacing value.
         */
        static const float defaultTopSpacing;

        /**
         * Value holding true if the default top spacing value is valid.
         */
        static const bool defaultTopSpacingValid;

        /**
         * Value holding the default bottom spacing value.
         */
        static const float defaultBottomSpacing;

        /**
         * Value holding true if the default bottom spacing value is valid.
         */
        static const bool defaultBottomSpacingValid;

        /**
         * Value holding the default top gutter value.
         */
        static const float defaultTopGutter;

        /**
         * Value holding true if the top gutter value is valid.
         */
        static const bool defaultTopGutterValid;

        /**
         * Value holding the default left gutter value.
         */
        static const float defaultLeftGutter;

        /**
         * Value holding true if the left gutter value is valid.
         */
        static const bool defaultLeftGutterValid;

        /**
         * Value holding the default right gutter value.
         */
        static const float defaultRightGutter;

        /**
         * Value holding true if the right gutter value is valid.
         */
        static const bool defaultRightGutterValid;

        /**
         * Value holding the default bottom gutter value.
         */
        static const float defaultBottomGutter;

        /**
         * Value holding true if the bottom gutter value is valid.
         */
        static const bool defaultBottomGutterValid;

        /**
         * Value holding the default number of rows.
         */
        static const unsigned defaultNumberRows;

        /**
         * Value holding the default number of columns.
         */
        static const unsigned defaultNumberColumns;

        /**
         * Value holding the default vertical line style.
         */
        static const LineStyle defaultVerticalLineStyle;

        /**
         * Value holding the default vertical line width.
         */
        static const float defaultVerticalLineWidth;

        /**
         * Value holding the default horizontal line style.
         */
        static const LineStyle defaultHorizontalLineStyle;

        /**
         * Value holding the default horizontal line width.
         */
        static const float defaultHorizontalLineWidth;

        /**
         * Value holding the default cell background color.
         */
        static const QColor defaultCellBackgroundColor;

        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        TableFormatDialog(QWidget* parent = Q_NULLPTR);

        ~TableFormatDialog() override;

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
         * Method you can use to determine if the apply button is visible or hidden.
         *
         * \return Returns true if the Apply button is visible.  Returns false if the Apply button is hidden.
         */
        bool applyButtonVisible() const;

        /**
         * Method you can use to determine if the apply button is hidden or visible.
         *
         * \return Returns true if the Apply button is hidden.  Returns false if the Apply button is viible.
         */
        bool applyButtonHidden() const;

        /**
         * Method you can use to show or hide the apply button.
         *
         * \param[in] nowVisible If true, the Apply button will be made visible.  If false, the Apply button will be
         *                       hidden.
         */
        void setApplyButtonVisible(bool nowVisible = true);

        /**
         * Method you can use to hide or show the apply button.
         *
         * \param[in] nowHidden If true, the Apply button will be hidden.  If false, the Apply button will be made
         *                      visible.
         */
        void setApplyButtonHidden(bool nowHidden = true);

        /**
         * Method you can use to determine if the OK and Cancel button is visible or hidden.
         *
         * \return Returns true if the OK button and Cancel button is visible.  Returns false if the OK button and
         *         Cancel button is hidden.
         */
        bool okCancelButtonVisible() const;

        /**
         * Method you can use to determine if the OK and Cancel button is hidden or visibie.
         *
         * \return Returns true if the OK button and Cancel button is hidden.  Returns false if the OK button and
         *         Cancel button is visible.
         */
        bool okCancelButtonHidden() const;

        /**
         * Method you can use to show or hide the OK and Cancel button.
         *
         * \param[in] nowVisible If true, the OK and Cancel button will be made visible.  If false, the OK and Cancel
         *                       button will be hidden.
         */
        void setOkCancelButtonVisible(bool nowVisible = true);

        /**
         * Method you can use to hide or show the OK and Cancel button.
         *
         * \param[in] nowHidden If true, the OK and Cancel button will be hidden.  If false, the OK and Cancel button
         *                      will be made visible.
         */
        void setOkCancelButtonHidden(bool nowHidden = true);

        /**
         * Method you can use to determine if the OK button will always be enabled.
         *
         * \return Returns true if the OK button will always be enabled.  Returns false if the OK button will only be
         *         enabled if the dialog contents are changed (and the button is visible).
         */
        bool okButtonAlwaysEnabled() const;

        /**
         * Method you can use to determine if the OK button will be enabled only if the dialog contents are changed.
         *
         * \return Returns true if the OK button will be enabled only if the dialog contents have been changed. Returns
         *         false if the OK button will always be enabled.
         */
        bool okButtonConditionallyEnabled() const;

        /**
         * Method you can use to specify that the OK button should always be enabled.
         *
         * \param[in] nowAlwaysEnabled If true, the OK button will always be enabled.  If false, the OK button will be
         *                             enabled only if the dialog has been updated.
         */
        void setOkButtonAlwaysEnabled(bool nowAlwaysEnabled = true);

        /**
         * Method you can use to specify that the OK button should be enabled only if the dialog contents have been
         * changed.
         *
         * \param[in] nowConditionallyEnabled If true, the OK button will be enabled only if the dialog contents have
         *                                    been changed.  If false, the OK button will always be enabled.
         */
        void setOkButtonConditionallyEnabled(bool nowConditionallyEnabled = true);

        /**
         * Method you can use to determine if advanced mode is enabled or disabled.
         *
         * \return Returns true if advanced mode has been enabled.  Returns false if advanced mode has been disabled.
         */
        bool advancedModeEnabled() const;

        /**
         * Method you can use to determine if advanced mode is enabled or disabled.
         *
         * \return Returns true if advanced mode has been enabled.  Returns false if advanced mode has been disabled.
         */
        bool advancedModeDisabled() const;

        /**
         * Method you can use to determine if the advanced mode button is visible or hidden.
         *
         * \return Returns true if the advanced mode button is visible.  Returns false if the advanced mode button is
         *         hidden.
         */
        bool advancedModeButtonVisible() const;

        /**
         * Method you can use to determine if the advanced mode button is visible or hidden.
         *
         * \return Returns true if the advanced mode button is hidden.  Returns false if the advanced mode button is
         *         visible.
         */
        bool advancedModeButtonHidden() const;

        /**
         * Method you can use to determine if the table size settings will be displayed or hidden.
         *
         * \return Returns true if the table size settings will be shown.  Returns false if the table size settings
         *         will be hidden.
         */
        bool tableSizeControlsVisible() const;

        /**
         * Method you can use to determine if the table size settings will be hidden or displayed.
         *
         * \return Returns true if the table size settings will be hidden.  Returns false if the table size settings
         *         will be shown.
         */
        bool tableSizeControlsHidden() const;

        /**
         * Method you can use to obtain the current justification.
         *
         * \return Returns the current justification mode.  The justification mode Justification::NONE is returned if
         *         no or multiple justification modes are selected.
         */
        Justification justification() const;

        /**
         * Method you can use to obtain the current left indentation value.
         *
         * \return Returns the paragraph left indentation value.
         */
        float leftIndentation() const;

        /**
         * Method you can use to determine if the left indentation value has been set.
         *
         * \return Returns true if the left indentation value has been set.  Returns false if the left indentation
         *         value has not been set.
         */
        bool leftIndentationSet() const;

        /**
         * Method you can use to determine if the left indentation value has not been set.
         *
         * \return Returns true if the left indentation value has not been set.  Returns false if the left indentation
         *         value has been set.
         */
        bool leftIndentationUnset() const;

        /**
         * Method you can use to obtain the current right indentation value.
         *
         * \return Returns the paragraph right indentation value.
         */
        float rightIndentation() const;

        /**
         * Method you can use to determine if the right indentation value has been set.
         *
         * \return Returns true if the right indentation value has been set.  Returns false if the right indentation
         *         value has not been set.
         */
        bool rightIndentationSet() const;

        /**
         * Method you can use to determine if the right indentation value has not been set.
         *
         * \return Returns true if the right indentation value has not been set.  Returns false if the right
         *         indentation value has been set.
         */
        bool rightIndentationUnset() const;

        /**
         * Method you can use to obtain the current top spacing value.
         *
         * \return Returns the paragraph top spacing.
         */
        float topSpacing() const;

        /**
         * Method you can use to determine if the top spacing value has been set.
         *
         * \return Returns true if the top spacing value has been set.  Returns false if the top spacing value has
         *         not been set.
         */
        bool topSpacingSet() const;

        /**
         * Method you can use to determine if the top spacing value has not been set.
         *
         * \return Returns true if the top spacing value has not been set.  Returns false if the top spacing value has
         *         not been set.
         */
        bool topSpacingUnset() const;

        /**
         * Method you can use to obtain the current bottom spacing value.
         *
         * \return Returns the paragraph bottom spacing.
         */
        float bottomSpacing() const;

        /**
         * Method you can use to determine if the bottom spacing value has been set.
         *
         * \return Returns true if the bottom spacing value has been set.  Returns false if the bottom spacing value
         *         has not been set.
         */
        bool bottomSpacingSet() const;

        /**
         * Method you can use to determine if the bottom spacing value has not been set.
         *
         * \return Returns true if the bottom spacing value has not been set.  Returns false if the bottom spacing
         *         value has not been set.
         */
        bool bottomSpacingUnset() const;

        /**
         * Method you can use to determine the current top gutter value.
         *
         * \return Returns the top gutter value.
         */
        float topGutter() const;

        /**
         * Method you can use to determine if the top gutter has been set.
         *
         * \return Returns true if the top gutter setting has been set.
         */
        bool topGutterSet() const;

        /**
         * Method you can use to determine if the top gutter has been unset.
         *
         * \return Returns true if the top gutter setting has been unset.
         */
        bool topGutterUnset() const;

        /**
         * Method you can use to determine the current left gutter value.
         *
         * \return Returns the left gutter value.
         */
        float leftGutter() const;

        /**
         * Method you can use to determine if the left gutter has been set.
         *
         * \return Returns true if the left gutter setting has been set.
         */
        bool leftGutterSet() const;

        /**
         * Method you can use to determine if the left gutter has been unset.
         *
         * \return Returns true if the left gutter setting has been unset.
         */
        bool leftGutterUnset() const;

        /**
         * Method you can use to determine the current right gutter value.
         *
         * \return Returns the right gutter value.
         */
        float rightGutter() const;

        /**
         * Method you can use to determine if the right gutter has been set.
         *
         * \return Returns true if the right gutter setting has been set.
         */
        bool rightGutterSet() const;

        /**
         * Method you can use to determine if the right gutter has been unset.
         *
         * \return Returns true if the right gutter setting has been unset.
         */
        bool rightGutterUnset() const;

        /**
         * Method you can use to determine the current bottom gutter value.
         *
         * \return Returns the bottom gutter value.
         */
        float bottomGutter() const;

        /**
         * Method you can use to determine if the bottom gutter has been set.
         *
         * \return Returns true if the bottom gutter setting has been set.
         */
        bool bottomGutterSet() const;

        /**
         * Method you can use to determine if the bottom gutter has been unset.
         *
         * \return Returns true if the bottom gutter setting has been unset.
         */
        bool bottomGutterUnset() const;

        /**
         * Method you can use to obtain the number of rows.
         *
         * \return Returns the currently selected number of rows.
         */
        unsigned numberRows() const;

        /**
         * Method you can use to determine if the user has set the number of rows.
         *
         * \return Returns true if the number of rows has been been set.
         */
        bool numberRowsSet() const;

        /**
         * Method you can use to determine if the user has not set the number of rows.
         *
         * \return Returns true if the number of rows has not been set.
         */
        bool numberRowsUnset() const;

        /**
         * Method you can use to obtain the number of columns.
         *
         * \return Returns the currently selected number of columns.
         */
        unsigned numberColumns() const;

        /**
         * Method you can use to determine if the user has set the number of columns.
         *
         * \return Returns true if the number of columns has been been set.
         */
        bool numberColumnsSet() const;

        /**
         * Method you can use to determine if the user has not set the number of columns.
         *
         * \return Returns true if the number of columns has not been been set.
         */
        bool numberColumnsUnset() const;

        /**
         * Method you can use to obtain the default vertical line style.
         *
         * \return Returns the default vertical line style.
         */
        LineStyle verticalLineStyle() const;

        /**
         * Method you can use to determine if the default vertical line style is set.
         *
         * \return Returns true if the user has set the vertical line style.
         */
        bool verticalLineStyleSet() const;

        /**
         * Method you can use to determine if the default vertical line style is not set.
         *
         * \return Returns true if the user has not set the vertical line style.
         */
        bool verticalLineStyleUnset() const;

        /**
         * Method you can use to obtain the default vertical line width.
         *
         * \return Returns the default vertical line width.
         */
        float verticalLineWidth() const;

        /**
         * Method you can use to determine if the default vertical line width has been set.
         *
         * \return Returns true if the user has set the vertical line width.
         */
        bool verticalLineWidthSet() const;

        /**
         * Method you can use to determine if the default vertical line width has not been set.
         *
         * \return Returns true if the user has not set the vertical line width.
         */
        bool verticalLineWidthUnset() const;

        /**
         * Method you can use to obtain the default horizontal line style.
         *
         * \return Returns the default horizontal line style.
         */
        LineStyle horizontalLineStyle() const;

        /**
         * Method you can use to determine if the default horizontal line style is set.
         *
         * \return Returns true if the user has set the horizontal line style.
         */
        bool horizontalLineStyleSet() const;

        /**
         * Method you can use to determine if the default horizontal line style is not set.
         *
         * \return Returns true if the user has not set the horizontal line style.
         */
        bool horizontalLineStyleUnset() const;

        /**
         * Method you can use to obtain the default horizontal line width.
         *
         * \return Returns the default horizontal line width.
         */
        float horizontalLineWidth() const;

        /**
         * Method you can use to determine if the default horizontal line width has been set.
         *
         * \return Returns true if the user has set the horizontal line width.
         */
        bool horizontalLineWidthSet() const;

        /**
         * Method you can use to determine if the default horizontal line width has not been set.
         *
         * \return Returns true if the user has set the horizontal line width.
         */
        bool horizontalLineWidthUnset() const;

        /**
         * Method you can use to obtain the default cell color.
         *
         * \return Returns the default cell color.
         */
        QColor defaultCellColor() const;

        /**
         * Method you can use to determine if the default cell color is set.
         *
         * \return Returns true if the default cell color has been set.
         */
        bool defaultCellColorSet() const;

        /**
         * Method you can use to determine if the default cell color is not set.
         *
         * \return Returns true if the default cell color has not been set.
         */
        bool defaultCellColorUnset() const;

        /**
         * Method you can use to obtain the column width settings.  An invalid column width indicates an unset column
         * value.
         *
         * \return Returns a list of column settings values.
         */
        QList<ColumnWidth> columnWidths() const;

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
        void apply(TableFormatDialog* thisDialog);

    public slots:
        /**
         * Slot you can trigger to show or hide the table size settings controls.
         *
         * \param[in] nowVisible If true, the table size controls would be displayed.  If false, the table size
         *                       controls will be hidden.
         */
        void setTableSizeControlsVisible(bool nowVisible = true);

        /**
         * Slot you can trigger to hide or show the table size settings controls.
         *
         * \param[in] nowHidden If true, the table size controls would be hidden.  If false, the table size controls
         *                      will be visible.
         */
        void setTableSizeControlsHidden(bool nowHidden = true);

        /**
         * Slot you can trigger to display or hide the advanced controls.
         *
         * \param[in] nowEnabled If true, the advanced controls will be made visible.  If false, the advanced controls
         *                       will be hidden.
         */
        void setAdvancedControlsEnabled(bool nowEnabled = true);

        /**
         * Slot you can trigger to hide or display the advanced controls.
         *
         * \param[in] nowHidden If true, the advanced controls will be hidden.  If false, the advanced controls will
         *                      be made visible.
         */
        void setAdvancedControlsDisabled(bool nowHidden = true);

        /**
         * Slot you can trigger to display or hide the advanced mode button.
         *
         * \param[in] nowVisible If true, the advanced mode button will be displayed.  If false, the advanced mode
         *                       button will be hidden.
         */
        void setAdvancedModeButtonVisible(bool nowVisible = true);

        /**
         * Slot you can trigger to hide or display the advanced mode button.
         *
         * \param[in] nowHidden If true, the advanced mode button will be hidden.  If false, the advanced mode button
         *                      will be made visible.
         */
        void setAdvancedModeButtonHidden(bool nowHidden = true);

        /**
         * Slot you can trigger to set the justification modes.
         *
         * \param[in] justificationModes A set listing the supported justification modes.
         */
        void setJustification(const QSet<Justification>& justificationModes);

        /**
         * Slot you can trigger to set the justification mode.
         *
         * \param[in] justificationMode A set listing the supported justification mode.
         */
        void setJustification(Justification justificationMode);

        /**
         * Slot you can trigger to set the left indentation value.
         *
         * \param[in] newLeftIndentation The new left indentation value, in points.
         */
        void setLeftIndentation(float newLeftIndentation);

        /**
         * Slot you can trigger to clear the left indentation value.
         */
        void setLeftIndentationCleared();

        /**
         * Slot you can trigger to set the right indentation value.
         *
         * \param[in] newRightIndentation The new right indentation value, in points.
         */
        void setRightIndentation(float newRightIndentation);

        /**
         * Slot you can trigger to clear the right indentation value.
         */
        void setRightIndentationCleared();

        /**
         * Slot you can trigger to set the top spacing value.
         *
         * \param[in] newTopSpacing The new top spacing value, in points.
         */
        void setTopSpacing(float newTopSpacing);

        /**
         * Slot you can trigger to clear the top spacing value.
         */
        void setTopSpacingCleared();

        /**
         * Slot you can trigger to set the bottom spacing value.
         *
         * \param[in] newBottomSpacing The new bottom spacing value, in points.
         */
        void setBottomSpacing(float newBottomSpacing);

        /**
         * Slot you can trigger to clear the bottom spacing value.
         */
        void setBottomSpacingCleared();

        /**
         * Slot you can trigger to set the top gutter value.
         *
         * \param[in] newTopGutter The new top gutter value, in points.
         */
        void setTopGutter(float newTopGutter);

        /**
         * Slot you can trigger to clear the top gutter value.
         */
        void setTopGutterCleared();

        /**
         * Slot you can trigger to set the left gutter value.
         *
         * \param[in] newLeftGutter The new left gutter value, in points.
         */
        void setLeftGutter(float newLeftGutter);

        /**
         * Slot you can trigger to clear the left gutter value.
         */
        void setLeftGutterCleared();

        /**
         * Slot you can trigger to set the right gutter value.
         *
         * \param[in] newRightGutter The new right gutter value, in points.
         */
        void setRightGutter(float newRightGutter);

        /**
         * Slot you can trigger to clear the right gutter value.
         */
        void setRightGutterCleared();

        /**
         * Slot you can trigger to set the bottom gutter value.
         *
         * \param[in] newBottomGutter The new bottom gutter value, in points.
         */
        void setBottomGutter(float newBottomGutter);

        /**
         * Slot you can trigger to clear the bottom gutter value.
         */
        void setBottomGutterCleared();

        /**
         * Slot you can trigger to set the number of rows.
         *
         * \param[in] newNumberRows The new number of rows.
         */
        void setNumberRows(unsigned newNumberRows);

        /**
         * Slot you can trigger to indicate that there are multiple row numbers.
         */
        void setMultipleNumberRows();

        /**
         * Slot you can trigger to set the number columns.
         *
         * \param[in] newNumberColumns The new number of columns.
         */
        void setNumberColumns(unsigned newNumberColumns);

        /**
         * Slot you can trigger to indicate that there are multiple number columns.
         */
        void setMultipleNumberColumns();

        /**
         * Slot you can trigger to set the vertical line style.
         *
         * \param[in] newLineStyle The new vertical line style.
         */
        void setVerticalLineStyle(LineStyle newLineStyle);

        /**
         * Slot you can trigger to set multiple vertical line styles.
         *
         * \param[in] newLineStyles The new vertical line styles.
         */
        void setVerticalLineStyles(const QSet<LineStyle>& newLineStyles);

        /**
         * Slot you can trigger to set the vertical line width.
         *
         * \param[in] newLineWidth The new vertical line width.
         */
        void setVerticalLineWidth(float newLineWidth);

        /**
         * Slot you can trigger to set the vertical line widths.
         *
         * \param[in] newLineWidths The new vertical line widths.
         */
        void setVerticalLineWidths(const QSet<float>& newLineWidths);

        /**
         * Slot you can trigger to set the horizontal line style.
         *
         * \param[in] newLineStyle The new horizontal line style.
         */
        void setHorizontalLineStyle(LineStyle newLineStyle);

        /**
         * Slot you can trigger to set multiple horizontal line styles.
         *
         * \param[in] newLineStyles The new horizontal line styles.
         */
        void setHorizontalLineStyles(const QSet<LineStyle>& newLineStyles);

        /**
         * Slot you can trigger to set the horizontal line width.
         *
         * \param[in] newLineWidth The new horizontal line width.
         */
        void setHorizontalLineWidth(float newLineWidth);

        /**
         * Slot you can trigger to set the horizontal line widths.
         *
         * \param[in] newLineWidths The new horizontal line widths.
         */
        void setHorizontalLineWidths(const QSet<float>& newLineWidths);

        /**
         * Slot you can trigger to set the default cell color.
         *
         * \param[in] newCellColor The new cell color value.
         */
        void setDefaultCellColor(const QColor& newCellColor);

        /**
         * Slot you can trigger to set the default cell colors.
         *
         * \param[in] newCellColors The new cell color values.
         */
        void setDefaultCellColors(const QSet<QColor>& newCellColors);

        /**
         * Slot you can trigger to set the column width values.  This slot will also set the number of columns.
         *
         * \param[in] newColumnWidthValues The new column width values.  An invalid column width indicates a unset
         *                                 column width value.
         */
        void setColumnWidths(const QList<ColumnWidth>& newColumnWidthValues);

    protected:
        /**
         * Performs binding of signals to slots.  This method will be executed by the
         * EQt::ProgrammaticDialog::runBuilders method.
         *
         * You can overload this method if you want to bind signals to slots prior to the proxy classes.
         */
        void bind() final;

        /**
         * Method that is called when the user closes the dialog. This method's behavior is platform specific.  On
         * Windows and Linux, the method will call the base class function.  On MacOS, this method will either accept
         * or reject the close event and set the result code to QDialog::Accepted if the window can be closed.
         */
        void closeEvent(QCloseEvent* event) override;

    private slots:
        /**
         * Slot that is triggered when the number of rows have changed.
         *
         * \param[in] newNumberRows The new number of rows.
         */
        void numberRowsChanged(int newNumberRows);

        /**
         * Slot that is triggered when the number of columns has changed.
         *
         * \param[in] newNumberColumns The new number of columns.
         */
        void numberColumnsChanged(int newNumberColumns);

        /**
         * Slot that is triggered when the left alignment button is clicked.
         */
        void leftAlignmentButtonClicked();

        /**
         * Slot that is triggered when the center button is clicked.
         */
        void centerButtonClicked();

        /**
         * Slot that is triggered when the right alignment button is clicked.
         */
        void rightAlignmentButtonClicked();

        /**
         * Slot that is triggered when the left indentation button is clicked.
         *
         * \param[in] newValue The new left indentation value.
         */
        void leftIndentationChanged(double newValue);

        /**
         * Slot that is triggered when the right indentation button is clicked.
         *
         * \param[in] newValue The new right indentation value.
         */
        void rightIndentationChanged(double newValue);

        /**
         * Slot that is triggered when the top spacing button is clicked.
         *
         * \param[in] newValue The new top spacing value.
         */
        void topSpacingChanged(double newValue);

        /**
         * Slot that is triggered when the bottom spacing button is clicked.
         *
         * \param[in] newValue The new bottom spacing value.
         */
        void bottomSpacingChanged(double newValue);

        /**
         * Slot that is triggered when the top gutter value is changed.
         *
         * \param[in] newValue The new top spacing value.
         */
        void topGutterChanged(double newValue);

        /**
         * Slot that is triggered when the left gutter value is changed.
         *
         * \param[in] newValue The new left spacing value.
         */
        void leftGutterChanged(double newValue);

        /**
         * Slot that is triggered when the right gutter value is changed.
         *
         * \param[in] newValue The new right spacing value.
         */
        void rightGutterChanged(double newValue);

        /**
         * Slot that is triggered when the bottom gutter value is changed.
         *
         * \param[in] newValue The new bottom spacing value.
         */
        void bottomGutterChanged(double newValue);

        /**
         * Slot that is triggered when the horizontal line style is changed.
         *
         * \param[in] newLineStyle The newly selected horizontal line style.
         */
        void horizontalLineStyleChanged(LineStyle newLineStyle);

        /**
         * Signal that is triggered when the horizontal line style is changed.
         *
         * \param[in] newLineWidth The newly selected horizontal line width.
         */
        void horizontalLineWidthChanged(float newLineWidth);

        /**
         * Slot that is triggered when the vertical line style is changed.
         *
         * \param[in] newLineStyle The newly selected vertical line style.
         */
        void verticalLineStyleChanged(LineStyle newLineStyle);

        /**
         * Signal that is triggered when the vertical line style is changed.
         *
         * \param[in] newLineWidth The newly selected vertical line width.
         */
        void verticalLineWidthChanged(float newLineWidth);

        /**
         * Slot that is triggered when a column value is changed.
         *
         * \param[in] columnIndex     The zero based table column index that was changed.
         *
         * \param[in] nowProportional Value that holds true if the table column is now proportionally sized.  The
         *                            value will hold false if the table is now fixed size.
         *
         * \param[in] newWidth        The new width value for the table in points or fractional remaining space.
         */
        void columnWidthChanged(unsigned columnIndex, bool nowProportional, float newWidth);

        /**
         * Slot that is triggered when the default background color button is clicked.
         */
        void changeDefaultCellColorButtonClicked();

        /**
         * Slot that is triggered when the clear default background color button is clicked.
         */
        void removeDefaultCellColorButtonClicked();

        /**
         * Slot that is triggered when the advanced button is clicked.
         *
         * \param[in] nowEnabled If true, the advanced button is now checked.  If false, the advanced button is now
         *                       unchecked.
         */
        void advancedButtonClicked(bool nowEnabled);

        /**
         * Slot that is triggered when the user selects a new displayed unit.
         *
         * \param[in] unitText Text representing the new unit to be displayed.
         */
        void newUnitSelected(const QString& unitText);

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
         * Spacing between columns.
         */
        static constexpr unsigned embellishmentsColumnsSpacing = 80;

        /**
         * The font size used for filler text.
         */
        static constexpr unsigned fillerTextPointSize = 12;

        /**
         * Value indicating the size of the alignment tool buttons.
         */
        static constexpr unsigned alignmentToolButtonSize = 128;

        /**
         * Enumeration used internally to track the state of flags that can take on four states.
         */
        enum class QuadState:std::uint8_t {
            /**
             * Indicates no values set or cleared.
             */
            NO_VALUE,

            /**
             * Indicates all values are enabled.
             */
            ENABLED,

            /**
             * Indicates all values are disabled.
             */
            DISABLED,

            /**
             * Indicates we have both enabled and disabled values.
             */
            ENABLED_AND_DISABLED
        };

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
         * Flag that indicates if the OK button should always be enabled.
         */
        bool currentOkButtonAlwaysEnabled;

        /**
         * The input justification modes.
         */
        QSet<Justification> inputJustifications;

        /**
         * The input left indentation.
         */
        float inputLeftIndentation;

        /**
         * Flag indicating if the input left indentation is valid.
         */
        bool inputLeftIndentationValid;

        /**
         * The input right indentation.
         */
        float inputRightIndentation;

        /**
         * Flag indicating if the input right indentation is valid.
         */
        bool inputRightIndentationValid;

        /**
         * The input top spacing.
         */
        float inputTopSpacing;

        /**
         * Flag indicating if the input top spacing is valid.
         */
        bool inputTopSpacingValid;

        /**
         * The input bottom spacing.
         */
        float inputBottomSpacing;

        /**
         * Flag indicating if the input bottom spacing is valid.
         */
        bool inputBottomSpacingValid;

        /**
         * The input top gutter value.
         */
        float inputTopGutter;

        /**
         * Flag indicating if the top gutter value is valid.
         */
        bool inputTopGutterValid;

        /**
         * The input left gutter value.
         */
        float inputLeftGutter;

        /**
         * Flag indicating if the left gutter value is valid.
         */
        bool inputLeftGutterValid;

        /**
         * The input right gutter value.
         */
        float inputRightGutter;

        /**
         * Flag indicating if the right gutter value is valid.
         */
        bool inputRightGutterValid;

        /**
         * The input bottom gutter value.
         */
        float inputBottomGutter;

        /**
         * Flag indicating if the bottom gutter value is valid.
         */
        bool inputBottomGutterValid;

        /**
         * The input number of rows.  A value of 0 indicates an invalid or multiple number of rows.
         */
        unsigned inputNumberRows;

        /**
         * The input number of columns.  A value of 0 indicates an invalid or multiple number of columns.
         */
        unsigned inputNumberColumns;

        /**
         * The input set of vertical line styles.
         */
        QSet<LineStyle> inputVerticalLineStyles;

        /**
         * The input vertical line width.  A value of 0 or a negative value indicates an invalid or multiple input
         * vertical line widths.
         */
        float inputVerticalLineWidth;

        /**
         * The input set of horizontal line styles.
         */
        QSet<LineStyle> inputHorizontalLineStyles;

        /**
         * The input horizontal line width.  A value of 0 or a negative value indicates an invalid or multiple input
         * horizontal line widths.
         */
        float inputHorizontalLineWidth;

        /**
         * The input set of default cell colors.
         */
        QSet<QColor> inputDefaultCellColors;

        /**
         * The input column width values.
         */
        QList<ColumnWidth> inputColumnWidths;
};

#endif
