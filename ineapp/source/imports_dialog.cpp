/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ImportsDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableView>
#include <QItemSelectionModel>
#include <QItemSelection>
#include <QHeaderView>
#include <QGroupBox>
#include <QPushButton>
#include <QMenu>
#include <QMap>
#include <QVariant>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QFont>
#include <QFontMetrics>
#include <QCloseEvent>

#include <algorithm>

#include <ld_element.h>
#include <ld_root_element.h>
#include <ld_root_import.h>

#include <eqt_programmatic_dialog.h>
#include <eqt_builder_base.h>
#include <eqt_builder.h>

#include "document.h"
#include "application.h"
#include "plug_in_manager.h"
#include "document_file_dialog.h"
#include "imports_model.h"
#include "imports_delegate.h"
#include "imports_dialog.h"

const unsigned ImportsDialog::firstSectionPadding = 20;

ImportsDialog::ImportsDialog(QWidget* parent):ProgrammaticDialog("ImportsDialog", parent) {
    setWindowTitle(tr("Imports"));
    setSizeGripEnabled(true);

    configureWidget();
    runBuilders();
}


ImportsDialog::ImportsDialog(
        const QString&               newReferencePath,
        const QList<Ld::RootImport>& newImports,
        QWidget*                     parent
    ):ProgrammaticDialog(
        "ImportsDialog",
        parent
    ) {
    setWindowTitle(tr("Imports"));
    setSizeGripEnabled(true);

    configureWidget();
    runBuilders();

    setReferencePath(newReferencePath);
    setImports(newImports);
}


ImportsDialog::~ImportsDialog() {}


void ImportsDialog::setReferencePath(const QString& newReferencePath) {
    object<ImportsModel>("imports_model")->setReferencePath(newReferencePath);
}


QString ImportsDialog::referencePath() const {
    return object<ImportsModel>("imports_model")->referencePath();
}


void ImportsDialog::setImports(const QList<Ld::RootImport>& newImports) {
    currentImports = newImports;
    object<ImportsModel>("imports_model")->setImports(newImports);
}


QList<Ld::RootImport> ImportsDialog::imports() const {
    return object<ImportsModel>("imports_model")->imports();
}


void ImportsDialog::populate() {
    updateButtonState();
    ProgrammaticDialog::populate();
}


void ImportsDialog::bind() {
    QMenu* addImportMenu = widget<QMenu>("add_import_menu");
    connect(addImportMenu, &QMenu::aboutToShow, this, &ImportsDialog::aboutToShowAddImportsMenu);
    connect(addImportMenu, &QMenu::triggered, this, &ImportsDialog::addImportsMenuItemTriggered);

    QPushButton* removeImportsButton = widget<QPushButton>("remove_imports_push_button");
    connect(removeImportsButton, &QPushButton::clicked, this, &ImportsDialog::removeImportsClicked);

    QPushButton* moveUpButton = widget<QPushButton>("move_up_push_button");
    connect(moveUpButton, &QPushButton::clicked, this, &ImportsDialog::moveUpClicked);

    QPushButton* moveDownButton = widget<QPushButton>("move_down_push_button");
    connect(moveDownButton, &QPushButton::clicked, this, &ImportsDialog::moveDownClicked);

    QPushButton* resetButton = widget<QPushButton>("reset_push_button");
    connect(resetButton, &QPushButton::clicked, this, &ImportsDialog::resetClicked);

    QPushButton* okButton = widget<QPushButton>("ok_push_button");
    connect(okButton, &QPushButton::clicked, this, &ImportsDialog::accept);

    QPushButton* cancelButton = widget<QPushButton>("cancel_push_button");
    connect(cancelButton, &QPushButton::clicked, this, &ImportsDialog::reject);

    ImportsModel* model = object<ImportsModel>("imports_model");
    connect(model, &ImportsModel::failedToAccess, this, &ImportsDialog::failedToOpen);
    connect(model, &ImportsModel::dataChanged, this, &ImportsDialog::modelDataChanged);

    QItemSelectionModel* selectionModel = object<QItemSelectionModel>("imports_selection_model");
    connect(selectionModel, &QItemSelectionModel::selectionChanged, this, &ImportsDialog::selectionChanged);

    ProgrammaticDialog::bind();
}


