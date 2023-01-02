/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref SetsAndLogicViewProxy class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef SETS_AND_LOGIC_VIEW_PROXY_H
#define SETS_AND_LOGIC_VIEW_PROXY_H

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
 * View proxy used to manage functions related to sets and logic.
 */
class SetsAndLogicViewProxy:public MathViewProxyBase {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] view   The view this class is proxying for.
         *
         * \param[in] window The main window that this view is within.
         */
        SetsAndLogicViewProxy(EQt::ProgrammaticView* view, EQt::ProgrammaticMainWindow* window);

        ~SetsAndLogicViewProxy() override;

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
         * Slot that is triggered when the null set special value button is clicked.
         */
        void nullSetSpecialValueButtonClicked();

        /**
         * Slot that is triggered when the set button is clicked.
         */
        void setButtonClicked();

        /**
         * Slot that is triggered when the tuple button is clicked.
         */
        void tupleButtonClicked();

        /**
         * Slot that is triggered when the range button (implied increment) is clicked.
         */
        void range2ButtonClicked();

        /**
         * Slot that is triggered when the range button (explicit increment) is clicked.
         */
        void range3ButtonClicked();

        /**
         * Slot that is triggered when the union operator button is clicked.
         */
        void unionOperatorButtonClicked();

        /**
         * Slot that is triggered when the intersection operator button is clicked.
         */
        void intersectionOperatorButtonClicked();

        /**
         * Slot that is triggered when the disjoint union operator button is clicked.
         */
        void disjointUnionOperatorButtonClicked();

        /**
         * Slot that is triggered when the cartesian product button is clicked.
         */
        void cartesianProductOperatorButtonClicked();

        /**
         * Slot that is triggered when the set difference operator button is clicked.
         */
        void setDifferenceOperatorButtonClicked();

        /**
         * Slot that is triggered when the symmetric difference operator button is clicked.
         */
        void symmetricDifferenceOperatorButtonClicked();

        /**
         * Slot that is triggered when the N-ary union operator button is clicked.
         */
        void naryUnionOperatorButtonClicked();

        /**
         * Slot that is triggered when the N-ary disjoint union operator button is clicked.
         */
        void naryDisjointUnionOperatorButtonClicked();

        /**
         * Slot that is triggered when the not element of set operator button is clicked.
         */
        void notElementOfSetOperatorButtonClicked();

        /**
         * Slot that is triggered when the set contains as member operator button is clicked.
         */
        void setContainsAsMemberOperatorButtonClicked();

        /**
         * Slot that is triggered when the set does not contains as member operator button is clicked.
         */
        void notSetContainsAsMemberOperatorButtonClicked();

        /**
         * Slot that is triggered when the subset operator button is clicked.
         */
        void subsetOperatorButtonClicked();

        /**
         * Slot that is triggered when the proper subset operator button is clicked.
         */
        void properSubsetOperatorButtonClicked();

        /**
         * Slot that is triggered when the superset operator button is clicked.
         */
        void supersetOperatorButtonClicked();

        /**
         * Slot that is triggered when the proper superset operator button is clicked.
         */
        void properSupersetOperatorButtonClicked();

        /**
         * Slot that is triggered when the not subset operator button is clicked.
         */
        void notSubsetOperatorButtonClicked();

        /**
         * Slot that is triggered when the not proper subset operator button is clicked.
         */
        void notProperSubsetOperatorButtonClicked();

        /**
         * Slot that is triggered when the not superset operator button is clicked.
         */
        void notSupersetOperatorButtonClicked();

        /**
         * Slot that is triggered when the not proper superset operator button is clicked.
         */
        void notProperSupersetOperatorButtonClicked();

        /**
         * Slot that is triggered when logical unary not operator is clicked.
         */
        void logicalUnaryNotOperatorButtonClicked();

        /**
         * Slot that is triggered when logical overbar not operator is clicked.
         */
        void logicalOverbarNotOperatorButtonClicked();

        /**
         * Slot that is triggered when logical AND operator is clicked.
         */
        void logicalAndOperatorButtonClicked();

        /**
         * Slot that is triggered when logical OR operator is clicked.
         */
        void logicalOrOperatorButtonClicked();

        /**
         * Slot that is triggered when logical exclusive OR operator is clicked.
         */
        void logicalExclusiveOrOperatorButtonClicked();

        /**
         * Slot that is triggered when logical conditional operator is clicked.
         */
        void logicalConditionalOperatorButtonClicked();

        /**
         * Slot that is triggered when logical biconditional operator is clicked.
         */
        void logicalBiconditionalOperatorButtonClicked();

        /**
         * Slot that is triggered when logical true special value is clicked.
         */
        void logicalTrueSpecialValueButtonClicked();

        /**
         * Slot that is triggered when logical false special value is clicked.
         */
        void logicalFalseSpecialValueButtonClicked();

    private:
        /**
         * Method you can use to create a new logical not operator format instance.
         *
         * \return Returns a logical not operator format instance suitable for math functions.
         */
        Ld::FormatPointer logicalUnaryNotOperatorFormat() const;
};

#endif
