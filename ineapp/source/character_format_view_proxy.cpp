/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref CharacterFormatViewProxy class.
***********************************************************************************************************************/

#include <QObject>
#include <QString>
#include <QChar>
#include <QSharedPointer>
#include <QWeakPointer>
#include <QSet>
#include <QList>
#include <QSettings>
#include <QAction>
#include <QGroupBox>
#include <QMessageBox>
#include <QAction>
#include <QVariant>
#include <QPageSize>
#include <QList>
#include <QMap>
#include <QSet>
#include <QColor>

#include <util_page_size.h>

#include <eqt_programmatic_main_window.h>
#include <eqt_programmatic_view.h>
#include <eqt_programmatic_view_proxy.h>
#include <eqt_programmatic_dock_widget.h>
#include <eqt_font_selector_widget.h>
#include <eqt_font_combo_box.h>
#include <eqt_font_data.h>
#include <eqt_color_dialog.h>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_element.h>
#include <ld_format.h>
#include <ld_aggregations_by_capability.h>
#include <ld_font_format.h>
#include <ld_character_format.h>
#include <ld_element_cursor.h>

#include "application.h"
#include "document.h"
#include "main_window.h"
#include "view_widget.h"
#include "cursor.h"
#include "update_format_command.h"
#include "character_format_helpers.h"
#include "character_format_dialog.h"
#include "character_format_builder_late.h"
#include "character_format_view_proxy.h"

CharacterFormatViewProxy::CharacterFormatViewProxy(
        EQt::ProgrammaticView*       view,
        EQt::ProgrammaticMainWindow* window
    ):ViewProxy(
        view,
        window
    ) {
    currentCharacterFormatDialog = Q_NULLPTR;
}


CharacterFormatViewProxy::~CharacterFormatViewProxy() {}


void CharacterFormatViewProxy::bindDynamic() {
    ViewWidget* view = CharacterFormatViewProxy::view();
    updateCharacterFormatDockState(view->aggregationsByCapabilityWithoutAncestors());

    connectSignals();
}


void CharacterFormatViewProxy::unbindDynamic() {
    disconnectSignals();
}


void CharacterFormatViewProxy::bind() {
//    ViewWidget* view = CharacterFormatViewProxy::view();

//    connect(view, &ViewWidget::boldKeyPressed, this, &CharacterFormatViewProxy::boldKeyPressed);
//    connect(view, &ViewWidget::italicsKeyPressed, this, &CharacterFormatViewProxy::italicsKeyPressed);
//    connect(view, &ViewWidget::underlineKeyPressed, this, &CharacterFormatViewProxy::underlineKeyPressed);
}


void CharacterFormatViewProxy::formatsUnderCursorChanged(
        const Ld::AggregationsByCapability& /* aggregationsByCapabilityWithAncestors */,
        const Ld::AggregationsByCapability& aggregationsByCapabilityWithoutAncestors
    ) {
    // We temporarily disconnect signals to avoid a storm of superflouous activity when we update the dock widget to
    // to actions being triggered which then trigger call-backs to this class instance.
    //
    // Because of round-off, these superflouos events can in turn trigger generation of UpdateFormatCommand instances
    // modifying the document unexpectedly.

    disconnectSignals();
    updateCharacterFormatDockState(aggregationsByCapabilityWithoutAncestors);
    connectSignals();

    if (currentCharacterFormatDialog != Q_NULLPTR) {
        updateCharacterFormatDialog(currentCharacterFormatDialog);
    }
}


void CharacterFormatViewProxy::fontFamilyChanged() {
    Q_ASSERT(currentAggregationsByCapability.contains(Ld::FontFormat::formatName));

    MainWindow* window                 = CharacterFormatViewProxy::window();
    QAction*    formatFontFamilyAction = window->action("format_font_family_action");
    QVariant    variant                = formatFontFamilyAction->data();
    QString     fontFamily             = variant.isValid() ? variant.toString() : QString();

    if (!fontFamily.isEmpty()) {
        QSharedPointer<Ld::FontFormat> format = getCachableFormat();
        if (!format.isNull()) {
            format->setFamily(fontFamily);
            setCachedFormat(format);
        } else {
            UpdateFormatCommand::FormatsByElement formatsByElement;

            const Ld::FontFormat::Aggregation& aggregation =
                currentAggregationsByCapability.aggregationForFormat<Ld::FontFormat>();

            for (  Ld::FontFormat::Aggregation::const_iterator it  = aggregation.constBegin(),
                                                               end = aggregation.constEnd()
                 ; it != end
                 ; ++it
                ) {
                Ld::ElementPointer element = it->toStrongRef();
                if (!element.isNull()) {
                    Ld::FormatPointer rawFormat = element->format();
                    if (!rawFormat.isNull() && rawFormat->capabilities().contains(Ld::FontFormat::formatName)) {
                        QSharedPointer<Ld::FontFormat> format = rawFormat.dynamicCast<Ld::FontFormat>();
                        Q_ASSERT(!format.isNull());

                        if (format->family() != fontFamily) {
                            QSharedPointer<Ld::FontFormat> newFormat = format->clone().dynamicCast<Ld::FontFormat>();

                            newFormat->setFamily(fontFamily);
                            formatsByElement.insert(element, newFormat);
                        }
                    }
                }
            }

            processUpdates(formatsByElement);
        }
    }

    restoreFocus();
}