void ImportsDialog::aboutToShowAddImportsMenu() {
    QMenu* menu = widget<QMenu>("add_import_menu");
    menu->clear();

    QMap<QString, QString>           identifiersByShortformName;
    Ld::RootElement::RootElementList rootElements = Ld::RootElement::rootElements();

    for (  Ld::RootElement::RootElementList::const_iterator it  = rootElements.begin(),
                                                            end = rootElements.end()
         ; it != end
         ; ++it
        ) {
        QSharedPointer<Ld::RootElement> element       = *it;
        QString                         identifier    = element->identifier();
        QString                         shortformName = element->shortformName(std::numeric_limits<unsigned>::max());

        identifiersByShortformName.insert(shortformName, identifier);
    }

    for (  QMap<QString, QString>::const_iterator pos    = identifiersByShortformName.constBegin(),
                                                  mapEnd = identifiersByShortformName.constEnd()
         ; pos != mapEnd
         ; ++pos
        ) {
        QString shortformName = pos.key();
        QString identifier    = pos.value();

        QAction* action = menu->addAction(shortformName);
        action->setData(QVariant::fromValue(identifier));
    }

    menu->addSeparator();

    menu->addAction(tr("Select new file..."));
}


void ImportsDialog::addImportsMenuItemTriggered(QAction* action) {
    QString identifier = action->data().toString();

    QSharedPointer<Ld::RootElement> rootElement;

    if (identifier.isEmpty()) {
        DocumentFileDialog* fileDialog = Application::documentFileDialog();
        fileDialog->configureForFileOpen();

        bool accepted = fileDialog->exec();
        if (accepted) {
            Q_ASSERT(fileDialog->selectedFiles().count() == 1);
            QString filename   = fileDialog->selectedFiles().at(0);
            QString identifier = Ld::RootElement::identifier(filename);

            rootElement = Ld::RootElement::byIdentifier(identifier);
            if (rootElement.isNull()) {
                rootElement = Ld::Element::createElement<Ld::RootElement>();
                Ld::RootElement::registerRootElement(rootElement);

                Ld::PlugInsByName plugInsByName = Application::plugInManager()->plugInsByName();
                bool success = rootElement->openExisting(filename, false, plugInsByName);

                if (!success) {
                    rootElement.clear();
                    failedToOpen(identifier);
                }
            }
        }
    } else {
        rootElement = Ld::RootElement::byIdentifier(identifier);
        if (rootElement.isNull()) {
            failedToOpen(identifier);
        }
    }

    if (!rootElement.isNull()) {
        ImportsModel* model = object<ImportsModel>("imports_model");
        Q_ASSERT(model != Q_NULLPTR);

        QList<Ld::RootImport> imports = model->imports();
        imports.append(Ld::RootImport(rootElement));

        model->setImports(imports);
        updateButtonState();
    }
}


void ImportsDialog::removeImportsClicked() {
    QItemSelectionModel* selectionModel = object<QItemSelectionModel>("imports_selection_model");

    if (selectionModel->hasSelection()) {
        QList<unsigned> rowList;
        QModelIndexList selections = selectionModel->selectedIndexes();
        for (  QModelIndexList::const_iterator it  = selections.constBegin(),
                                               end = selections.constEnd()
             ; it!=end
             ; ++it
            ) {
            rowList.push_back(it->row());
        }

        std::sort(rowList.begin(), rowList.end(), std::greater<unsigned>());

        ImportsModel* model = object<ImportsModel>("imports_model");
        Q_ASSERT(model != Q_NULLPTR);

        QList<Ld::RootImport> imports = model->imports();

        unsigned lastRow = static_cast<unsigned>(-1);
        for (QList<unsigned>::const_iterator it=rowList.constBegin(),end=rowList.constEnd() ; it!=end ; ++it) {
            unsigned currentRow = *it;
            if (currentRow != lastRow) {
                lastRow = currentRow;

                Q_ASSERT(currentRow < static_cast<unsigned>(imports.size()));
                imports.removeAt(currentRow);
            }
        }

        model->setImports(imports);
        updateButtonState();
    }
}


