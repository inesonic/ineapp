/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref CursorPositionSetting class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef CURSOR_POSITION_SETTING_H
#define CURSOR_POSITION_SETTING_H

#include <QCoreApplication> // For Q_DECLARE_TR_FUNCTIONS macro.
#include <QString>
#include <QSharedPointer>

#include "ld_handle.h"
#include "ld_element_cursor.h"
#include "ld_xml_writer.h"
#include "ld_xml_reader.h"
#include "ld_xml_attributes.h"
#include "ld_xml_element.h"
#include "ld_document_setting.h"

namespace Ld {
    class RootElement;
}

/**
 * Class you can use to embed a cursor position value into the document settings data.
 */
class CursorPositionSetting:public Ld::DocumentSetting {
    Q_DECLARE_TR_FUNCTIONS(CursorPositionSetting)

    public:
        /**
         * The setting name used to reference this setting.
         */
        static const char settingName[];

        CursorPositionSetting();

        ~CursorPositionSetting() override;

        /**
         * Creator method you can use to create instances of this class.
         *
         * \param[in] typeName The name used to reference this creator function.  A single creator function can be used
         *                     to create multiple object types using this value to identify what type of format to be
         *                     created.
         *
         * \return Returns a pointer to this instance.
         */
        static Ld::DocumentSetting* creator(const QString& typeName);

        /**
         * Method that returns the type name associated with this object.
         *
         * \return Returns the typename associated with this object.  The value must match the type used to invoke
         *         the static XmlElement::create and XmlElement::registerCreator methods.
         */
        QString typeName() const override;

        /**
         * Pure virtual method that indicates if this format contains valid information.
         *
         * \return Returns true if the format is valid.  Returns false if the format is invalid.
         */
        bool isValid() const override;

        /**
         * Method you can use to update this instance from a cursor position.
         *
         * \param[in] elementCursor The element cursor to be used to update this instance.
         */
        void setCursorPosition(const Ld::ElementCursor& elementCursor);

        /**
         * Method you can use to obtain the element cursor value.
         *
         * \param[in] rootElement A pointer to the root element.
         *
         * \return Returns an element cursor instance.
         */
        Ld::ElementCursor cursorPosition(QSharedPointer<Ld::RootElement> rootElement) const;

    protected:
        /**
         * Method you can optionally overload to add additional attributes to the XML description of this element.
         *
         * \param[in] attributes A reference to the list of attributes to be modified.
         */
        void writeAddAttributes(Ld::XmlAttributes& attributes) override;

        /**
         * Method you can overload to parse incoming XML attributes.  This method will only be called if there are
         * provided attributes.
         *
         * Errors are reported directly to the reader using the raiseError method.
         *
         * \param[in] reader     The reader used to parse the incoming XML stream.
         *
         * \param[in] attributes The XML attributes to be parsed.
         *
         * \param[in] xmlVersion The XML major version code.  The value can be used to modify how the incoming XML
         *                       stream is parsed.
         */
        void readAttributes(
            QSharedPointer<Ld::XmlReader> reader,
            const Ld::XmlAttributes&      attributes,
            unsigned                      xmlVersion
        ) override;

    private:
        /**
         * Method that invalidates the underlying data values.
         */
        void invalidate();

        /**
         * The element handle.
         */
        Ld::Handle currentHandle;

        /**
         * The child index.  Used when the cursor points to an empty position.
         */
        unsigned long currentChildIndex;

        /**
         * The region index within the element.
         */
        unsigned currentRegionIndex;

        /**
         * The text index within the region.
         */
        unsigned long currentTextIndex;
};

#endif
