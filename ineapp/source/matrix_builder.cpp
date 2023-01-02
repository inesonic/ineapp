/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref MatrixBuilder class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QAction>
#include <QMenu>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFontMetrics>

#include <eqt_builder.h>
#include <eqt_application.h>
#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_programmatic_view_proxy.h>
#include <eqt_tool_button.h>
#include <eqt_tool_button_array.h>

#include <ld_capabilities.h>
#include <ld_character_format.h>

#include <ld_matrix_operator_element.h>
#include <ld_matrix_combine_left_to_right_operator_element.h>
#include <ld_matrix_combine_top_to_bottom_operator_element.h>
#include <ld_subscript_index_operator_element.h>
#include <ld_subscript_row_column_operator_element.h>
#include <ld_hadamard_product_operator_element.h>
#include <ld_kronecker_product_operator_element.h>
#include <ld_matrix_transpose_operator_element.h>
#include <ld_matrix_conjugate_transpose_operator_element.h>

#include "application.h"
#include "main_window.h"
#include "matrix_main_window_proxy.h"
#include "matrix_view_proxy.h"
#include "matrix_builder.h"

const char MatrixBuilder::builderName[] = "MatrixBuilder";
const char MatrixBuilder::matrixDockName[] = "matrix";

MatrixBuilder::MatrixBuilder(QObject* parent):EQt::Builder(parent) {}


MatrixBuilder::~MatrixBuilder() {}


unsigned MatrixBuilder::priority() const {
    return builderPriority;
}


const char* MatrixBuilder::builderIdentifier() const {
    return builderName;
}


const char* MatrixBuilder::plugInName() const {
    return "";
}

#if (defined(Q_OS_DARWIN))

    #define MENU_TEXT(_text,_suffix) ((_text) + (_suffix))

#else

    #define MENU_TEXT(_text,_suffix) (_text)

#endif

