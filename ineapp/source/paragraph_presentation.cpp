/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \filef
*
* This file implements the \ref ParagraphPresentation class.
***********************************************************************************************************************/

#include <QString>
#include <QSharedPointer>
#include <QFont>
#include <QColor>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_paragraph_element.h>
#include <ld_paragraph_format.h>
#include <ld_unordered_list_paragraph_format.h>
#include <ld_ordered_list_paragraph_format.h>
#include <ld_list_additional_paragraph_format.h>

#include "application.h"
#include "paragraph_presentation_base.h"
#include "paragraph_presentation.h"

ParagraphPresentation::ParagraphPresentation() {
    currentParagraphNumber = 0;
}


ParagraphPresentation::~ParagraphPresentation() {}


Ld::Visual* ParagraphPresentation::creator(const QString&) {
    return new ParagraphPresentation();
}


QString ParagraphPresentation::typeName() const {
    return Ld::ParagraphElement::elementName;
}


QString ParagraphPresentation::plugInName() const {
    return QString();
}


void ParagraphPresentation::processFormatChange(Ld::FormatPointer oldFormat, Ld::FormatPointer newFormat) {
    updateFormatData(newFormat);
    ParagraphPresentationBase::processFormatChange(oldFormat, newFormat);
}


void ParagraphPresentation::processGraftedToTree() {
    updateFormatData(element()->format());
    ParagraphPresentationBase::processGraftedToTree();
}


ParagraphPresentation::Justification ParagraphPresentation::lineJustification() {
    Justification result;

    QSharedPointer<Ld::ParagraphFormat> format = drivingFormat();
    if (!format.isNull()) {
        result = format->justificationMode();
    } else {
        result = Justification::LEFT;
    }

    return result;
}


float ParagraphPresentation::leftMarginSceneUnits() {
    float result;

    QSharedPointer<Ld::ParagraphFormat> format = drivingFormat();
    if (!format.isNull()) {
        result = format->leftIndentation() * sceneUnitsPerPoint().width();
    } else {
        result = 0;
    }

    return result;
}


float ParagraphPresentation::firstLineLeftMarginSceneUnits() {
    float result;

    QSharedPointer<Ld::ParagraphFormat> format = drivingFormat();
    if (!format.isNull()) {
        result = format->firstLineLeftIndentation() * sceneUnitsPerPoint().width();
    } else {
        result = 0.0F;
    }

    return result;
}


float ParagraphPresentation::rightMarginSceneUnits() {
    float result;

    QSharedPointer<Ld::ParagraphFormat> format = drivingFormat();
    if (!format.isNull()) {
        result = format->rightIndentation() * sceneUnitsPerPoint().width();
    } else {
        result = 0.0F;
    }

    return result;
}


float ParagraphPresentation::topSpacingSceneUnits() {
    float result;

    QSharedPointer<Ld::ParagraphFormat> format = drivingFormat();
    if (!format.isNull()) {
        result = format->topSpacing() * sceneUnitsPerPoint().height();
    } else {
        result = 0.0F;
    }

    return result;
}


float ParagraphPresentation::bottomSpacingSceneUnits() {
    float result;

    QSharedPointer<Ld::ParagraphFormat> format = drivingFormat();
    if (!format.isNull()) {
        result = format->bottomSpacing() * sceneUnitsPerPoint().height();
    } else {
        result = 0.0F;
    }

    return result;
}


float ParagraphPresentation::listIndentationSceneUnits() {
    float result;

    QSharedPointer<Ld::ParagraphFormat> format = drivingFormat();
    if (!format.isNull()) {
        QString paragraphFormatName = format->typeName();
        if (paragraphFormatName == Ld::ParagraphFormat::formatName) {
            result = 0;
        } else {
            QSharedPointer<Ld::ListParagraphFormatBase> listFormat = format.dynamicCast<Ld::ListParagraphFormatBase>();
            if (!listFormat.isNull()) {
                result = toScene(QPointF(listFormat->listIndentation(), 0)).x();
            } else {
                result = 0;
            }
        }
    } else {
        result = 0.0F;
    }

    return result;
}


