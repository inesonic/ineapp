/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref TableLineStyleDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef TABLE_LINE_STYLE_DIALOG_H
#define TABLE_LINE_STYLE_DIALOG_H

#include <QSet>
#include <QSharedPointer>
#include <QColor>

#include <cstdint>

#include <eqt_programmatic_dialog.h>

#include <ld_format.h>
#include <ld_table_frame_format.h>
#include <ld_table_line_settings.h>

#include "app_common.h"

class QWidget;

namespace Ld {
    class BlockFormat;
    class PageFormat;
};

/**
 * Dialog used to obtain table default line style settings.
 */
class APP_PUBLIC_API TableLineStyleDialog:public EQt::ProgrammaticDialog {
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
         * Constructor
         *
         * \param[in] horizontalOrientation If true, the dialog will be configured for horizontal orientation.  If
         *                                  false, the dialog will be configured for vertical orientatin.
         *
         * \param[in] parent                Pointer to the parent object.
         */
        TableLineStyleDialog(bool horizontalOrientation, QWidget* parent = Q_NULLPTR);

        ~TableLineStyleDialog() override;

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
         * Method you can use to obtain the default left/top line style.
         *
         * \return Returns the default left/top line style.
         */
        LineStyle leftTopLineStyle() const;

        /**
         * Method you can use to determine if the default left/top line style is set.
         *
         * \return Returns true if the user has set the left/top line style.
         */
        bool leftTopLineStyleSet() const;

        /**
         * Method you can use to determine if the default left/top line style is not set.
         *
         * \return Returns true if the user has not set the left/top line style.
         */
        bool leftTopLineStyleUnset() const;

        /**
         * Method you can use to obtain the default left/top line width.
         *
         * \return Returns the default left/top line width.
         */
        float leftTopLineWidth() const;

        /**
         * Method you can use to determine if the default left/top line width has been set.
         *
         * \return Returns true if the user has set the left/top line width.
         */
        bool leftTopLineWidthSet() const;

        /**
         * Method you can use to determine if the default left/top line width has not been set.
         *
         * \return Returns true if the user has not set the left/top line width.
         */
        bool leftTopLineWidthUnset() const;

        /**
         * Method you can use to obtain the default right/bottom line style.
         *
         * \return Returns the default right/bottom line style.
         */
        LineStyle rightBottomLineStyle() const;

        /**
         * Method you can use to determine if the default right/bottom line style is set.
         *
         * \return Returns true if the user has set the right/bottom line style.
         */
        bool rightBottomLineStyleSet() const;

        /**
         * Method you can use to determine if the default right/bottom line style is not set.
         *
         * \return Returns true if the user has not set the right/bottom line style.
         */
        bool rightBottomLineStyleUnset() const;

        /**
         * Method you can use to obtain the default right/bottom line width.
         *
         * \return Returns the default right/bottom line width.
         */
        float rightBottomLineWidth() const;

        /**
         * Method you can use to determine if the default right/bottom line width has been set.
         *
         * \return Returns true if the user has set the right/bottom line width.
         */
        bool rightBottomLineWidthSet() const;

        /**
         * Method you can use to determine if the default right/bottom line width has not been set.
         *
         * \return Returns true if the user has set the right/bottom line width.
         */
        bool rightBottomLineWidthUnset() const;

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
        void apply(TableLineStyleDialog* thisDialog);

    public slots:
        /**
         * Slot you can trigger to set the left/top line style.
         *
         * \param[in] newLineStyle The new left/top line style.
         */
        void setLeftTopLineStyle(LineStyle newLineStyle);

        /**
         * Slot you can trigger to set multiple left/top line styles.
         *
         * \param[in] newLineStyles The new left/top line styles.
         */
        void setLeftTopLineStyles(const QSet<LineStyle>& newLineStyles);

        /**
         * Slot you can trigger to set the left/top line width.
         *
         * \param[in] newLineWidth The new left/top line width.
         */
        void setLeftTopLineWidth(float newLineWidth);

        /**
         * Slot you can trigger to set the left/top line widths.
         *
         * \param[in] newLineWidths The new left/top line widths.
         */
        void setLeftTopLineWidths(const QSet<float>& newLineWidths);

        /**
         * Slot you can trigger to set the right/bottom line style.
         *
         * \param[in] newLineStyles The new right/bottom line style.
         */
        void setRightBottomLineStyle(LineStyle newLineStyles);

        /**
         * Slot you can trigger to set multiple right/bottom line styles.
         *
         * \param[in] newLineStyles The new right/bottom line styles.
         */
        void setRightBottomLineStyles(const QSet<LineStyle>& newLineStyles);

        /**
         * Slot you can trigger to set the right/bottom line width.
         *
         * \param[in] newLineWidth The new right/bottom line width.
         */
        void setRightBottomLineWidth(float newLineWidth);

        /**
         * Slot you can trigger to set the right/bottom line widths.
         *
         * \param[in] newLineWidths The new right/bottom line widths.
         */
        void setRightBottomLineWidths(const QSet<float>& newLineWidths);

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
         * Slot that is triggered when the left/top line style is changed.
         *
         * \param[in] newLineStyle The newly selected left/top line style.
         */
        void leftTopLineStyleChanged(LineStyle newLineStyle);

        /**
         * Signal that is triggered when the left/top line style is changed.
         *
         * \param[in] newLineWidth The newly selected left/top line width.
         */
        void leftTopLineWidthChanged(float newLineWidth);

        /**
         * Slot that is triggered when the right/bottom line style is changed.
         *
         * \param[in] newLineStyle The newly selected right/bottom line style.
         */
        void rightBottomLineStyleChanged(LineStyle newLineStyle);

        /**
         * Signal that is triggered when the right/bottom line style is changed.
         *
         * \param[in] newLineWidth The newly selected right/bottom line width.
         */
        void rightBottomLineWidthChanged(float newLineWidth);

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
         * Spacer size, in points.
         */
        static constexpr unsigned spacerSize = 32;

        /**
         * Called from the constructor to configure this dialog.
         *
         * \param[in] horizontalOrientation If true, the dialog will be configured for horizontal orientation.  If
         *                                  false, the dialog will be configured for vertical orientatin.
         */
        void configureWidget(bool horizontalOrientation);

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
         * The input set of right/bottom line styles.
         */
        QSet<LineStyle> inputRightBottomLineStyles;

        /**
         * The input right/bottom line width.  A value of 0 or a negative value indicates an invalid or multiple input
         * right/bottom line widths.
         */
        float inputRightBottomLineWidth;

        /**
         * The input set of left/top line styles.
         */
        QSet<LineStyle> inputLeftTopLineStyles;

        /**
         * The input left/top line width.  A value of 0 or a negative value indicates an invalid or multiple input
         * left/top line widths.
         */
        float inputLeftTopLineWidth;

        /**
         * The input set of default cell colors.
         */
        QSet<QColor> inputDefaultCellColors;
};

#endif
