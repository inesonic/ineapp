/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref PageFormatDialog class.
***********************************************************************************************************************/

#include <QWidget>
#include <QDialog>
#include <QSet>
#include <QSharedPointer>
#include <QLocale>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QLabel>
#include <QPixmap>
#include <QIcon>
#include <QSize>
#include <QToolButton>
#include <QPushButton>
#include <QDialogButtonBox>

#include <limits>

#include <util_page_size.h>

#include <eqt_dimension_line_edit.h>
#include <eqt_custom_paper_size_dialog.h>
#include <eqt_programmatic_dialog.h>
#include <eqt_builder_base.h>
#include <eqt_builder.h>

#include <ld_format_structures.h>
#include <ld_page_format.h>

#include "application.h"
#include "page_format_dialog.h"

PageFormatDialog::PageFormatDialog(QWidget* parent):ProgrammaticDialog("PageFormatDialog", parent) {
    configureWidget();
    runBuilders();
}


PageFormatDialog::PageFormatDialog(
        const Ld::PageFormat::Aggregation& newAggregation,
        QWidget*                           parent
    ):ProgrammaticDialog(
        "PageFormatDialog",
        parent
    ) {
    configureWidget();
    runBuilders();

    setAggregation(newAggregation);
}


PageFormatDialog::~PageFormatDialog() {}


void PageFormatDialog::loadSettings(const QString& groupName) {
    QSettings* settings = Application::settings();

    settings->beginGroup(groupName);

    EQt::DimensionLineEdit* pageTopMarginLineEdit    = widget<EQt::DimensionLineEdit>("page_top_margin_line_edit");
    EQt::DimensionLineEdit* pageLeftMarginLineEdit   = widget<EQt::DimensionLineEdit>("page_left_margin_line_edit");
    EQt::DimensionLineEdit* pageRightMarginLineEdit  = widget<EQt::DimensionLineEdit>("page_right_margin_line_edit");
    EQt::DimensionLineEdit* pageBottomMarginLineEdit = widget<EQt::DimensionLineEdit>("page_bottom_margin_line_edit");

    pageTopMarginLineEdit->loadSettings(
        settings,
        "page_top_margin_line_edit",
        EQt::DimensionLineEdit::Unit::PER_REGION
    );

    pageLeftMarginLineEdit->loadSettings(
        settings,
        "page_left_margin_line_edit",
        EQt::DimensionLineEdit::Unit::PER_REGION
    );

    pageRightMarginLineEdit->loadSettings(
        settings,
        "page_right_margin_line_edit",
        EQt::DimensionLineEdit::Unit::PER_REGION
    );

    pageBottomMarginLineEdit->loadSettings(
        settings,
        "page_bottom_margin_line_edit",
        EQt::DimensionLineEdit::Unit::PER_REGION
    );

    settings->endGroup();

    EQt::ProgrammaticDialog::loadSettings(groupName);
}


void PageFormatDialog::saveSettings(const QString& groupName) {
    QSettings* settings = Application::settings();

    settings->beginGroup(groupName);

    EQt::DimensionLineEdit* pageTopMarginLineEdit    = widget<EQt::DimensionLineEdit>("page_top_margin_line_edit");
    EQt::DimensionLineEdit* pageLeftMarginLineEdit   = widget<EQt::DimensionLineEdit>("page_left_margin_line_edit");
    EQt::DimensionLineEdit* pageRightMarginLineEdit  = widget<EQt::DimensionLineEdit>("page_right_margin_line_edit");
    EQt::DimensionLineEdit* pageBottomMarginLineEdit = widget<EQt::DimensionLineEdit>("page_bottom_margin_line_edit");

    pageTopMarginLineEdit->saveSettings(settings, "page_top_margin_line_edit");
    pageLeftMarginLineEdit->saveSettings(settings, "page_left_margin_line_edit");
    pageRightMarginLineEdit->saveSettings(settings, "page_right_margin_line_edit");
    pageBottomMarginLineEdit->saveSettings(settings, "page_bottom_margin_line_edit");

    settings->endGroup();

    EQt::ProgrammaticDialog::saveSettings(groupName);
}