void MatrixBuilder::buildMainWindowGui(EQt::ProgrammaticMainWindow* window) {
    QAction* matrixOperatorAction = window->addMenuAction(
        EQt::Application::icon("define_matrix_operator"),
        MENU_TEXT(tr("&Insert | Matri&x | &Matrix"), tr(" [ x ]")),
        Ld::MatrixOperatorElement::childProvides,
        "matrix_operator_action"
    );
    matrixOperatorAction->setToolTip(tr("Insert a new matrix"));
    matrixOperatorAction->setWhatsThis(tr("You can use the matrix operator to insert a new matrix."));

    QAction* matrixCombineLeftToRightOperatorAction = window->addMenuAction(
        EQt::Application::icon("combine_matrix_left_to_right_operator"),
        MENU_TEXT(tr("&Insert | Matri&x | Combine &Left-To-Right"), tr(" [ x %1 x ]").arg(QChar(0x2502))),
        Ld::MatrixCombineLeftToRightOperatorElement::childProvides,
        "matrix_combine_left_to_right_operator_action"
    );
    matrixCombineLeftToRightOperatorAction->setToolTip(
        tr("Combines two matrices into a single matrix.  Matrices are stacked left-to-right.")
    );
    matrixCombineLeftToRightOperatorAction->setWhatsThis(
        tr(
           "You can use the combine left-to-right operator to combine two matrices.  Matrices are combined "
           "left-to-right.  If the matrices have differing number of rows, then matrices are aligned to the top with "
           "the remaining rows filled with zeros."
        )
    );

    QAction* matrixCombineTopToBottomOperatorAction = window->addMenuAction(
        EQt::Application::icon("combine_matrix_top_to_bottom_operator"),
        tr("&Insert | Matri&x | Combine &Top-To_Bottom"),
        Ld::MatrixCombineTopToBottomOperatorElement::childProvides,
        "matrix_combine_top_to_bottom_operator_action"
    );
    matrixCombineTopToBottomOperatorAction->setToolTip(
        tr("Combines two matrices into a single matrix.  Matrices are stacked top-to-bottom.")
    );
    matrixCombineTopToBottomOperatorAction->setWhatsThis(
        tr(
           "You can use the combine top-to-bottom operator to combine two matrices.  Matrices are combined "
           "top-to_bottom.  If the matrices have differing number of columns, then matrices are aligned to the left "
           "with the remaining columns filled with zeros."
        )
    );

    QAction* hadamardProductOperatorAction = window->addMenuAction(
        EQt::Application::icon("hadamard_product_operator"),
        tr("&Insert | Matri&x | &Hadamard Product"),
        Ld::HadamardProductOperatorElement::childProvides,
        "hadamard_product_operator_action"
    );
    hadamardProductOperatorAction->setToolTip(tr("Calculates the Hadamard product of two matricies."));
    hadamardProductOperatorAction->setWhatsThis(
        tr(
           "You can use the Hadamard product operator to calculate the entry-wise product of two identically sized "
           "matrices."
        )
    );

    QAction* kroneckerProductOperatorAction = window->addMenuAction(
        EQt::Application::icon("kronecker_product_operator"),
        tr("&Insert | Matri&x | &Kronecker Product"),
        Ld::KroneckerProductOperatorElement::childProvides,
        "kronecker_product_operator_action"
    );
    kroneckerProductOperatorAction->setToolTip(tr("Calculates the Kronecker product of two matricies."));
    kroneckerProductOperatorAction->setWhatsThis(
        tr(
           "You can use the Kronecker product operator to calculate the Kronecker product of two matrices."
        )
    );

    QAction* matrixTransposeOperatorAction = window->addMenuAction(
        EQt::Application::icon("matrix_transpose_operator"),
        tr("&Insert | Matri&x | T&ranspose"),
        Ld::MatrixTransposeOperatorElement::childProvides,
        "matrix_transpose_operator_action"
    );
    matrixTransposeOperatorAction->setToolTip(tr("Performs a matrix transpose."));
    matrixTransposeOperatorAction->setWhatsThis(
        tr("You can use the matrix transpose operator to transpose a matrix.")
    );

    QAction* matrixConjugateTransposeOperatorAction = window->addMenuAction(
        EQt::Application::icon("matrix_hermitian_operator"),
        tr("&Insert | Matri&x | &Conjugate Transpose"),
        Ld::MatrixConjugateTransposeOperatorElement::childProvides,
        "matrix_conjugate_transpose_operator_action"
    );
    matrixConjugateTransposeOperatorAction->setToolTip(tr("Performs a conjugate transpose of a matrix."));
    matrixConjugateTransposeOperatorAction->setWhatsThis(
        tr("You can use the conjugate transpose operator to calculate the conjugate transpose or adjoint of a matrix.")
    );
}


void MatrixBuilder::buildMainWindowDocks(EQt::ProgrammaticMainWindow* window) {
    EQt::ProgrammaticDockWidget* dock = window->addDockWidget(
        matrixDockName,
        tr("Matrix"),
        EQt::DockWidgetDefaults(EQt::DockWidgetDefaults::Relationship::AFTER, "sets_and_logic", true)
    );
    dock->setCloseButtonEnabled();

    QAction* dockToggleAction = window->addDockToggleMenuItem(
        matrixDockName,
        tr("&View | &Docks"),
        "matrix_toggle_action"
    );
    dockToggleAction->setToolTip(tr("Show/hide the Matrixc dock"));
    dockToggleAction->setWhatsThis(
        tr(
            "You can use this menu item to show or hide the Matrix dock.  You can use the Matrix dock to add math "
            "elements associated with matrices."
        )
    );

    QHBoxLayout* horizontalLayout = dock->newHBoxLayout("horizontal");

    dock->setHorizontalLayout(horizontalLayout);
    buildDockHorizontalOrientation(dock, horizontalLayout, window);

    QVBoxLayout* verticalLayout = dock->newVBoxLayout("vertical");
    dock->setVerticalLayout(verticalLayout);

    buildDockVerticalOrientation(dock, verticalLayout, window);
}


EQt::ProgrammaticMainWindowProxy* MatrixBuilder::createMainWindowProxy(EQt::ProgrammaticMainWindow* window) {
    return new MatrixMainWindowProxy(window);
}