QString ParagraphPresentation::listString(QFont* font, QColor* fontColor, QColor* fontBackgroundColor) {
    QString result;

    QSharedPointer<Ld::ParagraphFormat> format = drivingFormat();
    Q_ASSERT(!format.isNull()); // Debug
    if (!format.isNull()) {
        QString paragraphFormatName = format->typeName();
        if (paragraphFormatName == Ld::ParagraphFormat::formatName) {
            result = QString();
        } else {
            bool displayString;

            Ld::FormatPointer thisFormat    = element()->format();
            if (!thisFormat.isNull() && thisFormat->typeName() == Ld::ListAdditionalParagraphFormat::formatName) {
                QSharedPointer<Ld::ListAdditionalParagraphFormat> additionalListFormat =
                    thisFormat.dynamicCast<Ld::ListAdditionalParagraphFormat>();
                Q_ASSERT(!additionalListFormat.isNull());

                displayString = additionalListFormat->bulletNumberDisplayEnabled();
            } else {
                displayString = true;
            }

            if (displayString) {
                QSharedPointer<Ld::ListParagraphFormatBase> listFormat =
                    drivingListParagraphFormat.toStrongRef().dynamicCast<Ld::ListParagraphFormatBase>();

                if (!listFormat.isNull()) {
                    if (font != Q_NULLPTR) {
                        *font = listFormat->toQFont();
                    }

                    if (fontColor != Q_NULLPTR) {
                        *fontColor = listFormat->fontColor();
                    }

                    if (fontBackgroundColor != Q_NULLPTR) {
                        *fontBackgroundColor = listFormat->fontBackgroundColor();
                    }

                    paragraphFormatName = listFormat->typeName();
                    if (paragraphFormatName == Ld::UnorderedListParagraphFormat::formatName) {
                        QSharedPointer<Ld::UnorderedListParagraphFormat> unorderedListlistParagraphFormat =
                            listFormat.dynamicCast<Ld::UnorderedListParagraphFormat>();
                        Q_ASSERT(!unorderedListlistParagraphFormat.isNull());

                        result = unorderedListlistParagraphFormat->bulletStyle();
                    } else {
                        Q_ASSERT(paragraphFormatName == Ld::OrderedListParagraphFormat::formatName);

                        QSharedPointer<Ld::OrderedListParagraphFormat> orderedListParagraphFormat =
                            listFormat.dynamicCast<Ld::OrderedListParagraphFormat>();
                        Q_ASSERT(!orderedListParagraphFormat.isNull());

                        result = paragraphNumberText(
                            currentParagraphNumber,
                            orderedListParagraphFormat->prefix(),
                            orderedListParagraphFormat->style(),
                            orderedListParagraphFormat->suffix()
                        );
                    }
                }
            }
        }
    }

    return result;
}


float ParagraphPresentation::lineSpacing() {
    float result;

    QSharedPointer<Ld::ParagraphFormat> format = drivingFormat();
    if (!format.isNull()) {
        result = format->lineSpacing();
    } else {
        result = 1.0F;
    }

    return result;
}