void PageFormatDialog::setAutoApplyEnabled(bool nowAutoApply) {
    currentAutoApplyEnabled = nowAutoApply;
}


void PageFormatDialog::setAutoApplyDisabled(bool nowAutoApplyDisabled) {
    currentAutoApplyEnabled = !nowAutoApplyDisabled;
}


bool PageFormatDialog::autoApplyEnabled() const {
    return currentAutoApplyEnabled;
}


bool PageFormatDialog::autoApplyDisabled() const {
    return currentAutoApplyEnabled;
}


const Ld::PageFormat::Aggregation& PageFormatDialog::inputAggregation() const {
    return currentInputAggregation;
}


const Ld::PageFormat::Aggregation& PageFormatDialog::outputAggregation() const {
    return workingAggregation;
}


void PageFormatDialog::populate() {
    QComboBox* pageSizeComboBox = widget<QComboBox>("page_size_combo_box");
    pageSizeComboBox->setCurrentIndex(-1);

    QToolButton* pagePortraitOrientationToolButton = widget<QToolButton>("page_portrait_orientation_tool_button");
    pagePortraitOrientationToolButton->setChecked(false);

    QToolButton* pageLandscapeOrientationToolButton = widget<QToolButton>("page_landscape_orientation_tool_button");
    pageLandscapeOrientationToolButton->setChecked(false);

    EQt::DimensionLineEdit* pageTopMarginLineEdit = widget<EQt::DimensionLineEdit>("page_top_margin_line_edit");
    pageTopMarginLineEdit->clear();

    EQt::DimensionLineEdit* pageLeftMarginLineEdit = widget<EQt::DimensionLineEdit>("page_left_margin_line_edit");
    pageLeftMarginLineEdit->clear();

    EQt::DimensionLineEdit* pageRightMarginLineEdit = widget<EQt::DimensionLineEdit>("page_right_margin_line_edit");
    pageRightMarginLineEdit->clear();

    EQt::DimensionLineEdit* pageBottomMarginLineEdit = widget<EQt::DimensionLineEdit>("page_bottom_margin_line_edit");
    pageBottomMarginLineEdit->clear();

    QSet<Util::PageSize> pageSizes = workingAggregation.pageSizes();
    if (pageSizes.count() == 1) {
        bool useMetric;

        QLocale::MeasurementSystem measurementSystem = Application::locale()->measurementSystem();
        if (measurementSystem == QLocale::MeasurementSystem::ImperialUSSystem ||
            measurementSystem == QLocale::MeasurementSystem::ImperialUKSystem    ) {
            useMetric = false;
        } else {
            useMetric = true;
        }

        QString pageSizeDescription = pageSizes.begin()->description(useMetric);
        pageSizeComboBox->setCurrentText(pageSizeDescription);
    }

    Ld::Format::BooleanAggregation landscapeOrientation = workingAggregation.landscape();

    if (landscapeOrientation.allTrue()) {
        pageLandscapeOrientationToolButton->setChecked(true);
    } else if (landscapeOrientation.allFalse()) {
        pagePortraitOrientationToolButton->setChecked(true);
    }

    QSet<float> leftMargins = workingAggregation.leftMargins();
    if (leftMargins.count() == 1) {
        pageLeftMarginLineEdit->setValue(*leftMargins.begin());
    }

    QSet<float> rightMargins = workingAggregation.rightMargins();
    if (rightMargins.count() == 1) {
        pageRightMarginLineEdit->setValue(*rightMargins.begin());
    }

    QSet<float> topMargins = workingAggregation.topMargins();
    if (topMargins.count() == 1) {
        pageTopMarginLineEdit->setValue(*topMargins.begin());
    }

    QSet<float> bottomMargins = workingAggregation.bottomMargins();
    if (bottomMargins.count() == 1) {
        pageBottomMarginLineEdit->setValue(*bottomMargins.begin());
    }

    pageLeftMarginLineEdit->setMaximumValue(workingAggregation.maximumAllowableLeftMargin());
    pageRightMarginLineEdit->setMaximumValue(workingAggregation.maximumAllowableRightMargin());
    pageTopMarginLineEdit->setMaximumValue(workingAggregation.maximumAllowableTopMargin());
    pageBottomMarginLineEdit->setMaximumValue(workingAggregation.maximumAllowableBottomMargin());

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QPushButton* okButton = widget<QPushButton>("ok_push_button");
        okButton->setEnabled(false);

        QPushButton* applyButton = widget<QPushButton>("apply_push_button");
        applyButton->setEnabled(false);

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif
}


