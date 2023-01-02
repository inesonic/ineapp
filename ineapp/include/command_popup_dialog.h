/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref CommandPopupDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef COMMAND_POPUP_DIALOG_H
#define COMMAND_POPUP_DIALOG_H

#include <QDialog>
#include <QString>
#include <QFont>
#include <QAction>
#include <QSharedPointer>
#include <QMap>

#include <eqt_map_key_validator.h>

#include <ld_special_characters.h>

#include "app_common.h"

class QWidget;
class QTableView;

namespace EQt {
    class CommandLineEdit;
}

class CommandListModel;

/**
 * Class that provides a small command popup menu that the user can use to enter commands.  The dialog will
 * automatically trigger the associated action.
 */
class APP_PUBLIC_API CommandPopupDialog:public QDialog {
    Q_OBJECT

    public:
        /**
         * Dictionary of Qt actions by command string.
         */
        typedef QMap<QString, QAction*> ActionsByCommand;

        /**
         * The default prefix to be displayed in front of the user's command text.
         */
        static const QString defaultPrefix;

        /**
         * Constructor.
         *
         * \param[in] parent The parent object for this menu.
         */
        CommandPopupDialog(QWidget* parent = Q_NULLPTR);

        /**
         * Constructor.
         *
         * \param[in] commandMap A map of QAction instances indexed by command.  The \ref CommandPopupDialog will use
         *                       this map to tie commands to actions.  The tool-tip for each QAction will be used as
         *                       the description for the command.
         *
         * \param[in] parent     The parent object for this window.
         */
        CommandPopupDialog(const ActionsByCommand& commandMap, QWidget* parent = Q_NULLPTR);

        /**
         * Constructor.
         *
         * \param[in] font   The font to be used for the list and completer.
         *
         * \param[in] parent The parent object for this menu.
         */
        CommandPopupDialog(const QFont& font, QWidget* parent = Q_NULLPTR);

        /**
         * Constructor.
         *
         * \param[in] commandMap A map of QAction instances indexed by command.  The \ref CommandPopupDialog will use
         *                       this map to tie commands to actions.  The tool-tip for each QAction will be used as
         *                       the description for the command.
         *
         * \param[in] font       The font to be used for the list and completer.
         *
         * \param[in] parent     The parent object for this window.
         */
        CommandPopupDialog(const ActionsByCommand& commandMap, const QFont& font, QWidget* parent = Q_NULLPTR);

        ~CommandPopupDialog() override;

        /**
         * Method you can use to associate a set of actions to commands.
         *
         * \param[in] newCommandMap A map of QAction instances indexed by command.  The \ref CommandPopupDialog will
         *                          use this map to tie commands to actions.  The tool-tip for each QAction will be
         *                          used as the description for the command.
         */
        void setCommands(const ActionsByCommand& newCommandMap);

        /**
         * Method you can use to obtain the current map of actions, keyed by commands.
         *
         * \return Returns a map of QAction instances indexed by command.
         */
        const ActionsByCommand& commands() const;

        /**
         * Method you can use to tie a specific action to a command.
         *
         * \param[in] command           The command to tie to the action.
         *
         * \param[in] action            The action to tie to the command.
         *
         * \param[in] overwriteExisting If true, existing commands will be replaced.  If false, the method will refuse
         *                              to overwrite existing commands.
         *
         * \return Returns true if the command was new.  Returns false if the command already existed in the command
         *         map.
         */
        bool setCommand(const QString& command, QAction* action, bool overwriteExisting = false);

        /**
         * Method you can use to specify the prefix to be placed in front of the user's command.
         *
         * \param[in] newPrefix The prefix to be placed in front of the user's command.  The value will be displayed but
         *                      will not be included in the command string.
         */
        void setPrefix(const QString& newPrefix);

        /**
         * Method you can use to obtain the current prefix.
         *
         * \return Returns the prefix to be placed in front of the user's command.  The value will be displayed but will
         *         not be included in the command string.
         */
        QString prefix() const;

        /**
         * Method you can use to set the font to use for both the line editor and completer.
         *
         * \param[in] newFont The font to be used.
         */
        void setFont(const QFont& newFont);

        /**
         * Method you can use to query the font being used for both the line editor and completer.
         *
         * \return Returns the font being used.
         */
        QFont font() const;

    public slots:
        /**
         * Slot that can be triggered to display this dialog as an application model dialog.  On success, the dialog
         * will trigger the appropriate action for the command.
         */
        void execute();

    private slots:
        /**
         * Slot that can be triggered to display this dialog as an application modal dialog.  Made private to prevent
         * the normal version from being called.
         *
         * \return Returns the completion status of the dialog, either QDialog::Accepted or QDialog::Rejected.
         */
        int exec() final;

        /**
         * Slot that can be triggered to display this dialog as a window modal dialog.  Made private to prevent the
         * normal version from being called.
         */
        void open() final;

    private slots:
        /**
         * Slot that is triggered when a special key in the line editor is pressed.
         *
         * \param[in] key The key that was pressed.
         */
        void specialKeyPressed(int key);

        /**
         * Slot that is triggered when the recommended minimum column widths or row height changes.  The method adjusts
         * the minimum width of each column and the row heights of the completer's popup when this slot is triggered.
         *
         * \param[in] model The model that triggered the call to this slot.
         */
        void recommendedGeometryChanged(CommandListModel* model);

    private:
        /**
         * Method that updates the current descriptions by command map.
         */
        void updateDescriptionsByCommand();

        /**
         * Map of descriptions by command.
         */
        typedef QMap<QString, QString> DescriptionsByCommand;

        /**
         * Performs initialization common to all constructors.
         */
        void configure();

        /**
         * Map binding commands to actions.
         */
        ActionsByCommand currentActionsByCommand;

        /**
         * Map of descriptions by command.  The map is updated each time the dialog is displayed and cleared when the
         * dialog closes.
         */
        DescriptionsByCommand currentDescriptionsByCommand;

        /**
         * The dialog combo box.
         */
        EQt::CommandLineEdit* currentLineEditor;

        /**
         * Validator used by the line editor.
         */
        EQt::MapKeyValidator<DescriptionsByCommand>* currentValidator;

        /**
         * The model used by the line editor.
         */
        CommandListModel* currentModel;

        /**
         * The table view used by the completer.
         */
        QTableView* currentView;
};

#endif
