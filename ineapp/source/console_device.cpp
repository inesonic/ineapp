/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref ConsoleDevice class.
***********************************************************************************************************************/

#include <QBuffer>
#include <QString>
#include <QMap>
#include <QDateTime>

#include <QDebug>

#include <cstdint>
#include <atomic>

#include <m_intrinsic_types.h>
#include <m_set_iterator.h>
#include <m_set.h>
#include <m_tuple.h>
#include <m_variant.h>
#include <m_console.h>

#include "console_device.h"

ConsoleDevice::ConsoleDevice(QObject* parent):QObject(parent) {
    currentNumberThreads  = 0;
    threadBuffer          = Q_NULLPTR;
    threadAttributes      = Q_NULLPTR;
    messageTypes          = Q_NULLPTR;
    activeMessageTypes    = static_cast<std::uint64_t>(-1);
    runningSequenceNumber = 0;
    currentOutputMode     = OutputMode::PLAIN_TEXT;

    currentMessageColors.insert(MessageType::INVALID, "#FF00FF");         // magenta
    currentMessageColors.insert(MessageType::INFORMATION, "#0000A0");     // dark blue
    currentMessageColors.insert(MessageType::DATA, "#000000");            // black
    currentMessageColors.insert(MessageType::DEBUG, "#C88141");           // bronze
    currentMessageColors.insert(MessageType::BUILD_WARNING, "#FF9900");   // orange
    currentMessageColors.insert(MessageType::BUILD_ERROR, "#800000");     // dark red
    currentMessageColors.insert(MessageType::RUNTIME_WARNING, "#FF9900"); // orange
    currentMessageColors.insert(MessageType::RUNTIME_ERROR, "#800000");   // dark red

    currentIncludeMessageType = true;
    currentIncludeTimeStamp   = true;
    currentIncludeThreadId    = true;
}


ConsoleDevice::~ConsoleDevice() {
    if (threadBuffer != Q_NULLPTR) {
        delete[] threadBuffer;
    }

    if (threadAttributes != Q_NULLPTR) {
        delete[] threadAttributes;
    }

    if (messageTypes != Q_NULLPTR) {
        delete[] messageTypes;
    }
}


bool ConsoleDevice::isMessageTypeEnabled(ConsoleDevice::MessageType messageType) const {
    unsigned      index = static_cast<unsigned>(messageType);
    std::uint64_t mask  = std::uint64_t(1) << index;

    return (activeMessageTypes & mask) != 0;
}


bool ConsoleDevice::isMessageTypeDisable(ConsoleDevice::MessageType messageType) const {
    return !isMessageTypeEnabled(messageType);
}


ConsoleDevice::OutputMode ConsoleDevice::outputMode() const {
    return currentOutputMode;
}


QString ConsoleDevice::messageColor(ConsoleDevice::MessageType messageType) const {
    return currentMessageColors.value(messageType);
}


bool ConsoleDevice::includeMessageType() const {
    return currentIncludeMessageType;
}


bool ConsoleDevice::excludeMessageType() const {
    return !currentIncludeMessageType;
}


bool ConsoleDevice::includeTimeStamp() const {
    return currentIncludeTimeStamp;
}


bool ConsoleDevice::excludeTimeStamp() const {
    return !currentIncludeTimeStamp;
}


bool ConsoleDevice::includeThreadId() const {
    return currentIncludeThreadId;
}


bool ConsoleDevice::excludeThreadId() const {
    return !currentIncludeThreadId;
}


QString ConsoleDevice::readAll() {
    QString result;

    accessMutex.lock();
    result = bufferContents.join("");
    bufferContents.clear();
    accessMutex.unlock();

    return result;
}


QString ConsoleDevice::readLine() {
    QString result;

    accessMutex.lock();
    result = bufferContents.takeFirst();
    accessMutex.unlock();

    return result;
}


void ConsoleDevice::clear() {
    if (threadBuffer != Q_NULLPTR) {
        delete[] threadBuffer;
        threadBuffer = Q_NULLPTR;
    }

    if (threadAttributes != Q_NULLPTR) {
        delete[] threadAttributes;
        threadAttributes = Q_NULLPTR;
    }

    if (messageTypes != Q_NULLPTR) {
        delete[] messageTypes;
        messageTypes = Q_NULLPTR;
    }

    bufferContents.clear();
}


void ConsoleDevice::setAllMessagesEnabled() {
    activeMessageTypes = static_cast<std::uint64_t>(-1);
}


void ConsoleDevice::setAllMessagesDisabled() {
    activeMessageTypes = 0;
}


