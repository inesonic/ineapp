/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ImageFileDialog class.
***********************************************************************************************************************/

#include <QFileDialog>
#include <QWidget>
#include <QString>
#include <QStringList>
#include <QSettings>
#include <QByteArray>
#include <QImageReader>
#include <QDir>

#include <eqt_file_dialog.h>

#include "application.h"
#include "application_settings.h"
#include "image_file_dialog.h"

ImageFileDialog::ImageFileDialog(
        ImageFileDialog::DialogMode dialogMode,
        QWidget*                    parent
    ):EQt::FileDialog(
        parent
    ) {
    configureMode(dialogMode);
}


ImageFileDialog::~ImageFileDialog() {}


void ImageFileDialog::configureMode(ImageFileDialog::DialogMode dialogMode) {
    switch (dialogMode) {
        case DialogMode::LOAD_IMAGE: {
            setAcceptMode(QFileDialog::AcceptOpen);
            setFileMode(QFileDialog::ExistingFile);

            setWindowTitle(tr("Load Image"));
            setLabelText(DialogLabel::Accept, tr("Open"));

            break;
        }

        case DialogMode::IMPORT_IMAGE: {
            setAcceptMode(QFileDialog::AcceptOpen);
            setFileMode(QFileDialog::ExistingFile);

            setWindowTitle(tr("Import Image"));
            setLabelText(DialogLabel::Accept, tr("Import"));

            break;
        }

        case DialogMode::SAVE_IMAGE: {
            setAcceptMode(QFileDialog::AcceptSave);
            setFileMode(QFileDialog::AnyFile);

            setWindowTitle(tr("Save Image"));
            setLabelText(DialogLabel::Accept, tr("Save"));

            break;
        }

        case DialogMode::EXPORT_IMAGE: {
            setAcceptMode(QFileDialog::AcceptSave);
            setFileMode(QFileDialog::AnyFile);

            setWindowTitle(tr("Export Image"));
            setLabelText(DialogLabel::Accept, tr("Export"));

            break;
        }

        default: {
            Q_ASSERT(false);
            break;
        }
    }
}


QString ImageFileDialog::settingsGroupName() const {
    return "imageFileDialog";
}


QStringList ImageFileDialog::filterList() const {
    QStringList filters;

    QList<QByteArray> imageFormats = QImageReader::supportedImageFormats();
    QStringList formats;
    QStringList formatDescriptions;
    for (  QList<QByteArray>::const_iterator it  = imageFormats.constBegin(),
                                             end = imageFormats.constEnd()
         ; it != end
         ; ++it
        ) {
        QString formatName = QString::fromLocal8Bit(*it);
        formats << formatName.toLower();

        #if (defined(Q_OS_WIN))

            formatDescriptions << tr("*.%1").arg(formatName.toUpper());

        #elif (defined(Q_OS_DARWIN) | defined(Q_OS_LINUX))

            formatDescriptions << tr("*.%1").arg(formatName.toLower());

        #else

            #error Unknown platform

        #endif
    }

    filters << tr("All Image Formats (%1)").arg(formatDescriptions.join(tr(" ")));

    #if (defined(Q_OS_WIN))

        filters << tr("All Files (*.*)");

    #elif (defined(Q_OS_DARWIN) || defined(Q_OS_LINUX))

        filters << tr("All Files (*)");

    #else

        #error Unknown platform

    #endif

    for (  QStringList::const_iterator formatIterator    = formats.constBegin(),
                                       formatEndIterator = formats.constEnd()
         ; formatIterator != formatEndIterator
         ; ++formatIterator
        ) {
        QString format = *formatIterator;

        QString description;
        if (format == "bmp") {
            description = tr("Windows Bitmap Files");
        } else if (format == "cur") {
            description = tr("Windows Cursor Files");
        } else if (format == "gif") {
            description = tr("Graphics Interchange Format Files");
        } else if (format == "icns") {
            description = tr("Apple Icon Image Files");
        } else if (format == "ico") {
            description = tr("Windows Icon Files");
        } else if (format == "jpeg") {
            description = tr("Joint Photographic Experts Group Image Files");
        } else if (format == "jpg") {
            description = tr("Joint Photographic Experts Group Image Files");
        } else if (format == "jp2") {
            description = tr("Joint Photographic Experts Group Image Files (ISO/IEC 15444-1)");
        } else if (format == "pbm") {
            description = tr("Portable Bitmap Files");
        } else if (format == "pgm") {
            description = tr("Portable Graymap Files");
        } else if (format == "png") {
            description = tr("Portable Network Graphics Files");
        } else if (format == "ppm") {
            description = tr("Portable Pixmap Files");
        } else if (format == "svg") {
            description = tr("Scalable Vector Graphics File");
        } else if (format == "svgz") {
            description = tr("Compressed Scalable Vector Graphics Files");
        } else if (format == "tga") {
            description = tr("TARGA Image Files");
        } else if (format == "tif") {
            description = tr("Tagged Image Files");
        } else if (format == "tiff") {
            description = tr("Tagged Image Files");
        } else if (format == "wbmp") {
            description = tr("Wireless Application Protocol Bitmap Files");
        } else if (format == "webp") {
            description = tr("Google WebP Image Files");
        } else if (format == "xbm") {
            description = tr("X BitMap Files");
        } else if (format == "xpm") {
            description = tr("X PixMap Files");
        } else if (format == "heic") {
            description = tr("High Efficiency Image Containers");
        } else if (format == "heif") {
            description = tr("High Efficiency Image Files");
        } else {
            Q_ASSERT(false); // Unknown image format.
        }

        #if (defined(Q_OS_WIN))

            filters << tr("%1 (*.%2)").arg(description).arg(format.toUpper());

        #elif (defined(Q_OS_DARWIN) || defined(Q_OS_LINUX))

            filters << tr("%1 (*.%2)").arg(description).arg(format.toLower());

        #else

            #error Unknown platform

        #endif
    }

    return filters;
}
