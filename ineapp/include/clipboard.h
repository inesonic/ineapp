/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref Clipboard class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <QObject>
#include <QClipboard>
#include <QRegularExpression>

#include <ld_html_code_generator.h>

#include "app_common.h"
#include "selection_data.h"

class QMimeData;
class ClipboardMimeData;

/**
 * This class provides a wrapper on the Qt QClipboard object making it aware of the application specific data structures
 * and types.  You can use this class to manage the system clipboard.
 */
class APP_PUBLIC_API Clipboard:public QObject {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent              An optional pointer to the parent object.
         */
        Clipboard(QObject* parent = Q_NULLPTR);

        ~Clipboard() override;

        /**
         * Method you can use to determine if the clipboard currently holds any content.
         *
         * \return Returns true if the clipboard holds content, returns false if the clipboard is empty.
         */
        bool containsData();

        /**
         * Method you can use to determine if the clipboard is empty.
         *
         * \return Returns true if the clipboard is empty.  Returns false if the clipboard is not empty.
         */
        bool isEmpty();

        /**
         * Method you can use to add new application content to the clipboard.
         *
         * \param[in] newSelectionData The selection data to be added to the clipboard.
         */
        void addContent(SelectionDataPointer newSelectionData);

        /**
         * Method you can use to obtain the current application content from the clipboard.
         *
         * \return Returns a selection data instance representing the content in the clipboard.
         */
        SelectionDataPointer content();

    signals:
        /**
         * Signal that is emitted when the clipboard contains new data.
         */
        void clipboardUpdated();

        /**
         * Signal that is emitted when the clipboard contains new data.
         *
         * \param[out] externalSource Holds true if the source of the data is external to the application.  Holds false
         *                            if the source of the data is internal.
         */
        void clipboardUpdated(bool externalSource);

        /**
         * Signal that is emitted when the find clipboard buffer contents is updated.  This signal will only be
         * emitted on MacOS and can be used to trigger searches.
         *
         * \param[out] newFindBufferContents The new find buffer contents.
         */
        void findBufferUpdated(const QString& newFindBufferContents);

    private slots:
        /**
         * Slot that is triggered when the system clipboard state changes.
         *
         * \param[in] mode The Qt clipboard mode that changed.
         */
        void systemClipboardChanged(QClipboard::Mode mode);

    private:
        /**
         * The HTML style to use for the clipboard.
         */
        static const Ld::HtmlCodeGenerator::HtmlStyle htmlStyle;

        /**
         * THe HTML math mode to use for the clipboard.
         */
        static const Ld::HtmlCodeGenerator::MathMode mathMode;

        /**
         * Value indicating if the HTML code generator should ignore missing translators.
         */
        static const bool ignoreMissingTranslators;

        /**
         * Value indicating the default image format to use for received clipboard images.
         */
        static const char defaultImageFormat[];

        /**
         * Regular expression used to locate any non-printable character.
         */
        static const QRegularExpression nonPrintableRegularExpression;

        /**
         * The default image resolution for content copied to the clipboard.
         */
        static const float defaultImageDpi;

        /**
         * Method that is called to update the system clipboard from provided selection data.
         *
         * \param[in] newSelectionData The newly created selection data.
         */
        static void updateSystemClipboard(SelectionDataPointer newSelectionData);

        /**
         * Method that is called to generate selection data from information provided in the clipboard.
         *
         * \return Returns a \ref SelectionData instance that represents an interpretation of the clipboard.  A null
         *         pointer is returned if no selection data could be generated from the clipboard.
         */
        static SelectionDataPointer generateSelectionData();

        /**
         * Method that is called to convert received MIME data into a \ref SelectionData instance containing an image.
         *
         * \param[in] mimeData Pointer to the received MIME data.
         *
         * \return Returns a shared pointer to the newly created selection data.  A null pointer is returned if the data
         *         could not be converted.
         */
        static SelectionDataPointer convertMimeImage(const QMimeData* mimeData);

        /**
         * Method that is called to convert received MIME data into a \ref SelectionData instance containing one or
         * more members converted from HTML.
         *
         * \param[in] mimeData Pointer to the received MIME data.
         *
         * \return Returns a shared pointer to the newly created selection data.  A null pointer is returned if the data
         *         could not be converted.
         */
        static SelectionDataPointer convertMimeHtml(const QMimeData* mimeData);

        /**
         * Method that is called to convert received MIME data into a \ref SelectionData instance containing one or
         * more members converted from HTML.
         *
         * \param[in] mimeData Pointer to the received MIME data.
         *
         * \return Returns a shared pointer to the newly created selection data.  A null pointer is returned if the data
         *         could not be converted.
         */
        static SelectionDataPointer convertMimeText(const QMimeData* mimeData);

        /**
         * Method that cleans received text of any dangerous characters.
         *
         * \param[in] rawText The raw received text.
         *
         * \return Returns the cleaned up text.
         */
        static QString scrubText(const QString& rawText);

        /**
         * Method that adds an image element as a raw image to the supported mime data.
         *
         * \param[in]    imageElement The image element we are adding.
         *
         * \param[in,out] mimeData    The MIME data to contain the image element contents.
         *
         * \return Returns true on success, returns false if an error was detected.
         */
        static bool addImageElement(Ld::ElementPointer imageElement, ClipboardMimeData* mimeData);

        /**
         * Method that adds an arbitrary element as an image to the supported mime data.
         *
         * \param[in]     element  The element we are adding.
         *
         * \param[in,out] mimeData The MIME data to contain the image element contents.
         *
         * \return Returns true on success, returns false if an error was detected.
         */
        static bool addElementAsImage(Ld::ElementPointer element, ClipboardMimeData* mimeData);

        /**
         * Flag that indicates if the current internal selection data needs to be updated from the external clipboard.
         */
        bool currentUpdateFromExternalNeeded;

        /**
         * The current application selection data.
         */
        SelectionDataPointer currentInternalSelection;
};

#endif