void ConsoleDevice::setMessageTypeEnabled(ConsoleDevice::MessageType messageType, bool nowEnabled) {
    unsigned      index = static_cast<unsigned>(messageType);
    std::uint64_t mask  = std::uint64_t(1) << index;

    if (nowEnabled) {
        activeMessageTypes |= mask;
    } else {
        activeMessageTypes &= ~mask;
    }
}


void ConsoleDevice::setMessageTypeDisabled(ConsoleDevice::MessageType messageType, bool nowDisabled) {
    setMessageTypeEnabled(messageType, !nowDisabled);
}


void ConsoleDevice::setOutputMode(ConsoleDevice::OutputMode newOutputMode) {
    currentOutputMode = newOutputMode;
}


void ConsoleDevice::setMessageTypeColor(ConsoleDevice::MessageType messageType, const QString& colorString) {
    currentMessageColors.insert(messageType, colorString);
}


void ConsoleDevice::setMessageTypeIncluded(bool nowIncludeMessageType) {
    currentIncludeMessageType = nowIncludeMessageType;
}


void ConsoleDevice::setMessageTypeExcluded(bool nowExcludeMessageType) {
    currentIncludeMessageType = !nowExcludeMessageType;
}


void ConsoleDevice::setTimeStampIncluded(bool nowIncludeTimeStamp) {
    currentIncludeTimeStamp = nowIncludeTimeStamp;
}


void ConsoleDevice::setTimeStampExcluded(bool nowExcludeTimeStamp) {
    currentIncludeTimeStamp = !nowExcludeTimeStamp;
}


void ConsoleDevice::setThreadIdIncluded(bool nowIncludeThreadId) {
    currentIncludeThreadId = nowIncludeThreadId;
}


void ConsoleDevice::setThreadIdExcluded(bool nowExcludeThreadId) {
    currentIncludeThreadId = !nowExcludeThreadId;
}


void ConsoleDevice::reportImmediate(
        unsigned                   threadId,
        ConsoleDevice::MessageType messageType,
        const QString&             message
    ) {
    if (threadId > currentNumberThreads) {
        threadId = currentNumberThreads;
    }

    std::uint64_t sequence = runningSequenceNumber.fetch_add(1);

    accessMutex.lock();
    QDateTime timeStamp = QDateTime::currentDateTimeUtc();
    accessMutex.unlock();

    QString preamble;
    if (currentIncludeThreadId) {
        if (threadId == currentNumberThreads) {
            preamble = tr("---");
        } else {
            preamble = tr("%1").arg(threadId + 1, 3);
        }
    }

    if (currentIncludeTimeStamp) {
        if (currentIncludeThreadId) {
            preamble += tr(" ");
        }

        preamble += timeStamp.toString(tr("ddd-MMM-yyyy HH:mm:ss.zzz"));
    }

    if (currentIncludeMessageType) {
        if (currentIncludeThreadId || currentIncludeTimeStamp) {
            preamble += tr(" ");
        }

        switch (messageType) {
            case MessageType::INVALID:         { preamble += tr("        INVALID");    break; }
            case MessageType::INFORMATION:     { preamble += tr("    INFORMATION");    break; }
            case MessageType::DATA:            { preamble += tr("           DATA");    break; }
            case MessageType::DEBUG:           { preamble += tr("          DEBUG");    break; }
            case MessageType::BUILD_WARNING:   { preamble += tr("  BUILD_WARNING");    break; }
            case MessageType::BUILD_ERROR:     { preamble += tr("    BUILD_ERROR");    break; }
            case MessageType::RUNTIME_WARNING: { preamble += tr("RUNTIME_WARNING");    break; }
            case MessageType::RUNTIME_ERROR:   { preamble += tr("  RUNTIME_ERROR");    break; }

            default: {
                Q_ASSERT(false);
                break;
            }
        }
    }

    QString toBuffer;
    if (!preamble.isEmpty()) {
        toBuffer = tr("%1 : %2").arg(preamble).arg(message);
    } else {
        toBuffer = message;
    }

    qDebug() << toBuffer;

    if (currentOutputMode == OutputMode::HTML_COLOR) {
        toBuffer = toBuffer.toHtmlEscaped().replace(QChar(' '), QString("&nbsp;"));

        if (currentMessageColors.contains(messageType)) {
            QString colorString = currentMessageColors.value(messageType);
            toBuffer = tr("<font color=\"%1\">%2</font><br/>").arg(colorString).arg(toBuffer);
        } else {
            toBuffer += tr("<br/>");
        }
    } else {
        Q_ASSERT(currentOutputMode == OutputMode::PLAIN_TEXT);
        toBuffer += "\n";
    }

    accessMutex.lock();
    bufferContents.append(toBuffer);
    accessMutex.unlock();

    emit textAdded(toBuffer);
    emit messageReceived(timeStamp, sequence, threadId, messageType, message);
}


