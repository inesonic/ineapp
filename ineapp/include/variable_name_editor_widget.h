/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref VariableNameEditorWidget class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef VARIABLE_NAME_EDITOR_WIDGET_H
#define VARIABLE_NAME_EDITOR_WIDGET_H

#include <QWidget>
#include <QFont>
#include <QLineEdit>

#include <ld_variable_name.h>

#include "app_common.h"

class VariableNameFieldsWidget;
class VariableNameSpecialCharactersWidget;

/**
 * Widget you can use to edit a variable name.
 */
class APP_PUBLIC_API VariableNameEditorWidget:public QWidget {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        VariableNameEditorWidget(QWidget* parent = Q_NULLPTR);

        /**
         * Constructor
         *
         * \param[in] text1  The primary variable name text.
         *
         * \param[in] parent Pointer to the parent object.
         */
        VariableNameEditorWidget(const QString& text1, QWidget* parent = Q_NULLPTR);

        /**
         * Constructor
         *
         * \param[in] text1  The primary variable name text.
         *
         * \param[in] text2  The subscript text.
         *
         * \param[in] parent Pointer to the parent object.
         */
        VariableNameEditorWidget(const QString& text1, const QString& text2, QWidget* parent = Q_NULLPTR);

        /**
         * Constructor
         *
         * \param[in] variableName The variable name.
         *
         * \param[in] parent       Pointer to the parent object.
         */
        VariableNameEditorWidget(const Ld::VariableName& variableName, QWidget* parent = Q_NULLPTR);

        ~VariableNameEditorWidget() override;

        /**
         * Method you can use to determine if the current variable name is valid.
         *
         * \return Returns true if the variable name is valid.  Returns false if the variable name is invalid.
         */
        bool isValid() const;

        /**
         * Method you can use to determine if the current variable name is invalid.
         *
         * \return Returns true if the variable name is invalid.  Returns false if the variable name is valid.
         */
        bool isInvalid() const;

        /**
         * Method you can use to obtain the current text1 value.
         *
         * \return Returns the main variable text.
         */
        QString text1() const;

        /**
         * Method you can use to obtain the current text2 value.
         *
         * \return Returns the main variable text.
         */
        QString text2() const;

        /**
         * Method you can use to obtain the current variable name.
         *
         * \return Returns the currently displayed variable name.
         */
        Ld::VariableName variableName() const;

        /**
         * Method you can use to obtain the current variable name font.
         *
         * \return Returns the current variable name font.
         */
        QFont variableNameFont() const;

        /**
         * Method you can use to validate a variable name.
         *
         * \param[in] text1 The first text line to validate.
         *
         * \param[in] text2 The subscript text to validate.
         *
         * \return Returns true if the variable name is correctly formed.  Returns false if the variable name is not
         *         correctly formed.
         */
        static bool validate(const QString& text1, const QString& text2);

        /**
         * Method you can use to validate a variable name.
         *
         * \param[in] variableName The variable name to validate.
         *
         * \return Returns true if the variable name is correctly formed.  Returns false if the variable name is not
         *         correctly formed.
         */
        static bool validate(const Ld::VariableName& variableName);

    signals:
        /**
         * Signal that is emitted when the variable name is changed.
         *
         * \param[out] text1 The variable name text1.
         *
         * \param[out] text2 The variable name subscript text.
         */
        void changed(const QString& text1, const QString& text2);

        /**
         * Signal that is emitted when the return key is pressed.
         */
        void returnKeyPressed();

    public slots:
        /**
         * Slot you can use to set the font used for the variable name.
         *
         * \param[in] newFont The new variable name font.
         */
        void setVariableNameFont(const QFont& newFont);

        /**
         * Slot you can use to change the variable name, text1 field.
         *
         * \param[in] newText The new text to apply to the field.
         */
        void setText1(const QString& newText);

        /**
         * Slot you can use to change the variable name, text2 field.
         *
         * \param[in] newText The new text to apply to the field.
         */
        void setText2(const QString& newText);

        /**
         * Slot you can use to change the variable name.
         *
         * \param[in] newVariableName The new variable name.
         */
        void setVariableName(const Ld::VariableName& newVariableName);

    private slots:
        /**
         * Slot that is triggered when one of the edit dialogs receives or loses focus.
         *
         * \param[in] nowHasFocus If true, one of the editor windows has focus.  If false, none of the editor windows
         *                        have focus.
         */
        void focusChanged(bool nowHasFocus);

    private:
        /**
         * Called from the constructor to configure this widget.
         */
        void configureWidget();

        /**
         * The variable name edit fields.
         */
        VariableNameFieldsWidget* fieldsWidget;

        /**
         * The special characters tool button array.
         */
        VariableNameSpecialCharactersWidget* specialCharactersWidget;
};

#endif