void CharacterFormatViewProxy::fontSizeChanged() {
    MainWindow* window               = CharacterFormatViewProxy::window();
    QAction*    formatFontSizeAction = window->action("format_font_size_action");
    QVariant    variant              = formatFontSizeAction->data();

    bool     ok       = false;
    unsigned fontSize = variant.isValid() ? variant.toUInt(&ok) : 0;

    if (ok && fontSize > 0) {
        QSharedPointer<Ld::FontFormat> format = getCachableFormat();
        if (!format.isNull()) {
            format->setFontSize(fontSize);
            setCachedFormat(format);
        } else {
            UpdateFormatCommand::FormatsByElement formatsByElement;

            const Ld::FontFormat::Aggregation& aggregation =
                currentAggregationsByCapability.aggregationForFormat<Ld::FontFormat>();

            for (  Ld::FontFormat::Aggregation::const_iterator it  = aggregation.constBegin(),
                                                               end = aggregation.constEnd()
                 ; it != end
                 ; ++it
                ) {
                Ld::ElementPointer element = it->toStrongRef();
                if (!element.isNull()) {
                    Ld::FormatPointer rawFormat = element->format();
                    if (!rawFormat.isNull() && rawFormat->capabilities().contains(Ld::FontFormat::formatName)) {
                        QSharedPointer<Ld::FontFormat> format = rawFormat.dynamicCast<Ld::FontFormat>();
                        Q_ASSERT(!format.isNull());

                        if (format->fontSize() != fontSize) {
                            QSharedPointer<Ld::FontFormat> newFormat = format->clone().dynamicCast<Ld::FontFormat>();

                            newFormat->setFontSize(fontSize);
                            formatsByElement.insert(element, newFormat);
                        }
                    }
                }
            }

            processUpdates(formatsByElement);
        }
    }

    restoreFocus();
}


void CharacterFormatViewProxy::boldToolButtonClicked() {
    MainWindow* window               = CharacterFormatViewProxy::window();
    QAction*    boldToolButtonAction = window->action("format_character_bold_action");
    bool        nowBold              = boldToolButtonAction->isChecked();

    QSharedPointer<Ld::FontFormat> format = getCachableFormat();
    if (!format.isNull()) {
        QSet<Ld::FontFormat::Weight> supportedWeights = supportedFontWeights(format);
        Ld::FontFormat::Weight       newFontWeight    = bestWeight(nowBold, supportedWeights);

        if (newFontWeight != Ld::FontFormat::Weight::INVALID) {
            format->setFontWeight(newFontWeight);
            setCachedFormat(format);
        }
    } else {
        UpdateFormatCommand::FormatsByElement formatsByElement;

        const Ld::FontFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::FontFormat>();

        QSet<Ld::FontFormat::Weight> supportedWeights = supportedFontWeights(aggregation);
        Ld::FontFormat::Weight       newFontWeight    = bestWeight(nowBold, supportedWeights);

        for (  Ld::FontFormat::Aggregation::const_iterator it = aggregation.constBegin(), end = aggregation.constEnd()
             ; it != end
             ; ++it
            ) {
            Ld::ElementPointer element = it->toStrongRef();
            if (!element.isNull()) {
                Ld::FormatPointer rawFormat = element->format();
                if (!rawFormat.isNull() && rawFormat->capabilities().contains(Ld::FontFormat::formatName)) {
                    QSharedPointer<Ld::FontFormat> format = rawFormat.dynamicCast<Ld::FontFormat>();
                    Q_ASSERT(!format.isNull());

                    if (format->fontWeight() != newFontWeight) {
                        QSharedPointer<Ld::FontFormat> newFormat = format->clone().dynamicCast<Ld::FontFormat>();

                        newFormat->setFontWeight(newFontWeight);
                        formatsByElement.insert(element, newFormat);
                    }
                }
            }
        }

        processUpdates(formatsByElement);
    }

    restoreFocus();
}


void CharacterFormatViewProxy::italicsSelected() {
    MainWindow* window                  = CharacterFormatViewProxy::window();
    QAction*    italicsToolButtonAction = window->action("format_character_italics_action");
    bool        nowItalics              = italicsToolButtonAction->isChecked();

    QSharedPointer<Ld::FontFormat> format = getCachableFormat();
    if (!format.isNull()) {
        format->setItalics(nowItalics);
        setCachedFormat(format);
    } else {
        UpdateFormatCommand::FormatsByElement formatsByElement;

        const Ld::FontFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::FontFormat>();

        for (  Ld::FontFormat::Aggregation::const_iterator it = aggregation.constBegin(), end = aggregation.constEnd()
             ; it != end
             ; ++it
            ) {
            Ld::ElementPointer element = it->toStrongRef();
            if (!element.isNull()) {
                Ld::FormatPointer rawFormat = element->format();
                if (!rawFormat.isNull() && rawFormat->capabilities().contains(Ld::FontFormat::formatName)) {
                    QSharedPointer<Ld::FontFormat> format = rawFormat.dynamicCast<Ld::FontFormat>();
                    Q_ASSERT(!format.isNull());

                    if (format->italics() != nowItalics) {
                        QSharedPointer<Ld::FontFormat> newFormat = format->clone().dynamicCast<Ld::FontFormat>();

                        newFormat->setItalics(nowItalics);
                        formatsByElement.insert(element, newFormat);
                    }
                }
            }
        }

        processUpdates(formatsByElement);
    }

    restoreFocus();
}