void ConsoleDevice::reportImmediate(ConsoleDevice::MessageType messageType, const QString& message) {
    reportImmediate(noThreadId, messageType, message);
}


void ConsoleDevice::initialize(unsigned numberThreads) {
    if (threadBuffer != Q_NULLPTR) {
        delete[] threadBuffer;
    }

    if (threadAttributes != Q_NULLPTR) {
        delete[] threadAttributes;
    }

    if (messageTypes != Q_NULLPTR) {
        delete[] messageTypes;
    }

    threadBuffer     = new QString[numberThreads + 1];
    threadAttributes = new ThreadAttributes[numberThreads + 1];
    messageTypes     = new MessageType[numberThreads + 1];

    for (unsigned i=0 ; i<=numberThreads ; ++i) {
        messageTypes[i] = MessageType::INVALID;
    }

    currentNumberThreads = numberThreads;
}


bool ConsoleDevice::startMessage(unsigned threadId, ConsoleDevice::MessageType messageType) {
    bool result;

    if (isMessageTypeEnabled(messageType) && threadBuffer != Q_NULLPTR && messageTypes != Q_NULLPTR) {
        if (threadId > currentNumberThreads) {
            threadId = currentNumberThreads;
        }

        Q_ASSERT(threadBuffer[threadId].isEmpty());
        messageTypes[threadId] = messageType;

        threadAttributes[threadId].base        = 10;
        threadAttributes[threadId].width       = 0;
        threadAttributes[threadId].pad         = ' ';
        threadAttributes[threadId].caseSetting = CaseSetting::DEFAULT_CASE;

        result = true;
    } else {
        result = false;
    }

    return result;
}


void ConsoleDevice::endMessage(unsigned threadId) {
    if (threadId > currentNumberThreads) {
        threadId = currentNumberThreads;
    }

    MessageType   messageType = messageTypes[threadId];
    QString       message     = threadBuffer[threadId];

    threadBuffer[threadId].clear();
    messageTypes[threadId] = MessageType::INVALID;

    reportImmediate(threadId, messageType, message);
}


void ConsoleDevice::append(unsigned threadId, const char* newText) {
    if (threadId > currentNumberThreads) {
        threadId = currentNumberThreads;
    }

    QString&                messageBuffer = threadBuffer[threadId];
    const ThreadAttributes& attributes    = threadAttributes[threadId];

    messageBuffer += applyAttributes(newText, attributes);
}


void ConsoleDevice::append(unsigned threadId, M::Boolean value) {
    if (threadId > currentNumberThreads) {
        threadId = currentNumberThreads;
    }

    QString& messageBuffer = threadBuffer[threadId];
    messageBuffer += applyAttributes(value ? tr("true") : tr("false"), threadAttributes[threadId]);
}


void ConsoleDevice::append(unsigned threadId, int value) {
    if (threadId > currentNumberThreads) {
        threadId = currentNumberThreads;
    }

    QString&                messageBuffer = threadBuffer[threadId];
    const ThreadAttributes& attributes    = threadAttributes[threadId];

    messageBuffer += applyAttributes(QString::number(value, attributes.base), attributes);
}


void ConsoleDevice::append(unsigned threadId, unsigned value) {
    if (threadId > currentNumberThreads) {
        threadId = currentNumberThreads;
    }

    QString&                messageBuffer = threadBuffer[threadId];
    const ThreadAttributes& attributes    = threadAttributes[threadId];

    messageBuffer += applyAttributes(QString::number(value, attributes.base), attributes);
}


void ConsoleDevice::append(unsigned threadId, unsigned long long value) {
    if (threadId > currentNumberThreads) {
        threadId = currentNumberThreads;
    }

    QString&                messageBuffer = threadBuffer[threadId];
    const ThreadAttributes& attributes    = threadAttributes[threadId];

    messageBuffer += applyAttributes(QString::number(value, attributes.base), attributes);
}


void ConsoleDevice::append(unsigned threadId, M::Integer value) {
    if (threadId > currentNumberThreads) {
        threadId = currentNumberThreads;
    }

    QString&                messageBuffer = threadBuffer[threadId];
    const ThreadAttributes& attributes    = threadAttributes[threadId];

    messageBuffer += applyAttributes(QString::number(value, attributes.base), attributes);
}


