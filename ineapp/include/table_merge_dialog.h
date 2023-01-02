/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref TableMergeDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef TABLE_MERGE_DIALOG_H
#define TABLE_MERGE_DIALOG_H

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
 * Dialog used to set parameters for a merge operation.
 */
class APP_PUBLIC_API TableMergeDialog:public EQt::ProgrammaticDialog {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        TableMergeDialog(QWidget* parent = Q_NULLPTR);

        /**
         * Constructor
         *
         * \param[in] maximumRowSpan    The maximum row span that can be managed for the current position.
         *
         * \param[in] maximumColumnSpan The maximum column span that can be managed for the current position.
         *
         * \param[in] parent            Pointer to the parent object.
         */
        TableMergeDialog(unsigned maximumRowSpan, unsigned maximumColumnSpan, QWidget* parent = Q_NULLPTR);

        ~TableMergeDialog() override;

        /**
         * Method you can use to obtain the row span requested by the user.
         *
         * \return Returns the row span requested by the user.
         */
        unsigned rowSpan() const;

        /**
         * Method you can use to obtain the column span requested by the user.
         *
         * \return Returns the column span requested by the user.
         */
        unsigned columnSpan() const;

        /**
         * Method you can use to obtain the maximum row span.
         *
         * \return Returns the maximum row span.
         */
        unsigned maximumRowSpan() const;

        /**
         * Method you can use to obtain the maximum column span.
         *
         * \return Returns the maximum column span.
         */
        unsigned maximumColumnSpan() const;

        /**
         * Method that can be called to populate the dialog's widgets.
         */
        void populate() final;

    public slots:
        /**
         * Slot you can trigger to set the row span.
         *
         * \param[in] newRowSpan The new row span.
         */
        void setRowSpan(unsigned newRowSpan);

        /**
         * Slot you can trigger to set the column span.
         *
         * \param[in] newColumnSpan The new column span.
         */
        void setColumnSpan(unsigned newColumnSpan);

        /**
         * Slot you can trigger to set the maximum row span.
         *
         * \param[in] newMaximumRowSpan The new maximum row span.
         */
        void setMaximumRowSpan(unsigned newMaximumRowSpan);

        /**
         * Slot you can trigger to set the maximum column span.
         *
         * \param[in] newMaximumColumnSpan The new maximum column span.
         */
        void setMaximumColumnSpan(unsigned newMaximumColumnSpan);

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
         * Slot that is triggered when the row span has been changed.
         *
         * \param[in] newValue The new row span value.
         */
        void rowSpanChanged(int newValue);

        /**
         * Slot that is triggered when the column span has been changed.
         *
         * \param[in] newValue The new column span value.
         */
        void columnSpanChanged(int newValue);

    private:
        /**
         * Called from the constructor to configure this dialog.
         */
        void configureWidget();

        /**
         * The input row span.
         */
        unsigned inputRowSpan;

        /**
         * The input column span.
         */
        unsigned inputColumnSpan;

        /**
         * The input maximum row span.
         */
        unsigned inputMaximumRowSpan;

        /**
         * The input maximum column span.
         */
        unsigned inputMaximumColumnSpan;
};

#endif