void CharacterFormatViewProxy::underlineSelected() {
    MainWindow* window                    = CharacterFormatViewProxy::window();
    QAction*    underlineToolButtonAction = window->action("format_character_underline_action");
    bool        nowUnderline              = underlineToolButtonAction->isChecked();

    QSharedPointer<Ld::FontFormat> format = getCachableFormat();
    if (!format.isNull()) {
        format->setUnderline(nowUnderline);
        setCachedFormat(format);
    } else {
        UpdateFormatCommand::FormatsByElement formatsByElement;

        const Ld::FontFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::FontFormat>();

        for (  Ld::FontFormat::Aggregation::const_iterator it = aggregation.constBegin(), end = aggregation.constEnd()
             ; it != end
             ; ++it
            ) {
            Ld::ElementPointer element = it->toStrongRef();
            if (!element.isNull()) {
                Ld::FormatPointer rawFormat = element->format();
                if (!rawFormat.isNull() && rawFormat->capabilities().contains(Ld::FontFormat::formatName)) {
                    QSharedPointer<Ld::FontFormat> format = rawFormat.dynamicCast<Ld::FontFormat>();
                    Q_ASSERT(!format.isNull());

                    if (format->underline() != nowUnderline) {
                        QSharedPointer<Ld::FontFormat> newFormat = format->clone().dynamicCast<Ld::FontFormat>();

                        newFormat->setUnderline(nowUnderline);
                        formatsByElement.insert(element, newFormat);
                    }
                }
            }
        }

        processUpdates(formatsByElement);
    }

    restoreFocus();
}


void CharacterFormatViewProxy::overlineSelected() {
    MainWindow* window                   = CharacterFormatViewProxy::window();
    QAction*    overlineToolButtonAction = window->action("format_character_overline_action");
    bool        nowOverline              = overlineToolButtonAction->isChecked();

    QSharedPointer<Ld::FontFormat> format = getCachableFormat();
    if (!format.isNull()) {
        format->setOverline(nowOverline);
        setCachedFormat(format);
    } else {
        UpdateFormatCommand::FormatsByElement formatsByElement;

        const Ld::FontFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::FontFormat>();

        for (  Ld::FontFormat::Aggregation::const_iterator it = aggregation.constBegin(), end = aggregation.constEnd()
             ; it != end
             ; ++it
            ) {
            Ld::ElementPointer element = it->toStrongRef();
            if (!element.isNull()) {
                Ld::FormatPointer rawFormat = element->format();
                if (!rawFormat.isNull() && rawFormat->capabilities().contains(Ld::FontFormat::formatName)) {
                    QSharedPointer<Ld::FontFormat> format = rawFormat.dynamicCast<Ld::FontFormat>();
                    Q_ASSERT(!format.isNull());

                    if (format->overline() != nowOverline) {
                        QSharedPointer<Ld::FontFormat> newFormat = format->clone().dynamicCast<Ld::FontFormat>();

                        newFormat->setOverline(nowOverline);
                        formatsByElement.insert(element, newFormat);
                    }
                }
            }
        }

        processUpdates(formatsByElement);
    }

    restoreFocus();
}


void CharacterFormatViewProxy::strikethroughSelected() {
    MainWindow* window                        = CharacterFormatViewProxy::window();
    QAction*    strikethroughToolButtonAction = window->action("format_character_strikethrough_action");
    bool        nowStrikethrough              = strikethroughToolButtonAction->isChecked();

    QSharedPointer<Ld::FontFormat> format = getCachableFormat();
    if (!format.isNull()) {
        format->setStrikeout(nowStrikethrough);
        setCachedFormat(format);
    } else {
        UpdateFormatCommand::FormatsByElement formatsByElement;

        const Ld::FontFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::FontFormat>();

        for (  Ld::FontFormat::Aggregation::const_iterator it = aggregation.constBegin(), end = aggregation.constEnd()
             ; it != end
             ; ++it
            ) {
            Ld::ElementPointer element = it->toStrongRef();
            if (!element.isNull()) {
                Ld::FormatPointer rawFormat = element->format();
                if (!rawFormat.isNull() && rawFormat->capabilities().contains(Ld::FontFormat::formatName)) {
                    QSharedPointer<Ld::FontFormat> format = rawFormat.dynamicCast<Ld::FontFormat>();
                    Q_ASSERT(!format.isNull());

                    if (format->strikeout() != nowStrikethrough) {
                        QSharedPointer<Ld::FontFormat> newFormat = format->clone().dynamicCast<Ld::FontFormat>();

                        newFormat->setStrikeout(nowStrikethrough);
                        formatsByElement.insert(element, newFormat);
                    }
                }
            }
        }

        processUpdates(formatsByElement);
    }

    restoreFocus();
}


void CharacterFormatViewProxy::subscriptSelected() {
    MainWindow* window            = CharacterFormatViewProxy::window();
    QAction*    subscriptAction   = window->action("format_character_subscript_action");
    QAction*    superscriptAction = window->action("format_character_superscript_action");

    Ld::CharacterFormat::Position position;
    if (subscriptAction->isChecked()) {
        position = Ld::CharacterFormat::Position::SUBSCRIPT;
    } else {
        position = Ld::CharacterFormat::Position::NORMAL;
    }

    disconnectSignals();
    superscriptAction->setChecked(false);
    connectSignals();

    applyPosition(position);
}


void CharacterFormatViewProxy::superscriptSelected() {
    MainWindow* window            = CharacterFormatViewProxy::window();
    QAction*    subscriptAction   = window->action("format_character_subscript_action");
    QAction*    superscriptAction = window->action("format_character_superscript_action");

    Ld::CharacterFormat::Position position;
    if (superscriptAction->isChecked()) {
        position = Ld::CharacterFormat::Position::SUPERSCRIPT;
    } else {
        position = Ld::CharacterFormat::Position::NORMAL;
    }

    disconnectSignals();
    subscriptAction->setChecked(false);
    connectSignals();

    applyPosition(position);
}


void CharacterFormatViewProxy::normalBaselineSelected() {
    MainWindow* window            = CharacterFormatViewProxy::window();
    QAction*    subscriptAction   = window->action("format_character_subscript_action");
    QAction*    superscriptAction = window->action("format_character_superscript_action");

    disconnectSignals();
    subscriptAction->setChecked(false);
    superscriptAction->setChecked(false);
    connectSignals();

    applyPosition(Ld::CharacterFormat::Position::NORMAL);
}