void ConsoleDevice::append(unsigned threadId, M::Real value) {
    if (threadId > currentNumberThreads) {
        threadId = currentNumberThreads;
    }

    QString&                messageBuffer = threadBuffer[threadId];
    const ThreadAttributes& attributes    = threadAttributes[threadId];

    messageBuffer += applyAttributes(QString::number(value), attributes);
}


void ConsoleDevice::append(unsigned threadId, M::Complex value) {
    if (threadId > currentNumberThreads) {
        threadId = currentNumberThreads;
    }

    QString&                messageBuffer = threadBuffer[threadId];
    const ThreadAttributes& attributes    = threadAttributes[threadId];

    M::Real r = value.real();
    M::Real i = value.imag();

    QString t;
    if (r != M::Real(0)) {
        if (i != M::Real(0)) {
            if (i > 0) {
                t = tr("%1+%2i").arg(r).arg(i);
            } else {
                t = tr("%1%2i").arg(r).arg(i);
            }
        } else {
            t = QString::number(r);
        }
    } else {
        if (i != M::Real(0)) {
            t = tr("%1i").arg(i);
        } else {
            t = tr("0");
        }
    }

    messageBuffer += applyAttributes(t, attributes);
}


void ConsoleDevice::append(unsigned threadId, const Model::Set& value) {
    if (threadId > currentNumberThreads) {
        threadId = currentNumberThreads;
    }

    QString& messageBuffer = threadBuffer[threadId];

    messageBuffer += tr("{ ");
    bool isFirst = true;
    for (Model::Set::ConstIterator it=value.constBegin(),end=value.constEnd() ; it!=end ; ++it) {
        const Model::Variant& variant = it.constReference();

        if (!isFirst) {
            messageBuffer += tr(", ");
        } else {
            isFirst = false;
        }

        append(threadId, variant);
    }

    messageBuffer += tr(" }");
}


void ConsoleDevice::append(unsigned threadId, const M::Set& value) {
    if (threadId > currentNumberThreads) {
        threadId = currentNumberThreads;
    }

    QString& messageBuffer = threadBuffer[threadId];

    messageBuffer += tr("{ ");
    bool isFirst = true;
    for (M::Set::ConstIterator it=value.constBegin(),end=value.constEnd() ; it!=end ; ++it) {
        const M::Variant& variant = it.constReference();

        if (!isFirst) {
            messageBuffer += tr(", ");
        } else {
            isFirst = false;
        }

        append(threadId, variant);
    }

    messageBuffer += tr(" }");
}


void ConsoleDevice::append(unsigned threadId, const Model::Tuple& value) {
    if (threadId > currentNumberThreads) {
        threadId = currentNumberThreads;
    }

    QString& messageBuffer = threadBuffer[threadId];

    messageBuffer += tr("( ");
    bool isFirst = true;
    for (Model::Tuple::ConstIterator it=value.constBegin(),end=value.constEnd() ; it!=end ; ++it) {
        const Model::Variant& variant = it.constReference();

        if (!isFirst) {
            messageBuffer += tr(", ");
        } else {
            isFirst = false;
        }

        append(threadId, variant);
    }

    messageBuffer += tr(" )");
}


void ConsoleDevice::append(unsigned threadId, const M::Tuple& value) {
    if (threadId > currentNumberThreads) {
        threadId = currentNumberThreads;
    }

    QString& messageBuffer = threadBuffer[threadId];

    messageBuffer += tr("( ");
    bool isFirst = true;
    for (M::Tuple::ConstIterator it=value.constBegin(),end=value.constEnd() ; it!=end ; ++it) {
        const M::Variant& variant = it.constReference();

        if (!isFirst) {
            messageBuffer += tr(", ");
        } else {
            isFirst = false;
        }

        append(threadId, variant);
    }

    messageBuffer += tr(" )");
}


