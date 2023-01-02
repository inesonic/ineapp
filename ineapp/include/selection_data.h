/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref SelectionData class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef SELECTION_DATA_H
#define SELECTION_DATA_H

#include <QCoreApplication> // For Q_DECLARE_TR_FUNCTIONS macro.
#include <QSharedPointer>
#include <QString>
#include <QList>
#include <QByteArray>

#include <ld_common.h>
#include <ld_element_structures.h>
#include <ld_root_element.h>
#include <ld_data_type.h>
#include <ld_calculated_value.h>
#include <ld_location.h>
#include <ld_element.h>

#include "app_common.h"

namespace Ld {
    class CursorStateCollection;
}

/**
 * You can use this class to store information about a selected range of elements.  The class is derived from
 * Ld::RootElement so it can be used by translators.
 */
class APP_PUBLIC_API SelectionData:public Ld::RootElement {
    Q_DECLARE_TR_FUNCTIONS(SelectionData)

    public:
        /**
         * The typename for the paragraph element.
         */
        static const QString elementName;

        /**
         * Static value indicating the capabilities provides by this element.
         */
        static const Ld::Capabilities childProvides;

        /**
         * Enumeration indicating the type of selection.
         */
        enum class SelectionType : std::uint8_t {
            /**
             * Indicates no selection available.
             */
            NO_SELECTION,

            /**
             * Indicates text annotation.  This indicates that the cursors start and end in text.
             */
            TEXT_ANNOTATION,

            /**
             * Indicates one or more operators have been selected.  This can include the case where the element cursor
             * is resting on a single element with no active selection.
             */
            OPERATOR_LIST,

            /**
             * Indicates text in a single operator has been selected.
             */
            TEXT_IN_OPERATOR
        };

        /**
         * Constructor
         *
         * \param[in] selectionType The selection type.
         */
        SelectionData(SelectionType selectionType = SelectionType::NO_SELECTION);

        ~SelectionData() override;

        /**
         * Factory method that can be used to create a default instance of a paragraph element.
         *
         * \param[in] typeName The name used to reference the creator function.
         *
         * \return Returns a newly created, default, paragraph element instance.
         */
        static Ld::Element* creator(const QString& typeName);

        /**
         * Method that returns the type name associated with this object.
         *
         * \return Returns the typename associated with this object.  The value must match the type used to invoke
         *         the static Ld::XmlElement::create and Ld::XmlElement::registerCreator methods.
         */
        QString typeName() const override;

        /**
         * Method that returns the name of the plug-in that provides this element.
         *
         * \return Returns the name of the plug-in that provides this element. The name must match the name provided
         *         in the Ld::InesonicPlugInData structure.  Elements defined as part of the ineld library should
         *         return an empty string.
         */
        QString plugInName() const override;

        /**
         * Method that returns a description of this element.
         *
         * \return Returns a description of this element suitable for use in a GUI.
         */
        QString description() const override;

        /**
         * Method that returns the selection type.
         *
         * \return Returns the selection type.
         */
        SelectionType selectionType() const;

        /**
         * Method you can use to create a clone of this element.  Note that this method will also set the weak
         * this pointer on each element.
         *
         * \param[in] includeChildren If true, clones of every child will be included in generated clone.  If
         *                            false, children will be excluded from the clone.
         *
         * \return Returns a new instance of this element with clones of each child included.  Note that a null
         *         pointer may be returned in the unlikely event of an error.
         */
        Ld::ElementPointer clone(bool includeChildren) const override;

