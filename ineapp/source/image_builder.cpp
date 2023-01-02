/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ImageBuilder class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QAction>
#include <QMenu>
#include <QImageReader>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QFontMetrics>

#include <eqt_builder.h>
#include <eqt_application.h>
#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_programmatic_main_window_proxy.h>
#include <eqt_programmatic_view_proxy.h>
#include <eqt_spin_box.h>
#include <eqt_dimension_line_edit.h>
#include <eqt_tool_button.h>
#include <eqt_stacking_layout.h>

#include <ld_capabilities.h>

#include "application.h"
#include "main_window.h"
#include "image_main_window_proxy.h"
#include "image_view_proxy.h"
#include "image_builder.h"

const char ImageBuilder::builderName[] = "ImageBuilder";
const char ImageBuilder::imageFormatDockName[] = "image_format";

ImageBuilder::ImageBuilder(QObject* parent):EQt::Builder(parent) {}


ImageBuilder::~ImageBuilder() {}


unsigned ImageBuilder::priority() const {
    return builderPriority;
}


const char* ImageBuilder::builderIdentifier() const {
    return builderName;
}


const char* ImageBuilder::plugInName() const {
    return "";
}


void ImageBuilder::buildMainWindowGui(EQt::ProgrammaticMainWindow* window) {
    QString supportedFormats = supportedImageFormatString();

    window->addMenuAction(
        EQt::Application::icon("insert_image"),
        tr("&Insert | &Image..."),
        Ld::Capabilities::nonTextAnnotations,
        "insert_image_dialog"
    );
    window->setToolTip("insert_image_dialog", tr("Insert an image from  a file"));
    window->setWhatsThis(
        "insert_image_dialog",
        tr("Use this option to insert an image in a file.  Supported image formats include: %1").arg(supportedFormats)
    );

    window->addSeparator(tr("F&ormat"));

    window->addMenuAction(EQt::Application::icon("format_image"), tr("F&ormat | &Image..."), "format_image_dialog");
    window->setToolTip("format_image_dialog", tr("Adjusts image size and orientation"));
    window->setWhatsThis(
        "format_image_dialog",
        tr("Use this option to update or change the size and/or orientation of an image.")
    );

    buildScalingModeMenusAndActions(window);
}


void ImageBuilder::buildAdditionalMainWindowActions(EQt::ProgrammaticMainWindow* window) {
    QAction* imageRotateAction = new QAction(
        EQt::Application::icon("format_image_rotate"),
        tr("Rotate image"),
        window
    );

    window->addAction(imageRotateAction, "format_image_rotate");
    imageRotateAction->setToolTip(tr("Click here to rotate an image."));
    imageRotateAction->setWhatsThis(tr("You can use this button to rotate an image."));

    QAction* imageHorizontalPercentageWidthAction = new QAction(window);
    window->addAction(imageHorizontalPercentageWidthAction, "format_image_percentage_width");
    imageHorizontalPercentageWidthAction->setToolTip(tr("Changes the horizontal width by a fixed percentage"));
    imageHorizontalPercentageWidthAction->setWhatsThis(
        tr(
            "You can use this spin box to adjust the horizontal width of the image by a specified percentage value."
        )
    );

    QAction* imageHorizontalDimensionWidthAction = new QAction(window);
    window->addAction(imageHorizontalDimensionWidthAction, "format_image_dimension_width");
    imageHorizontalPercentageWidthAction->setToolTip(tr("Changes the horizontal width to a fixed size."));
    imageHorizontalPercentageWidthAction->setWhatsThis(
        tr(
            "You can use this spin box to adjust the horizontal width of the image.  Values can be entered in "
            "inches (\"), centimeters (cm), millimeters (mm), points (pt), or picas (pc)."
        )
    );

    QAction* imageVerticalPercentageHeightAction = new QAction(window);
    window->addAction(imageVerticalPercentageHeightAction, "format_image_percentage_height");
    imageVerticalPercentageHeightAction->setToolTip(tr("Changes the vertical height by a fixed percentage"));
    imageVerticalPercentageHeightAction->setWhatsThis(
        tr(
            "You can use this spin box to adjust the vertical height of the image by a specified percentage value."
        )
    );

    QAction* imageVerticalDimensionHeightAction = new QAction(window);
    window->addAction(imageVerticalDimensionHeightAction, "format_image_dimension_height");
    imageVerticalPercentageHeightAction->setToolTip(tr("Changes the vertical height to a fixed size."));
    imageVerticalPercentageHeightAction->setWhatsThis(
        tr(
            "You can use this spin box to adjust the vertical height of the image.  Values can be entered in "
            "inches (\"), centimeters (cm), millimeters (mm), points (pt), or picas (pc)."
        )
    );

    QAction* horizontalModeAction = new QAction(window);
    horizontalModeAction->setToolTip(tr("Click here to change the image horizontal scaling mode."));
    horizontalModeAction->setToolTip(
        tr(
            "You can use this button to toggle between no scaling of images, scaling to a fixed size, and scaling"
            "based on a percentage of the original size.  This button affects the horizontal axis only."
        )
    );
    window->addAction(horizontalModeAction, "format_image_width_mode");

    QAction* verticalModeAction = new QAction(window);
    verticalModeAction->setToolTip(tr("Click here to change the image vertical scaling mode."));
    verticalModeAction->setToolTip(
        tr(
           "You can use this button to toggle between no scaling of images, scaling to a fixed size, and scaling"
           "based on a percentage of the original size.  You can also use this button to indicate that images should "
           "maintain their aspect ratio."
        )
    );
    window->addAction(verticalModeAction, "format_image_height_mode");
}


