/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref FullListFormatUpdater class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef FULL_LIST_FORMAT_UPDATER_H
#define FULL_LIST_FORMAT_UPDATER_H

#include <QStringList>
#include <QColor>

#include <ld_element_structures.h>
#include <ld_aggregations_by_capability.h>
#include <ld_font_format.h>
#include <ld_ordered_list_paragraph_format.h>

#include "update_format_command.h"
#include "list_format_updater.h"

namespace Ld {
    class Element;
    class ListParagraphFormatBase;
    class UnorderedListParagraphFormat;
    class ListAdditionalParagraphFormat;
    class FontFormat;
}

/**
 * Class that extends the \ref ListFormatUpdater to update arbitrary list format variables.  This method is intended to
 * be used in conjunction with the \ref ListFormatDialog.
 *
 * If you wish to remove all list formatting, use the \ref RemoveListFormatUpdater class.
 */
class FullListFormatUpdater:public ListFormatUpdater {
    public:
        /**
         * Enumeration used for tri-stated values.
         */
        enum class TriState {
            /**
             * Indicated the value should be set to true.
             */
            TRUE,

            /**
             * Indicates the value should be set to false.
             */
            FALSE,

            /**
             * Indicates the value should be left unchanged.
             */
            DO_NOT_CHANGE
        };

        /**
         * Type used to indicate a new font weight.
         */
        typedef Ld::FontFormat::Weight FontWeight;

        /**
         * Value that indicates an invalid starting number.
         */
        static const unsigned invalidStartingNumber;

        /**
         * Value used to represent an invalid font size.
         */
        static const unsigned invalidFontSize;

        /**
         * Value used to represent an invalid letter spacing.
         */
        static const float invalidLetterSpacing;


        FullListFormatUpdater();

        ~FullListFormatUpdater() override;

        /**
         * Enumeration of supported number styles.
         */
        typedef Ld::OrderedListParagraphFormat::Style NumberStyle;

        /**
         * Method you can use to indicate whether font parameters should be updated.
         *
         * \param[in] nowUpdateFonts If true, font parameters will be updated.  If false, font parameters will not be
         *                           updated.
         */
        void setFontUpdatesEnabled(bool nowUpdateFonts = true);

        /**
         * Method you can use to indicate whether font parameters should be ignored.
         *
         * \param[in] nowNoUpdateFonts If true, font parameters will be ignored.  If false, font parameters will be
         *                           updated.
         */
        void setFontUpdatesDisabled(bool nowNoUpdateFonts = true);

        /**
         * Method you can use to determine if font parameters will be updated.
         *
         * \return Returns true if font parameters will be updated.  Returns false if font parameters will not be
         *         updated.
         */
        bool fontUpdatesEnabled() const;

        /**
         * Method you can use to determine if font parameters will not be updated.
         *
         * \return Returns true if font parameters will not be updated.  Returns false if font parameters will be
         *         updated.
         */
        bool fontUpdatesDisabled() const;

        /**
         * Method you can use to specify whether bullets or numbers should be shown or hidden.
         *
         * \param[in] nowShow Indicates if the bullet or number should be shown, hidden, or left unchanged.
         */
        void setShowBulletOrNumberState(TriState nowShow);

        /**
         * Method you can use to determine if the bullet or numbers will be shown, hidden, or left as-is.
         *
         * \return Returns the current state of the show bullet or number check box.
         */
        TriState showBulletOrNumberState() const;

        /**
         * Method you can use to specify the new bullet style.  Setting the bullet style will clear the requested
         * number style.
         *
         * \param[in] newBulletStyle The new bullet style.  An empty string will leave the bullet/number style
         *                           unchanged.
         */
        void setBulletStyle(const QString& newBulletStyle);

        /**
         * Method you can use to obtain the requested bullet style.
         *
         * \return Returns a string indicating the requested bullet.  An empty string indicates that bullets should not
         *         be applied.
         */
        QString bulletStyle() const;

        /**
         * Method you can use to specify the new number style.  Setting the number style will clear the requested
         * bullet style.
         *
         * \param[in] newNumberStyle The newly requested number style.   A value of NumberStyle::INVALID will leave
         *                           the number style unchanged.
         */
        void setNumberStyle(NumberStyle newNumberStyle);

        /**
         * Method you can use to determine the currently selected number style.
         *
         * \return Returns the currently selected number style.  A value of NumberStyle::INVALID indicates that there
         *         is no set number style.
         */
        NumberStyle numberStyle() const;