void CharacterFormatViewProxy::fontColorSelected() {
    MainWindow* window          = CharacterFormatViewProxy::window();
    QAction*    fontColorAction = window->action("format_character_color_action");
    bool        applyColor      = fontColorAction->isChecked();

    QSharedPointer<Ld::FontFormat> format = getCachableFormat();
    if (!format.isNull()) {
        QColor selectedColor;
        bool   performAction;

        if (applyColor) {
            EQt::ColorDialog colorDialog(window);

            colorDialog.setCurrentColor(format->fontColor());

            colorDialog.setWindowTitle(tr("Font Color"));
            colorDialog.loadSettings();

            int result = colorDialog.exec();
            if (result == EQt::ColorDialog::Accepted) {
                colorDialog.saveSettings();

                performAction = true;
                selectedColor = colorDialog.selectedColor();
            } else {
                performAction = false;
            }
        } else {
            performAction = true;
        }

        if (performAction) {
            format->setFontColor(selectedColor);
            setCachedFormat(format);
        }
    } else {
        const Ld::FontFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::FontFormat>();

        QColor selectedColor;
        bool   performAction;

        if (applyColor) {
            EQt::ColorDialog colorDialog(window);

            if (aggregation.fontColor().count() == 1) {
                colorDialog.setCurrentColor(*aggregation.fontColor().begin());
            }

            colorDialog.setWindowTitle(tr("Font Color"));
            colorDialog.loadSettings();

            int result = colorDialog.exec();
            if (result == EQt::ColorDialog::Accepted) {
                colorDialog.saveSettings();

                performAction = true;
                selectedColor = colorDialog.selectedColor();
            } else {
                performAction = false;
            }
        } else {
            performAction = true;
        }

        if (performAction) {
            UpdateFormatCommand::FormatsByElement formatsByElement;
            for (  Ld::FontFormat::Aggregation::const_iterator it  = aggregation.constBegin(),
                                                               end = aggregation.constEnd()
                 ; it != end
                 ; ++it
                ) {
                Ld::ElementPointer element = it->toStrongRef();
                if (!element.isNull()) {
                    Ld::FormatPointer rawFormat = element->format();
                    if (!rawFormat.isNull() && rawFormat->capabilities().contains(Ld::FontFormat::formatName)) {
                        QSharedPointer<Ld::FontFormat> format = rawFormat.dynamicCast<Ld::FontFormat>();
                        Q_ASSERT(!format.isNull());

                        if (format->fontColor() != selectedColor) {
                            QSharedPointer<Ld::FontFormat> newFormat = format->clone().dynamicCast<Ld::FontFormat>();

                            newFormat->setFontColor(selectedColor);
                            formatsByElement.insert(element, newFormat);
                        }
                    }
                }
            }

            processUpdates(formatsByElement);
        }
    }
}


void CharacterFormatViewProxy::fontBackgroundColorSelected() {
    MainWindow* window          = CharacterFormatViewProxy::window();
    QAction*    fontColorAction = window->action("format_character_background_color_action");
    bool        applyColor      = fontColorAction->isChecked();

    QSharedPointer<Ld::FontFormat> format = getCachableFormat();
    if (!format.isNull()) {
        QColor selectedColor;
        bool   performAction;

        if (applyColor) {
            EQt::ColorDialog colorDialog(window);

            colorDialog.setCurrentColor(format->fontBackgroundColor());

            colorDialog.setWindowTitle(tr("Highlight Color"));
            colorDialog.loadSettings();

            int result = colorDialog.exec();
            if (result == EQt::ColorDialog::Accepted) {
                colorDialog.saveSettings();

                performAction = true;
                selectedColor = colorDialog.selectedColor();
            } else {
                performAction = false;
            }
        } else {
            performAction = true;
        }

        if (performAction) {
            format->setFontBackgroundColor(selectedColor);
            setCachedFormat(format);
        }
    } else {
        const Ld::FontFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::FontFormat>();

        QColor selectedColor;
        bool   performAction;

        if (applyColor) {
            EQt::ColorDialog colorDialog(window);

            if (aggregation.fontColor().count() == 1) {
                colorDialog.setCurrentColor(*aggregation.fontBackgroundColor().begin());
            }

            colorDialog.setWindowTitle(tr("Highlight Color"));
            colorDialog.loadSettings();

            int result = colorDialog.exec();
            if (result == EQt::ColorDialog::Accepted) {
                colorDialog.saveSettings();

                performAction = true;
                selectedColor = colorDialog.selectedColor();
            } else {
                performAction = false;
            }
        } else {
            performAction = true;
        }

        if (performAction) {
            UpdateFormatCommand::FormatsByElement formatsByElement;
            for (  Ld::FontFormat::Aggregation::const_iterator it  = aggregation.constBegin(),
                                                               end = aggregation.constEnd()
                 ; it != end
                 ; ++it
                ) {
                Ld::ElementPointer element = it->toStrongRef();
                if (!element.isNull()) {
                    Ld::FormatPointer rawFormat = element->format();
                    if (!rawFormat.isNull() && rawFormat->capabilities().contains(Ld::FontFormat::formatName)) {
                        QSharedPointer<Ld::FontFormat> format = rawFormat.dynamicCast<Ld::FontFormat>();
                        Q_ASSERT(!format.isNull());

                        if (format->fontBackgroundColor() != selectedColor) {
                            QSharedPointer<Ld::FontFormat> newFormat = format->clone().dynamicCast<Ld::FontFormat>();

                            newFormat->setFontBackgroundColor(selectedColor);
                            formatsByElement.insert(element, newFormat);
                        }
                    }
                }
            }

            processUpdates(formatsByElement);
        }
    }
}