void ImportsDialog::moveUpClicked() {
    QItemSelectionModel* selectionModel = object<QItemSelectionModel>("imports_selection_model");

    if (selectionModel->hasSelection()) {
        QList<unsigned> rowList;
        QModelIndexList selections = selectionModel->selectedIndexes();
        for (  QModelIndexList::const_iterator it  = selections.constBegin(),
                                               end = selections.constEnd()
             ; it!=end
             ; ++it
            ) {
            rowList.push_back(it->row());
        }

        std::sort(rowList.begin(), rowList.end(), std::greater<unsigned>());
        unsigned insertLocation = static_cast<unsigned>(-1);

        QList<Ld::RootImport> movedImports;

        ImportsModel* model = object<ImportsModel>("imports_model");
        Q_ASSERT(model != Q_NULLPTR);

        QList<Ld::RootImport> imports = model->imports();

        unsigned lastRow = static_cast<unsigned>(-1);
        for (QList<unsigned>::const_iterator it=rowList.constBegin(),end=rowList.constEnd() ; it!=end ; ++it) {
            unsigned currentRow = *it;
            if (currentRow != lastRow) {
                lastRow = currentRow;

                Q_ASSERT(currentRow < static_cast<unsigned>(imports.size()));
                movedImports.prepend(imports.takeAt(currentRow));

                insertLocation = currentRow;
            }
        }

        if (insertLocation > 0) {
            --insertLocation;
        }

        for (  QList<Ld::RootImport>::const_iterator importIterator    = movedImports.constBegin(),
                                                     importEndIterator = movedImports.constEnd()
             ; importIterator != importEndIterator
             ; ++importIterator
            ) {
            imports.insert(insertLocation, *importIterator);
            ++insertLocation;
        }

        model->setImports(imports);
        updateButtonState();
    }
}


void ImportsDialog::moveDownClicked() {
    QItemSelectionModel* selectionModel = object<QItemSelectionModel>("imports_selection_model");

    if (selectionModel->hasSelection()) {
        QList<unsigned> rowList;
        QModelIndexList selections = selectionModel->selectedIndexes();
        for (  QModelIndexList::const_iterator it  = selections.constBegin(),
                                               end = selections.constEnd()
             ; it!=end
             ; ++it
            ) {
            rowList.push_back(it->row());
        }

        std::sort(rowList.begin(), rowList.end(), std::greater<unsigned>());
        unsigned insertLocation = rowList.last() + 1;

        QList<Ld::RootImport> movedImports;

        ImportsModel* model = object<ImportsModel>("imports_model");
        Q_ASSERT(model != Q_NULLPTR);

        QList<Ld::RootImport> imports = model->imports();

        unsigned lastRow = static_cast<unsigned>(-1);
        for (QList<unsigned>::const_iterator it=rowList.constBegin(),end=rowList.constEnd() ; it!=end ; ++it) {
            unsigned currentRow = *it;
            if (currentRow != lastRow) {
                lastRow = currentRow;

                Q_ASSERT(currentRow < static_cast<unsigned>(imports.size()));
                movedImports.prepend(imports.takeAt(currentRow));
            }
        }

        if (insertLocation >= static_cast<unsigned>(imports.size())) {
            imports += movedImports;
        } else {
            for (  QList<Ld::RootImport>::const_iterator importIterator    = movedImports.constBegin(),
                                                         importEndIterator = movedImports.constEnd()
                 ; importIterator != importEndIterator
                 ; ++importIterator
                ) {
                imports.insert(insertLocation, *importIterator);
                ++insertLocation;
            }
        }

        model->setImports(imports);
        updateButtonState();
    }
}


void ImportsDialog::resetClicked() {
    ImportsModel* model = object<ImportsModel>("imports_model");
    Q_ASSERT(model != Q_NULLPTR);

    model->setImports(currentImports);
    updateButtonState();
}


void ImportsDialog::accept() {
    ImportsModel* model = object<ImportsModel>("imports_model");
    Q_ASSERT(model != Q_NULLPTR);

    currentImports = model->imports();

    EQt::ProgrammaticDialog::accept();
}


void ImportsDialog::selectionChanged(const QItemSelection&, const QItemSelection&) {
    updateButtonState();
}


void ImportsDialog::failedToOpen(const QString& identifier) {
    QMessageBox::warning(this, tr("Could Not Open File"), tr("Could not open file:\n%1").arg(identifier));
}


