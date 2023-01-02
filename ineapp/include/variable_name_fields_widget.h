/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref VariableNameFieldsWidget class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef VARIABLE_NAME_FIELDS_WIDGET_H
#define VARIABLE_NAME_FIELDS_WIDGET_H

#include <QWidget>
#include <QFont>
#include <QLineEdit>

#include <ld_variable_name.h>

#include "app_common.h"

class QWidget;
class QGroupBox;
class QComboBox;
class QShowEvent;
class QTimer;

namespace EQt {
    class LineEdit;
}

/**
 * Widget you can use to edit a variable name.
 */
class APP_PUBLIC_API VariableNameFieldsWidget:public QWidget {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        VariableNameFieldsWidget(QWidget* parent = Q_NULLPTR);

        /**
         * Constructor
         *
         * \param[in] text1  The primary variable name text.
         *
         * \param[in] parent Pointer to the parent object.
         */
        VariableNameFieldsWidget(const QString& text1, QWidget* parent = Q_NULLPTR);

        /**
         * Constructor
         *
         * \param[in] text1  The primary variable name text.
         *
         * \param[in] text2  The subscript text.
         *
         * \param[in] parent Pointer to the parent object.
         */
        VariableNameFieldsWidget(const QString& text1, const QString& text2, QWidget* parent = Q_NULLPTR);

        /**
         * Constructor
         *
         * \param[in] variableName The variable name.
         *
         * \param[in] parent       Pointer to the parent object.
         */
        VariableNameFieldsWidget(const Ld::VariableName& variableName, QWidget* parent = Q_NULLPTR);

        ~VariableNameFieldsWidget() override;

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
         * Method you can use to determine if this widget currently has focus.
         *
         * \return Returns true if this widget currently has the focus.
         */
        bool hasFocus() const;

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
         * Slot that is triggered when the return key is pressed.
         */
        void returnKeyPressed();

        /**
         * Signal that is emitted when this widget receives focus.
         */
        void receivedFocus();

        /**
         * Signal that is emitted when this widget loses focus.
         */
        void lostFocus();

        /**
         * Signal that is emitted when the focus changes.
         *
         * \param[out] receivedFocus A value that holds true this widget received focus.  Holds false if the widget
         *                           lost focus.
         */
        void focusChanged(bool receivedFocus);

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

        /**
         * Slot you can trigger to insert text at the cursor position.
         *
         * \param[in] text The text to be inserted.
         */
        void insertText(const QString& text);

    protected:
        /**
         * Method that is triggered when the widget is shown.
         *
         * \param[in] event Event that triggered the call to this method.
         */
        void showEvent(QShowEvent* event) override;

    private slots:
        /**
         * Slot that is triggered when the variable text 1 line edit is changed by the user.
         *
         * \param[in] newText The newly entered text.
         */
        void text1Edited(const QString& newText);

        /**
         * Slot that is triggered when the variable text 2 line edit is changed by the user.
         *
         * \param[in] newText The newly entered text.
         */
        void text2Edited(const QString& newText);

        /**
         * Slot that is triggered when the focus changes.
         */
        void editorFocusChanged();

        /**
         * Slot that is triggered after this widget becomes visible.  We use this to conditionally steal the
         * applicaiton focus.
         */
        void nowVisible();

    private:
        /**
         * Called from the constructor to configure this widget.
         */
        void configureWidget();

        /**
         * Method that is called to conditionally build the variable line edit validator regular expressions.
         */
        static void buildRegularExpressions();

        /**
         * Method that updates text in the variable line editor when the user enters new text.  This method substitutes
         * LaTeX commands with the entered content.
         *
         * \param[in]     newText The newly entered text.
         *
         * \param[in,out] isValid An optional boolean value that will hold true on exit if the entered text is
         *                        valid.
         *
         * \return Returns the modified text.
         */
        static QString unLaTeXVariableName(const QString& newText, bool* isValid = Q_NULLPTR);

        /**
         * The text1 variable field regular expression.
         */
        static QRegularExpression text1RegularExpression;

        /**
         * The text1 variable field regular expression.
         */
        static QRegularExpression text2RegularExpression;

        /**
         * The text1 line editor.
         */
        EQt::LineEdit* text1LineEdit;

        /**
         * The text2 line editor.
         */
        EQt::LineEdit* text2LineEdit;

        /**
         * Timer used to defer focus changes when this widget is shown.
         */
        QTimer* showTimer;
};

#endif