void CharacterFormatViewProxy::characterFormatDialogRequested() {
    MainWindow* window = CharacterFormatViewProxy::window();

    CharacterFormatDialog characterFormatDialog(window);
    currentCharacterFormatDialog = &characterFormatDialog;

    characterFormatDialog.loadSettings(QString("CharacterFormatDialog"));

    connect(
        &characterFormatDialog,
        &CharacterFormatDialog::apply,
        this,
        &CharacterFormatViewProxy::characterFormatDialogApplyUpdates
    );

    updateCharacterFormatDialog(&characterFormatDialog);

    int result = characterFormatDialog.exec();
    currentCharacterFormatDialog = Q_NULLPTR;

    if (result == CharacterFormatDialog::Accepted) {
        characterFormatDialogApplyUpdates(&characterFormatDialog);
    }
}


void CharacterFormatViewProxy::characterFormatDialogApplyUpdates(CharacterFormatDialog* dialog) {
    dialog->saveSettings(QString("CharacterFormatDialog"));

    QSharedPointer<Ld::FontFormat> format = getCachableFormat();
    if (!format.isNull()) {
        bool formatUpdated = updateFormatFromDialog(dialog, format);

        if (formatUpdated) {
            setCachedFormat(format);
        }
    } else {
        const Ld::FontFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::FontFormat>();

        UpdateFormatCommand::FormatsByElement formatsByElement;

        for (  QSet<Ld::ElementWeakPointer>::const_iterator elementIterator    = aggregation.constBegin(),
                                                            elementEndIterator = aggregation.constEnd()
             ; elementIterator != elementEndIterator
             ; ++elementIterator
            ) {
            Ld::ElementPointer element = elementIterator->toStrongRef();
            if (!element.isNull()) {
                Ld::FormatPointer format = element->format();
                if (!format.isNull()) {
                    Ld::FormatPointer clonedFormat = format->clone();
                    bool formatUpdated = updateFormatFromDialog(dialog, clonedFormat);
                    if (formatUpdated) {
                        formatsByElement.insert(element, clonedFormat);
                    }
                }
            }
        }

        processUpdates(formatsByElement);
    }
}


//void CharacterFormatViewProxy::boldKeyPressed() {
//    qDebug() << "boldKeyPressed()";
//}


//void CharacterFormatViewProxy::italicsKeyPressed() {
//    qDebug() << "italicsKeyPressed()";
//}


//void CharacterFormatViewProxy::underlineKeyPressed() {
//    qDebug() << "underlineKeyPressed()";
//}


