/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref PlotWidgetBase class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PLOT_WIDGET_BASE_H
#define PLOT_WIDGET_BASE_H

#include <QWizardPage>

#include "app_common.h"

class QWidget;

/**
 * Base class for plot configuration widgets.
 */
class APP_PUBLIC_API PlotWidgetBase:public QWizardPage {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent wizard dialog.
         */
        PlotWidgetBase(QWidget* parent = Q_NULLPTR);

        ~PlotWidgetBase() override;

        /**
         * Method that returns the next widget ID.
         *
         * \return Returns the next widget ID or -1 if this is the last widget.
         */
        int nextId() const override;

        /**
         * Method you can use to change the next wizard page ID.
         *
         * \param[in] newNextId The ID of the next wizard page.
         */
        void setNextId(int newNextId);

        /**
         * Method you can use to determine if the current settings are valid or invalid.
         *
         * \return Returns true if the settings are valid.  Returns false if the settings are invalid.
         */
        virtual bool isValid() const = 0;

        /**
         * Method you can use to determine if the current settings are invalid or valid.
         *
         * \return Returns true if the settings are invalid.  Returns false if the settings are valid.
         */
        bool isInvalid() const;

        /**
         * Method that determines if this page is complete.  This version returns the state of
         * \ref PlotWidgetBase::isValid when \ref PlotWidgetBase::updateChangedStatus was last called.
         *
         * \return Returns true if the widget page is complete.  Returns false if the widget page is not complete.
         */
        bool isComplete() const override;

    signals:
        /**
         * Signal that is emitted whenever any change occurs.
         */
        void changed();

    protected:
        /**
         * Method you can call whenever the widget contents changes.  This method will emit the appropriate signals
         * for the widget.
         */
        void updateChangedStatus();

    private:
        /**
         * The current next page ID.
         */
        int currentNextId;

        /**
         * Enumeration of last widget state values.
         */
        enum class ValidState:unsigned char {
            /**
             * Indicates the last state is unknown.
             */
            UNKNOWN,

            /**
             * Indicates the last state was invalid.
             */
            INVALID,

            /**
             * Indicates the last known state was valid.
             */
            VALID
        };

        /**
         * Flag indicating if the last widget state was valid.
         */
        mutable ValidState lastValidState;
};

#endif
