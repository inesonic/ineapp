/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref DocumentPreferencesDialog class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef DOCUMENT_PREFERENCES_DIALOG_H
#define DOCUMENT_PREFERENCES_DIALOG_H

#include <QSize>
#include <QFont>

#include <eqt_programmatic_dialog.h>

#include <ld_root_element.h>

#include "app_common.h"

class QWidget;

namespace Ld {
    class PageFormat;
}

/**
 * Dialog used to display and manage document preferences.
 */
class APP_PUBLIC_API DocumentPreferencesDialog:public EQt::ProgrammaticDialog {
    Q_OBJECT

    public:
        /**
         * Type used to represent the type of RNG to be used.
         */
        typedef Ld::RootElement::RngType RngType;

        /**
         * Type used to represent an initial seed used by the RNG.
         */
        typedef Ld::RootElement::RngSeed RngSeed;

        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        DocumentPreferencesDialog(QWidget* parent = Q_NULLPTR);

        ~DocumentPreferencesDialog() override;

        /**
         * Method you can use to obtain the new page format.
         *
         * \return Returns a shared pointer to the new page format.  A null pointer is returned if the currently
         *         input page format is invalid.
         */
        QSharedPointer<Ld::PageFormat> pageFormat() const;

        /**
         * Method you can use to determine if the page format has been changed.
         *
         * \return Returns true if the page format has been changed.  Returns false if the page format settings are
         *         left unchanged.
         */
        bool pageFormatChanged() const;

        /**
         * Method you can use to determine the new text format.
         *
         * \return Returns the new text font format.
         */
        QFont textFont() const;

        /**
         * Method you can use to determine if the text format has changed.
         *
         * \return Returns true if the text format has changed.  Returns false if the text format is unchanged.
         */
        bool textFontChanged() const;

        /**
         * Method you can use to determine the new math format.
         *
         * \return Returns the new math font format.
         */
        QFont mathFont() const;

        /**
         * Method you can use to determine if the math format has changed.
         *
         * \return Returns true if the math format has changed.  Returns false if the math format is unchanged.
         */
        bool mathFontChanged() const;

        /**
         * Method you can use to determine the new math identifier format.
         *
         * \return Returns the new math identifier font format.
         */
        QFont mathIdentifierFont() const;

        /**
         * Method you can use to determine if the math identifier format has changed.
         *
         * \return Returns true if the math identifier format has changed.  Returns false if the math identifier format
         *         is unchanged.
         */
        bool mathIdentifierFontChanged() const;

        /**
         * Method you can use to determine the new math function format.
         *
         * \return Returns the new math function font format.
         */
        QFont mathFunctionFont() const;

        /**
         * Method you can use to determine if the math function format has changed.
         *
         * \return Returns true if the math function format has changed.  Returns false if the math function format is
         *         unchanged.
         */
        bool mathFunctionFontChanged() const;

        /**
         * Method you can use to determine the new brace conditional font.
         *
         * \return Returns the new brace conditional font.
         */
        QFont braceConditionalFont() const;

        /**
         * Method you can use to determine if the brace conditional font has changed.
         *
         * \return Returns true if the brace conditional font has changed.  Returns false if the brace conditional
         *         font is unchanged.
         */
        bool braceConditionalFontChanged() const;

        /**
         * Method you can use to determine the current brace conditional preamble string.
         *
         * \return Returns the currently entered brace conditional preamble string.
         */
        QString braceConditionalPreambleString() const;

        /**
         * Method you can use to determine the if the brace conditional preamble string was changed.
         *
         * \return Returns true if the brace conditional preamble string was changed.  Returns false if the brace
         *         conditional preamble string is unchanged.
         */
        bool braceConditionalPreambleStringChanged() const;

        /**
         * Method you can use to determine the current brace conditional else string.
         *
         * \return Returns the currently entered brace conditional else string.
         */
        QString braceConditionalElseString() const;

        /**
         * Method you can use to determine the if the brace conditional else string was changed.
         *
         * \return Returns true if the brace conditional else string was changed.  Returns false if the brace
         *         conditional else string is unchanged.
         */
        bool braceConditionalElseStringChanged() const;

        /**
         * Method you can use to determine if the brace conditional "else" clause should be included.
         *
         * \return Returns true if the brace conditional "else" clause should be included.  Returns false if the brace
         *         conditional else "clause" should not be included.
         */
        bool braceConditionalElseClauseIncluded() const;

        /**
         * Method you can use to determine the desired RNG type.
         *
         * \return Returns the desired RNG type.
         */
        RngType rngType() const;

        /**
         * Method you can use to obtain the selected RNG seed.
         *
         * \return Returns the selected RNG seed.
         */
        const RngSeed& rngSeed() const;

        /**
         * Method you can use to determine if the reported RNG seed should be used as a preset.
         *
         * \return Returns true if the selected RNG seed should be used as a preset seed.
         */
        bool useSeedAsPreset() const;

        /**
         * Method that can be called to populate the dialog's widgets.
         */
        void populate() final;