void ImportsDialog::configureWidget() {
    QVBoxLayout* mainLayout = newVBoxLayout("main_layout");
    setLayout(mainLayout);

    QHBoxLayout* horizontalLayout = newHBoxLayout("horizontal_layout");
    mainLayout->addLayout(horizontalLayout);

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QGroupBox* tableGroupBox = new QGroupBox;
        registerWidget(tableGroupBox, "table_group_box");
        horizontalLayout->addWidget(tableGroupBox);

        QHBoxLayout* tableLayout = newHBoxLayout("table_horizontal_layout");
        tableGroupBox->setLayout(tableLayout);

        QTableView* importsTableView = new QTableView;
        registerWidget(importsTableView, "imports_table_view");
        tableLayout->addWidget(importsTableView);

        QVBoxLayout* moveUpDownLayout = newVBoxLayout("move_up_down_layout");
        tableLayout->addLayout(moveUpDownLayout);

        QPushButton* moveUpButton = new QPushButton(Application::icon("move_up"), QString(""));
        registerWidget(moveUpButton, "move_up_push_button");
        moveUpDownLayout->addWidget(moveUpButton);

        moveUpDownLayout->addStretch(1);

        QPushButton* moveDownButton = new QPushButton(Application::icon("move_down"), QString(""));
        registerWidget(moveDownButton, "move_down_push_button");
        moveUpDownLayout->addWidget(moveDownButton);

        horizontalLayout->addSpacing(10);

        QVBoxLayout* addRemoveButtonLayout = newVBoxLayout("add_remove_button_layout");
        horizontalLayout->addLayout(addRemoveButtonLayout);

        QPushButton* addImportButton = new QPushButton(Application::icon("add_entry"), tr("Add Imports"));
        registerWidget(addImportButton, "add_import_push_button");
        addRemoveButtonLayout->addWidget(addImportButton);

        QPushButton* removeImportsButton = new QPushButton(
            Application::icon("remove_entry"),
            tr("Remove Imports")
        );

        registerWidget(removeImportsButton, "remove_imports_push_button");
        addRemoveButtonLayout->addWidget(removeImportsButton);

        addRemoveButtonLayout->addStretch(1);

        QPushButton* resetButton = new QPushButton(tr("Reset"));
        registerWidget(resetButton, "reset_push_button");
        addRemoveButtonLayout->addWidget(resetButton);

    #elif (defined(Q_OS_DARWIN))

        horizontalLayout->setSpacing(0);

        #if (QT_VERSION < 0x060000)

            horizontalLayout->setMargin(0);

        #else

            horizontalLayout->setContentsMargins(0, 0, 0, 0);

        #endif

        QTableView* importsTableView = new QTableView;
        registerWidget(importsTableView, "imports_table_view");
        horizontalLayout->addWidget(importsTableView);

        QHBoxLayout* addRemoveButtonLayout = newHBoxLayout("add_remove_button_layout");
        mainLayout->addLayout(addRemoveButtonLayout);

        addRemoveButtonLayout->setSpacing(0);

        #if (QT_VERSION < 0x060000)

            addRemoveButtonLayout->setMargin(0);

        #else

            addRemoveButtonLayout->setContentsMargins(0, 0, 0, 0);

        #endif

        QPushButton* addImportButton = new QPushButton(Application::icon("add_entry"), QString(""));
        registerWidget(addImportButton, "add_import_push_button");
        addRemoveButtonLayout->addWidget(addImportButton);

        QPushButton* removeImportsButton = new QPushButton(Application::icon("remove_entry"), QString(""));
        registerWidget(removeImportsButton, "remove_imports_push_button");
        addRemoveButtonLayout->addWidget(removeImportsButton);

        addRemoveButtonLayout->addStretch(1);

        QPushButton* resetButton = new QPushButton(tr("Reset"));
        registerWidget(resetButton, "reset_push_button");
        addRemoveButtonLayout->addWidget(resetButton);

        QVBoxLayout* moveUpDownLayout = newVBoxLayout("move_up_down_layout");
        horizontalLayout->addLayout(moveUpDownLayout);

        QPushButton* moveUpButton = new QPushButton(Application::icon("move_up"), QString(""));
        registerWidget(moveUpButton, "move_up_push_button");
        moveUpDownLayout->addWidget(moveUpButton);

        moveUpDownLayout->addStretch(1);

        QPushButton* moveDownButton = new QPushButton(Application::icon("move_down"), QString(""));
        registerWidget(moveDownButton, "move_down_push_button");
        moveUpDownLayout->addWidget(moveDownButton);

    #else

        #error Unknown platform

    #endif

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    registerWidget(buttonBox, "dialog_button_box");
    registerWidget(buttonBox->button(QDialogButtonBox::Ok), "ok_push_button");
    registerWidget(buttonBox->button(QDialogButtonBox::Cancel), "cancel_push_button");
    mainLayout->addWidget(buttonBox);

    ImportsModel* model = new ImportsModel(this);
    registerObject(model, "imports_model");

    importsTableView->setModel(model);
    QHeaderView* horizontalHeader = importsTableView->horizontalHeader();

    QFont font = Application::font();
    QFontMetrics fontMetrics(font);

    unsigned headerWidth   = fontMetrics.horizontalAdvance(tr("Position"));
    unsigned relativeWidth = fontMetrics.horizontalAdvance(tr("Relative"));
    unsigned absoluteWidth = fontMetrics.horizontalAdvance(tr("Absolute"));
    unsigned contentsWidth = std::max(relativeWidth, absoluteWidth);

    horizontalHeader->setSectionResizeMode(0, QHeaderView::Fixed);
    horizontalHeader->resizeSection(0, std::max(headerWidth, contentsWidth) + firstSectionPadding);
    horizontalHeader->setStretchLastSection(true);

    ImportsDelegate* delegate = new ImportsDelegate(importsTableView);
    importsTableView->setItemDelegate(delegate);

    importsTableView->setSelectionBehavior(QTableView::SelectRows);
    importsTableView->setSelectionMode(QTableView::SingleSelection);

    registerObject(importsTableView->selectionModel(), "imports_selection_model");

    importsTableView->verticalHeader()->setVisible(false);

    moveUpButton->setToolTip(tr("Move the selected imports up"));
    moveDownButton->setToolTip(tr("Move the selected imports down"));
    addImportButton->setToolTip(tr("Add a new import"));
    removeImportsButton->setToolTip(tr("Remove an import"));
    resetButton->setToolTip(tr("Resets the list of imports"));

    moveUpButton->setWhatsThis(
        tr("Use this buttom to move the selected import(s) earlier in the list of imports.")
    );

    moveDownButton->setWhatsThis(
        tr("Use this button to move the selected import(s) later in the list of imports.")
    );

    addImportButton->setWhatsThis(
        tr(
            "Use this button to add a new import to the list of imports.  The imports will be appended to the bottom "
            "of the list."
           )
    );

    removeImportsButton->setWhatsThis(
        tr("Use this button to remove the selected import(s) from the list of imports.")
    );

    resetButton->setWhatsThis(
        tr(
            "Use this button to restore the list of imports back to the state it was in when this dialog was "
            "invoked."
        )
    );

    QMenu* addImportMenu = new QMenu(addImportButton);
    registerWidget(addImportMenu, "add_import_menu");
    addImportButton->setMenu(addImportMenu);

    moveUpButton->setAutoDefault(false);
    moveDownButton->setAutoDefault(false);
    addImportButton->setAutoDefault(false);
    removeImportsButton->setAutoDefault(false);
    resetButton->setAutoDefault(false);

    moveUpButton->setDefault(false);
    moveDownButton->setDefault(false);
    addImportButton->setDefault(false);
    removeImportsButton->setDefault(false);
    resetButton->setDefault(false);
}


