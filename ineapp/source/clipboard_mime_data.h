/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ClipboardMimeData class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef CLIPBOARD_MIME_DATA_H
#define CLIPBOARD_MIME_DATA_H

#include <QSharedPointer>
#include <QMimeData>

namespace Ld {
    class XmlTemporaryFileExportContext;
}

/**
 * This class provides a wrapper on the Qt QMimeData class that allows an Ld::XmlTemporaryFileExportContext to be
 * used as a source for clipboard data.  The context is managed as a shared pointer so that it can be destroyed with
 * this object.
 */
class ClipboardMimeData:public QMimeData {
    Q_OBJECT

    public:
        /**
         * Value that holds the expected name for the generated HTML.
         */
        static const char htmlPayloadName[];

        ClipboardMimeData();

        ~ClipboardMimeData();

        /**
         * Method you can use to add an XML temporary file export context to the clipboard.
         *
         * \param[in] context A shared pointer to the context to be added.
         */
        void setExportContext(QSharedPointer<Ld::XmlTemporaryFileExportContext> context);

        /**
         * Method you can use to obtain the previously added export context.
         *
         * \return Returns a shared pointer to the export context.
         */
        QSharedPointer<Ld::XmlTemporaryFileExportContext> exportContext() const;

    private:
        /**
         * The current shared pointer to the context.
         */
        QSharedPointer<Ld::XmlTemporaryFileExportContext> currentExportContext;
};

#endif
