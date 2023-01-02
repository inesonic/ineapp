/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref UnicodeMarshaller class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef UNICODE_MARSHALLER_H
#define UNICODE_MARSHALLER_H

#include <QObject>
#include <QString>

#include <cstdint>

namespace Ld {
    class CommandContainer;
}

/**
 * Marshaller for the slots the accept a unicode value.
 */
class UnicodeMarshaller:public QObject {
    Q_OBJECT

    public:
        /**
         * Type used to represent a unicode value.
         */
        typedef std::uint16_t UnicodeValue;

        /**
         * Constructor
         *
         * \param[in] unicodeValue The unicode value to be marshalled to the proxy.
         *
         * \param[in] parent       Pointer to the parent object.
         */
        explicit UnicodeMarshaller(UnicodeValue unicodeValue, QObject* parent = Q_NULLPTR);

        ~UnicodeMarshaller() override;

        /**
         * Method you can use to set the unicode value to be marshalled.
         *
         * \param[in] newUnicodeValue The new unicode value to be marshalled.
         */
        void setUnicodeValue(UnicodeValue newUnicodeValue);

        /**
         * Method you can use to obtain the current unicode value.
         *
         * \return Returns the unicode value being marshalled by this instance.
         */
        UnicodeValue unicodeValue() const;

    signals:
        /**
         * Signal that is emitted when this marshaller is triggered.
         *
         * \param[out] unicodeValue The emitted unicode value.
         */
        void triggered(UnicodeValue unicodeValue);

        /**
         * Signal that is emitted when this marshaller is triggered.
         *
         * \param[out] text A textual representation of this unicode value.
         */
        void triggered(const QString& text);

    public slots:
        /**
         * Slot that can be triggered by the action.
         */
        void trigger();

    private:
        /**
         * The unicode value to be marshalled.
         */
        UnicodeValue currentUnicodeValue;
};

#endif