void CharacterFormatViewProxy::updateCharacterFormatDockState(
        const Ld::AggregationsByCapability& aggregationsByCapability
    ) {
    currentAggregationsByCapability = aggregationsByCapability;

    MainWindow* window = CharacterFormatViewProxy::window();

    QAction* formatCharacterDialogAction = window->action("format_character_dialog");
    QAction* formatFontFamilyAction = window->action("format_font_family_action");
    QAction* formatFontSizeAction = window->action("format_font_size_action");
    QAction* boldAction = window->action("format_character_bold_action");
    QAction* italicsAction = window->action("format_character_italics_action");
    QAction* underlineAction = window->action("format_character_underline_action");
    QAction* overlineAction = window->action("format_character_overline_action");
    QAction* strikethroughAction = window->action("format_character_strikethrough_action");
    QAction* subscriptAction = window->action("format_character_subscript_action");
    QAction* superscriptAction = window->action("format_character_superscript_action");
    QAction* normalBaselineAction = window->action("format_character_normal_baseline_action");
    QAction* fontColorAction = window->action("format_character_color_action");
    QAction* fontBackgroundColorAction = window->action("format_character_background_color_action");

    if (aggregationsByCapability.contains(Ld::FontFormat::formatName)) {
        formatCharacterDialogAction->setEnabled(true);
        formatFontFamilyAction->setEnabled(true);
        formatFontSizeAction->setEnabled(true);
        italicsAction->setEnabled(true);
        underlineAction->setEnabled(true);
        overlineAction->setEnabled(true);
        strikethroughAction->setEnabled(true);
        fontColorAction->setEnabled(true);
        fontBackgroundColorAction->setEnabled(true);

        const Ld::FontFormat::Aggregation& aggregation =
            aggregationsByCapability.aggregationForFormat<Ld::FontFormat>();

        if (aggregation.families().count() == 1) {
            formatFontFamilyAction->setData(QVariant::fromValue(*aggregation.families().begin()));
        } else {
            formatFontFamilyAction->setData(QVariant());
        }

        formatFontFamilyAction->trigger();

        if (aggregation.sizes().count() == 1) {
            formatFontSizeAction->setData(QVariant::fromValue(*aggregation.sizes().begin()));
        } else {
            formatFontSizeAction->setData(QVariant());
        }

        formatFontSizeAction->trigger();

        QSet<Ld::FontFormat::Weight> supportedWeights = supportedFontWeights(aggregation);

        bool mediumWeightSupported    = supportedWeights.contains(Ld::FontFormat::Weight::MEDIUM);
        bool demiBoldWeightSupported  = supportedWeights.contains(Ld::FontFormat::Weight::DEMI_BOLD);
        bool boldWeightSupported      = supportedWeights.contains(Ld::FontFormat::Weight::BOLD);
        bool extraBoldWeightSupported = supportedWeights.contains(Ld::FontFormat::Weight::EXTRA_BOLD);
        bool blackWeightSupported     = supportedWeights.contains(Ld::FontFormat::Weight::BLACK);

        boldAction->setEnabled(
               mediumWeightSupported
            || demiBoldWeightSupported
            || boldWeightSupported
            || extraBoldWeightSupported
            || blackWeightSupported
        );

        QSet<Ld::FontFormat::Weight> weights = aggregation.weights();

        bool mediumWeightFound    = weights.contains(Ld::FontFormat::Weight::MEDIUM);
        bool demiBoldWeightFound  = weights.contains(Ld::FontFormat::Weight::DEMI_BOLD);
        bool boldWeightFound      = weights.contains(Ld::FontFormat::Weight::BOLD);
        bool extraBoldWeightFound = weights.contains(Ld::FontFormat::Weight::EXTRA_BOLD);
        bool blackWeightFound     = weights.contains(Ld::FontFormat::Weight::BLACK);

        boldAction->setChecked(
               mediumWeightFound
            || demiBoldWeightFound
            || boldWeightFound
            || extraBoldWeightFound
            || blackWeightFound
        );

        italicsAction->setChecked(!aggregation.italics().allFalse());
        underlineAction->setChecked(!aggregation.underline().allFalse());
        overlineAction->setChecked(!aggregation.overline().allFalse());
        strikethroughAction->setChecked(!aggregation.strikeOut().allFalse());

        bool fontColorFound;
        if (aggregation.fontColor().count() == 1) {
            fontColorFound = (aggregation.fontColor().begin()->isValid());
        } else {
            fontColorFound = (aggregation.fontColor().count() > 1);
        }

        bool fontBackgroundColorFound;
        if (aggregation.fontBackgroundColor().count() == 1) {
            fontBackgroundColorFound = (aggregation.fontBackgroundColor().begin()->isValid());
        } else {
            fontBackgroundColorFound = (aggregation.fontBackgroundColor().count() > 1);
        }

        fontColorAction->setChecked(fontColorFound);
        fontBackgroundColorAction->setChecked(fontBackgroundColorFound);
    } else {
        formatCharacterDialogAction->setEnabled(false);
        formatFontFamilyAction->setEnabled(false);
        formatFontSizeAction->setEnabled(false);
        boldAction->setEnabled(false);
        italicsAction->setEnabled(false);
        underlineAction->setEnabled(false);
        overlineAction->setEnabled(false);
        strikethroughAction->setEnabled(false);
        fontColorAction->setEnabled(false);
        fontBackgroundColorAction->setEnabled(false);

        boldAction->setChecked(false);
        italicsAction->setChecked(false);
        underlineAction->setChecked(false);
        overlineAction->setChecked(false);
        strikethroughAction->setChecked(false);

        fontColorAction->setChecked(false);
        fontBackgroundColorAction->setChecked(false);
    }

    if (aggregationsByCapability.contains(Ld::CharacterFormat::formatName)) {
        const Ld::CharacterFormat::Aggregation& aggregation =
            aggregationsByCapability.aggregationForFormat<Ld::CharacterFormat>();

        subscriptAction->setEnabled(true);
        superscriptAction->setEnabled(true);
        normalBaselineAction->setEnabled(true);

        QSet<Ld::CharacterFormat::Position> positions = aggregation.positions();

        bool subscriptFound      = positions.contains(Ld::CharacterFormat::Position::SUBSCRIPT);
        bool superscriptFound    = positions.contains(Ld::CharacterFormat::Position::SUPERSCRIPT);
        bool normalBaselineFound = positions.contains(Ld::CharacterFormat::Position::NORMAL);

        subscriptAction->setChecked(subscriptFound);
        superscriptAction->setChecked(superscriptFound);
        normalBaselineAction->setChecked(normalBaselineFound);
    } else {
        subscriptAction->setEnabled(false);
        superscriptAction->setEnabled(false);
        normalBaselineAction->setEnabled(false);

        subscriptAction->setChecked(false);
        superscriptAction->setChecked(false);
        normalBaselineAction->setChecked(false);
    }
}


void CharacterFormatViewProxy::connectSignals() {
    MainWindow* window = CharacterFormatViewProxy::window();
    ViewWidget* view   = CharacterFormatViewProxy::view();

    sessionConnect(view, &ViewWidget::formatsAtCursorChanged, &CharacterFormatViewProxy::formatsUnderCursorChanged);

    QAction* formatCharacterDialogAction = window->action("format_character_dialog");
    sessionConnect(
        formatCharacterDialogAction,
        &QAction::triggered,
        &CharacterFormatViewProxy::characterFormatDialogRequested
    );

    QAction* formatFontFamilyAction = window->action("format_font_family_action");
    sessionConnect(
        formatFontFamilyAction,
        &QAction::triggered,
        &CharacterFormatViewProxy::fontFamilyChanged
    );

    QAction* formatFontSizeAction = window->action("format_font_size_action");
    sessionConnect(
        formatFontSizeAction,
        &QAction::triggered,
        &CharacterFormatViewProxy::fontSizeChanged
    );

    QAction* boldAction = window->action("format_character_bold_action");
    sessionConnect(boldAction, &QAction::triggered, &CharacterFormatViewProxy::boldToolButtonClicked);

    QAction* italicsAction = window->action("format_character_italics_action");
    sessionConnect(italicsAction, &QAction::triggered, &CharacterFormatViewProxy::italicsSelected);

    QAction* underlineAction = window->action("format_character_underline_action");
    sessionConnect(underlineAction, &QAction::triggered, &CharacterFormatViewProxy::underlineSelected);

    QAction* overlineAction = window->action("format_character_overline_action");
    sessionConnect(overlineAction, &QAction::triggered, &CharacterFormatViewProxy::overlineSelected);

    QAction* strikethroughAction = window->action("format_character_strikethrough_action");
    sessionConnect(strikethroughAction, &QAction::triggered, &CharacterFormatViewProxy::strikethroughSelected);

    QAction* subscriptAction = window->action("format_character_subscript_action");
    sessionConnect(subscriptAction, &QAction::triggered, &CharacterFormatViewProxy::subscriptSelected);

    QAction* superscriptAction = window->action("format_character_superscript_action");
    sessionConnect(superscriptAction, &QAction::triggered, &CharacterFormatViewProxy::superscriptSelected);

    QAction* normalBaselineAction = window->action("format_character_normal_baseline_action");
    sessionConnect(normalBaselineAction, &QAction::triggered, &CharacterFormatViewProxy::normalBaselineSelected);

    QAction* fontColorAction = window->action("format_character_color_action");
    sessionConnect(fontColorAction, &QAction::triggered, &CharacterFormatViewProxy::fontColorSelected);

    QAction* fontBackgroundColorAction = window->action("format_character_background_color_action");
    sessionConnect(
        fontBackgroundColorAction,
        &QAction::triggered,
        &CharacterFormatViewProxy::fontBackgroundColorSelected
    );
}