void ImageBuilder::buildMainWindowDocks(EQt::ProgrammaticMainWindow* window) {
    EQt::ProgrammaticDockWidget* dock = window->addDockWidget(
        imageFormatDockName,
        tr("Image"),
        EQt::DockWidgetDefaults(
            EQt::DockWidgetDefaults::Relationship::AFTER_IN_TABBED_REGION,
            "value_field_format",
            false
        )
    );
    dock->setCloseButtonEnabled();

    QAction* dockToggleAction = window->addDockToggleMenuItem(
        imageFormatDockName,
        tr("&View | &Docks"),
        "format_image_dock_toggle_action"
    );
    dockToggleAction->setToolTip(tr("Show/hide the Image Format dock"));
    dockToggleAction->setWhatsThis(
        tr(
            "You can use this menu item to show or hide the Image Format dock.  You can use the Image Format dock "
            "to set the size and orientation of one or more images."
        )
    );

    QHBoxLayout* horizontalLayout = dock->newHBoxLayout("horizontal");

    dock->setHorizontalLayout(horizontalLayout);
    buildDockHorizontalOrientation(dock, horizontalLayout, window);

    QVBoxLayout* verticalLayout = dock->newVBoxLayout("vertical");
    dock->setVerticalLayout(verticalLayout);

    buildDockVerticalOrientation(dock, verticalLayout, window);

    updateInsertDock(window);
}


EQt::ProgrammaticMainWindowProxy* ImageBuilder::createMainWindowProxy(EQt::ProgrammaticMainWindow* window) {
    return new ImageMainWindowProxy(window);
}


void ImageBuilder::buildMainWindowFinal(EQt::ProgrammaticMainWindow *window) {
    MainWindow* mainWindow = dynamic_cast<MainWindow*>(window);

    QAction* imageFormatDockToggleAction = mainWindow->action("format_image_dock_toggle_action");
    bool success = mainWindow->setCommand(tr("imgdock"), imageFormatDockToggleAction);

    QAction* formatImageDialogAction = mainWindow->action("format_image_dialog");
    success = mainWindow->setCommand(tr("imgfmt"), formatImageDialogAction);
    Q_ASSERT(success);

    QAction* insertImageDialogAction = mainWindow->action("insert_image_dialog");
    success = mainWindow->setCommand(tr("includegraphics"), insertImageDialogAction);
    Q_ASSERT(success);
}


EQt::ProgrammaticViewProxy* ImageBuilder::createViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ) {
    return new ImageViewProxy(view, window);
}


