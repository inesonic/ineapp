/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref Clipboard class.
***********************************************************************************************************************/

#include <QSharedPointer>
#include <QByteArray>
#include <QMimeData>

#include <ld_xml_temporary_file_export_context.h>

#include "clipboard_mime_data.h"

const char ClipboardMimeData::htmlPayloadName[] = "index.html";

ClipboardMimeData::ClipboardMimeData() {}


ClipboardMimeData::~ClipboardMimeData() {}


void ClipboardMimeData::setExportContext(QSharedPointer<Ld::XmlTemporaryFileExportContext> context) {
    QByteArray html = context->payload(htmlPayloadName);
    setHtml(QString::fromUtf8(html));

    currentExportContext = context;
}


QSharedPointer<Ld::XmlTemporaryFileExportContext> ClipboardMimeData::exportContext() const {
    return currentExportContext;
}