void MatrixBuilder::buildMainWindowFinal(EQt::ProgrammaticMainWindow *window) {
    MainWindow* mainWindow = dynamic_cast<MainWindow*>(window);

    QAction* matrixOperatorAction = mainWindow->action("matrix_operator_action");
    bool success = mainWindow->setCommand(tr("matrix"), matrixOperatorAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("begin{array}"), matrixOperatorAction);
    Q_ASSERT(success);

    QAction* matrixCombineLeftToRightOperatorAction = window->action("matrix_combine_left_to_right_operator_action");
    success = mainWindow->setCommand(tr("mcomblr"), matrixCombineLeftToRightOperatorAction);
    Q_ASSERT(success);

    QAction* matrixCombineTopToBottomOperatorAction = window->action("matrix_combine_top_to_bottom_operator_action");
    success = mainWindow->setCommand(tr("mcombtb"), matrixCombineTopToBottomOperatorAction);
    Q_ASSERT(success);

    QAction* hadamardProductOperatorAction = window->action("hadamard_product_operator_action");
    success = mainWindow->setCommand(tr("hadamard"), hadamardProductOperatorAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("circ"), hadamardProductOperatorAction);
    Q_ASSERT(success);

    QAction* kroneckerProductOperatorAction = window->action("kronecker_product_operator_action");
    success = mainWindow->setCommand(tr("kronecker"), kroneckerProductOperatorAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("otimes"), kroneckerProductOperatorAction);
    Q_ASSERT(success);

    QAction* matrixTransposeOperatorAction = window->action("matrix_transpose_operator_action");
    success = mainWindow->setCommand(tr("transpose"), matrixTransposeOperatorAction);
    Q_ASSERT(success);

    QAction* matrixConjugateTransposeOperatorAction = window->action("matrix_conjugate_transpose_operator_action");
    success = mainWindow->setCommand(tr("adjoint"), matrixConjugateTransposeOperatorAction);
    Q_ASSERT(success);
    success = mainWindow->setCommand(tr("conjtran"), matrixConjugateTransposeOperatorAction);
    Q_ASSERT(success);
}


EQt::ProgrammaticViewProxy* MatrixBuilder::createViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ) {
    return new MatrixViewProxy(view, window);
}