void ImageBuilder::buildScalingModeMenusAndActions(EQt::ProgrammaticMainWindow* window) {
    QMenu* imageHorizontalScaleModePopupMenu = new QMenu(window);
    window->registerWidget(imageHorizontalScaleModePopupMenu, "format_image_width_scale_mode_popup_menu");

    QAction* ignoreHorizontalScalingSettingAction = window->addAction(
        EQt::Application::icon("format_image_no_horizontal_scaling"),
        tr("&No Horizontal Scaling"),
        "format_image_width_no_scaling"
    );

    QAction* honorAspectRatioHorizontalScalingSettingAction = window->addAction(
        EQt::Application::icon("format_image_aspect_ratio"),
        tr("Honor &Aspect Ratio"),
        "format_image_width_honor_aspect_ratio"
    );

    QAction* percentageHorizontalScalingSettingAction = window->addAction(
        EQt::Application::icon("format_image_scale_percentage"),
        tr("&Percentage Scaling"),
        "format_image_width_percentage_scaling"
    );

    QAction* dimensionHorizontalScalingSettingAction = window->addAction(
        EQt::Application::icon("format_image_scale_horizontal_dimension"),
        tr("Scale To &Fixed Width"),
        "format_image_width_dimension_scaling"
    );

    imageHorizontalScaleModePopupMenu->addAction(ignoreHorizontalScalingSettingAction);
    imageHorizontalScaleModePopupMenu->addAction(honorAspectRatioHorizontalScalingSettingAction);
    imageHorizontalScaleModePopupMenu->addAction(percentageHorizontalScalingSettingAction);
    imageHorizontalScaleModePopupMenu->addAction(dimensionHorizontalScalingSettingAction);

    QMenu* imageVerticalScaleModePopupMenu = new QMenu(window);
    window->registerWidget(imageVerticalScaleModePopupMenu, "format_image_height_scale_mode_popup_menu");

    QAction* ignoreVerticalScalingSettingAction = window->addAction(
        EQt::Application::icon("format_image_no_vertical_scaling"),
        tr("&No Vertical Scaling"),
        "format_image_height_no_scaling"
    );

    QAction* honorAspectRatioVerticalScalingSettingAction = window->addAction(
        EQt::Application::icon("format_image_aspect_ratio"),
        tr("Honor &Aspect Ratio"),
        "format_image_height_honor_aspect_ratio"
    );

    QAction* percentageVerticalScalingSettingAction = window->addAction(
        EQt::Application::icon("format_image_scale_percentage"),
        tr("&Percentage Scaling"),
        "format_image_height_percentage_scaling"
    );

    QAction* dimensionVerticalScalingSettingAction = window->addAction(
        EQt::Application::icon("format_image_scale_vertical_dimension"),
        tr("Scale To &Fixed Height"),
        "format_image_height_dimension_scaling"
    );

    imageVerticalScaleModePopupMenu->addAction(ignoreVerticalScalingSettingAction);
    imageVerticalScaleModePopupMenu->addAction(honorAspectRatioVerticalScalingSettingAction);
    imageVerticalScaleModePopupMenu->addAction(percentageVerticalScalingSettingAction);
    imageVerticalScaleModePopupMenu->addAction(dimensionVerticalScalingSettingAction);
}


