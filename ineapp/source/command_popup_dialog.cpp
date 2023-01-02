/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref CommandPopupDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QDialog>
#include <QString>
#include <QFont>
#include <QLineEdit>
#include <QValidator>
#include <QVBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QHeaderView>
#include <QCompleter>
#include <QPen>
#include <QSharedPointer>
#include <QStyle>
#include <QSet>
#include <QMap>
#include <QList>

#include <eqt_map_key_validator.h>
#include <eqt_command_line_edit.h>

#include <ld_capabilities.h>
#include <ld_special_characters.h>

#include "application.h"
#include "command_list_model.h"
#include "command_popup_dialog.h"

const QString CommandPopupDialog::defaultPrefix("\\");


CommandPopupDialog::CommandPopupDialog(QWidget* parent):QDialog(parent, Qt::Popup) {
    configure();
}


CommandPopupDialog::CommandPopupDialog(
        const CommandPopupDialog::ActionsByCommand& commandMap,
        QWidget*                                    parent
    ):QDialog(
        parent,
        Qt::Popup
    ) {
    configure();
    setCommands(commandMap);
}


CommandPopupDialog::CommandPopupDialog(const QFont& font, QWidget* parent):QDialog(parent, Qt::Popup) {
    configure();
    setFont(font);
}


CommandPopupDialog::CommandPopupDialog(
        const CommandPopupDialog::ActionsByCommand& commandMap,
        const QFont&                                font,
        QWidget*                                    parent
    ):QDialog(
        parent,
        Qt::Popup
    ) {
    configure();
    setFont(font);
    setCommands(commandMap);
}


CommandPopupDialog::~CommandPopupDialog() {}


void CommandPopupDialog::setCommands(const ActionsByCommand& newCommandMap) {
    currentActionsByCommand = newCommandMap;
}


const CommandPopupDialog::ActionsByCommand& CommandPopupDialog::commands() const {
    return currentActionsByCommand;
}


bool CommandPopupDialog::setCommand(const QString& command, QAction* action, bool overwriteExisting) {
    bool result = !currentActionsByCommand.contains(command);

    if (overwriteExisting || result) {
        currentActionsByCommand.insert(command, action);
    }

    return result;
}


void CommandPopupDialog::setPrefix(const QString& newPrefix) {
    currentValidator->setPrefix(newPrefix);
    currentModel->setPrefix(newPrefix);
}


QString CommandPopupDialog::prefix() const {
    return currentValidator->prefix();
}


void CommandPopupDialog::setFont(const QFont& newFont) {
    currentLineEditor->setFont(newFont);
    currentModel->setFont(newFont);
}


QFont CommandPopupDialog::font() const {
    return currentLineEditor->font();
}


void CommandPopupDialog::execute() {
    updateDescriptionsByCommand();
    currentModel->setMap(&currentDescriptionsByCommand);

    currentLineEditor->setText(currentValidator->prefix());
    currentLineEditor->setFocus();

    int dialogResult = QDialog::exec();

    if (dialogResult == QDialog::Accepted) {
        QString  command = currentLineEditor->text().mid(currentValidator->prefix().length());
        QAction* action  = currentActionsByCommand.value(command, Q_NULLPTR);

        if (action != Q_NULLPTR) {
            action->trigger();
        }
    }

    currentView->hide();
    currentDescriptionsByCommand.clear();
}


int CommandPopupDialog::exec() {
    return QDialog::Rejected;
}


void CommandPopupDialog::open() {}


void CommandPopupDialog::specialKeyPressed(int) {
    const QValidator* validator = currentLineEditor->validator();
    QString           text      = currentLineEditor->text();
    int               position  = currentLineEditor->cursorPosition();

    if (validator->validate(text, position) == QValidator::Acceptable) {
        accept();
    }
}