        /**
         * Method you can call to clear the requested bullet/number style button.
         */
        void clearBulletAndNumberStyles();

        /**
         * Method you can use to specify the starting number to be applied to ordered lists.
         *
         * \param[in] newStartingNumber The new starting number to be applied.  A value of
         *                              \ref FullListFormatUpdater::invalidStartingNumber indicates that no new
         *                              starting number should be set.
         */
        void setStartingNumber(unsigned newStartingNumber);

        /**
         * Convenience method that clears the requested starting number.
         */
        void clearStartingNumber();

        /**
         * Method you can use to determine the selected starting number value.
         *
         * \return Returns the selected starting number value.
         */
        unsigned startingNumber() const;

        /**
         * Method you can use to set the requested prefix string.
         *
         * \param[in] newPrefixString The newly requested prefix string.
         */
        void setPrefixString(const QString& newPrefixString);

        /**
         * Method you can use to clear the currently set prefix string.
         */
        void clearPrefixString();

        /**
         * Method you can use to obtain the currently requested prefix string
         *
         * \return Returns the currently requested list prefix string.  An empty string is returned if no prefix string
         *         is set.
         */
        QString prefixString();

        /**
         * Method you can use to determine if a prefix string has been set.
         *
         * \return Returns true if the prefix string is set.  Returns false if the prefix string is not set.
         */
        bool isPrefixStringSet();

        /**
         * Method you can use to determine if the prefix string has not been set.
         *
         * \return Returns true if the prefix string is not set.  Returns false if the prefix string is set.
         */
        bool isPrefixStringCleared();

        /**
         * Method you can use to set the requested suffix string.
         *
         * \param[in] newSuffixString The newly requested suffix string.
         */
        void setSuffixString(const QString& newSuffixString);

        /**
         * Method you can use to clear the currently set suffix string.
         */
        void clearSuffixString();

        /**
         * Method you can use to obtain the currently requested suffix string
         *
         * \return Returns the currently requested list suffix string.  An empty string is returned if no suffix string
         *         is set.
         */
        QString suffixString();

        /**
         * Method you can use to determine if a suffix string has been set.
         *
         * \return Returns true if the suffix string is set.  Returns false if the suffix string is not set.
         */
        bool isSuffixStringSet();

        /**
         * Method you can use to determine if the suffix string has not been set.
         *
         * \return Returns true if the suffix string is not set.  Returns false if the suffix string is set.
         */
        bool isSuffixStringCleared();

        /**
         * Method you can use to specify the new font family.
         *
         * \param[in] newFontFamily The newly requested font family.  An empty string will indicate that the font
         *                          family should not be changed.
         */
        void setFontFamily(const QString& newFontFamily);

        /**
         * Method you can use to clear the font family.
         */
        void clearFontFamily();

        /**
         * Method you can use to determine the font family selected by the user.
         *
         * \return Returns the name of the selected font family.  An empty string is returned if no font family is
         *         selected.
         */
        QString fontFamily() const;

        /**
         * Method you can use to set the desired font size.
         *
         * \param[in] newFontSize The newly requested font size.  A value of \ref FullListFormatUpdater::invalidFontSize
         *                        indicates no font size changes should be performed.
         */
        void setFontSize(unsigned newFontSize);

        /**
         * Method you can use to clear the requested font size.
         */
        void clearFontSize();

        /**
         * Method you can use to obtain the currently requested font size.
         *
         * \return Returns the currently indicated new font size.  A value of
         *         \ref FullListFormatUpdater::invalidFontSize indicates that the font size will not be changed.
         */
        unsigned fontSize() const;

        /**
         * Method you can use to specify the new font weight.
         *
         * \param[in] newFontWeight The newly requested font weight.  A value of
         *                          FullListFormatUpdater::FontWeight::INVALID indicates that the font weight should
         *                          not be updated.
         */
        void setFontWeight(FontWeight newFontWeight);

        /**
         * Method you can use to indicate that font weights should not be updated.
         */
        void clearFontWeight();

        /**
         * Method you can use to obtain the currently requested font weight.
         *
         * \return Returns the currently requested font weight.  A value of
         *         FullListFontUpdater::FontWeight::INVALID indicates that the font weight should not be set.
         */
        FontWeight fontWeight() const;

        /**
         * Method you can use to indicate the italics mode.
         *
         * \param[in] newItalicsMode The new italics mode.
         */
        void setItalics(TriState newItalicsMode);

        /**
         * Method you can use to determine the current italics mode.
         *
         * \return Returns the current italics mode.
         */
        TriState italics() const;

