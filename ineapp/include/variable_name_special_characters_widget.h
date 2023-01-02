/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref VariableNameSpecialCharactersWidget class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef VARIABLE_NAME_SPECIAL_CHARACTERS_WIDGET_H
#define VARIABLE_NAME_SPECIAL_CHARACTERS_WIDGET_H

#include <QWidget>

#include <ld_variable_name.h>
#include <ld_special_characters.h>

#include "app_common.h"

namespace EQt {
    class ToolButton;
    class ToolButtonArray;
}

/**
 * Widget you can use to display a matrix of special characters allowed in variable names.
 */
class APP_PUBLIC_API VariableNameSpecialCharactersWidget:public QWidget {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        VariableNameSpecialCharactersWidget(QWidget* parent = Q_NULLPTR);

        ~VariableNameSpecialCharactersWidget() override;

    signals:
        /**
         * Signal that is emitted when a new special character is selected.
         *
         * \param[out] character The newly selected special character.
         */
        void selected(const QString& character);

    private slots:
        /**
         * Slot that is triggered when a greek or special characters button is clicked.
         *
         * \param[in] button Pointer to the activated button.
         */
        void buttonClicked(EQt::ToolButton* button);

    private:
        /**
         * Method that builds a tool button array from a list of characters.
         *
         * \param[in]     characterList The list of characters to be added to the array.
         *
         * \param[in]     maximumHeight The maximum height of the array, in buttons.
         *
         * \param[in,out] buttonList    A list that will be appended with the list of newly added buttons.
         *
         * \return Returns a newly created tool button array.
         */
        static EQt::ToolButtonArray* buildButtonArray(
            const QList<Ld::SpecialCharacters::UnicodeValue>& characterList,
            unsigned                                          maximumHeight,
            QList<EQt::ToolButton*>&                          buttonList
        );

        /**
         * The tool button array for greek characters.
         */
        EQt::ToolButtonArray* greekToolButtonArray;

        /**
         * The tool button array for special characters.
         */
        EQt::ToolButtonArray* specialCharactersButtonArray;
};

#endif
