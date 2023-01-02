/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref Clipboard class.
***********************************************************************************************************************/

#include <QObject>
#include <QClipboard>
#include <QImage>
#include <QTransform>
#include <QMimeData>
#include <QByteArray>
#include <QBuffer>
#include <QStringList>
#include <QRegularExpression>

#include <ld_element.h>
#include <ld_root_element.h>
#include <ld_code_generator.h>
#include <ld_html_code_generator.h>
#include <ld_xml_temporary_file_export_context.h>
#include <ld_latex_code_generator.h>
#include <ld_text_export_context.h>
#include <ld_text_element.h>
#include <ld_paragraph_element.h>
#include <ld_character_format.h>
#include <ld_paragraph_format.h>
#include <ld_image_element.h>
#include <ld_format_structures.h>
#include <ld_image_format.h>

#include "application.h"
#include "selection_data.h"
#include "clipboard_mime_data.h"
#include "clipboard.h"

const Ld::HtmlCodeGenerator::HtmlStyle Clipboard::htmlStyle = Ld::HtmlCodeGenerator::HtmlStyle::HTML4_WITHOUT_CSS;
const Ld::HtmlCodeGenerator::MathMode  Clipboard::mathMode  = Ld::HtmlCodeGenerator::MathMode::IMAGES;
const bool                             Clipboard::ignoreMissingTranslators = true;
const char                             Clipboard::defaultImageFormat[] = "png";
const QRegularExpression               Clipboard::nonPrintableRegularExpression("[\\x00-\\x1F]+");
const float                            Clipboard::defaultImageDpi = 300.0F;

Clipboard::Clipboard(QObject* parent):QObject(parent) {
    QClipboard* systemClipboard = QGuiApplication::clipboard();
    connect(systemClipboard, &QClipboard::changed, this, &Clipboard::systemClipboardChanged);

    if (systemClipboard->mimeData() != Q_NULLPTR) {
        currentUpdateFromExternalNeeded = true;
    } else {
        currentUpdateFromExternalNeeded = false;
    }
}


Clipboard::~Clipboard() {}


bool Clipboard::containsData() {
    bool dataPresent = false;

    if (!currentInternalSelection.isNull() && !currentInternalSelection->isEmpty()) {
        dataPresent = true;
    } else {
        QClipboard* systemClipboard = QGuiApplication::clipboard();
        if (systemClipboard->mimeData() != Q_NULLPTR) {
            currentInternalSelection = content();
            dataPresent = (currentInternalSelection->numberChildren() > 0);
        }
    }

    return dataPresent;
}


bool Clipboard::isEmpty() {
    return !containsData();
}


void Clipboard::addContent(SelectionDataPointer newSelectionData) {
    currentInternalSelection = newSelectionData;
    updateSystemClipboard(newSelectionData);

    currentUpdateFromExternalNeeded = false;

    emit clipboardUpdated();
    emit clipboardUpdated(false);
}


SelectionDataPointer Clipboard::content() {
    if (currentUpdateFromExternalNeeded) {
        currentInternalSelection        = generateSelectionData();
        currentUpdateFromExternalNeeded = false;
    }

    return currentInternalSelection;
}


void Clipboard::systemClipboardChanged(QClipboard::Mode mode) {
    QClipboard* systemClipboard = QGuiApplication::clipboard();
    if (!systemClipboard->ownsClipboard()) {
        if (mode == QClipboard::Mode::Clipboard) {
            currentUpdateFromExternalNeeded = true;

            emit clipboardUpdated();
            emit clipboardUpdated(true);
        } else if (mode == QClipboard::Mode::FindBuffer) {
            QString text = systemClipboard->text(QClipboard::Mode::FindBuffer);
            emit findBufferUpdated(text);
        }
    }
}