void PageFormatDialog::setAggregation(const Ld::PageFormat::Aggregation& newAggregation) {
    currentInputAggregation = newAggregation;
    workingAggregation      = newAggregation;

    allFormats.clear();

    Ld::FormatPointerSet formats = newAggregation.formats();
    for (  Ld::FormatPointerSet::const_iterator it  = formats.constBegin(), end = formats.constEnd()
         ; it != end
         ; ++it
        ) {
        QSharedPointer<Ld::PageFormat> format = (*it)->clone().dynamicCast<Ld::PageFormat>();
        allFormats.insert(format);
    }

    if (isVisible()) {
        populate();
    }
}


void PageFormatDialog::bind() {
    QComboBox* pageSizeComboBox = widget<QComboBox>("page_size_combo_box");
    connect(pageSizeComboBox, &QComboBox::textActivated, this, &PageFormatDialog::pageSizeChanged);

    QToolButton* pagePortraitOrientationToolButton = widget<QToolButton>("page_portrait_orientation_tool_button");
    connect(
        pagePortraitOrientationToolButton,
        &QToolButton::clicked,
        this,
        &PageFormatDialog::portraitOrientationClicked
    );

    QToolButton* pageLandscapeOrientationToolButton = widget<QToolButton>("page_landscape_orientation_tool_button");
    connect(
        pageLandscapeOrientationToolButton,
        &QToolButton::clicked,
        this,
        &PageFormatDialog::landscapeOrientationClicked
    );

    EQt::DimensionLineEdit* pageTopMarginLineEdit = widget<EQt::DimensionLineEdit>("page_top_margin_line_edit");
    connect(pageTopMarginLineEdit, &EQt::DimensionLineEdit::valueChanged, this, &PageFormatDialog::topMarginChanged);

    EQt::DimensionLineEdit* pageLeftMarginLineEdit = widget<EQt::DimensionLineEdit>("page_left_margin_line_edit");
    connect(pageLeftMarginLineEdit, &EQt::DimensionLineEdit::valueChanged, this, &PageFormatDialog::leftMarginChanged);

    EQt::DimensionLineEdit* pageRightMarginLineEdit = widget<EQt::DimensionLineEdit>("page_right_margin_line_edit");
    connect(
        pageRightMarginLineEdit,
        &EQt::DimensionLineEdit::valueChanged,
        this,
        &PageFormatDialog::rightMarginChanged
    );

    EQt::DimensionLineEdit* pageBottomMarginLineEdit = widget<EQt::DimensionLineEdit>("page_bottom_margin_line_edit");
    connect(
        pageBottomMarginLineEdit,
        &EQt::DimensionLineEdit::valueChanged,
        this,
        &PageFormatDialog::bottomMarginChanged
    );

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QPushButton* okButton = widget<QPushButton>("ok_push_button");
        connect(okButton, &QPushButton::clicked, this, &PageFormatDialog::accept);

        QPushButton* applyButton = widget<QPushButton>("apply_push_button");
        connect(applyButton, &QPushButton::clicked, this, &PageFormatDialog::applyClicked);

        QPushButton* cancelButton = widget<QPushButton>("cancel_push_button");
        connect(cancelButton, &QPushButton::clicked, this, &PageFormatDialog::reject);

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif

    ProgrammaticDialog::bind();
}


