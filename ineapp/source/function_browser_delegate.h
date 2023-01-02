/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref FunctionBrowserDelegate class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef FUNCTION_BROWSER_DELEGATE_H
#define FUNCTION_BROWSER_DELEGATE_H

#include <QFont>
#include <QRect>
#include <QStyledItemDelegate>

class QObject;
class QStringRef;
class QPainter;
class QStyleOptionViewItem;
class QModelIndex;

/**
 * Class that provides a delegate for rendering a small subset of HTML in a table or tree view.
 */
class FunctionBrowserDelegate:public QStyledItemDelegate {
    Q_OBJECT

    public:
        /**
         * Constructor.
         *
         * \param[in] parent The parent object for this menu.
         */
        FunctionBrowserDelegate(QObject* parent = Q_NULLPTR);

        ~FunctionBrowserDelegate() override;

        /**
         * Method that performs painting of content.
         *
         * \param[in] painter The painter that the delegate should paint upon.
         *
         * \param[in] option  The option used to control the painting.  The parameter provides the drawing rectangle
         *                    as well as parameters to indicate if the field is selected or enabled.
         *
         * \param[in] index   The index into the model indicating the location being painted.
         */
        void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

        /**
         * Method that calcualtes the size for the painted item.
         *
         * \param[in] option  The option used to control the size estimation.  The parameter provides the drawing
         *                    rectangle as well as parameters to indicate if the field is selected or enabled.
         *
         * \param[in] index   The index into the model indicating the location being sized.
         *
         * \return Returns the size needed by the delegate to fully display the item.
         */
        QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    private:
        /**
         * Pure virtual private base class used to process interpreted HTML.
         */
        class Engine {
            public:
                Engine();

                virtual ~Engine();

                /**
                 * Method that is called process a contiguous region of text.
                 *
                 * \param[in] text The normal text to be parsed.
                 */
                virtual void processText(const QString& text) = 0;

                /**
                 * Method that is called to indicate the start of a subscript block.
                 */
                virtual void startSubscript();

                /**
                 * Method that is called to indicate the end of a subscript block.
                 */
                virtual void endSubscript();

                /**
                 * Method that is called to indicate the start of a superscript block.
                 */
                virtual void startSuperscript();

                /**
                 * Method that is called to indicate the end of a superscript block.
                 */
                virtual void endSuperscript();

                /**
                 * Method that is called to indicate the start of an italicized block.
                 */
                virtual void startItalics();

                /**
                 * Method that is called to indicate the end of an italicized block.
                 */
                virtual void endItalics();

                /**
                 * Method that is called to indicate the start of a boldface block.
                 */
                virtual void startBold();

                /**
                 * Method that is called to indicate the end of a boldface block.
                 */
                virtual void endBold();

            protected:
                /**
                 * Method that returns true if the current mode is subscript.
                 *
                 * \return Returns true if the current mode is subscript.  Returns false if the current mode is not
                 *         subscript.
                 */
                bool isSubscript() const;

                /**
                 * Method that returns true if the current mode is superscript.
                 *
                 * \return Returns true if the current mode is superscript.  Returns false if the current mode is not
                 *         superscript.
                 */
                bool isSuperscript() const;

                /**
                 * Method that returns true if the current mode is italics.
                 *
                 * \return Returns true if the current mode is italics.  Returns false if the current mode is not
                 *         italics.
                 */
                bool isItalics() const;

                /**
                 * Method that returns true if the current mode is boldface.
                 *
                 * \return Returns true if the current mode is boldface.  Returns false if the current mode is not
                 *         boldface.
                 */
                bool isBoldface() const;

                /**
                 * Method you can use to obtain an adjusted font based on the current parameters.
                 *
                 * \param[in] font The desired normal font.
                 *
                 * \return Returns appropriate font for the specified font.
                 */
                QFont adjustedFont(const QFont& font) const;

            private:
                /**
                 * Flag indicating true if subscript mode is enabled.
                 */
                bool currentSubscript;

                /**
                 * Flag indicating true if superscript mode is enabled.
                 */
                bool currentSuperscript;

                /**
                 * Flag indicating true if italics mode is enabled.
                 */
                bool currentItalics;

                /**
                 * Flag indicating true if boldface mode is enabled.
                 */
                bool currentBoldface;
        };

        /**
         * Engine used to calculate the required size of the text.
         */
        class SizingEngine:public Engine {
            public:
                /**
                 * Constructor
                 *
                 * \param[in] font The font used for size calculations.
                 */
                SizingEngine(const QFont& font);

                ~SizingEngine() override;

                /**
                 * Method that is called process a contiguous region of text.
                 *
                 * \param[in] text The normal text to be parsed.
                 */
                void processText(const QString& text) override;

                /**
                 * Method you can use to obtain the text size.
                 *
                 * \return Returns the size of the text.
                 */
                QSize sizeHint() const;

            private:
                /**
                 * The font used for rendering.
                 */
                QFont currentFont;

                /**
                 * The current top.  May be negative for superscript.
                 */
                int currentTop;

                /**
                 * The current bottom.
                 */
                int currentBottom;

                /**
                 * The current width.
                 */
                unsigned currentWidth;
        };

        /**
         * Engine used to paint the text.
         */
        class TextPaintingEngine:public Engine {
            public:
                /**
                 * Constructor
                 *
                 * \param[in] painter The painter used to paint the text.
                 *
                 * \param[in] rectangle The bounding rectangle for the text.
                 *
                 * \param[in] font      The font used for size calculations.
                 */
                TextPaintingEngine(QPainter* painter, const QRect& rectangle, const QFont& font);

                ~TextPaintingEngine() override;

                /**
                 * Method that is called process a contiguous region of text.
                 *
                 * \param[in] text The normal text to be parsed.
                 */
                void processText(const QString& text) override;

            private:
                /**
                 * The painter.
                 */
                QPainter* currentPainter;

                /**
                 * The remaining bounding rectangle.
                 */
                QRect currentBoundingRectangle;

                /**
                 * The font used for rendering.
                 */
                QFont currentFont;
        };

        /**
         * Private method that parses the provided HTML.
         *
         * \param[in] str    The string to be parsed.
         *
         * \param[in] engine The engine to receive the processed HTML.
         */
        void parse(const QString& str, Engine& engine) const;
};

#endif