void CommandPopupDialog::recommendedGeometryChanged(CommandListModel *model) {
    // Thanks to: http://stackoverflow.com/questions/31585184/qt-completer-cant-have-its-size-set for details on how
    // to make this work nicely.

    const QStyle* style       = Application::style();
    unsigned padding          = currentView->width() - currentView->viewport()->width();
    unsigned scrollbarWidth   = style->pixelMetric(QStyle::PM_ScrollBarExtent);
    unsigned frameWidth       = currentView->frameWidth();
    unsigned cellLeftPadding  = style->pixelMetric(QStyle::PM_LayoutLeftMargin);
    unsigned cellRightPadding = style->pixelMetric(QStyle::PM_LayoutRightMargin);
    unsigned column1Width     = model->recommendedColumn1Width() + cellLeftPadding + cellRightPadding;
    unsigned column2Width     = model->recommendedColumn2Width() + cellLeftPadding + cellRightPadding;
    unsigned recommendedWidth = column1Width + column2Width + 4 * padding + 2 * frameWidth + scrollbarWidth;

    currentView->horizontalHeader()->resizeSection(0, column1Width);
    currentView->horizontalHeader()->resizeSection(1, column2Width);

    currentView->setColumnWidth(0, column1Width);
    currentView->setColumnWidth(1, column2Width);

    currentView->setMinimumWidth(recommendedWidth);

    unsigned rowHeight = model->recommendedRowHeight();
    currentView->verticalHeader()->setDefaultSectionSize(rowHeight);
}


void CommandPopupDialog::configure() {
    currentActionsByCommand.clear();
    currentDescriptionsByCommand.clear();

    QVBoxLayout* layout = new QVBoxLayout;
    setLayout(layout);

    layout->setContentsMargins(0, 0, 0, 0);

    currentLineEditor = new EQt::CommandLineEdit;
    layout->addWidget(currentLineEditor);

    currentValidator = new EQt::MapKeyValidator<DescriptionsByCommand>(currentLineEditor);
    currentValidator->setPrefix(defaultPrefix);
    currentValidator->setMap(&currentDescriptionsByCommand);

    currentLineEditor->setValidator(currentValidator);
    currentLineEditor->setCommandCompletionEnabled(true);

    QSet<int> specialKeys;
    specialKeys.insert(Qt::Key_Space);
    specialKeys.insert(Qt::Key_Return);
    specialKeys.insert(Qt::Key_Enter);
    currentLineEditor->setSpecialKeys(specialKeys);

    currentLineEditor->setToolTip(tr("Enter command.\nNote: esc to cancel, space to accept, tab to auto-complete"));

    QCompleter* completer = new QCompleter(currentLineEditor);

    currentModel = new CommandListModel(&currentDescriptionsByCommand, completer);
    currentModel->setPrefix(defaultPrefix);

    connect(
        currentModel,
        SIGNAL(recommendedGeometryChanged(CommandListModel*)),
        this,
        SLOT(recommendedGeometryChanged(CommandListModel*))
    );

    completer->setModel(currentModel);

    currentView = new QTableView;
    completer->setPopup(currentView);

    currentView->setShowGrid(false);
    currentView->horizontalHeader()->hide();
    currentView->verticalHeader()->hide();
    currentView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    currentView->setWordWrap(false);
    currentView->setTextElideMode(Qt::ElideNone);
    currentView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    currentView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    currentLineEditor->setCompleter(completer);

    connect(currentLineEditor, SIGNAL(specialKeyPressed(int)), this, SLOT(specialKeyPressed(int)));
}


void CommandPopupDialog::updateDescriptionsByCommand() {
    if (!currentDescriptionsByCommand.isEmpty()) {
        currentDescriptionsByCommand.clear();
    }

    for (  ActionsByCommand::const_iterator pos = currentActionsByCommand.constBegin(),
                                            end = currentActionsByCommand.constEnd()
         ; pos != end
         ; ++pos
        ) {
        QAction* action = pos.value();
        if (action->isEnabled()) {
            QString command     = pos.key();
            QString description = action->toolTip();
            currentDescriptionsByCommand.insert(command, description);
        }
    }
}