void CharacterFormatViewProxy::disconnectSignals() {
    sessionDisconnectAll();
}


void CharacterFormatViewProxy::applyPosition(Ld::CharacterFormat::Position newPosition) {
    Q_ASSERT(currentAggregationsByCapability.contains(Ld::CharacterFormat::formatName));

    QSharedPointer<Ld::CharacterFormat> format = getCachableFormat().dynamicCast<Ld::CharacterFormat>();

    if (!format.isNull()) {
        format->setPosition(newPosition);
        setCachedFormat(format);
    } else {
        UpdateFormatCommand::FormatsByElement formatsByElement;

        const Ld::CharacterFormat::Aggregation& aggregation =
            currentAggregationsByCapability.aggregationForFormat<Ld::CharacterFormat>();

        for (  Ld::CharacterFormat::Aggregation::const_iterator it  = aggregation.constBegin(),
                                                                end = aggregation.constEnd()
             ; it != end
             ; ++it
            ) {
            Ld::ElementPointer element = it->toStrongRef();
            if (!element.isNull()) {
                Ld::FormatPointer rawFormat = element->format();
                if (!rawFormat.isNull() && rawFormat->capabilities().contains(Ld::CharacterFormat::formatName)) {
                    QSharedPointer<Ld::CharacterFormat> format = rawFormat.dynamicCast<Ld::CharacterFormat>();
                    Q_ASSERT(!format.isNull());

                    if (format->position() != newPosition) {
                        QSharedPointer<Ld::CharacterFormat> newFormat = format->clone()
                                                                        .dynamicCast<Ld::CharacterFormat>();

                        newFormat->setPosition(newPosition);
                        formatsByElement.insert(element, newFormat);
                    }
                }
            }
        }

        processUpdates(formatsByElement);
    }

    restoreFocus();
}


QSharedPointer<Ld::FontFormat> CharacterFormatViewProxy::getCachableFormat() const {
    QSharedPointer<Ld::CharacterFormat> result;

    ViewWidget* view = CharacterFormatViewProxy::view();
    if (view->cursorType() == Cursor::Type::TEXT) {
        const Ld::ElementCursor& elementCursor = view->cursor().elementCursor();
        Ld::ElementPointer element = elementCursor.element();
        const Fixer* fixer = Fixer::fixer(element->typeName());

        if (fixer != Q_NULLPTR && fixer->isSplitAllowed(element, Fixer::SplitReason::FORMAT_CHANGE, elementCursor)) {
            result = view->nextTextInsertionFormat(view->cursor().elementCursor()).dynamicCast<Ld::CharacterFormat>();
            if (result.isNull()) {
                Q_ASSERT(currentAggregationsByCapability.contains(Ld::CharacterFormat::formatName));

                const Ld::CharacterFormat::Aggregation& aggregation =
                    currentAggregationsByCapability.aggregationForFormat<Ld::CharacterFormat>();

                Ld::FormatPointerSet formats = aggregation.formats();
                Q_ASSERT(formats.size() == 1);
                result = (*formats.begin())->clone().dynamicCast<Ld::CharacterFormat>();
            }
        }
    }

    return result;
}


void CharacterFormatViewProxy::setCachedFormat(QSharedPointer<Ld::FontFormat> format) {
    ViewWidget* view = CharacterFormatViewProxy::view();
    view->setFormatForNextTextInsertion(format);
}


void CharacterFormatViewProxy::processUpdates(const UpdateFormatCommand::FormatsByElement& formatsByElement) {
    if (!formatsByElement.isEmpty()) {
        ViewWidget* view = CharacterFormatViewProxy::view();
        view->insertCommand(new UpdateFormatCommand(formatsByElement));
    }
}


void CharacterFormatViewProxy::issueUpdateFormatCommand(
        const UpdateFormatCommand::FormatsByElement& formatsByElement
    ) {
    ViewWidget* view = CharacterFormatViewProxy::view();
    view->insertCommand(new UpdateFormatCommand(formatsByElement));
}


