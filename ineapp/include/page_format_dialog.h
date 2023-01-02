/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PageFormatDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PAGE_FORMAT_DIALOG_H
#define PAGE_FORMAT_DIALOG_H

#include <QSet>
#include <QSharedPointer>

#include <util_page_size.h>
#include <eqt_programmatic_dialog.h>

#include <ld_page_format.h>

#include "app_common.h"

class QWidget;
class QGroupBox;
class QComboBox;

/**
 * Dialog used to adjust one or more page format instances.
 *
 * Note that due to the level of visibility required, this dialog operates directly on page format aggregations rather
 * than on the various parameters within the page format aggregation.  Eventually we should find a way to cleanly
 * decouple the aggregations from the dialog.
 */
class APP_PUBLIC_API PageFormatDialog:public EQt::ProgrammaticDialog {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        PageFormatDialog(QWidget* parent = Q_NULLPTR);

        /**
         * Constructor
         *
         * \param[in] newAggregation The page format aggregation that should be used to populate this dialog.
         *
         * \param[in] parent         Pointer to the parent object.
         */
        PageFormatDialog(const Ld::PageFormat::Aggregation& newAggregation, QWidget* parent = Q_NULLPTR);

        ~PageFormatDialog() override;

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
         * Method you can use to obtain the aggregation being used by this dialog.
         *
         * \return Returns a pointer to the aggregation being used by this dialog.
         */
        const Ld::PageFormat::Aggregation& inputAggregation() const;

        /**
         * Method you can use to obtain a reference to the aggregation representing the updated values form this dialog.
         *
         * \return Returns a reference to a modified version of the aggregation.
         */
        const Ld::PageFormat::Aggregation& outputAggregation() const;

        /**
         * Method that can be called to populate the dialog's widgets.
         */
        void populate() final;

    public slots:
        /**
         * Slot you can use to set the page format aggregation that this dialog will use.
         *
         * \param[in] newAggregation The page format aggregation that should be used to populate this dialog.
         */
        void setAggregation(const Ld::PageFormat::Aggregation& newAggregation);

    signals:
        /**
         * Signal that is emitted when the Apply button is clicked on Windows or when any update occurs on MacOS.
         *
         * \param[out] thisDialog A pointer to this dialog.  You can use this parameter to obtain the current values.
         */
        void apply(PageFormatDialog* thisDialog);

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
        void closeEvent(QCloseEvent* event) final;

    private slots:
        /**
         * Slot that is triggered when the page size is changed.
         *
         * \param[in] newPageSize The description for the newly seleted page size.
         */
        void pageSizeChanged(const QString& newPageSize);

        /**
         * Slot that is triggered when the user clicks on the portrait page orientation.
         */
        void portraitOrientationClicked();

        /**
         * Slot that is triggered when the user clicks on the landscape page orientation.
         */
        void landscapeOrientationClicked();

        /**
         * Slot that is triggered when the user updates the left page margin value.
         *
         * \param[in] newLeftMarginValue the new left margin value.
         */
        void leftMarginChanged(double newLeftMarginValue);

        /**
         * Slot that is triggered when the user updates the right page margin value.
         *
         * \param[in] newRightMarginValue the new right margin value.
         */
        void rightMarginChanged(double newRightMarginValue);

        /**
         * Slot that is triggered when the user updates the top page margin value.
         *
         * \param[in] newTopMarginValue the new top margin value.
         */
        void topMarginChanged(double newTopMarginValue);

        /**
         * Slot that is triggered when the user updates the bottom page margin value.
         *
         * \param[in] newBottomMarginValue the new bottom margin value.
         */
        void bottomMarginChanged(double newBottomMarginValue);

        /**
         * Slot that is triggered when the "Apply" button is clicked.
         */
        void applyClicked();

    private:
        /**
         * Called from the constructor to configure this dialog.
         */
        void configureWidget();

        /**
         * Method that is called when a value changes.  The method checks for updates and adjusts the dialog
         * accordingly.
         */
        void updateDialog();

        /**
         * Method that creates the page size group box.
         *
         * \return Returns a pointer to a newly created, populated, group box.
         */
        QGroupBox* createPageSizeGroupBox();

        /**
         * Method that creates the page orientation group box.
         *
         * \return Returns a pointer to a newly created, populated, group box.
         */
        QGroupBox* createPageOrientationGroupBox();

        /**
         * Method that creates the page margins group box.
         *
         * \return Returns a pointer to the newly created, populated, group box.
         */
        QGroupBox* createPageMarginsGroupBox();

        /**
         * Method that populates a QComboBox with paper size values.
         *
         * \param[in] comboBox A combo box to be populated with page size values.
         */
        void populateComboBox(QComboBox* comboBox);

        /**
         * Flag that indicates if auto-apply is enabled.
         */
        bool currentAutoApplyEnabled;

        /**
         * The current page format aggregation used to populate this dialog.
         */
        Ld::PageFormat::Aggregation currentInputAggregation;

        /**
         * A set of clones of all the formats we may be adjusting in this dialog.
         */
        QSet<QSharedPointer<Ld::PageFormat>> allFormats;

        /**
         * Aggregation we used to calculate dialog limits based on user updates.
         */
        Ld::PageFormat::Aggregation workingAggregation;
};

#endif