void ParagraphPresentation::updateFormatData(Ld::FormatPointer format) {
    if (graftedToRoot() && !element()->format().isNull()) {
        QString formatTypeName = format->typeName();
        if (formatTypeName == Ld::ParagraphFormat::formatName) {
            drivingListParagraphFormat = format.dynamicCast<Ld::ParagraphFormat>().toWeakRef();
            currentParagraphNumber     = static_cast<unsigned long>(-1);
        } else {
            unsigned           paragraphNumber;
            Ld::ElementPointer sibling;

            if (formatTypeName == Ld::OrderedListParagraphFormat::formatName) {
                QSharedPointer<Ld::OrderedListParagraphFormat> drivingFormat =
                    format.dynamicCast<Ld::OrderedListParagraphFormat>();

                drivingListParagraphFormat = drivingFormat.toWeakRef();
                currentParagraphNumber     = drivingFormat->startingNumber();

                paragraphNumber = currentParagraphNumber + 1;
                sibling         = element()->nextSibling();
            } else if (formatTypeName == Ld::UnorderedListParagraphFormat::formatName) {
                QSharedPointer<Ld::UnorderedListParagraphFormat> drivingFormat =
                    format.dynamicCast<Ld::UnorderedListParagraphFormat>();

                drivingListParagraphFormat = drivingFormat.toWeakRef();
                currentParagraphNumber     = static_cast<unsigned long>(-1);

                paragraphNumber = static_cast<unsigned>(-1);
                sibling         = element()->nextSibling();
            } else {
                Q_ASSERT(formatTypeName == Ld::ListAdditionalParagraphFormat::formatName);

                Ld::ElementPointer previousElement = element()->previousSibling();
                if (!previousElement.isNull() && previousElement->typeName() == Ld::ParagraphElement::elementName) {
                    ParagraphPresentation* previousPresentation =
                        dynamic_cast<ParagraphPresentation*>(previousElement->visual());
                    Q_ASSERT(previousPresentation != Q_NULLPTR);

                    drivingListParagraphFormat = previousPresentation->drivingListParagraphFormat;
                    paragraphNumber            = previousPresentation->currentParagraphNumber;

                    if (paragraphNumber != static_cast<unsigned>(-1)) {
                        QSharedPointer<Ld::ListAdditionalParagraphFormat> thisFormat =
                            format.dynamicCast<Ld::ListAdditionalParagraphFormat>();
                        Q_ASSERT(!thisFormat.isNull());

                        if (thisFormat->bulletNumberDisplayEnabled()) {
                            currentParagraphNumber = paragraphNumber + 1;
                            paragraphNumber += 2;
                        } else {
                            ++paragraphNumber;
                        }
                    } else {
                        currentParagraphNumber = static_cast<unsigned long>(-1);
                    }

                    sibling = element()->nextSibling();
                } else {
                    drivingListParagraphFormat.clear();
                    currentParagraphNumber = static_cast<unsigned long>(-1);

                    paragraphNumber = static_cast<unsigned>(-1);
                    sibling.clear();
                }
            }

            while (!sibling.isNull()                                                              &&
                   sibling->typeName() == Ld::ParagraphElement::elementName                       &&
                   !sibling->format().isNull()                                                    &&
                   sibling->format()->typeName() == Ld::ListAdditionalParagraphFormat::formatName &&
                   sibling->visual() != Q_NULLPTR                                                    ) {
                ParagraphPresentation* siblingPresentation = dynamic_cast<ParagraphPresentation*>(sibling->visual());
                Q_ASSERT(siblingPresentation != Q_NULLPTR);

                siblingPresentation->drivingListParagraphFormat = drivingListParagraphFormat;

                QSharedPointer<Ld::ListAdditionalParagraphFormat> siblingFormat =
                    sibling->format().dynamicCast<Ld::ListAdditionalParagraphFormat>();
                Q_ASSERT(!siblingFormat.isNull());

                if (siblingFormat->bulletNumberDisplayEnabled()) {
                    siblingPresentation->currentParagraphNumber = paragraphNumber;
                    ++paragraphNumber;
                } else {
                    siblingPresentation->currentParagraphNumber = static_cast<unsigned long>(-1);
                }

                sibling = sibling->nextSibling();
            }
        }
    } else {
        drivingListParagraphFormat.clear();
        currentParagraphNumber = static_cast<unsigned long>(-1);
    }
}


