/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref TableDefaultLineStyleDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef TABLE_DEFAULT_LINE_STYLE_DIALOG_H
#define TABLE_DEFAULT_LINE_STYLE_DIALOG_H

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
class APP_PUBLIC_API TableDefaultLineStyleDialog:public EQt::ProgrammaticDialog {
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
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        TableDefaultLineStyleDialog(QWidget* parent = Q_NULLPTR);

        ~TableDefaultLineStyleDialog() override;

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
         * Method that can be called to populate the dialog's widgets.
         */
        void populate() final;

    signals:
        /**
         * Signal that is emitted when the Apply button is clicked on Windows or when any update occurs on MacOS.
         *
         * \param[out] thisDialog A pointer to this dialog.  You can use this parameter to obtain the current values.
         */
        void apply(TableDefaultLineStyleDialog* thisDialog);

    public slots:
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
};

#endif