void ImportsDialog::modelDataChanged(
        const QModelIndex&  /* topleft */,
        const QModelIndex&  /* bottomRight */,
        const QVector<int>& /* roles */
    ) {
    updateButtonState();
}


void ImportsDialog::updateButtonState() {
    QPushButton* removeImportsButton = widget<QPushButton>("remove_imports_push_button");
    QPushButton* moveUpButton        = widget<QPushButton>("move_up_push_button");
    QPushButton* moveDownButton      = widget<QPushButton>("move_down_push_button");
    QPushButton* resetButton         = widget<QPushButton>("reset_push_button");
    QPushButton* okButton            = widget<QPushButton>("ok_push_button");

    ImportsModel* model = object<ImportsModel>("imports_model");
    Q_ASSERT(model != Q_NULLPTR);

    QList<Ld::RootImport> imports = model->imports();
    unsigned numberImports = imports.count();

    if (imports != currentImports) {
        resetButton->setEnabled(true);

        bool containsBadImports = false;
        for (QList<Ld::RootImport>::const_iterator it=imports.constBegin(),end=imports.constEnd() ; it!=end ; ++it) {
            if (it->isInvalid()) {
                containsBadImports = true;
            }
        }

        okButton->setEnabled(!containsBadImports);
    } else {
        resetButton->setEnabled(false);
        okButton->setEnabled(false);
    }

    if (numberImports > 0) {
        QItemSelectionModel* selectionModel = object<QItemSelectionModel>("imports_selection_model");

        if (selectionModel->hasSelection()) {
            removeImportsButton->setEnabled(true);

            bool firstRowSelected = false;
            bool lastRowSelected  = false;

            QModelIndexList selections = selectionModel->selectedIndexes();
            for (  QModelIndexList::const_iterator it  = selections.constBegin(),
                                                   end = selections.constEnd()
                 ; it!=end
                 ; ++it
                ) {
                if (it->row() == 0) {
                    firstRowSelected = true;
                }

                if (static_cast<unsigned>(it->row()) == numberImports - 1) {
                    lastRowSelected = true;
                }
            }

            moveUpButton->setDisabled(firstRowSelected);
            moveDownButton->setDisabled(lastRowSelected);
        } else {
            removeImportsButton->setEnabled(false);
            moveUpButton->setEnabled(false);
            moveDownButton->setEnabled(false);
        }
    } else {
        removeImportsButton->setEnabled(false);
        moveUpButton->setEnabled(false);
        moveDownButton->setEnabled(false);
    }
}
