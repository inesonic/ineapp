/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*
* This file implements the \ref FunctionBrowserDelegate class.
***********************************************************************************************************************/

#include <QApplication>
#include <QObject>
#include <QString>
#include <QStyle>
#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QStyledItemDelegate>

#include <algorithm>

#include <util_string.h>

#include "application.h"
#include "presentation.h"
#include "function_browser_delegate.h"

/***********************************************************************************************************************
 * FunctionBrowserDelegate::Engine
 */

FunctionBrowserDelegate::Engine::Engine() {
    currentSubscript   = false;
    currentSuperscript = false;
    currentItalics     = false;
    currentBoldface    = false;
}


FunctionBrowserDelegate::Engine::~Engine() {}


void FunctionBrowserDelegate::Engine::startSubscript() {
    currentSubscript = true;
}


void FunctionBrowserDelegate::Engine::endSubscript() {
    currentSubscript = false;
}


void FunctionBrowserDelegate::Engine::startSuperscript() {
    currentSuperscript = true;
}


void FunctionBrowserDelegate::Engine::endSuperscript() {
    currentSuperscript = false;
}


void FunctionBrowserDelegate::Engine::startItalics() {
    currentItalics = true;
}


void FunctionBrowserDelegate::Engine::endItalics() {
    currentItalics = false;
}


void FunctionBrowserDelegate::Engine::startBold() {
    currentBoldface = true;
}


void FunctionBrowserDelegate::Engine::endBold() {
    currentBoldface = false;
}


bool FunctionBrowserDelegate::Engine::isSubscript() const {
    return currentSubscript;
}


bool FunctionBrowserDelegate::Engine::isSuperscript() const {
    return currentSuperscript;
}


bool FunctionBrowserDelegate::Engine::isItalics() const {
    return currentItalics;
}


bool FunctionBrowserDelegate::Engine::isBoldface() const {
    return currentBoldface;
}


QFont FunctionBrowserDelegate::Engine::adjustedFont(const QFont& normalFont) const {
    QFont updatedFont = normalFont;

    if (currentSubscript) {
        updatedFont.setPointSizeF(normalFont.pointSizeF() * Presentation::subscriptSizeAdjustment);

        unsigned weight       = static_cast<unsigned>(normalFont.weight());
        int      weightOffset = static_cast<int>(
              (static_cast<int>(QFont::Black) - weight)
            * Presentation::subscriptWeightAdjustment
        );

        updatedFont.setWeight(static_cast<QFont::Weight>(weight + weightOffset));
    } else if (currentSuperscript) {
        updatedFont.setPointSizeF(normalFont.pointSizeF() * Presentation::superscriptSizeAdjustment);

        unsigned weight       = static_cast<unsigned>(normalFont.weight());
        int      weightOffset = static_cast<int>(
              (static_cast<int>(QFont::Black) - weight)
            * Presentation::superscriptWeightAdjustment
        );

        updatedFont.setWeight(static_cast<QFont::Weight>(weight + weightOffset));
    } else {
        updatedFont.setBold(currentBoldface);
    }

    updatedFont.setItalic(currentItalics);

    return updatedFont;
}

/***********************************************************************************************************************
 * FunctionBrowserDelegate::SizingEngine
 */

FunctionBrowserDelegate::SizingEngine::SizingEngine(const QFont& font):currentFont(font) {
    currentTop    = 0;
    currentBottom = 0;
    currentWidth  = 0;
}


FunctionBrowserDelegate::SizingEngine::~SizingEngine() {}


void FunctionBrowserDelegate::SizingEngine::processText(const QString& text) {
    QFontMetrics fontMetrics(adjustedFont(currentFont));

    if (isSubscript()) {
        QFontMetrics normalFontMetrics(currentFont);
        int positionAdjustment = normalFontMetrics.height() * Presentation::subscriptBaseline;
        int baseline           = normalFontMetrics.ascent() - positionAdjustment;

        int fontTop    = baseline - fontMetrics.ascent();
        int fontBottom = baseline + fontMetrics.descent();

        if (fontTop < currentTop) {
            fontTop = currentTop;
        }

        if (fontBottom > currentBottom) {
            fontBottom = currentBottom;
        }
    } else if (isSuperscript()) {
        QFontMetrics normalFontMetrics(currentFont);
        int positionAdjustment = normalFontMetrics.height() * Presentation::superscriptBaseline;
        int baseline           = normalFontMetrics.ascent() - positionAdjustment;

        int fontTop    = baseline - fontMetrics.ascent();
        int fontBottom = baseline + fontMetrics.descent();

        if (fontTop < currentTop) {
            fontTop = currentTop;
        }

        if (fontBottom > currentBottom) {
            fontBottom = currentBottom;
        }
    } else {
        unsigned height = static_cast<unsigned>(fontMetrics.height());
        if (static_cast<int>(height) > currentBottom) {
            currentBottom = height;
        }
    }

    currentWidth += fontMetrics.horizontalAdvance(text);
}


QSize FunctionBrowserDelegate::SizingEngine::sizeHint() const {
    return QSize(currentWidth, currentBottom - std::min(0, currentTop));
}

