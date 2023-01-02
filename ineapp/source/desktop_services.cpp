/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref HelpBrowser class.
***********************************************************************************************************************/

#include <QString>
#include <QObject>
#include <QUrl>
#include <QDesktopServices>
#include <QDateTime>

#include <cstdint>

#include <crypto_xtea_encryptor.h>
#include <crypto_hmac.h>

#include "application.h"
#include "application_settings.h"
#include "desktop_services.h"

static const Crypto::XteaEncryptor::Keys sendFeedbackEncryptionKey = SEND_FEEDBACK_ENCRYPTION_KEYS;
static const std::uint8_t                sendFeedbackSecret[]      = SEND_FEEDBACK_HMAC_SECRET;

bool DesktopServices::openHelp() {
    QString url = QObject::tr(HELP_MAIN_PAGE_URL).arg(Application::applicationVersion());
    return openUrl(QUrl(url));
}


bool DesktopServices::openGettingStarted() {
    QString url = QObject::tr(HELP_GETTING_STARTED_URL).arg(Application::applicationVersion());
    return openUrl(QUrl(url));
}


bool DesktopServices::openHelpPage(const QString& keyword) {
    QString url = QObject::tr(HELP_KEYWORD_URL)
                  .arg(Application::applicationVersion())
                  .arg(QString(QUrl::toPercentEncoding(keyword)));

    return openUrl(QUrl(url));
}


bool DesktopServices::searchHelp(const QString& searchString) {
    QString url = QObject::tr(HELP_SEARCH_URL)
                  .arg(QString(QUrl::toPercentEncoding(searchString)));

    return openUrl(QUrl(url));
}


bool DesktopServices::openFunctionReference(const QString& functionIdentifier) {
    QString url = QObject::tr(HELP_FUNCTION_URL_PREFIX).arg(functionIdentifier);
    return openUrl(QUrl(url));
}


bool DesktopServices::sendFeedback(unsigned long long customerId) {
    unsigned long long secondsSinceEpoch = QDateTime::currentSecsSinceEpoch();

    // The algorithm below is pretty cheezey but it at least provides some defense against using the
    // send feedback portal to issue attacks.  Longer term, we should probably just XTEA or AES256
    // encrypt the timestamp and customer ID.

    QByteArray secret(reinterpret_cast<const char*>(sendFeedbackSecret), sizeof(sendFeedbackSecret));

    QByteArray rawData;
    std::uint8_t nonce = 0;
    for (unsigned i=0 ; i<8 ; ++i) {
        std::uint8_t t = static_cast<std::uint8_t>((secondsSinceEpoch >> 8*i) & 0xFF);
        rawData.append(static_cast<char>(t));
        secret.append(static_cast<char>(t));

        nonce += t;
    }

    for (unsigned i=0 ; i<5 ; ++i) {
        std::uint8_t m = static_cast<std::uint8_t>((customerId >> (8*i)) & 0xFF);
        rawData.append(m ^ nonce ^ (0xFF - 27 * i));
    }

    Crypto::Hmac hmac(secret);
    hmac.addData(rawData);
    rawData += hmac.digest();

    QByteArray encoded = rawData.toBase64(
          QByteArray::Base64Option::Base64UrlEncoding
        | QByteArray::Base64Option::OmitTrailingEquals
    );

    QString url = QString(FEEDBACK_URL).arg(QString::fromUtf8(encoded));
    return openUrl(QUrl(url));
}
