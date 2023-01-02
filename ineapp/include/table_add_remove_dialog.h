/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref TableAddRemoveDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef TABLE_ADD_REMOVE_DIALOG_H
#define TABLE_ADD_REMOVE_DIALOG_H

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
 * Dialog used to obtain row/column counts when adding or removing rows or columns.
 */
class APP_PUBLIC_API TableAddRemoveDialog:public EQt::ProgrammaticDialog {
    Q_OBJECT

    public:
        /**
         * Dialog mode
         */
        enum class DialogMode {
            /**
             * Indicates the dialog is being used to add rows.
             */
            ADD_ROWS,

            /**
             * Indicates the dialog is being used to add columns.
             */
            ADD_COLUMNS,

            /**
             * Indicates the dialog is being used to remove rows.
             */
            REMOVE_ROWS,

            /**
             * Indicates teh dialog is being used to remove columns.
             */
            REMOVE_COLUMNS
        };

        /**
         * Constructor
         *
         * \param[in] dialogMode Indicates the dialog mode.
         *
         * \param[in] parent     Pointer to the parent object.
         */
        TableAddRemoveDialog(DialogMode dialogMode, QWidget* parent = Q_NULLPTR);

        ~TableAddRemoveDialog() override;

        /**
         * Method you can use to obtain the dialog mode setting.
         *
         * \return Returns the dialog mode setting.
         */
        DialogMode dialogMode() const;

        /**
         * Method you can use to obtain the number of rows or columns.
         *
         * \return Returns the number of rows or columns.
         */
        unsigned numberEntries() const;

        /**
         * Method you can use to obtain the maximum number of rows or columns if "before" is clicked.
         *
         * \return Returns the maximum number of rows or columns if "before" is clicked.
         */
        unsigned beforeMaximumNumberEntries() const;

        /**
         * Method you can use to obtain the maximum number of rows or columns if "after" is clicked.
         *
         * \return Returns the maximum number of rows or columns if "after" is clicked.
         */
        unsigned afterMaximumNumberEntries() const;

        /**
         * Method you can use to determine if the entry should be added before.
         *
         * \return Returns true if the entry should be added before.  Returns false if the entry should be added after.
         */
        bool addBefore() const;

        /**
         * Method you can use to determine if the entry should be added after.
         *
         * \return Returns true if the entry should be added after.  Returns false if the entry should be added before.
         */
        bool addAfter() const;

        /**
         * Method that can be called to populate the dialog's widgets.
         */
        void populate() final;

    public slots:
        /**
         * Slot you can trigger to specify the number of rows or columns.
         *
         * \param[in] newNumberEntries The new number of rows or columns to be added.
         */
        void setNumberEntries(unsigned newNumberEntries);

        /**
         * Slot you can trigger to set the before maximum number of rows or columns.
         *
         * \param[in] newMaximumNumberEntries The new maximum number entries.
         */
        void setBeforeMaximumNumberEntries(unsigned newMaximumNumberEntries);

        /**
         * Slot you can trigger to set the after maximum number of rows or columns.
         *
         * \param[in] newMaximumNumberEntries The new maximum number entries.
         */
        void setAfterMaximumNumberEntries(unsigned newMaximumNumberEntries);

        /**
         * Slot you can trigger to specify that the entry should be added before or after.
         *
         * \param[in] nowBefore If true, the entry will be flagged to be added before the current entry.  If false, the
         *                      entry will be flagged to be added after the current entry.
         */
        void setEntryAddedBefore(bool nowBefore = true);

        /**
         * Slot you can trigger to specify that the entry should be added after or before.
         *
         * \param[in] nowAfter If true, the entry will be flagged to be added after the current entry.  If false, the
         *                     entry will be flagged to be added before the current entry.
         */
        void setEntryAddedAfter(bool nowAfter = true);

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
         * Slot that is triggered when the "before" button is clicked.
         */
        void beforeClicked();

        /**
         * Slot that is triggered when the "after" button is clicked.
         */
        void afterClicked();

    private:
        /**
         * Called from the constructor to configure this dialog.
         *
         * \param[in] dialogMode Indicates the dialog mode.
         */
        void configureWidget(DialogMode dialogMode);

        /**
         * The current dialog mode.
         */
        DialogMode currentDialogMode;

        /**
         * The input number of entries.
         */
        unsigned inputNumberEntries;

        /**
         * The input before maximum number of entries.
         */
        unsigned inputBeforeMaximumNumberEntries;

        /**
         * The input after maximum number of entries.
         */
        unsigned inputAfterMaximumNumberEntries;

        /**
         * The input flag indicating if the entries should be added before.
         */
        unsigned inputAddBefore;
};

#endif