void ImageBuilder::buildDockHorizontalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QHBoxLayout*                 horizontalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    QVBoxLayout* buttonsLayout = dock->newVBoxLayout("horizontal_buttons");
    horizontalLayout->addLayout(buttonsLayout, Qt::AlignVCenter);

    QAction* imageRotateAction = window->action("format_image_rotate");
    EQt::ToolButton* imageRotateToolButton = new EQt::ToolButton(imageRotateAction);
    buttonsLayout->addWidget(imageRotateToolButton);

    QAction* imageFormatDialogAction = window->action("format_image_dialog");
    EQt::ToolButton* formatImageToolButton = new EQt::ToolButton(imageFormatDialogAction);
    buttonsLayout->addWidget(formatImageToolButton);

    QVBoxLayout* verticalAlignmentLayout = dock->newVBoxLayout("horizontal_scaling_alignment");
    horizontalLayout->addLayout(verticalAlignmentLayout);

    verticalAlignmentLayout->addStretch(1);
    QFormLayout* scalingLayout = dock->newFormLayout("horizontal_scaling");
    verticalAlignmentLayout->addLayout(scalingLayout);
    verticalAlignmentLayout->addStretch(1);

    QFontMetrics fontMetrics(Application::font());
    unsigned spinBoxWidth = static_cast<unsigned>(fontMetrics.horizontalAdvance(tr("XXXXX%   ")));


    QAction* imagePercentageWidthAction = window->action("format_image_percentage_width");
    EQt::SpinBox* imageWidthSpinBox = new EQt::SpinBox(imagePercentageWidthAction);
    imageWidthSpinBox->setFixedWidth(spinBoxWidth);
    imageWidthSpinBox->setSuffix(tr("%"));
    imageWidthSpinBox->setMinimum(minimumScaleFactor);
    imageWidthSpinBox->setMaximum(maximumScaleFactor);
    dock->registerWidget(imageWidthSpinBox, "horizontal_width_spin_box");

    QAction* imageDimensionWidthAction = window->action("format_image_dimension_width");
    EQt::DimensionLineEdit* imageWidthLineEdit = new EQt::DimensionLineEdit(imageDimensionWidthAction);
    imageWidthLineEdit->setDynamicUnits();
    dock->registerWidget(imageWidthLineEdit, "horizontal_width_line_edit");

    QAction* imageWidthModeAction         = window->action("format_image_width_mode");
    QMenu*   imageWidthScaleModePopupMenu = window->EQt::ProgrammaticWidget::widget<QMenu>(
        "format_image_width_scale_mode_popup_menu"
    );
    EQt::ToolButton* imageWidthScalingModeToolButton = new EQt::ToolButton(
        imageWidthModeAction,
        imageWidthScaleModePopupMenu
    );
    imageWidthScalingModeToolButton->setGlobalScaleFactorSettingsDisabled();
//    imageWidthScalingModeToolButton->setFixedSize(scalingModeButtonSize, scalingModeButtonSize);
    dock->registerWidget(imageWidthScalingModeToolButton, "horizontal_width_scaling_mode_tool_button");

    QHBoxLayout* widthScalingModeLayout = dock->newHBoxLayout("horizontal_width_scaling_controls");
    widthScalingModeLayout->addWidget(imageWidthSpinBox, Qt::AlignVCenter);
    widthScalingModeLayout->addWidget(imageWidthLineEdit, Qt::AlignVCenter);
    widthScalingModeLayout->addWidget(imageWidthScalingModeToolButton, Qt::AlignVCenter);

    scalingLayout->addRow(tr("Width: "), widthScalingModeLayout);


    QAction* imageHeightPecentageHeightAction = window->action("format_image_percentage_height");
    EQt::SpinBox* imageHeightSpinBox = new EQt::SpinBox(imageHeightPecentageHeightAction);
    imageHeightSpinBox->setFixedWidth(spinBoxWidth);
    imageHeightSpinBox->setSuffix(tr("%"));
    imageHeightSpinBox->setMinimum(minimumScaleFactor);
    imageHeightSpinBox->setMaximum(maximumScaleFactor);
    dock->registerWidget(imageHeightSpinBox, "horizontal_height_spin_box");

    QAction* imageHeightDimensionHeightAction = window->action("format_image_dimension_height");
    EQt::DimensionLineEdit* imageHeightLineEdit = new EQt::DimensionLineEdit(imageHeightDimensionHeightAction);
    imageHeightLineEdit->setDynamicUnits();
    dock->registerWidget(imageHeightLineEdit, "horizontal_height_line_edit");

    QAction* imageHeightModeAction         = window->action("format_image_height_mode");
    QMenu*   imageHeightScaleModePopupMenu = window->EQt::ProgrammaticWidget::widget<QMenu>(
        "format_image_height_scale_mode_popup_menu"
    );
    EQt::ToolButton* imageHeightScalingModeToolButton = new EQt::ToolButton(
        imageHeightModeAction,
        imageHeightScaleModePopupMenu
    );
    imageHeightScalingModeToolButton->setGlobalScaleFactorSettingsDisabled();