void Clipboard::updateSystemClipboard(SelectionDataPointer newSelectionData) {
    ClipboardMimeData* mimeData = new ClipboardMimeData;

    bool success = true;
    if (newSelectionData->numberChildren() == 1) {
        Ld::ElementPointer element = newSelectionData->child(0);
        Q_ASSERT(!element.isNull());

        if (element->exportImageCapability() != Ld::Element::ExportImageCapability::NONE) {
            success = addElementAsImage(element, mimeData);
        }
    }

    if (success) {
        QSharedPointer<Ld::HtmlCodeGenerator>
            codeGenerator = Ld::CodeGenerator::codeGenerator(Ld::HtmlCodeGenerator::codeGeneratorName)
                            .dynamicCast<Ld::HtmlCodeGenerator>();

        Q_ASSERT(!codeGenerator.isNull());
        codeGenerator->setMathMode(mathMode);
        codeGenerator->setHtmlStyle(htmlStyle);
        codeGenerator->setImageHandlingMode(Ld::HtmlCodeGenerator::ImageHandlingMode::EMBEDDED);
        codeGenerator->setProcessImports(false);
        codeGenerator->setIgnoreMissingPerElementTranslators(ignoreMissingTranslators);

        success = codeGenerator->translate(
            newSelectionData,
            "index.html",
            Ld::CodeGeneratorOutputType::ExportMode::EXPORT_AS_MIXED_TEMPORARY_OBJECT
        );

        if (success) {
            codeGenerator->waitComplete();

            if (codeGenerator->reportedDiagnostics().isEmpty()) {
                mimeData->setExportContext(codeGenerator->context().dynamicCast<Ld::XmlTemporaryFileExportContext>());
            } else {
                success = false;
            }
        }
    }

    if (success) {
        QSharedPointer<Ld::LaTeXCodeGenerator>
            codeGenerator = Ld::CodeGenerator::codeGenerator(Ld::LaTeXCodeGenerator::codeGeneratorName)
                            .dynamicCast<Ld::LaTeXCodeGenerator>();

        Q_ASSERT(!codeGenerator.isNull());
        codeGenerator->setImageMode(Ld::LaTeXCodeGenerator::ImageMode::NO_IMAGES);
        codeGenerator->setSingleFile(false);
        codeGenerator->setCopyrightExcluded();
        codeGenerator->setUnicodeTranslationMode(Ld::LaTeXCodeGenerator::UnicodeMode::INSERT_UNICODE);
        codeGenerator->setProcessNoImports();
        codeGenerator->setIgnoreMissingPerElementTranslators(ignoreMissingTranslators);

        success = codeGenerator->translate(
            newSelectionData,
            Ld::LaTeXCodeGenerator::latexTopFilename,
            Ld::CodeGeneratorOutputType::ExportMode::EXPORT_IN_MEMORY
        );

        if (success) {
            codeGenerator->waitComplete();

            if (codeGenerator->reportedDiagnostics().isEmpty()) {
                QByteArray payload = codeGenerator->context()->payload(Ld::LaTeXCodeGenerator::latexTopFilename);
                mimeData->setText(QString(payload));
            } else {
                success = false;
            }
        }
    }

    if (success) {
        QClipboard* systemClipboard = QGuiApplication::clipboard();
        systemClipboard->setMimeData(mimeData);
    } else {
        delete mimeData;
        Application::beep();
    }
}


SelectionDataPointer Clipboard::generateSelectionData() {
    QClipboard*      systemClipboard = QGuiApplication::clipboard();
    const QMimeData* mimeData        = systemClipboard->mimeData();

    SelectionDataPointer newSelectionData = convertMimeHtml(mimeData);
    if (newSelectionData.isNull()) {
        newSelectionData = convertMimeImage(mimeData);
    }

    if (newSelectionData.isNull()) {
        newSelectionData = convertMimeText(mimeData);
    }

    return newSelectionData;
}


SelectionDataPointer Clipboard::convertMimeImage(const QMimeData* mimeData) {
    SelectionDataPointer result(new SelectionData(SelectionData::SelectionType::TEXT_ANNOTATION));
    result->setWeakThis(result.toWeakRef());

    if (mimeData->hasImage()) {
        QVariant imageVariant = mimeData->imageData();
        if (!imageVariant.isNull() && imageVariant.canConvert(QMetaType::QImage)) {
            QImage     image = imageVariant.value<QImage>();
            QByteArray imagePayload;
            QBuffer    imageBuffer(&imagePayload);

            bool success = image.save(&imageBuffer, defaultImageFormat);
            if (success) {
                QSharedPointer<Ld::ImageElement> imageElement = Ld::Element::create(Ld::ImageElement::elementName)
                                                                .dynamicCast<Ld::ImageElement>();

                QSharedPointer<Ld::ImageFormat>  imageFormat  = Ld::Format::create(Ld::ImageFormat::formatName)
                                                                .dynamicCast<Ld::ImageFormat>();

                imageFormat->setRotation(Ld::ImageFormat::Rotation::NO_ROTATION);
                imageFormat->setHorizontalAxis(Ld::ImageFormat::Axis(1.0, Ld::ImageFormat::ImageScalingMode::IGNORED));
                imageFormat->setVerticalAxis(Ld::ImageFormat::Axis(1.0, Ld::ImageFormat::ImageScalingMode::IGNORED));

                imageElement->setFormat(imageFormat);
                imageElement->updatePayload(imagePayload);

                result.reset(new SelectionData);
                result->setWeakThis(result.toWeakRef());

                QSharedPointer<Ld::TextElement> leadingText = Ld::Element::create(Ld::TextElement::elementName)
                                                              .dynamicCast<Ld::TextElement>();

                QSharedPointer<Ld::CharacterFormat> leadingFormat = Ld::Format::create(Ld::CharacterFormat::formatName)
                                                                    .dynamicCast<Ld::CharacterFormat>();

                leadingText->setFormat(leadingFormat);

                result->append(leadingText, Q_NULLPTR);
                result->append(imageElement, Q_NULLPTR);
                result->append(leadingText->clone(false), Q_NULLPTR);
            }
        }
    }

    return result;
}


