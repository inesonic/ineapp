/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ImportsDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef IMPORTS_DIALOG_H
#define IMPORTS_DIALOG_H

#include <QList>
#include <QItemSelection>

#include <eqt_programmatic_dialog.h>

#include <ld_root_import.h>

class QWidget;
class QAction;
class QCloseEvent;

/**
 * Dialog used to modify program imports information.
 */
class ImportsDialog:public EQt::ProgrammaticDialog {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent      Pointer to the parent object.
         */
        ImportsDialog(QWidget* parent = Q_NULLPTR);

        /**
         * Constructor
         *
         * \param[in] newReferencePath The reference path used for relative imports.
         *
         * \param[in] newImports       The list of imports to use as a starting point by this dialog.
         *
         * \param[in] parent           Pointer to the parent object.
         */
        ImportsDialog(
            const QString&               newReferencePath,
            const QList<Ld::RootImport>& newImports,
            QWidget*                     parent = Q_NULLPTR
        );

        ~ImportsDialog() override;

        /**
         * Method you can use to set the reference path used for relative references.
         *
         * \param[in] newReferencePath The reference path to use to locate relative references.
         */
        void setReferencePath(const QString& newReferencePath);

        /**
         * Method you can use to obtain the reference path used for relative references.
         *
         * \return Returns the reference path used for relative references.
         */
        QString referencePath() const;

        /**
         * Method you can use to set the list of imports.
         *
         * \param[in] newImports The new list of imports.
         */
        void setImports(const QList<Ld::RootImport>& newImports);

        /**
         * Method you can use to obtain the current/modified list of imports.
         *
         * \return Returns the current list of imports.
         */
        QList<Ld::RootImport> imports() const;

        /**
         * Method that can be called to populate the dialog's widgets.
         */
        void populate() final;

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
         * Slot that is triggered when the add imports menu is about to be shown.  This slot populates the menu.
         */
        void aboutToShowAddImportsMenu();

        /**
         * Slot that is triggered when an item in the add imports menu is triggered.
         *
         * \param[in] action The action that was triggered by the menu.
         */
        void addImportsMenuItemTriggered(QAction* action);

        /**
         * Slot that is triggered when the Remove Imports button is clicked.
         */
        void removeImportsClicked();

        /**
         * Slot that is triggered when the Move Up button is clicked.
         */
        void moveUpClicked();

        /**
         * Slot that tis triggered when the Move Down button is clicked.
         */
        void moveDownClicked();

        /**
         * Slot that is triggered when the Reset button is clicked.
         */
        void resetClicked();

        /**
         * Slot that can be used to indicate that the user accepted the dialog contents.  This slot updates the
         * current references and then calls the EQt::ProgrammaticDialog::accept method to propagate the new state to
         * every proxy.
         */
        void accept() final;

        /**
         * Slot that is triggered when the user's selection changes.
         *
         * \param[in] selected   The start of the selected region.
         *
         * \param[in] deselected The end of the selected region.
         */
        void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

        /**
         * Slot that is triggered when we are unable to open a file.
         *
         * \param[in] identifier An identifier for the file that we attempted to open.
         */
        void failedToOpen(const QString& identifier);

        /**
         * Slot that is triggered when the underlying model is changed.
         *
         * \param[in] topLeft     The top left index of the changed region.
         *
         * \param[in] bottomRight The bottom right index of the changed region.
         *
         * \param[in] roles       The roles associated with the changed data.
         */
        void modelDataChanged(
            const QModelIndex& topLeft,
            const QModelIndex& bottomRight,
            const QVector<int>& roles = QVector<int>()
        );

    private:
        /**
         * Value used as additional padding applied to the width of the first section.
         */
        static const unsigned firstSectionPadding;

        /**
         * Called from the constructor to configure this dialog.
         */
        void configureWidget();

        /**
         * Called whenever the table view's state changes to selectively enable/disable buttons.
         */
        void updateButtonState();

        /**
         * The list of root references.
         */
        QList<Ld::RootImport> currentImports;
};

#endif