//    imageHeightScalingModeToolButton->setFixedSize(scalingModeButtonSize, scalingModeButtonSize);
    dock->registerWidget(imageHeightScalingModeToolButton, "horizontal_height_scaling_mode_tool_button");

    QHBoxLayout* heightScalingModeLayout = dock->newHBoxLayout("horizontal_height_scaling_controls");
    heightScalingModeLayout->addWidget(imageHeightSpinBox, Qt::AlignVCenter);
    heightScalingModeLayout->addWidget(imageHeightLineEdit, Qt::AlignVCenter);
    heightScalingModeLayout->addWidget(imageHeightScalingModeToolButton, Qt::AlignVCenter);

    scalingLayout->addRow(tr("Height: "), heightScalingModeLayout);

    horizontalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedHorizontalGap);

    //dock->setHeightAdjustmentLayout(scalingLayout, 2);
    horizontalLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void ImageBuilder::buildDockVerticalOrientation(
        EQt::ProgrammaticDockWidget* dock,
        QVBoxLayout*                 verticalLayout,
        EQt::ProgrammaticMainWindow* window
    ) {
    QHBoxLayout* horizontalLayout = dock->newHBoxLayout("vertical_horizontal");
    verticalLayout->addLayout(horizontalLayout);

    QVBoxLayout* buttonsLayout = dock->newVBoxLayout("vertical_buttons");
    horizontalLayout->addLayout(buttonsLayout, Qt::AlignVCenter);

    QAction* imageRotateAction = window->action("format_image_rotate");
    EQt::ToolButton* imageRotateToolButton = new EQt::ToolButton(imageRotateAction);
    buttonsLayout->addWidget(imageRotateToolButton);

    QAction* imageFormatDialogAction = window->action("format_image_dialog");
    EQt::ToolButton* formatImageToolButton = new EQt::ToolButton(imageFormatDialogAction);
    buttonsLayout->addWidget(formatImageToolButton);

    QVBoxLayout* verticalAlignmentLayout = dock->newVBoxLayout("vertical_scaling_alignment");
    horizontalLayout->addLayout(verticalAlignmentLayout);

    verticalAlignmentLayout->addStretch(1);
    QFormLayout* scalingLayout = dock->newFormLayout("vertical_scaling");
    verticalAlignmentLayout->addLayout(scalingLayout);
    verticalAlignmentLayout->addStretch(1);

    QFontMetrics fontMetrics(Application::font());
    unsigned spinBoxWidth = static_cast<unsigned>(fontMetrics.horizontalAdvance(tr("XXXXX%   ")));


    QAction* imagePercentageWidthAction = window->action("format_image_percentage_width");
    EQt::SpinBox* imageWidthSpinBox = new EQt::SpinBox(imagePercentageWidthAction);
    imageWidthSpinBox->setFixedWidth(spinBoxWidth);
    imageWidthSpinBox->setSuffix(tr("%"));
    imageWidthSpinBox->setMinimum(minimumScaleFactor);
    imageWidthSpinBox->setMaximum(maximumScaleFactor);
    dock->registerWidget(imageWidthSpinBox, "vertical_width_spin_box");

    QAction* imageDimensionWidthAction = window->action("format_image_dimension_width");
    EQt::DimensionLineEdit* imageWidthLineEdit = new EQt::DimensionLineEdit(imageDimensionWidthAction);
    imageWidthLineEdit->setDynamicUnits();
    dock->registerWidget(imageWidthLineEdit, "vertical_width_line_edit");

    QAction* imageWidthModeAction         = window->action("format_image_width_mode");
    QMenu*   imageWidthScaleModePopupMenu = window->EQt::ProgrammaticWidget::widget<QMenu>(
        "format_image_width_scale_mode_popup_menu"
    );
    EQt::ToolButton* imageWidthScalingModeToolButton = new EQt::ToolButton(
        imageWidthModeAction,
        imageWidthScaleModePopupMenu
    );
    imageWidthScalingModeToolButton->setGlobalScaleFactorSettingsDisabled();
//    imageWidthScalingModeToolButton->setFixedSize(scalingModeButtonSize, scalingModeButtonSize);
    dock->registerWidget(imageWidthScalingModeToolButton, "vertical_width_scaling_mode_tool_button");

    QHBoxLayout* widthScalingModeLayout = dock->newHBoxLayout("vertical_width_scaling_controls");
    widthScalingModeLayout->addWidget(imageWidthSpinBox, Qt::AlignVCenter);
    widthScalingModeLayout->addWidget(imageWidthLineEdit, Qt::AlignVCenter);
    widthScalingModeLayout->addWidget(imageWidthScalingModeToolButton, Qt::AlignVCenter);

    scalingLayout->addRow(tr("Width: "), widthScalingModeLayout);


    QAction* imageHeightPecentageHeightAction = window->action("format_image_percentage_height");
    EQt::SpinBox* imageHeightSpinBox = new EQt::SpinBox(imageHeightPecentageHeightAction);
    imageHeightSpinBox->setFixedWidth(spinBoxWidth);
    imageHeightSpinBox->setSuffix(tr("%"));
    imageHeightSpinBox->setMinimum(minimumScaleFactor);
    imageHeightSpinBox->setMaximum(maximumScaleFactor);
    dock->registerWidget(imageHeightSpinBox, "vertical_height_spin_box");

    QAction* imageHeightDimensionHeightAction = window->action("format_image_dimension_height");
    EQt::DimensionLineEdit* imageHeightLineEdit = new EQt::DimensionLineEdit(imageHeightDimensionHeightAction);
    imageHeightLineEdit->setDynamicUnits();
    dock->registerWidget(imageHeightLineEdit, "vertical_height_line_edit");

    QAction* imageHeightModeAction         = window->action("format_image_height_mode");
    QMenu*   imageHeightScaleModePopupMenu = window->EQt::ProgrammaticWidget::widget<QMenu>(
        "format_image_height_scale_mode_popup_menu"
    );
    EQt::ToolButton* imageHeightScalingModeToolButton = new EQt::ToolButton(
        imageHeightModeAction,
        imageHeightScaleModePopupMenu
    );
    imageHeightScalingModeToolButton->setGlobalScaleFactorSettingsDisabled();
//    imageHeightScalingModeToolButton->setFixedSize(scalingModeButtonSize, scalingModeButtonSize);
    dock->registerWidget(imageHeightScalingModeToolButton, "vertical_height_scaling_mode_tool_button");

    QHBoxLayout* heightScalingModeLayout = dock->newHBoxLayout("vertical_height_scaling_controls");
    heightScalingModeLayout->addWidget(imageHeightSpinBox, Qt::AlignVCenter);
    heightScalingModeLayout->addWidget(imageHeightLineEdit, Qt::AlignVCenter);
    heightScalingModeLayout->addWidget(imageHeightScalingModeToolButton, Qt::AlignVCenter);

    scalingLayout->addRow(tr("Height: "), heightScalingModeLayout);

    verticalLayout->addSpacing(EQt::ProgrammaticDockWidget::recommendedVerticalGap);

    //dock->setWidthAdjustmentLayout(horizontalLayout, 2);
    verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
}