void ConsoleDevice::append(unsigned threadId, const Model::Variant& value) {
    M::ValueType valueType = value.valueType();

    bool ok;
    switch (valueType) {
        case Model::ValueType::NONE: {
            append(threadId, "Variant()");
            ok = true;

            break;
        }

        case Model::ValueType::BOOLEAN: {
            Model::Boolean b = value.toBoolean(&ok);
            if (ok) {
                append(threadId, b);
            }

            break;
        }

        case Model::ValueType::INTEGER: {
            Model::Integer i = value.toInteger(&ok);
            if (ok) {
                append(threadId, i);
            }

            break;
        }

        case Model::ValueType::REAL: {
            Model::Real r = value.toReal(&ok);
            if (ok) {
                append(threadId, r);
            }

            break;
        }

        case Model::ValueType::COMPLEX: {
            Model::Complex c = value.toComplex(&ok);
            if (ok) {
                append(threadId, c);
            }

            break;
        }

        case Model::ValueType::SET: {
            Model::Set s = value.toSet(&ok);
            if (ok) {
                append(threadId, s);
            }

            break;
        }

        case Model::ValueType::TUPLE: {
            Model::Tuple t = value.toTuple(&ok);
            if (ok) {
                append(threadId, t);
            }

            break;
        }

        case Model::ValueType::NUMBER_TYPES: {
            ok = false;
            break;
        }

        default: {
            ok = false;
        }
    }

    if (!ok) {
        QString errorMessage = tr("*** BAD VARIANT (%1) ***").arg(static_cast<unsigned>(valueType));
        append(threadId, errorMessage.toLocal8Bit().data());
    }
}


void ConsoleDevice::append(unsigned threadId, const M::Variant& value) {
    M::ValueType valueType = value.valueType();

    bool ok;
    switch (valueType) {
        case M::ValueType::NONE: {
            append(threadId, "Variant()");
            ok = true;

            break;
        }

        case M::ValueType::BOOLEAN: {
            M::Boolean b = value.toBoolean(&ok);
            if (ok) {
                append(threadId, b);
            }

            break;
        }

        case M::ValueType::INTEGER: {
            M::Integer i = value.toInteger(&ok);
            if (ok) {
                append(threadId, i);
            }

            break;
        }

        case M::ValueType::REAL: {
            M::Real r = value.toReal(&ok);
            if (ok) {
                append(threadId, r);
            }

            break;
        }

        case M::ValueType::COMPLEX: {
            M::Complex c = value.toComplex(&ok);
            if (ok) {
                append(threadId, c);
            }

            break;
        }

        case M::ValueType::SET: {
            M::Set s = value.toSet(&ok);
            if (ok) {
                append(threadId, s);
            }

            break;
        }

        case M::ValueType::TUPLE: {
            M::Tuple t = value.toTuple(&ok);
            if (ok) {
                append(threadId, t);
            }

            break;
        }

        case M::ValueType::NUMBER_TYPES: {
            ok = false;
            break;
        }

        default: {
            ok = false;
        }
    }

    if (!ok) {
        QString errorMessage = tr("*** BAD VARIANT (%1) ***").arg(static_cast<unsigned>(valueType));
        append(threadId, errorMessage.toLocal8Bit().data());
    }
}


void ConsoleDevice::setBase(unsigned threadId, unsigned newBase) {
    if (threadId > currentNumberThreads) {
        threadId = currentNumberThreads;
    }

    threadAttributes[threadId].base = newBase;
}


void ConsoleDevice::setWidth(unsigned threadId, unsigned newWidth) {
    if (threadId > currentNumberThreads) {
        threadId = currentNumberThreads;
    }

    threadAttributes[threadId].width = newWidth;
}


void ConsoleDevice::setPad(unsigned threadId, char newPad) {
    if (threadId > currentNumberThreads) {
        threadId = currentNumberThreads;
    }

    threadAttributes[threadId].pad = newPad;
}


void ConsoleDevice::setUpperCase(unsigned threadId) {
    if (threadId > currentNumberThreads) {
        threadId = currentNumberThreads;
    }

    threadAttributes[threadId].caseSetting = CaseSetting::UPPER_CASE;
}


void ConsoleDevice::setLowerCase(unsigned threadId) {
    if (threadId > currentNumberThreads) {
        threadId = currentNumberThreads;
    }

    threadAttributes[threadId].caseSetting = CaseSetting::LOWER_CASE;
}


void ConsoleDevice::resetCase(unsigned threadId) {
    if (threadId > currentNumberThreads) {
        threadId = currentNumberThreads;
    }

    threadAttributes[threadId].caseSetting = CaseSetting::DEFAULT_CASE;
}


QString ConsoleDevice::applyAttributes(const QString& s, const ThreadAttributes& attributes) {
    QString result;

    switch (attributes.caseSetting) {
        case CaseSetting::DEFAULT_CASE: { result = s;            break; }
        case CaseSetting::UPPER_CASE:   { result = s.toUpper();  break; }
        case CaseSetting::LOWER_CASE:   { result = s.toLower();  break; }

        default: {
            Q_ASSERT(false);
            break;
        }
    }

    return QString("%1").arg(result, attributes.width, QChar(attributes.pad));
}