    signals:
        /**
         * Signal that is emitted when the apply button is clicked.
         *
         * \param[in] dialog The dialog containing the updates.
         */
        void applyClicked(DocumentPreferencesDialog* dialog);

    public slots:
        /**
         * Method you can use to update the displayed page format.
         *
         * \param[in] newPageFormat The new page format.
         */
        void setPageFormat(QSharedPointer<Ld::PageFormat> newPageFormat);

        /**
         * Method you can use to set the text font.
         *
         * \param[in] newTextFont The new text font.
         */
        void setTextFont(const QFont& newTextFont);

        /**
         * Method you can use to set the math font.
         *
         * \param[in] newMathFont The new math font.
         */
        void setMathFont(const QFont& newMathFont);

        /**
         * Method you can use to set the math identifier font.
         *
         * \param[in] newMathIdentifierFont The new math function font.
         */
        void setMathIdentifierFont(const QFont& newMathIdentifierFont);

        /**
         * Method you can use to set the math function font.
         *
         * \param[in] newMathFunctionFont The new math function font.
         */
        void setMathFunctionFont(const QFont& newMathFunctionFont);

        /**
         * Method you can use to set the brace conditional font.
         *
         * \param[in] newBraceConditionalFont The new brace conditional font.
         */
        void setBraceConditionalFont(const QFont& newBraceConditionalFont);

        /**
         * Method you can use to set the brace conditional "if" string.
         *
         * \param[in] newBraceConditionalIfString The new string to use as the "if" preamble on brace conditional.
         */
        void setBraceConditionalPreambleString(const QString& newBraceConditionalIfString);

        /**
         * Method you can use to set the brace conditional "if" string.
         *
         * \param[in] newBraceConditionalElseString The new string to use as the "else" preamble on brace conditional.
         */
        void setBraceConditionalElseString(const QString& newBraceConditionalElseString);

        /**
         * Method you can use to determine if the brace conditional "else" clause should be included.
         *
         * \param[in] includeElseClause If true, the else clause should be included.  If false, the else clause should
         *                              be excluded.
         */
        void setBraceConditionalElseClauseIncluded(bool includeElseClause);

        /**
         * Method you can use to set the selected RNG type.
         *
         * \param[in] newRngType The desired RNG type.
         */
        void setRngType(RngType newRngType);

        /**
         * Method you can use to set the desired RNG seed.
         *
         * \param[in] newRngSeed The desired new RNG seed.
         */
        void setRngSeed(const RngSeed& newRngSeed);

        /**
         * Method you can use to specify whether the reported RNG seed should be used as a preset.
         *
         * \param[in] nowUseSeedAsPreset If true, the RNG seed should be used as a preset.  If false, the RNG seed
         *                               will not be used as a preset.
         */
        void setUseSeedAsPreset(bool nowUseSeedAsPreset);

        /**
         * Slot that is triggered when the apply button is clicked.
         */
        void apply();

    protected:
        /**
         * Performs binding of signals to slots.  This method will be executed by the
         * EQt::ProgrammaticDialog::runBuilders method.
         *
         * You can overload this method if you want to bind signals to slots prior to the proxy classes.
         */
        void bind() final;

    private slots:
        /**
         * Slot that is triggered when page settings are updated.
         */
        void pageSettingsChanged();

        /**
         * Slot that is triggered when the user selects a new RNG type.
         *
         * \param[in] index The index of the newly selected item.
         */
        void rngTypeChanged(int index);

    private:
        /**
         * Method that builds the default dialog.
         */
        void configureDialog();

        /**
         * Method that calculates the page format from the current input.
         *
         * \return Returns the current page format based on the current user input.
         */
        Ld::PageFormat pageFormatFromInput() const;

        /**
         * The input page format.
         */
        QSharedPointer<Ld::PageFormat> inputPageFormat;

        /**
         * The input text font.
         */
        QFont inputTextFont;

        /**
         * The input math font.
         */
        QFont inputMathFont;

        /**
         * The input math identifier font.
         */
        QFont inputMathIdentifierFont;

        /**
         * The input math function font.
         */
        QFont inputMathFunctionFont;

        /**
         * The input brace conditional font.
         */
        QFont inputBraceConditionalFont;

        /**
         * The brace conditional "if" string.
         */
        QString inputBraceConditionalIfString;

        /**
         * The brace conditional "else" string.
         */
        QString inputBraceConditionalElseString;

        /**
         * Flag indicating if the brace conditional "else" clause should be displayed.
         */
        bool inputBraceConditionalElseClauseIncluded;

        /**
         * The input RNG type.
         */
        RngType inputRngType;

        /**
         * The input RNG seed.
         */
        RngSeed inputRngSeed;

        /**
         * Flag indicating if the RNG seed is to be used as a preset.
         */
        bool inputUseRngSeedAsPreset;

        /**
         * Location used to report the currently displayed seed.
         */
        mutable RngSeed outputRngSeed;
};

#endif