void PageFormatDialog::closeEvent(QCloseEvent* event) {
    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        EQt::ProgrammaticDialog::closeEvent(event);

    #elif (defined(Q_OS_DARWIN))

        QComboBox* pageSizeComboBox = widget<QComboBox>("page_size_combo_box");
        pageSizeComboBox->setFocus();

        if (currentInputAggregation != workingAggregation) {
            setResult(QDialog::Accepted);
        } else {
            setResult(QDialog::Rejected);
        }

        EQt::ProgrammaticDialog::closeEvent(event);

    #else

        #error Unknown platform

    #endif
}


void PageFormatDialog::pageSizeChanged(const QString& newPageSize) {
    Util::PageSize newPage;

    if (newPageSize.left(6) == tr("Custom")) {
        EQt::CustomPaperSizeDialog customPaperSizeDialog(this);

        QSet<Util::PageSize> pageSizes = workingAggregation.pageSizes();
        if (pageSizes.count() == 1) {
            const Util::PageSize& pageSize = *pageSizes.begin();
            customPaperSizeDialog.setWidth(pageSize.width());
            customPaperSizeDialog.setHeight(pageSize.height());
        }

        int result = customPaperSizeDialog.exec();
        if (result == EQt::CustomPaperSizeDialog::Accepted) {
            newPage = Util::PageSize(customPaperSizeDialog.width(), customPaperSizeDialog.height());

            bool useMetric;

            QLocale::MeasurementSystem measurementSystem = Application::locale()->measurementSystem();
            if (measurementSystem == QLocale::MeasurementSystem::ImperialUSSystem ||
                measurementSystem == QLocale::MeasurementSystem::ImperialUKSystem    ) {
                useMetric = false;
            } else {
                useMetric = true;
            }

            QString pageSizeDescription = newPage.description(useMetric);
            QComboBox* pageSizeComboBox = widget<QComboBox>("page_size_combo_box");
            unsigned   customItem = static_cast<unsigned>(pageSizeComboBox->count()) - 1;
            pageSizeComboBox->setItemText(customItem, pageSizeDescription);
            pageSizeComboBox->setCurrentText(pageSizeDescription);
        }
    } else {
        newPage = Util::PageSize::fromDescription(newPageSize);
    }

    if (newPage.isValid()) {
        workingAggregation.clear();
        for (  QSet<QSharedPointer<Ld::PageFormat>>::const_iterator it  = allFormats.constBegin(),
                                                                    end = allFormats.constEnd()
             ; it != end
             ; ++it
            ) {
            QSharedPointer<Ld::PageFormat> format = *it;
            format->setPageSize(newPage);
            workingAggregation.addFormat(format);
        }

        populate();
        updateDialog();
    }
}


void PageFormatDialog::portraitOrientationClicked() {
    QToolButton* pagePortraitOrientationToolButton  = widget<QToolButton>("page_portrait_orientation_tool_button");
    QToolButton* pageLandscapeOrientationToolButton = widget<QToolButton>("page_landscape_orientation_tool_button");

    pagePortraitOrientationToolButton->setFocus();

    pagePortraitOrientationToolButton->setChecked(true);
    pageLandscapeOrientationToolButton->setChecked(false);

    if (!workingAggregation.portrait().allTrue()) {
        workingAggregation.clear();
        for (  QSet<QSharedPointer<Ld::PageFormat>>::const_iterator it  = allFormats.constBegin(),
                                                                    end = allFormats.constEnd()
             ; it != end
             ; ++it
            ) {
            QSharedPointer<Ld::PageFormat> format = *it;
            format->setPortrait(true);
            workingAggregation.addFormat(format);
        }

        populate();
        updateDialog();
    }
}