QSharedPointer<Ld::ParagraphFormat> ParagraphPresentation::drivingFormat() {
    if (drivingListParagraphFormat.isNull()) {
        Ld::ElementPointer element = ParagraphPresentation::element();
        if (!element.isNull()) {
            updateFormatData(element->format());
        }
    }

    QSharedPointer<Ld::ParagraphFormat> result = drivingListParagraphFormat.toStrongRef();
    return result;
}


QString ParagraphPresentation::paragraphNumberText(
        unsigned long                         paragraphNumber,
        const QString&                        prefixText,
        Ld::OrderedListParagraphFormat::Style listStyle,
        const QString&                        suffixText
    ) {
    QString numberString;

    switch (listStyle) {
        case Ld::OrderedListParagraphFormat::Style::NUMBER: {
            numberString = QString::number(paragraphNumber);
            break;
        }

        case Ld::OrderedListParagraphFormat::Style::LOWER_CASE: {
            numberString = numberToLetters(paragraphNumber);
            break;
        }

        case Ld::OrderedListParagraphFormat::Style::UPPER_CASE: {
            numberString = numberToLetters(paragraphNumber).toUpper();
            break;
        }

        case Ld::OrderedListParagraphFormat::Style::ROMAN_LOWER_CASE: {
            numberString = numberToRoman(paragraphNumber);
            break;
        }

        case Ld::OrderedListParagraphFormat::Style::ROMAN_UPPER_CASE: {
            numberString = numberToRoman(paragraphNumber);
            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    return prefixText + numberString + suffixText;
}


QString ParagraphPresentation::numberToLetters(unsigned long paragraphNumber) {
    QString result;
    if (paragraphNumber == 0) {
        result = tr("0");
    } else {
        unsigned long i = paragraphNumber;

        unsigned short unicodeBase = tr("a").at(0).unicode();

        bool     ok;
        unsigned modulus = tr("26").toUInt(&ok);
        Q_ASSERT(ok);

        if (i == 0) {
            result = tr("0");
        } else {
            --i;
            do {
                unsigned characterOffset = i % modulus;
                result.prepend(QChar(unicodeBase + characterOffset));

                i /= modulus;
            } while (i != 0);
        }
    }

    return result;
}


QString ParagraphPresentation::numberToRoman(unsigned long paragraphNumber) {
    QString result;

    if (paragraphNumber == 0) {
        result = tr("nulla");
    } else {
        const QString digits = tr("ivxlcdm??");

        unsigned residue    = paragraphNumber <= 3999 ? paragraphNumber : 3999;
        unsigned powerOf10  = 1000;
        unsigned digitIndex = 6;

        do {
            unsigned digit = residue / powerOf10;
            switch (digit) {
                case 0: {
                    break;
                }

                case 1: {
                    result += digits[digitIndex];
                    break;
                }

                case 2: {
                    result += digits[digitIndex];
                    result += digits[digitIndex];

                    break;
                }

                case 3: {
                    result += digits[digitIndex];
                    result += digits[digitIndex];
                    result += digits[digitIndex];

                    break;
                }

                case 4: {
                    result += digits[digitIndex];
                    result += digits[digitIndex + 1];

                    break;
                }

                case 5: {
                    result += digits[digitIndex + 1];
                    break;
                }

                case 6: {
                    result += digits[digitIndex + 1];
                    result += digits[digitIndex];

                    break;
                }

                case 7: {
                    result += digits[digitIndex + 1];
                    result += digits[digitIndex];
                    result += digits[digitIndex];

                    break;
                }

                case 8: {
                    result += digits[digitIndex + 1];
                    result += digits[digitIndex];
                    result += digits[digitIndex];
                    result += digits[digitIndex];

                    break;
                }

                case 9: {
                    result += digits[digitIndex];
                    result += digits[digitIndex+2];

                    break;
                }

                default: {
                    Q_ASSERT(false);
                    break;
                }
            }

            residue    -= digit * powerOf10;
            powerOf10  /= 10;
            digitIndex -= 2;
        } while (powerOf10 != 0);
    }

    return result;
}


