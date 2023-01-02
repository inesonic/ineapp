/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref BasicMathViewProxy class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef BASIC_MATH_VIEW_PROXY_H
#define BASIC_MATH_VIEW_PROXY_H

#include <QObject>

#include <ld_element_structures.h>

#include "page_list.h"
#include "update_format_command.h"
#include "view_widget.h"
#include "math_view_proxy_base.h"

class QKeyEvent;

namespace EQt {
    class ProgrammaticView;
    class ProgrammaticMainBWindow;
}

class InsertElementCommand;

/**
 * View proxy used to manage the basic math functions.
 */
class BasicMathViewProxy:public MathViewProxyBase {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] view   The view this class is proxying for.
         *
         * \param[in] window The main window that this view is within.
         */
        BasicMathViewProxy(EQt::ProgrammaticView* view, EQt::ProgrammaticMainWindow* window);

        ~BasicMathViewProxy() override;

        /**
         * Method that is called each time the user interface wants this proxy to be controlled.  The method should
         * bind slots or actions from the main window to slots in this class.
         */
        void bindDynamic() final;

        /**
         * Method that is called each time the user interface no longer wants this proxy to be controlled by the
         * main window.  The method should unbind slots or actions from this class.
         */
        void unbindDynamic() final;

    private slots:
        /**
         * Signal that is emitted when a text/character key is pressed while the cursor points to a placeholder
         * element.
         *
         * \param[out] keyCode          The key code reported by the OS.
         *
         * \param[out] modifiers        The keyboard modifiers as interpreted by Qt.
         *
         * \param[out] nativeModifiers  The OS's native key modifiers.
         *
         * \param[out] nativeScanCodes  The OS's native scan codes.
         *
         * \param[out] nativeVirtualKey The native virtual key code as reported by the OS.
         *
         * \param[out] text             A textual representation of the key as reported by the OS.
         */
        void textKeyPressedOnPlaceholder(
            int                   keyCode,
            Qt::KeyboardModifiers modifiers,
            unsigned long         nativeModifiers,
            unsigned long         nativeScanCodes,
            unsigned long         nativeVirtualKey,
            const QString&        text
        );

        /**
         * Slot that is triggered when the assignment operator button is clicked.
         */
        void assignmentOperatorButtonClicked();

        /**
         * Slot that is triggered when the variable button is clicked.
         */
        void variableButtonClicked();

        /**
         * Slot that is triggered when the literal button is clicked.
         */
        void literalButtonClicked();

        /**
         * Slot that is triggered when the Pi special value button is clicked.
         */
        void piSpecialValueButtonClicked();

        /**
         * Slot that is triggered when the Euler's number special value button is clicked.
         */
        void eulersNumberSpecialValueButtonClicked();

        /**
         * Slot that is triggered when the Epsilon special value button is clicked.
         */
        void epsilonSpecialValueButtonClicked();

//        /**
//         * Slot that is triggered when the infinity special value button is clicked.
//         */
//        void infinitySpecialValueButtonClicked();

        /**
         * Slot that is triggered when the element of set button is clicked.
         */
        void elementOfSetButtonClicked();

        /**
         * Slot that is triggered when the boolean type button is clicked.
         */
        void booleanTypeButtonClicked();

        /**
         * Slot that is triggered when the integer type button is clicked.
         */
        void integerTypeButtonClicked();

        /**
         * Slot that is triggered when the real type button is clicked.
         */
        void realTypeButtonClicked();

        /**
         * Slot that is triggered when the complex type button is clicked.
         */
        void complexTypeButtonClicked();

        /**
         * Slot that is triggered when the addition operator button is clicked.
         */
        void additionOperatorButtonClicked();

        /**
         * Slot that is triggered when the subtraction operator button is clicked.
         */
        void subtractionOperatorButtonClicked();

        /**
         * Slot that is triggered when the multiplication operator button is clicked.
         */
        void multiplicationOperatorButtonClicked();

        /**
         * Slot that is triggered when the division operator button is clicked.
         */
        void divisionOperatorButtonClicked();

        /**
         * Slot that is triggered when the fraction operator button is clicked.
         */
        void fractionOperatorButtonClicked();

        /**
         * Slot that is triggered when the unary plus operator button is clicked.
         */
        void unaryPlusOperatorButtonClicked();

        /**
         * Slot that is triggered when the unary minus operator button is clicked.
         */
        void unaryMinusOperatorButtonClicked();

        /**
         * Slot that is triggered when the grouping parenthesis button is clicked.
         */
        void groupingParenthesisButtonClicked();

        /**
         * Slot that is triggered when the grouping brackets button is clicked.
         */
        void groupingBracketsButtonClicked();

        /**
         * Slot that is triggered when the grouping braces button is clicked.
         */
        void groupingBracesButtonClicked();

        /**
         * Slot that is triggered when the power operator button is clicked.
         */
        void powerOperatorButtonClicked();

        /**
         * Slot that is triggered when the square root operator button is clicked.
         */
        void squareRootOperatorButtonClicked();

        /**
         * Slot that is triggered when the root operator button is clicked.
         */
        void rootOperatorButtonClicked();

        /**
         * Slot that is triggered when the complex conjugate operator button is clicked.
         */
        void complexConjugateOperatorButtonClicked();

        /**
         * Slot that is triggered when the factorial operator button is clicked.
         */
        void factorialOperatorButtonClicked();

        /**
         * Slot that is triggered when the absolute value button is clicked.
         */
        void absoluteValueOperatorButtonClicked();

        /**
         * Slot that is triggered when the floor operator button is clicked.
         */
        void floorOperatorButtonClicked();

        /**
         * Slot that is triggered when the ceiling operator button is clicked.
         */
        void ceilingOperatorButtonClicked();

        /**
         * Slot that is triggered when the nearest integer operator button is clicked.
         */
        void nearestIntegerOperatorButtonClicked();

        /**
         * Slot that is triggered when the not equal to operator button is clicked.
         */
        void notEqualToOperatorButtonClicked();

        /**
         * Slot that is triggered when the less than operator button is clicked.
         */
        void lessThanOperatorButtonClicked();

        /**
         * Slot that is triggered when the greater than operator button is clicked.
         */
        void greaterThanOperatorButtonClicked();

        /**
         * Slot that is triggered when the less than or equal to operator button is clicked.
         */
        void lessThanOrEqualToOperatorButtonClicked();

        /**
         * Slot that is triggered when the greater than or equal to operator button is clicked.
         */
        void greaterThanOrEqualToOperatorButtonClicked();

        /**
         * Slot that is triggered when the subscript index operator button is clicked.
         */
        void subscriptIndexOperatorButtonClicked();

        /**
         * Slot that is triggered when the subscript row/column operator button is clicked.
         */
        void subscriptRowColumnOperatorButtonClicked();
};

#endif