SelectionDataPointer Clipboard::convertMimeHtml(const QMimeData* /* mimeData */) {
    return SelectionDataPointer();
}


SelectionDataPointer Clipboard::convertMimeText(const QMimeData* mimeData) {
    SelectionDataPointer result;

    if (mimeData->hasText()) {
        QString     receivedText = mimeData->text();
        QStringList paragraphs   = receivedText.split(QChar('\n'), Qt::SplitBehaviorFlags::SkipEmptyParts);

        while (!paragraphs.isEmpty() && paragraphs.first().isEmpty()) {
            paragraphs.removeFirst();
        }

        while (!paragraphs.isEmpty() && paragraphs.last().isEmpty()) {
            paragraphs.removeLast();
        }

        unsigned numberSections = static_cast<unsigned>(paragraphs.size());

        if (numberSections > 0) {
            result.reset(new SelectionData);
            result->setWeakThis(result.toWeakRef());

            QString sectionText = scrubText(paragraphs.first());

            QSharedPointer<Ld::TextElement> element = Ld::Element::create(Ld::TextElement::elementName)
                                                      .dynamicCast<Ld::TextElement>();

            QSharedPointer<Ld::CharacterFormat> format = Ld::Format::create(Ld::CharacterFormat::formatName)
                                                         .dynamicCast<Ld::CharacterFormat>();

            element->setFormat(format);
            element->setText(sectionText);

            result->append(element, Q_NULLPTR);

            for (unsigned sectionIndex=1 ; sectionIndex<numberSections ; ++sectionIndex) {
                  QString sectionText = scrubText(paragraphs.at(sectionIndex));

                  QSharedPointer<Ld::ParagraphElement> paragraphElement =
                      Ld::Element::create(Ld::ParagraphElement::elementName).dynamicCast<Ld::ParagraphElement>();

                  QSharedPointer<Ld::ParagraphFormat>  paragraphFormat =
                      Ld::Format::create(Ld::ParagraphFormat::formatName).dynamicCast<Ld::ParagraphFormat>();

                  paragraphElement->setFormat(paragraphFormat);

                  QSharedPointer<Ld::TextElement> textElement =
                      Ld::Element::create(Ld::TextElement::elementName).dynamicCast<Ld::TextElement>();

                  QSharedPointer<Ld::CharacterFormat> characterFormat =
                      Ld::Format::create(Ld::CharacterFormat::formatName).dynamicCast<Ld::CharacterFormat>();

                  textElement->setFormat(characterFormat);
                  textElement->setText(sectionText);

                  paragraphElement->append(textElement, nullptr);

                  result->append(paragraphElement, Q_NULLPTR);
              }
      }
  }

    return result;
}


QString Clipboard::scrubText(const QString& rawText) {
    return QString(rawText).replace(nonPrintableRegularExpression, "");
}


bool Clipboard::addImageElement(Ld::ElementPointer imageElement, ClipboardMimeData* mimeData) {
    QByteArray payload;
    bool success = imageElement.dynamicCast<Ld::ImageElement>()->getPayload(payload);

    if (success) {
        QImage image;
        success = image.loadFromData(payload);

        if (success) {
            Ld::FormatPointer imageFormat = imageElement->format();
            if (!imageFormat.isNull()) {
                QTransform transformationMatrix;
                transformationMatrix.rotate(
                    -90.0 * static_cast<unsigned>(imageFormat.dynamicCast<Ld::ImageFormat>()->rotation())
                );

                image = image.transformed(
                    transformationMatrix,
                    Qt::TransformationMode::SmoothTransformation
                );
            }

            mimeData->setImageData(image);
        }
    }

    return success;
}


bool Clipboard::addElementAsImage(Ld::ElementPointer element, ClipboardMimeData* mimeData) {
    bool                            success;
    QSharedPointer<Ld::RootElement> root      = element->root().dynamicCast<Ld::RootElement>();
    QByteArray                      imageData = root->exportElementImage(element, defaultImageDpi);

    if (!imageData.isEmpty()) {
        QImage image;
        success = image.loadFromData(imageData, "PNG");
        if (success) {
            mimeData->setImageData(image);
        }
    } else {
        success = false;
    }

    return success;
}