        /**
         * Method you can use to indicate the underline mode.
         *
         * \param[in] newUnderlineMode The new underline mode.
         */
        void setUnderline(TriState newUnderlineMode);

        /**
         * Method you can use to determine the current underline mode.
         *
         * \return Returns the current underline mode.
         */
        TriState underline() const;

        /**
         * Method you can use to indicate the overline mode.
         *
         * \param[in] newOverlineMode The new overline mode.
         */
        void setOverline(TriState newOverlineMode);

        /**
         * Method you can use to determine the current overline mode.
         *
         * \return Returns the current overline mode.
         */
        TriState overline() const;

        /**
         * Method you can use to indicate the strikethrough mode.
         *
         * \param[in] newStrikethroughMode The new strikethrough mode.
         */
        void setStrikethrough(TriState newStrikethroughMode);

        /**
         * Method you can use to determine the current strikethrough mode.
         *
         * \return Returns the current strikethrough mode.
         */
        TriState strikethrough() const;

        /**
         * Method you can use to set the newly requested font color.
         *
         * \param[in] newFontColor The newly requested font color.
         */
        void setFontColor(const QColor& newFontColor);

        /**
         * Method you can use to clear the font color setting.
         */
        void clearFontColor();

        /**
         * Method you can use to determine the currently selected font color.
         *
         * \return Returns the currently selected font color.  An invalid font color is returned if the current font
         *         color is not set.
         */
        QColor fontColor() const;

        /**
         * Method you can use to determine if the font color has been set.
         *
         * \return Returns true if the font color is set.  Returns false if the color is not set.
         */
        bool isFontColorSet() const;

        /**
         * Method you can use to determine if the font color has been not been set.
         *
         * \return Returns true if the font color is not set.  Returns false if the color is set.
         */
        bool isFontColorCleared() const;

        /**
         * Method you can use to set the newly requested font background color.
         *
         * \param[in] newFontBackgroundColor The newly requested font background color.
         */
        void setFontBackgroundColor(const QColor& newFontBackgroundColor);

        /**
         * Method you can use to clear the font background color setting.
         */
        void clearFontBackgroundColor();

        /**
         * Method you can use to determine the currently selected font background color.
         *
         * \return Returns the currently selected font background color.  An invalid font background color is returned
         *         if the current font background color is not set.
         */
        QColor fontBackgroundColor() const;

        /**
         * Method you can use to determine if the font background color has been set.
         *
         * \return Returns true if the font background color is set.  Returns false if the background color is not set.
         */
        bool isFontBackgroundColorSet() const;

        /**
         * Method you can use to determine if the font background color has been not been set.
         *
         * \return Returns true if the font background color is not set.  Returns false if the background color is set.
         */
        bool isfontBackgroundColorCleared() const;

        /**
         * Method you can use to set the new letter spacing.
         *
         * \param[in] newLetterSpacing The new desired letter spacing value.  A value of
         *                             \ref FullListFormatUpdater::invalidLetterSpacing indicates that the letter
         *                             spacing should not be set.
         */
        void setLetterSpacing(float newLetterSpacing);

        /**
         * Method you can use to clear the letter spacing value.
         */
        void clearLetterSpacing();

        /**
         * Method you can use to determine the current letter spacing value.
         *
         * \return Returns the currently requested letter spacing.  A value of
         *         \ref FullListFormatUpdater::invalidLetterSpacing indicates that letter spacing should not be set.
         */
        float letterSpacing() const;

    protected:
        /**
         * Method that is called to reset this class instance prior to processing each group.
         */
        void reset() override;

        /**
         * Method that is called to scan backwards to locate a set of default formats.
         *
         * \param[in] precedingElement A shared pointer to the element that immediately precedes the group.  A null
         *                             pointer is issued if there is no preceding element.
         */
        void locateDefaultFormats(Ld::ElementPointer precedingElement) override;

        /**
         * Method that is called to update the first element in each group.
         *
         * \param[in] element A shared pointer to the first element in the group.
         *
         * \return Returns updates to be applied.
         */
        FormatsByElement updateFirstElementInGroup(Ld::ElementPointer element) override;

        /**
         * Method that is called to update an element other than the first element in each group.
         *
         * \param[in] element A shared pointer to the first element in the group.
         *
         * \return Returns updates to be applied.
         */
        FormatsByElement updateSubsequentElementInGroup(Ld::ElementPointer element) override;