void ImageBuilder::updateInsertDock(EQt::ProgrammaticMainWindow* window) {
    EQt::ProgrammaticDockWidget* insertDock = window->dockWidget("insert");

    EQt::StackingLayout* horizontalStackingLayout =
        insertDock->EQt::ProgrammaticWidget::layout<EQt::StackingLayout>("horizontal_annotations_stacking_layout");
    EQt::StackingLayout* verticalStackingLayout =
        insertDock->EQt::ProgrammaticWidget::layout<EQt::StackingLayout>("vertical_annotations_stacking_layout");

    QAction* insertImageDialogAction = window->action("insert_image_dialog");

    horizontalStackingLayout->appendWidget(new EQt::ToolButton(insertImageDialogAction));
    verticalStackingLayout->appendWidget(new EQt::ToolButton(insertImageDialogAction));
}


QStringList ImageBuilder::supportedImageFormats() {
    QStringList result;

    QList<QByteArray> imageFormats = QImageReader::supportedImageFormats();
    for (  QList<QByteArray>::const_iterator it  = imageFormats.constBegin(),
                                             end = imageFormats.constEnd()
         ; it != end
         ; ++it
        ) {
        QString format = QString::fromLocal8Bit(*it).toUpper();
        result << format;
    }

    result.sort();

    return result;
}


QString ImageBuilder::supportedImageFormatString() {
    QStringList supportedFormats = supportedImageFormats();

    QString lastSupportedFormat = supportedFormats.last();
    supportedFormats.removeLast();

    QString result = tr("%1, and %2").arg(supportedFormats.join(tr(", "))).arg(lastSupportedFormat);
    return result;
}