void PageFormatDialog::landscapeOrientationClicked() {
    QToolButton* pagePortraitOrientationToolButton  = widget<QToolButton>("page_portrait_orientation_tool_button");
    QToolButton* pageLandscapeOrientationToolButton = widget<QToolButton>("page_landscape_orientation_tool_button");

    pageLandscapeOrientationToolButton->setFocus();

    pagePortraitOrientationToolButton->setChecked(false);
    pageLandscapeOrientationToolButton->setChecked(true);

    if (!workingAggregation.landscape().allTrue()) {
        workingAggregation.clear();
        for (  QSet<QSharedPointer<Ld::PageFormat>>::const_iterator it  = allFormats.constBegin(),
                                                                    end = allFormats.constEnd()
             ; it != end
             ; ++it
            ) {
            QSharedPointer<Ld::PageFormat> format = it->dynamicCast<Ld::PageFormat>();
            format->setLandscape(true);
            workingAggregation.addFormat(format);
        }

        populate();
        updateDialog();
    }
}


void PageFormatDialog::leftMarginChanged(double newLeftMarginValue) {
    workingAggregation.clear();
    for (  QSet<QSharedPointer<Ld::PageFormat>>::const_iterator it  = allFormats.constBegin(),
                                                                end = allFormats.constEnd()
         ; it != end
         ; ++it
        ) {
        QSharedPointer<Ld::PageFormat> format = *it;
        format->setLeftMargin(newLeftMarginValue);
        workingAggregation.addFormat(format);
    }

    populate();
    updateDialog();
}


void PageFormatDialog::rightMarginChanged(double newRightMarginValue) {
    workingAggregation.clear();
    for (  QSet<QSharedPointer<Ld::PageFormat>>::const_iterator it  = allFormats.constBegin(),
                                                                end = allFormats.constEnd()
         ; it != end
         ; ++it
        ) {
        QSharedPointer<Ld::PageFormat> format = *it;
        format->setRightMargin(newRightMarginValue);
        workingAggregation.addFormat(format);
    }

    populate();
    updateDialog();
}


void PageFormatDialog::topMarginChanged(double newTopMarginValue) {
    workingAggregation.clear();
    for (  QSet<QSharedPointer<Ld::PageFormat>>::const_iterator it  = allFormats.constBegin(),
                                                                end = allFormats.constEnd()
         ; it != end
         ; ++it
        ) {
        QSharedPointer<Ld::PageFormat> format = *it;
        format->setTopMargin(newTopMarginValue);
        workingAggregation.addFormat(format);
    }

    populate();
    updateDialog();
}


void PageFormatDialog::bottomMarginChanged(double newBottomMarginValue) {
    workingAggregation.clear();
    for (  QSet<QSharedPointer<Ld::PageFormat>>::const_iterator it  = allFormats.constBegin(),
                                                                end = allFormats.constEnd()
         ; it != end
         ; ++it
        ) {
        QSharedPointer<Ld::PageFormat> format = *it;
        format->setBottomMargin(newBottomMarginValue);
        workingAggregation.addFormat(format);
    }

    populate();
    updateDialog();
}


void PageFormatDialog::applyClicked() {
    currentInputAggregation = workingAggregation;

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QPushButton* okPushButton = widget<QPushButton>("ok_push_button");
        okPushButton->setEnabled(false);

        QPushButton* applyPushButton = widget<QPushButton>("apply_push_button");
        applyPushButton->setEnabled(false);

    #elif (!defined(Q_OS_DARWIN))

        #error Unknown platform

    #endif

    emit apply(this);
}