        /**
         * Method that is called to update the first element following the group.
         *
         * \param[in] element A shared pointer to the element that follows the first element in the group.  A null
         *                    pointer will be issued if there is no element after the end of the group.
         *
         * \return Returns updates to be applied.
         */
        FormatsByElement updateElementFollowingGroup(Ld::ElementPointer element) override;

    private:
        /**
         * Method that updates an ordered list format, in place.
         *
         * \param[in] newFormat      A shared pointer to the new list format to be modified.
         *
         * \param[in] updateFontData If true, font data will also be updated.
         *
         * \return Returns true if the list format was updated.  Returns false if the list format was not updated.
         */
        bool updateOrderedListFormat(QSharedPointer<Ld::OrderedListParagraphFormat> newFormat, bool updateFontData);

        /**
         * Method that updates an unordered list format, in place.
         *
         * \param[in] newFormat      A shared pointer to the new list format to be modified.
         *
         * \param[in] updateFontData If true, font data will also be updated.
         *
         * \return Returns true if the list format was updated.  Returns false if the list format was not updated.
         */
        bool updateUnorderedListFormat(
            QSharedPointer<Ld::UnorderedListParagraphFormat> newFormat,
            bool                                             updateFontData
        );

        /**
         * Method that updates font data, in place.
         *
         * \param[in] newFormat A shared pointer to the new list format to be modified.
         *
         * \return Returns true if the format was updated.  Returns false if the format was not updated.
         */
        bool updateFontFormat(QSharedPointer<Ld::ListParagraphFormatBase> newFormat);

        /**
         * Method that updates a list additional paragraph format.
         *
         * \param[in] newFormat A shared pointer to the new list format to be modified.
         *
         * \return Returns true if the list format was updated.  REturns false if the list format was not updated.
         */
        bool updateListAdditionalFormat(QSharedPointer<Ld::ListAdditionalParagraphFormat> newFormat);

        /**
         * Enumeration of list styles.
         */
        enum class ListStyle {
            /**
             * Indicates no list style.
             */
            NO_LIST,

            /**
             * Indicates an ordered list.
             */
            ORDERED,

            /**
             * Indicates an unordered list.
             */
            UNORDERED
        };

        /**
         * Flag indicating if font parameters should be updated or ignored.
         */
        bool currentUpdateFonts;

        /**
         * Value that indicates if bullets/number should be displayed.
         */
        TriState currentShowBulletOrNumberState;

        /**
         * Value indicating the desired bullet style.
         */
        QString currentBulletStyle;

        /**
         * Value indicating the desired number style.
         */
        NumberStyle currentNumberStyle;

        /**
         * Value indicating the desired starting number.
         */
        unsigned currentStartingNumber;

        /**
         * Value holding the desired prefix string.
         */
        QString currentPrefixString;

        /**
         * Boolean value indicating if the prefix string should be updated.
         */
        bool currentUpdatePrefixString;

        /**
         * Value holding the desired suffix string.
         */
        QString currentSuffixString;

        /**
         * Boolean value indicating if the suffix string should be updated.
         */
        bool currentUpdateSuffixString;

        /**
         * Value holding the desired font family.  An empty string indicates the font family should not be changed.
         */
        QString currentFontFamily;

        /**
         * Value holding the desired font size.
         */
        unsigned currentFontSize;

        /**
         * Value holding the desired font weight.
         */
        FontWeight currentFontWeight;

        /**
         * Value holding the desired italics state.
         */
        TriState currentItalicsState;

        /**
         * Value holding the desired underline state.
         */
        TriState currentUnderlineState;

        /**
         * Value holding the desired overline state.
         */
        TriState currentOverlineState;

        /**
         * Value holding the desired strikethrough state.
         */
        TriState currentStrikethroughState;

        /**
         * Value holding the desired font color.
         */
        QColor currentFontColor;

        /**
         * Flag indicating if the font color should be applied.
         */
        bool currentUpdateFontColor;

        /**
         * Value holding the desired font background color.
         */
        QColor currentFontBackgroundColor;

        /**
         * Flag indicating if the font background color should be applied.
         */
        bool currentUpdateFontBackgroundColor;

        /**
         * The current letter spacing value.
         */
        float currentLetterSpacing;

        /**
         * The last list format used over a given range.  The format will be applied to the first paragraph after the
         * end of the range if it is currently a continuation of the list.
         */
        QSharedPointer<Ld::ListParagraphFormatBase> lastListFormat;

        /**
         * The type of list we're currently generating.
         */
        ListStyle currentListStyle;
};

#endif