/***********************************************************************************************************************
 * FunctionBrowserDelegate::TextPaintingEngine
 */

FunctionBrowserDelegate::TextPaintingEngine::TextPaintingEngine(
        QPainter*    painter,
        const QRect& rectangle,
        const QFont& font
    ):currentPainter(
        painter
    ),currentBoundingRectangle(
        rectangle
    ),currentFont(
        font
    ) {}


FunctionBrowserDelegate::TextPaintingEngine::~TextPaintingEngine() {}


void FunctionBrowserDelegate::TextPaintingEngine::processText(const QString& text) {
    QFont        font = adjustedFont(currentFont);
    QFontMetrics fontMetrics(font);

    int baseline;
    if (isSubscript()) {
        QFontMetrics normalFontMetrics(currentFont);
        int positionAdjustment = normalFontMetrics.height() * Presentation::subscriptBaseline;
        baseline               = normalFontMetrics.ascent() - positionAdjustment;
    } else if (isSuperscript()) {
        QFontMetrics normalFontMetrics(currentFont);
        int positionAdjustment = normalFontMetrics.height() * Presentation::superscriptBaseline;
        baseline               = normalFontMetrics.ascent() - positionAdjustment;
    } else {
        baseline = fontMetrics.ascent();
    }

    unsigned textWidth = fontMetrics.horizontalAdvance(text);

    currentPainter->setFont(font);
    currentPainter->drawText(currentBoundingRectangle.left(), currentBoundingRectangle.top() + baseline, text);

    int newLeftEdge = currentBoundingRectangle.left() + textWidth;
    if (newLeftEdge > currentBoundingRectangle.right()) {
        newLeftEdge = currentBoundingRectangle.right();
    }

    currentBoundingRectangle.setLeft(newLeftEdge);
}

/***********************************************************************************************************************
 * FunctionBrowserDelegate
 */

FunctionBrowserDelegate::FunctionBrowserDelegate(QObject* parent):QStyledItemDelegate(parent) {}


FunctionBrowserDelegate::~FunctionBrowserDelegate() {}


void FunctionBrowserDelegate::paint(
        QPainter*                   painter,
        const QStyleOptionViewItem& option,
        const QModelIndex&          index
    ) const {
    QStyleOptionViewItem o = option;
    initStyleOption(&o, index);

    QString text = o.text;

    o.text.clear();
    QStyle* style = o.widget != Q_NULLPTR ? o.widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &o, painter, o.widget);

    QPalette::ColorGroup colorGroup = o.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
    if (colorGroup == QPalette::Normal && (o.state & QStyle::State_Active) == 0) {
        colorGroup = QPalette::Inactive;
    }
    painter->setPen(
        o.palette.color(
            colorGroup,
              (o.state & QStyle::State_Selected) != 0
            ? QPalette::HighlightedText
            : QPalette::Text
        )
    );

    TextPaintingEngine engine(painter, o.rect, o.font);
    parse(text, engine);
}


QSize FunctionBrowserDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    QStyleOptionViewItem o = option;
    initStyleOption(&o, index);

    SizingEngine engine(o.font);
    parse(o.text, engine);

    return engine.sizeHint();
}


void FunctionBrowserDelegate::parse(const QString& str, Engine& engine) const {
    unsigned stringLength = static_cast<unsigned>(str.length());
    unsigned i            = 0;
    int      nextTag      = str.indexOf(QChar('<'));
    while (nextTag >= 0 && i<stringLength) {
        if (static_cast<unsigned>(nextTag) > i) {
            QString subString = str.mid(i, nextTag - i);
            engine.processText(subString);
            i = nextTag;
        }

        bool processed = false;
        if ((i + 5) < stringLength) {
            QString reference = str.mid(i,6);
            if (reference == "</sub>") {
                engine.endSubscript();
                processed = true;
                i += 6;
            } else if (reference == "</sup>") {
                engine.endSuperscript();
                processed = true;
                i += 6;
            }
        }

        if (!processed && (i+4) < stringLength) {
            QString reference = str.mid(i,5);
            if (reference == "<sub>") {
                engine.startSubscript();
                processed = true;
                i += 5;
            } else if (reference == "<sup>") {
                engine.startSuperscript();
                processed = true;
                i += 5;
            }
        }

        if (!processed && (i+3) < stringLength) {
            QString reference = str.mid(i,4);
            if (reference == "</i>") {
                engine.endItalics();
                processed = true;
                i += 4;
            } else if (reference == "</b>") {
                engine.endBold();
                processed = true;
                i += 4;
            }
        }

        if (!processed && (i+2) < stringLength) {
            QString reference = str.mid(i,3);
            if (reference == "<i>") {
                engine.startItalics();
                processed = true;
                i += 3;
            } else if (reference == "<b>") {
                engine.startBold();
                processed = true;
                i += 3;
            }
        }

        if (!processed) {
            nextTag = str.indexOf(QChar('<'), i + 1);
        } else {
            nextTag = str.indexOf(QChar('<'), i);
        }
    }

    if (i < stringLength) {
        QString subString = str.mid(i);
        engine.processText(subString);
    }
}