void MatrixBuilder::buildDockHorizontalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QHBoxLayout*                 horizontalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    QGridLayout* buttonsLayout = dock->newGridLayout("horizontal_buttons");
    horizontalLayout->addLayout(buttonsLayout);

    buildDockCommonComponents(buttonsLayout, window);

    horizontalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedHorizontalGap);

    //dock->setHeightAdjustmentLayout(buttonsLayout, 4);
    horizontalLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void MatrixBuilder::buildDockVerticalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QVBoxLayout*                 verticalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    QGridLayout* buttonsLayout = dock->newGridLayout("vertical_buttons");
    verticalLayout->addLayout(buttonsLayout);

    buildDockCommonComponents(buttonsLayout, window);

    verticalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedVerticalGap);

    //dock->setWidthAdjustmentLayout(buttonsLayout, 5);
    verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void MatrixBuilder::buildDockCommonComponents(
        QGridLayout*                 gridLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    QAction* matrixOperatorAction = window->action("matrix_operator_action");
    EQt::ToolButton* matrixOperatorToolButton = new EQt::ToolButton(matrixOperatorAction);
    matrixOperatorToolButton->setRelativeSizeFactor(0);
    matrixOperatorToolButton->setXScaling(1.0F);
    matrixOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    gridLayout->addWidget(matrixOperatorToolButton, 0, 0, 2, 2);

    QAction* matrixCombineLeftToRightOperatorAction = window->action("matrix_combine_left_to_right_operator_action");
    EQt::ToolButton* matrixCombineLeftToRightToolButton = new EQt::ToolButton(matrixCombineLeftToRightOperatorAction);
    matrixCombineLeftToRightToolButton->setRelativeSizeFactor(0);
    matrixCombineLeftToRightToolButton->setXScaling(1.0F);
    matrixCombineLeftToRightToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    gridLayout->addWidget(matrixCombineLeftToRightToolButton, 0, 2, 2, 2);

    QAction* matrixCombineTopToBottomOperatorAction = window->action("matrix_combine_top_to_bottom_operator_action");
    EQt::ToolButton* matrixCombineTopToBottomToolButton = new EQt::ToolButton(matrixCombineTopToBottomOperatorAction);
    matrixCombineTopToBottomToolButton->setRelativeSizeFactor(0);
    matrixCombineTopToBottomToolButton->setXScaling(1.0F);
    matrixCombineTopToBottomToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    gridLayout->addWidget(matrixCombineTopToBottomToolButton, 0, 4, 2, 2);

    QAction* subscriptIndexAction = window->action("subscript_index_action");
    EQt::ToolButton* subscriptIndexToolButton = new EQt::ToolButton(subscriptIndexAction);
    subscriptIndexToolButton->setRelativeSizeFactor(-2);
    subscriptIndexToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    gridLayout->addWidget(subscriptIndexToolButton, 0, 6, 1, 1);

    QAction* subscriptRowColumnAction = window->action("subscript_row_column_action");
    EQt::ToolButton* subscriptRowColumnToolButton = new EQt::ToolButton(subscriptRowColumnAction);
    subscriptRowColumnToolButton->setRelativeSizeFactor(-2);
    subscriptRowColumnToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    gridLayout->addWidget(subscriptRowColumnToolButton, 0, 7, 1, 1);

    QAction* absoluteValueOperatorAction = window->action("absolute_value_operator_action");
    EQt::ToolButton* absoluteValueToolButton = new EQt::ToolButton(absoluteValueOperatorAction);
    absoluteValueToolButton->setRelativeSizeFactor(-2);
    absoluteValueToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    gridLayout->addWidget(absoluteValueToolButton, 1, 6, 1, 1);

    QAction* complexConjugateOperatorAction = window->action("complex_conjugate_operator_action");
    EQt::ToolButton* complexConjugateToolButton = new EQt::ToolButton(complexConjugateOperatorAction);
    complexConjugateToolButton->setRelativeSizeFactor(-2);
    complexConjugateToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    gridLayout->addWidget(complexConjugateToolButton, 1, 7, 1, 1);

    QAction* multiplicationOperatorAction = window->action("multiplication_operator_action");
    EQt::ToolButton* multiplicationOperatorToolButton = new EQt::ToolButton(multiplicationOperatorAction);
    multiplicationOperatorToolButton->setRelativeSizeFactor(-2);
    multiplicationOperatorToolButton->setXScaling(2.0F);
    multiplicationOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    gridLayout->addWidget(multiplicationOperatorToolButton, 2, 0, 1, 2);

    QAction* hadamardProductOperatorAction = window->action("hadamard_product_operator_action");
    EQt::ToolButton* hadamardProductOperatorToolButton = new EQt::ToolButton(hadamardProductOperatorAction);
    hadamardProductOperatorToolButton->setRelativeSizeFactor(-2);
    hadamardProductOperatorToolButton->setXScaling(2.0F);
    hadamardProductOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    gridLayout->addWidget(hadamardProductOperatorToolButton, 2, 2, 1, 2);

    QAction* kroneckerProductOperatorAction = window->action("kronecker_product_operator_action");
    EQt::ToolButton* kroneckerProductOperatorToolButton = new EQt::ToolButton(kroneckerProductOperatorAction);
    kroneckerProductOperatorToolButton->setRelativeSizeFactor(-2);
    kroneckerProductOperatorToolButton->setXScaling(2.0F);
    kroneckerProductOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    gridLayout->addWidget(kroneckerProductOperatorToolButton, 2, 4, 1, 2);

    QAction* matrixTransposeOperatorAction = window->action("matrix_transpose_operator_action");
    EQt::ToolButton* matrixTransposeOperatorToolButton = new EQt::ToolButton(matrixTransposeOperatorAction);
    matrixTransposeOperatorToolButton->setRelativeSizeFactor(-2);
    matrixTransposeOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    gridLayout->addWidget(matrixTransposeOperatorToolButton, 2, 6, 1, 1);

    QAction* matrixConjugateTransposeOperatorAction = window->action("matrix_conjugate_transpose_operator_action");
    EQt::ToolButton*
        matrixConjugateTransposeOperatorToolButton = new EQt::ToolButton(matrixConjugateTransposeOperatorAction);
    matrixConjugateTransposeOperatorToolButton->setRelativeSizeFactor(-2);
    matrixConjugateTransposeOperatorToolButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    gridLayout->addWidget(matrixConjugateTransposeOperatorToolButton, 2, 7, 1, 1);
}
