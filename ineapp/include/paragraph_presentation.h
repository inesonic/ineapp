/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ParagraphPresentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef PARAGRAPH_PRESENTATION_H
#define PARAGRAPH_PRESENTATION_H

#include <QObject>
#include <QString>
#include <QFont>
#include <QColor>
#include <QWeakPointer>

#include <ld_format_structures.h>
#include <ld_ordered_list_paragraph_format.h>

#include "app_common.h"
#include "paragraph_presentation_base.h"
#include "scene_units.h"

namespace Ld {
    class Visual;
    class ParagraphFormat;
    class ListParagraphFormatBase;
};

/**
 * This class provides a visual for the Ld::ParagraphElement class with support for placement of one or more child
 * elements.
 */
class APP_PUBLIC_API ParagraphPresentation:public ParagraphPresentationBase {
    Q_OBJECT

    public:
        ParagraphPresentation();

        ~ParagraphPresentation() override;

        /**
         * Static method that creates an instance of a \ref RootPresentation class.
         *
         * \param[in] typeName The name used to reference this creator function.  A single creator function can be
         *                     used to create multiple visual types using this value to identify what type of
         *                     visual to be created.
         */
        static Ld::Visual* creator(const QString& typeName);

        /**
         * Method that returns the type name associated with this visual.
         *
         * \return Returns the typename associated with this visual.  The value must match the type used to invoke
         *         the static Ld::Visual::create and Ld::Visual::registerCreator methods.
         */
        QString typeName() const final;

        /**
         * Method that returns the name of the plug-in that provides this visual.
         *
         * \return Returns the name of the plug-in that provides this visual. The name must match the name provided
         *         in the Ld::InesonicPlugInData structure.  Visuals defined as part of the ineld library should return
         *         an empty string.
         */
        QString plugInName() const final;

    protected slots:
        /**
         * Slot that is triggered when a Ld::Element reports that its format has been changed.  You can use this slot
         * to perform updates to the visual representation of the element.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * The default version of this slot will trigger the presentation to be reposition and redrawn.
         *
         * \param[in] oldFormat A pointer to the old Ld::Format instance.  This value may be a null pointer if the
         *                      Ld::Element did not previously have an associated Ld::Format instance.
         *
         * \param[in] newFormat A pointer to the new Ld::Format instance.  This pointer will be the same as the
         *                      oldFormat pointer if the Ld::Format instance was not changed.
         */
        void processFormatChange(Ld::FormatPointer oldFormat, Ld::FormatPointer newFormat) final;

        /**
         * Slot that is triggered when the element tied to this presentation is newly graphed to a tree.
         */
        void processGraftedToTree() final;

    private:
        /**
         * Method that is called to determine the justification to apply to lines.
         *
         * \return Returns the line justification to apply.
         */
        Justification lineJustification() final;

        /**
         * Method that is called to determine the left margin to apply to all lines except the first line.  Bullets
         * and list numbers will be aligned to this margin.
         *
         * \return Returns the left margin to apply to all lines, except the first line, in scene units.
         */
        float leftMarginSceneUnits() final;

        /**
         * Method that is called to determine the left margin to apply to all lines except the first line.  The value
         * is added to the left margin and list indentation to determine the indentation for the first line.
         *
         * \return Returns the left margin to apply to all lines, except the first line, in scene units.
         */
        float firstLineLeftMarginSceneUnits() final;

        /**
         * Method that is called to determine the right margin to apply to all lines except the first line.
         *
         * \return Returns the right margin to apply to all lines, except the first line, in scene units.
         */
        float rightMarginSceneUnits() final;

        /**
         * Method that is called to determine additional spacing to be placed at the top of the first active area.
         *
         * \return Returns spacing, in scene units, to be applied to the top of the first active area.
         */
        float topSpacingSceneUnits() final;

        /**
         * Method called by the parent to determine the spacing that should be applied between this placement negotiator
         * and the next placement negotiator.  You should also return 0 for elements that are not full width.
         *
         * \return Returns the spacing to apply between this placement negotiator and the next placement negotiator.
         *         The spacing should be the maximum of this value and any top spacing requirement for the next
         *         placement negotiator.
         */
        float bottomSpacingSceneUnits() final;

        /**
         * Method that is called to determine the list indentation.  The value is added to the left margin to determine
         * the left edge of text in the paragraph.  The first line left margin is also added to the left margin and
         * this value to determine the left edge of the text on the first line.
         *
         * \return Returns the list indentation in scene units.
         */
        float listIndentationSceneUnits() final;

        /**
         * Method that is called to determine the bullet or number text to place to the left of the list indentation.
         *
         * This method is non-constant to allow the method to adjust local parameters, if needed.
         *
         * \param[out] font                The font to be used to render the list string.
         *
         * \param[out] fontColor           The color to apply to the rendered text.
         *
         * \param[out] fontBackgroundColor The color to apply to the background of the rendered text.
         *
         * \return Returns the text to present.
         */
        QString listString(
            QFont*  font = Q_NULLPTR,
            QColor* fontColor = Q_NULLPTR,
            QColor* fontBackgroundColor = Q_NULLPTR
        ) final;

        /**
         * Method you can use to obtain the current line spacing.
         *
         * \return Returns the current line spacing.  A value of 1.0 indicates single spacing.  A value of 1.5 indicates
         *         1 1/2 line spacing.  A value of 2.0 indicates double spacing.
         */
        float lineSpacing() final;

        /**
         * Method that is called to update internal format data on insertion or a format change.
         *
         * \param[in] format The format driving this presentation.
         */
        void updateFormatData(Ld::FormatPointer format);

        /**
         * Method you can call to obtain the format data to use for this class.
         *
         * \return Returns the format data to use for this class.
         */
        QSharedPointer<Ld::ParagraphFormat> drivingFormat();

        /**
         * Method that returns the paragraph number text.
         *
         * \param[in] paragraphNumber The paragraph number to be converted.
         *
         * \param[in] prefixText      Prefix text to prepend to the number.
         *
         * \param[in] listStyle       An enumeration indicating the paragraph number format.
         *
         * \param[in] suffixText      Suffix text to append to the number.
         *
         * \return Returns the paragraph number text.
         */
        static QString paragraphNumberText(
            unsigned long                         paragraphNumber,
            const QString&                        prefixText,
            Ld::OrderedListParagraphFormat::Style listStyle,
            const QString&                        suffixText
        );

        /**
         * Method that returns a number converted to a letter sequence.
         *
         * \param[in] paragraphNumber The paragraph number to be converted.
         *
         * \return Returns the paragraph number converted to a string.  Letters are lower case.
         */
        static QString numberToLetters(unsigned long paragraphNumber);

        /**
         * Method that returns a number converted to roman numerals.
         *
         * \param[in] paragraphNumber The paragraph number to be converted.
         *
         * \return Returns the paragraph number converted to a string.  Roman numerals are in lower case.
         */
        static QString numberToRoman(unsigned long paragraphNumber);

        /**
         * Value indicating the current paragraph number for this paragraph.
         */
        unsigned long currentParagraphNumber;

        /**
         * Weak pointer to the ordered list paragraph format.  This parameter is only used when we are continuing an
         * ordered list.
         */
        QWeakPointer<Ld::ParagraphFormat> drivingListParagraphFormat;
};

#endif