bool CharacterFormatViewProxy::updateFormatFromDialog(CharacterFormatDialog* dialog, Ld::FormatPointer format) {
    bool                     changed      = false;
    Ld::Format::Capabilities capabilities = format->capabilities();

    if (capabilities.contains(Ld::FontFormat::formatName)) {
        QSharedPointer<Ld::FontFormat> fontFormat = format.dynamicCast<Ld::FontFormat>();

        QString fontFamily = dialog->fontFamily();
        if (!fontFamily.isEmpty() && fontFormat->family() != fontFamily) {
            fontFormat->setFamily(fontFamily);
            changed = true;
        }

        unsigned fontSize = dialog->fontSize();
        if (fontSize >= Ld::FontFormat::minimumFontSize && fontFormat->fontSize() != fontSize) {
            fontFormat->setFontSize(fontSize);
            changed = true;
        }

        Ld::FontFormat::Weight weight = dialog->fontWeight();
        if (weight != Ld::FontFormat::Weight::INVALID && fontFormat->fontWeight() != weight) {
            fontFormat->setFontWeight(weight);
            changed = true;
        }

        if (!dialog->italicsBothEnabledAndDisabled() && fontFormat->italics() != dialog->italicsEnabled()) {
            fontFormat->setItalics(dialog->italicsEnabled());
            changed = true;
        }

        if (!dialog->underlineBothEnabledAndDisabled() && fontFormat->underline() != dialog->underlineEnabled()) {
            fontFormat->setUnderline(dialog->underlineEnabled());
            changed = true;
        }

        if (!dialog->overlineBothEnabledAndDisabled() && fontFormat->overline() != dialog->overlineEnabled()) {
            fontFormat->setOverline(dialog->overlineEnabled());
            changed = true;
        }

        if (!dialog->strikethroughBothEnabledAndDisabled()            &&
            fontFormat->strikeout() != dialog->strikethroughEnabled()    ) {
            fontFormat->setStrikeout(dialog->strikethroughEnabled());
            changed = true;
        }

        if (dialog->fontColorSelected() && fontFormat->fontColor() != dialog->fontColor()) {
            fontFormat->setFontColor(dialog->fontColor());
            changed = true;
        }

        if (dialog->fontBackgroundColorSelected()                              &&
            fontFormat->fontBackgroundColor() != dialog->fontBackgroundColor()    ) {
            fontFormat->setFontBackgroundColor(dialog->fontBackgroundColor());
            changed = true;
        }

        if (dialog->letterSpacingValid() && fontFormat->letterSpacing() != dialog->letterSpacing()) {
            fontFormat->setLetterSpacing(dialog->letterSpacing());
            changed = true;
        }
    }

    if (capabilities.contains(Ld::CharacterFormat::formatName)) {
        QSharedPointer<Ld::CharacterFormat> characterFormat = format.dynamicCast<Ld::CharacterFormat>();

        Ld::CharacterFormat::Position position = dialog->position();
        if (position != Ld::CharacterFormat::Position::INVALID && characterFormat->position() != position) {
            characterFormat->setPosition(position);
            changed = true;
        }
    }

    return changed;
}


void CharacterFormatViewProxy::updateCharacterFormatDialog(CharacterFormatDialog* dialog) {
    QSharedPointer<Ld::FontFormat> format = getCachableFormat();
    if (!format.isNull()) {
        dialog->setFontFamily(format->family());
        dialog->setFontSize(format->fontSize());
        dialog->setFontWeight(format->fontWeight());
        dialog->setSupportedFontWeights(supportedFontWeights(format));
        dialog->setItalicsEnabled(format->italics());
        dialog->setUnderlineEnabled(format->underline());
        dialog->setOverlineEnabled(format->overline());
        dialog->setStrikethroughEnabled(format->strikeout());
        dialog->setFontColor(format->fontColor());
        dialog->setFontBackgroundColor(format->fontBackgroundColor());
        dialog->setLetterSpacing(format->letterSpacing());

        if (format->capabilities().contains(Ld::CharacterFormat::formatName)) {
            QSharedPointer<Ld::CharacterFormat> characterFormat = format.dynamicCast<Ld::CharacterFormat>();
            Q_ASSERT(!characterFormat.isNull());

            dialog->setPosition(characterFormat->position());
            dialog->setPositionControlsEnabled();
        } else {
            dialog->setPositionControlsDisabled();
        }
    } else {
        Q_ASSERT(currentAggregationsByCapability.contains(Ld::FontFormat::formatName));
        {
            const Ld::FontFormat::Aggregation& aggregation =
                currentAggregationsByCapability.aggregationForFormat<Ld::FontFormat>();

            dialog->setFontFamilies(aggregation.families());
            dialog->setFontSizes(aggregation.sizes());
            dialog->setFontWeights(aggregation.weights());
            dialog->setSupportedFontWeights(supportedFontWeights(aggregation));

            if (aggregation.italics().bothTrueAndFalse()) {
                dialog->setItalicsBothEnabledAndDisabled();
            } else {
                dialog->setItalicsEnabled(aggregation.italics().allTrue());
            }

            if (aggregation.underline().bothTrueAndFalse()) {
                dialog->setUnderlineBothEnabledAndDisabled();
            } else {
                dialog->setUnderlineEnabled(aggregation.underline().allTrue());
            }

            if (aggregation.overline().bothTrueAndFalse()) {
                dialog->setOverlineBothEnabledAndDisabled();
            } else {
                dialog->setOverlineEnabled(aggregation.overline().allTrue());
            }

            if (aggregation.strikeOut().bothTrueAndFalse()) {
                dialog->setStrikethroughBothEnabledAndDisabled();
            } else {
                dialog->setStrikethroughEnabled(aggregation.strikeOut().allTrue());
            }

            dialog->setFontColors(aggregation.fontColor());
            dialog->setFontBackgroundColors(aggregation.fontBackgroundColor());
            dialog->setLetterSpacing(aggregation.letterSpacing());
        }

        if (currentAggregationsByCapability.contains(Ld::CharacterFormat::formatName)) {
            const Ld::CharacterFormat::Aggregation& aggregation =
                currentAggregationsByCapability.aggregationForFormat<Ld::CharacterFormat>();

            dialog->setPositions(aggregation.positions());
        }
    }
}


void CharacterFormatViewProxy::restoreFocus() {
    view()->restoreFocus();
}