void PageFormatDialog::configureWidget() {
    setWindowTitle(tr("Page Format"));
    setSizeGripEnabled(false);

    QVBoxLayout* mainLayout = newVBoxLayout("main_layout");
    setLayout(mainLayout);

    QHBoxLayout* horizontalLayout = newHBoxLayout("horizontal_layout");
    mainLayout->addLayout(horizontalLayout);

    QVBoxLayout* leftVerticalLayout = newVBoxLayout("left_vertical_layout");
    horizontalLayout->addLayout(leftVerticalLayout);

    leftVerticalLayout->addWidget(createPageSizeGroupBox());
    leftVerticalLayout->addStretch();
    leftVerticalLayout->addWidget(createPageOrientationGroupBox());

    horizontalLayout->addWidget(createPageMarginsGroupBox());

    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        QDialogButtonBox* buttonBox = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel
        );

        buttonBox->button(QDialogButtonBox::Ok)->setAutoDefault(false);
        buttonBox->button(QDialogButtonBox::Apply)->setAutoDefault(false);
        buttonBox->button(QDialogButtonBox::Cancel)->setAutoDefault(false);

        buttonBox->button(QDialogButtonBox::Ok)->setDefault(false);
        buttonBox->button(QDialogButtonBox::Apply)->setDefault(false);
        buttonBox->button(QDialogButtonBox::Cancel)->setDefault(false);

        registerWidget(buttonBox, "dialog_button_box");
        registerWidget(buttonBox->button(QDialogButtonBox::Ok), "ok_push_button");
        registerWidget(buttonBox->button(QDialogButtonBox::Apply), "apply_push_button");
        registerWidget(buttonBox->button(QDialogButtonBox::Cancel), "cancel_push_button");

        mainLayout->addWidget(buttonBox);

    #elif (defined(Q_OS_DARWIN))

    #else

        #error Unknown platform

    #endif
}


void PageFormatDialog::updateDialog() {
    #if (defined(Q_OS_WIN) || defined(Q_OS_LINUX))

        bool valuesHaveChanged = (currentInputAggregation != workingAggregation);

        QPushButton* okButton = widget<QPushButton>("ok_push_button");
        okButton->setEnabled(valuesHaveChanged);

        QPushButton* applyButton = widget<QPushButton>("apply_push_button");
        applyButton->setEnabled(valuesHaveChanged);

    #elif (defined(Q_OS_DARWIN))

        if (currentAutoApplyEnabled) {
            applyClicked();
        }

    #else

        #error Unknown platform

    #endif
}


QGroupBox* PageFormatDialog::createPageSizeGroupBox() {
    QGroupBox* pageSizeGroupBox = new QGroupBox(tr("Page Size"));
    registerWidget(pageSizeGroupBox, "page_size_group_box");

    QVBoxLayout* pageSizeVerticalLayout = newVBoxLayout("page_size_vertical_layout");
    pageSizeGroupBox->setLayout(pageSizeVerticalLayout);

    QComboBox* pageSizeComboBox = new QComboBox(this);
    pageSizeVerticalLayout->addWidget(pageSizeComboBox);
    registerWidget(pageSizeComboBox, "page_size_combo_box");

    pageSizeComboBox->setToolTip(tr("Select a page size here"));
    pageSizeComboBox->setWhatsThis(
        tr(
            "You can use this combo box to select your desired page size from a list.  Alternately, you can use the "
            "\"Custom\" value to create your own custom page size."
           )
    );

    populateComboBox(pageSizeComboBox);

    return pageSizeGroupBox;
}


QGroupBox* PageFormatDialog::createPageOrientationGroupBox() {
    QGroupBox* pageOrientationGroupBox = new QGroupBox(tr("Page Orientation"));
    registerWidget(pageOrientationGroupBox, "page_orientation_group_box");

    QHBoxLayout* pageOrientationLayout = newHBoxLayout("page_orientation_layout");
    pageOrientationGroupBox->setLayout(pageOrientationLayout);

    QIcon pagePortraitOrientationIcon(":format_page_orientation_portrait_active_128.png");
    QToolButton* pagePortraitOrientationToolButton = new QToolButton(this);
    pagePortraitOrientationToolButton->setIcon(pagePortraitOrientationIcon);
    pagePortraitOrientationToolButton->setIconSize(QSize(128, 128));
    pageOrientationLayout->addWidget(pagePortraitOrientationToolButton);
    registerWidget(pagePortraitOrientationToolButton, "page_portrait_orientation_tool_button");

    QIcon pageLandscapeOrientationIcon(":format_page_orientation_landscape_active_128.png");
    QToolButton* pageLandscapeOrientationToolButton = new QToolButton(this);
    pageLandscapeOrientationToolButton->setIcon(pageLandscapeOrientationIcon);
    pageLandscapeOrientationToolButton->setIconSize(QSize(128, 128));
    pageOrientationLayout->addWidget(pageLandscapeOrientationToolButton);
    registerWidget(pageLandscapeOrientationToolButton, "page_landscape_orientation_tool_button");

    pagePortraitOrientationToolButton->setCheckable(true);
    pageLandscapeOrientationToolButton->setCheckable(true);

    pagePortraitOrientationToolButton->setToolTip(tr("Click this button to select portrait page orientation"));
    pagePortraitOrientationToolButton->setWhatsThis(
        tr("You can use this button to select a portrait page orientation")
    );

    pageLandscapeOrientationToolButton->setToolTip(tr("Click this button to select landscape page orientation"));
    pageLandscapeOrientationToolButton->setWhatsThis(
        tr("You can use this button to select a landscape page orientation")
    );

    return pageOrientationGroupBox;
}