        /**
         * Method you can use to create a clone of this element based on a starting and ending cursor position.
         * This version is intended to be used to clone elements containing text in one or more regions.  Note that
         * this method will set the weak this pointer on each element.
         *
         * \param[in] startingTextIndex   The starting text index of the first specified region to include in the
         *                                clone.
         *
         * \param[in] startingRegionIndex The startng text region to include in the clone.
         *
         * \param[in] endingTextIndex     The ending text index of the last specified region to include in the
         *                                clone.  A value of Ld::Element::invalidTextIndex indicates all the text in
         *                                the ending region should be included.
         *
         * \param[in] endingRegionIndex   The ending text region to include in the clone.  A value of
         *                                Ld::Element::invalidRegionIndex indicates the close should include the last
         *                                region.
         *
         * \param[in] includeChildren     If true, clones of every child will be included in generated clone.  If
         *                                false, children will be excluded from the clone.
         *
         * \return Returns a new instance of the element.  The clone may optionally include children if this
         *         element supports children.  A null pointer will be returned if the specified ranges are invalid.
         */
        Ld::ElementPointer clone(
            unsigned long        startingTextIndex,
            unsigned             startingRegionIndex,
            unsigned long        endingTextIndex,
            unsigned             endingRegionIndex,
            bool                 includeChildren
        ) const override;

        /**
         * Method you can call to determine the requirements imposed by this element on a specific child element.
         * Note that no testing is performed by elements to confirm that requirements are met.  The information is
         * expected to be used by the user interface.
         *
         * \param[in] childIndex The zero based child index.
         *
         * \return Returns the requirements imposed by this parent on the child element.
         */
        Ld::Capabilities parentRequires(unsigned long childIndex) const override;

        /**
         * Method you can use to obtain a calculated value for an element.  Placed in the root element and made
         * virtual so that selections can modify how calculated values are obtained by the code generators.  The
         * default implementation calls Ld::Element::calculatedValue to obtain its values.
         *
         * \param[in] element    The element to obtain the calculated value for.
         *
         * \param[in] valueIndex The zero based index of the value to obtain.
         *
         * \return Returns the reported calculated value.
         */
        Ld::CalculatedValue elementCalculatedValue(Ld::ElementPointer element, unsigned valueIndex) const override;

        /**
         * Method you can use to add a new calculated value for an element.
         *
         * \param[in] element         The element to store the sideband calculated value for.
         *
         * \param[in] valueIndex      The value index of the value to assign the calculated value to.
         *
         * \param[in] calculatedValue The calculated value to assign to the element.
         */
        void addCalculatedValue(
            Ld::ElementPointer         element,
            unsigned                   valueIndex,
            const Ld::CalculatedValue& calculatedValue
        );

        /**
         * Method you can use to obtain an image for an element.  Placed in the root element and made virtual so
         * that selections can modify this mechanism.  The default implementation calls the
         * Ld::Element::exportImage method of the provided element.
         *
         * \param[in] element The element to obtain the image for.
         *
         * \param[in] dpi     The desired image resolution in DPI.  The DPI setting is intended to be used as a
         *                    hint and may not be honored.
         *
         * \return Returns a byte array holding the desired image.
         */
        QByteArray exportElementImage(Ld::ElementPointer element, float dpi) const override;

        /**
         * Method you can use to register image side-band data for a cloned element.  You should only add images for
         * elements that support image export via the visual.
         *
         * \param[in] element      The element to store sideband image data for.
         *
         * \param[in] imagePayload The image payload to be associated with the element.
         */
        void addImagePayload(Ld::ElementPointer element, const QByteArray& imagePayload);

    protected:
        /**
         * Method you can overload to exclude this root element from the list of user visible root elements.
         *
         * \return Returns true if this root element should be excluded.  Returns false if this root element should
         *         be included.  This version returns true.
         */
        bool excludeFromUserVisibleRoots() const override;

    private:
        /**
         * Type used to managed calculated values by index.
         */
        typedef QHash<unsigned, Ld::CalculatedValue> CalculatedValueByIndex;

        /**
         * Type used to manage calculated values by index by element.
         */
        typedef QHash<Ld::ElementPointer, CalculatedValueByIndex> CalculatedValueByIndexByElement;

        /**
         * The current selection type.
         */
        SelectionType currentSelectionType;

        /**
         * Hash of handles to image payloads.
         */
        QHash<Ld::ElementPointer, QByteArray> imagePayloadsByElement;

        /**
         * Hash of calculated values by value index by element pointer.
         */
        CalculatedValueByIndexByElement calculatedValuesByIndexByElement;
};

/**
 * Convenience type you can use to point to a \ref SelectionData instance.
 */
typedef QSharedPointer<SelectionData> SelectionDataPointer;

#endif
