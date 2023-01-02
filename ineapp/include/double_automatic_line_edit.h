/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref DoubleAutomaticLineEdit class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef DOUBLE_AUTOMATIC_LINE_EDIT_H
#define DOUBLE_AUTOMATIC_LINE_EDIT_H

#include <QWidget>
#include <QString>

#include "app_common.h"

class QLineEdit;
class QDoubleValidator;
class QCheckBox;

/**
 * Widget that support entry of double precision numbers with the ability to mark a value as "automatically"
 * calculated.
 */
class APP_PUBLIC_API DoubleAutomaticLineEdit:public QWidget {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent wizard dialog.
         */
        DoubleAutomaticLineEdit(QWidget* parent = Q_NULLPTR);

        /**
         * Constructor
         *
         * \param[in] initialValue The initial value.
         *
         * \param[in] parent       Pointer to the parent wizard dialog.
         */
        DoubleAutomaticLineEdit(double initialValue, QWidget* parent = Q_NULLPTR);

        /**
         * Constructor
         *
         * \param[in] initialValue The initial value.
         *
         * \param[in] automatic    A flag indicating if the initial value should be automatically calculated.
         *
         * \param[in] parent       Pointer to the parent wizard dialog.
         */
        DoubleAutomaticLineEdit(double initialValue, bool automatic, QWidget* parent = Q_NULLPTR);

        ~DoubleAutomaticLineEdit() override;

        /**
         * Method you can use to determine if the current value is valid.
         *
         * \return Returns true if the current value is valid.  Returns false if the current value is invalid.
         */
        bool isValid() const;

        /**
         * Method you can use to determine if the current value is invalid.
         *
         * \return Returns true if the current value is invalid.  Returns false if the current value is valid.
         */
        bool isInvalid() const;

        /**
         * Method you can use to determine if the current value should be automatically calculated.
         *
         * \return Returns true if the automatic check box is checked.
         */
        bool automatic() const;

        /**
         * Method you can use to determine the current value.  A value of NaN is returned if the automatic check box
         * is clicked or the entered value is currently invalid.
         *
         * \return Returns the currently entered value.
         */
        double value() const;

        /**
         * Method you can use to determine the minimum allowed value.
         *
         * \return Returns the minimum allowed value.
         */
        double minimumValue() const;

        /**
         * Method you can use to determine the maximum allowed value.
         *
         * \return Returns the maximum allowed value.
         */
        double maximumValue() const;

        /**
         * Method you can use to determine if the automatic check box is enabled.
         *
         * \return Returns true if the automatic check box is enabled.  Returns false if the automatic check box is
         *         disabled.
         */
        bool automaticCheckboxEnabled() const;

        /**
         * Method you can use to determine if the automatic check box is disabled.
         *
         * \return Returns true if the automatic check box is disabled.  Returns false if the automatic check box is
         *         enabled.
         */
        bool automaticCheckboxDisabled() const;

        /**
         * Method you can use to determine if the line editor is enabled.
         *
         * \return Returns true if the line editor is enabled.  Returns false if the line editor is disabled.
         */
        bool lineEditorEnabled() const;

        /**
         * Method you can use to determine if the line editor is disabled.
         *
         * \return Returns true if the line editor is disabled.  Returns false if the line editor is enabled.
         */
        bool lineEditorDisabled() const;

    signals:
        /**
         * Signal that is emitted when the current state has changed.
         *
         * \param[out] newValue  The newly entered value.
         *
         * \param[out] automatic Flag holding true if the value is now automatic.
         */
        void changed(double newValue, bool automatic);

    public slots:
        /**
         * Slot you can trigger to set or clear the automatic flag.
         *
         * \param[in] nowAutomatic If true, the automatic flag will be set.  If false, the automatic flag will be
         *                         cleared.
         */
        void setAutomatic(bool nowAutomatic = true);

        /**
         * Slot you can trigger to clear or set the automatic flag.
         *
         * \param[in] nowManual If true, the automatic flag will be cleared.  If false, the automatic flag will be
         *                      set.
         */
        void setManual(bool nowManual = true);

        /**
         * Slot you can trigger to set the current value.  This slot will also trigger manual entry.
         *
         * \param[in] newValue The new value to be displayed.
         */
        void setValue(double newValue);

        /**
         * Slot you can trigger to set the minimum allowed value.
         *
         * \param[in] newMinimum The new minimum value.
         */
        void setMinimumValue(double newMinimum);

        /**
         * Slot you can trigger to set the maximum allowed value.
         *
         * \param[in] newMaximum The new maximum value.
         */
        void setMaximumValue(double newMaximum);

        /**
         * Slot you can trigger to enable and show or disable and hide the automatic check box.
         *
         * \param[in] nowEnabled If true, the check box will be enabled and shown.  If false, the check box will be
         *                       disabled and hidden.
         */
        void setAutomaticCheckboxEnabled(bool nowEnabled = true);

        /**
         * Slot you can trigger to disable and hide or enable and show the automatic check box.
         *
         * \param[in] nowDisabled If true, the check box will be disabled and hidden.  If false, the check box will
         *                        be enabled and shown.
         */
        void setAutomaticCheckboxDisabled(bool nowDisabled = true);

        /**
         * Slot you can trigger to show this line editor.
         *
         * \param[in] nowEnabled If true, the line editor will be made visible.  If false, the line editor will be
         *                       hidden.
         */
        void setLineEditorEnabled(bool nowEnabled = true);

        /**
         * Slot you can trigger to hide this line editor.
         *
         * \param[in] nowDisabled If true, the line editor will be hidden and disabled.  If false, the line editor will
         *                        be shown and enabled.
         */
        void setLineEditorDisabled(bool nowDisabled = true);

    private slots:
        /**
         * Slot that is triggered when a new value is entered.
         *
         * \param[in] text The newly entered text.
         */
        void textChanged(const QString& text);

        /**
         * Slot that is triggered when the automatic check box is changed.
         *
         * \param[in] checked If true, the check box is now checked.  If false, the check box is now unchecked.
         */
        void automaticChanged(bool checked);

    private:
        /**
         * Method used by the constructors to create this widget.
         */
        void configureWidget();

        /**
         * Flag holding true if the axis scaling combo box should be enabled.
         */
        bool automaticControlsEnabled;

        /**
         * Flag holding true if the line editor should be enabled.
         */
        bool editorEnabled;

        /**
         * The line editor.
         */
        QLineEdit* doubleLineEditor;

        /**
         * The validator used by the line editor.
         */
        QDoubleValidator* doubleValidator;

        /**
         * The automatic check box.
         */
        QCheckBox* automaticCheckBox;

        /**
         * The last line editor value.
         */
        QString lastLineEditorValue;
};

#endif