QGroupBox* PageFormatDialog::createPageMarginsGroupBox() {
    QGroupBox* pageMarginsGroupBox = new QGroupBox(tr("Page Margins"));
    registerWidget(pageMarginsGroupBox, "page_margins_group_box");

    QGridLayout* pageMarginsGridLayout = newGridLayout("page_margins_grid_layout");
    pageMarginsGroupBox->setLayout(pageMarginsGridLayout);

    QLabel* topMarginLabel = new QLabel(tr("Top"));
    topMarginLabel->setAlignment((Qt::AlignHCenter));
    pageMarginsGridLayout->addWidget(topMarginLabel, 0, 2, 1, 1, Qt::AlignHCenter);
    registerWidget(topMarginLabel, "page_top_margin_label");

    EQt::DimensionLineEdit* pageTopMarginLineEdit = new EQt::DimensionLineEdit;
    pageMarginsGridLayout->addWidget(pageTopMarginLineEdit, 1, 2, 1, 1, Qt::AlignHCenter);
    pageTopMarginLineEdit->setAlignment(Qt::AlignHCenter);
    registerWidget(pageTopMarginLineEdit, "page_top_margin_line_edit");

    QLabel* leftMarginLabel = new QLabel(tr("Left"));
    leftMarginLabel->setAlignment((Qt::AlignRight | Qt::AlignVCenter));
    pageMarginsGridLayout->addWidget(leftMarginLabel, 2, 0, 1, 1, Qt::AlignRight | Qt::AlignVCenter);
    registerWidget(leftMarginLabel, "page_left_margin_label");

    EQt::DimensionLineEdit* pageLeftMarginLineEdit = new EQt::DimensionLineEdit;
    pageMarginsGridLayout->addWidget(pageLeftMarginLineEdit, 2, 1, 1, 1, Qt::AlignRight);
    pageLeftMarginLineEdit->setAlignment(Qt::AlignRight);
    registerWidget(pageLeftMarginLineEdit, "page_left_margin_line_edit");

    QPixmap pageMarginPixmap(":format_page_margins_97_127.png");
    QLabel* pageMarginsPixmapLabel = new QLabel;
    pageMarginsPixmapLabel->setPixmap(pageMarginPixmap);
    pageMarginsGridLayout->addWidget(pageMarginsPixmapLabel, 2, 2, 1, 1, Qt::AlignCenter);
    registerWidget(pageMarginsPixmapLabel, "page_margins_pixmap_label");

    EQt::DimensionLineEdit* pageRightMarginLineEdit = new EQt::DimensionLineEdit;
    pageMarginsGridLayout->addWidget(pageRightMarginLineEdit, 2, 3, 1, 1, Qt::AlignLeft);
    pageRightMarginLineEdit->setAlignment(Qt::AlignLeft);
    registerWidget(pageRightMarginLineEdit, "page_right_margin_line_edit");

    QLabel* rightMarginLabel = new QLabel(tr("Right"));
    rightMarginLabel->setAlignment((Qt::AlignLeft | Qt::AlignVCenter));
    pageMarginsGridLayout->addWidget(rightMarginLabel, 2, 4, 1, 1, Qt::AlignLeft | Qt::AlignVCenter);
    registerWidget(rightMarginLabel, "page_right_margin_label");

    EQt::DimensionLineEdit* pageBottomMarginLineEdit = new EQt::DimensionLineEdit;
    pageMarginsGridLayout->addWidget(pageBottomMarginLineEdit, 3, 2, 1, 1, Qt::AlignHCenter);
    pageBottomMarginLineEdit->setAlignment(Qt::AlignHCenter);
    registerWidget(pageBottomMarginLineEdit, "page_bottom_margin_line_edit");

    QLabel* bottomMarginLabel = new QLabel(tr("Bottom"));
    bottomMarginLabel->setAlignment((Qt::AlignHCenter));
    pageMarginsGridLayout->addWidget(bottomMarginLabel, 4, 2, 1, 1, Qt::AlignHCenter);
    registerWidget(bottomMarginLabel, "page_bottom_margin_label");

    pageLeftMarginLineEdit->setMinimumValue(0);
    pageRightMarginLineEdit->setMinimumValue(0);
    pageTopMarginLineEdit->setMinimumValue(0);
    pageBottomMarginLineEdit->setMinimumValue(0);

    pageLeftMarginLineEdit->setToolTip(tr("Enter left page margin settings here"));
    pageLeftMarginLineEdit->setWhatsThis(
        tr(
            "You can use this field to enter the page left margin.  Values can be in inches (indicated by \" or "
            "\"in\"), points (indicated by \"pt\"), centimeters (indicated by \"cm\") or millimeters (indicates by "
            "\"mm\").  You will be prevented from entering values that would provide no usable area for the page."
        )
    );

    pageRightMarginLineEdit->setToolTip(tr("Enter right page margin settings here"));
    pageRightMarginLineEdit->setWhatsThis(
        tr(
            "You can use this field to enter the page right margin.  Values can be in inches (indicated by \" or "
            "\"in\"), points (indicated by \"pt\"), centimeters (indicated by \"cm\") or millimeters (indicates by "
            "\"mm\").  You will be prevented from entering values that would provide no usable area for the page."
        )
    );

    pageTopMarginLineEdit->setToolTip(tr("Enter top page margin settings here"));
    pageTopMarginLineEdit->setWhatsThis(
        tr(
            "You can use this field to enter the page top margin.  Values can be in inches (indicated by \" or "
            "\"in\"), points (indicated by \"pt\"), centimeters (indicated by \"cm\") or millimeters (indicates by "
            "\"mm\").  You will be prevented from entering values that would provide no usable area for the page."
        )
    );

    pageBottomMarginLineEdit->setToolTip(tr("Enter bottom page margin settings here"));
    pageBottomMarginLineEdit->setWhatsThis(
        tr(
            "You can use this field to enter the page bottom margin.  Values can be in inches (indicated by \" or "
            "\"in\"), points (indicated by \"pt\"), centimeters (indicated by \"cm\") or millimeters (indicates by "
            "\"mm\").  You will be prevented from entering values that would provide no usable area for the page."
        )
    );

    pageLeftMarginLineEdit->setDynamicUnits();
    pageRightMarginLineEdit->setDynamicUnits();
    pageTopMarginLineEdit->setDynamicUnits();
    pageBottomMarginLineEdit->setDynamicUnits();

    return pageMarginsGroupBox;
}


void PageFormatDialog::populateComboBox(QComboBox* comboBox) {
    QStringList descriptions = Application::paperSizeDescriptions();

    for (  QStringList::const_iterator it  = descriptions.constBegin(),
                                       end = descriptions.constEnd()
         ; it != end
         ; ++it
        ) {
        if (it->isEmpty()) {
            comboBox->insertSeparator(comboBox->count() + 1);
        } else {
            comboBox->addItem(*it);
        }
    }

    comboBox->insertSeparator(comboBox->count() + 1);
    comboBox->addItem(tr("Custom"));
}
