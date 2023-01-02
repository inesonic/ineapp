/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ZoomSetting class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef ZOOM_SETTING_H
#define ZOOM_SETTING_H

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
 * Class you can use to embed a zoom value into the document settings data.
 */
class ZoomSetting:public Ld::DocumentSetting {
    Q_DECLARE_TR_FUNCTIONS(ZoomSetting)

    public:
        /**
         * Value that represents an invalid zoom setting.
         */
        static const float invalidZoomValue;

        /**
         * The setting name used to reference this setting.
         */
        static const char settingName[];

        ZoomSetting();

        ~ZoomSetting() override;

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
         *         the static Ld::XmlElement::create and Ld::XmlElement::registerCreator methods.
         */
        QString typeName() const override;

        /**
         * Pure virtual method that indicates if this format contains valid information.
         *
         * \return Returns true if the format is valid.  Returns false if the format is invalid.
         */
        bool isValid() const override;

        /**
         * Method you can use to update the zoom settings value.
         *
         * \param[in] newZoom The new zoom value.  While the value is technically arbitrary.  You are urged to set a
         *                    zoom value of 100% as 1.0, a zoom value of 200% as 2.0, etc.
         */
        void setZoomValue(float newZoom);

        /**
         * Method you can use to obtain the desired zoom settings value.
         *
         * \return Returns an element cursor instance.
         */
        float zoomValue() const;

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
         * The current zoom value.
         */
        float currentZoomValue;
};

#endif
