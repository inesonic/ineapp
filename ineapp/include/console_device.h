/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref ConsoleDevice class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef CONSOLE_DEVICE_H
#define CONSOLE_DEVICE_H

#include <QBuffer>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QDateTime>
#include <QMutex>

#include <cstdint>
#include <atomic>

#include <m_intrinsic_types.h>
#include <m_set_iterator.h>
#include <m_set.h>
#include <m_tuple.h>
#include <m_variant.h>
#include <m_console.h>

#include "app_common.h"

/**
 * Class you can use to access the underlying model console.  This class handles the callbacks from the model's
 * console API in a thread safe manner and time orders and buffers the incoming messages.
 *
 * The class also provides the ability to extract the buffer contents or query the buffer contents on a line-by-line
 * basis.
 *
 * You can use this class as a QBuffer.  Received messages will be inserted into the buffer in time order.
 */
class APP_PUBLIC_API ConsoleDevice:public QObject, public M::Console::Callback {
    Q_OBJECT

    public:
        /**
         * Enumeration of supported console output modes.
         */
        enum class OutputMode {
            /**
             * Indicates messages should be plain text only.
             */
            PLAIN_TEXT,

            /**
             * Indicates messages should be HTML encoded with color.
             */
            HTML_COLOR
        };

        /**
         * Enumeration of supported message types.
         */
        typedef M::Console::MessageType MessageType;

        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        ConsoleDevice(QObject* parent = Q_NULLPTR);

        ~ConsoleDevice() override;

        /**
         * Method you can use to determine if a message type is currently enabled.
         *
         * \param[in] messageType The message type to be checked.
         *
         * \return Returns true if the message type is enabled.  Returns false if the message type is disabled.
         */
        bool isMessageTypeEnabled(MessageType messageType) const;

        /**
         * Method you can use to determine if a message type is currently disabled.
         *
         * \param[in] messageType The message type to be checked.
         *
         * \return Returns true if the message type is disabled.  Returns false if the message type is enabled.
         */
        bool isMessageTypeDisable(MessageType messageType) const;

        /**
         * Method you can use to determine the current console output mode.
         *
         * \return Returns the currently selected output mode.
         */
        OutputMode outputMode() const;

        /**
         * Method you can use to determine the HTML color to be applied to each message type.
         *
         * \param[in] messageType The message type to be queried.
         *
         * \return Returns the selected color as an HTML string.
         */
        QString messageColor(MessageType messageType) const;

        /**
         * Method you can use to determine if the message type is included as a string.
         *
         * \return Returns true if the message type is included.  Returns false if the message type is excluded.
         */
        bool includeMessageType() const;

        /**
         * Method you can use to determine if the message type is included as a string.
         *
         * \return Returns true if the type is excluded.  Returns false if the message type is included in the string.
         */
        bool excludeMessageType() const;

        /**
         * Method you can use to determine if the time stamp is included as a string.
         *
         * \return Returns true if the time stamp is included.  Returns false if the time stamp is excluded.
         */
        bool includeTimeStamp() const;

        /**
         * Method you can use to determine if the message time stamp is included as a string.
         *
         * \return Returns true if the time stamp is excluded.  Returns false if the time stamp is included in the
         *         string.
         */
        bool excludeTimeStamp() const;

        /**
         * Method you can use to determine if the thread ID is included as a string.
         *
         * \return Returns true if the thread ID is included.  Returns false if the thread ID is excluded.
         */
        bool includeThreadId() const;

        /**
         * Method you can use to determine if the message thread ID is included as a string.
         *
         * \return Returns true if the thread ID is excluded.  Returns false if the thread ID is included in the
         *         string.
         */
        bool excludeThreadId() const;

        /**
         * Method you can use to extract the entire buffer contents into a string.  The buffer contents will be
         * cleared.
         *
         * \return Returns a string containing the entire buffer contents.  An empty string will be returned if the
         *         buffer is empty.
         */
        QString readAll();

        /**
         * Method you can use to extract a single line from the buffer.
         *
         * \return Returns a string containing the oldiest line in the buffer.  An empty string will be returned if the
         *         buffer is empty.
         */
        QString readLine();

    signals:
        /**
         * Signal that is emitted whenever a new message is received.
         *
         * \param[out] timeStamp   The UTC time that the message was emitted.
         *
         * \param[out] sequence    A large integer value representing the message sequence number.
         *
         * \param[out] threadId    The ID of the thread that originated the message.  The value
         *                         M::Console::noThreadId will be used when the message did not originate from a
         *                         distinct thread.
         *
         * \param[out] messageType The type of message being reported.
         *
         * \param[out] text        The message text.
         */
        void messageReceived(
            const QDateTime& timeStamp,
            std::uint64_t    sequence,
            unsigned         threadId,
            MessageType      messageType,
            const QString&   text
        );

        /**
         * Signal that is emitted whenever text is added to the buffer.
         *
         * \param[out] newText The new text being inserted into the buffer.
         */
        void textAdded(const QString& newText);

    public slots:
        /**
         * Slot you can use to clear the internal message buffer.
         */
        void clear();

        /**
         * Slot you can use to enable all message types.
         */
        void setAllMessagesEnabled();

        /**
         * Slot you can use to disable all message types.
         */
        void setAllMessagesDisabled();

        /**
         * Slot you can use to enable or disable support for specific types of messages.
         *
         * \param[in] messageType The message type to be enabled or disabled.
         *
         * \param[in] nowEnabled  If true, the message type will be enabled.  If false, the message type will be
         *                        disabled.
         */
        void setMessageTypeEnabled(MessageType messageType, bool nowEnabled = true);

        /**
         * Slot you can use to disable or enable support for specific types of messages.
         *
         * \param[in] messageType The message type to be disabled or enabled.
         *
         * \param[in] nowDisabled If true, the message type will be disabled.  If false, the message type will be
         *                        enabled.
         */
        void setMessageTypeDisabled(MessageType messageType, bool nowDisabled = true);

        /**
         * Slot yo can use to set the desired output mode.
         *
         * \param[in] newOutputMode The newly desired output mode.
         */
        void setOutputMode(OutputMode newOutputMode);

        /**
         * Slot you can use to set the HTML message color.
         *
         * \param[in] messageType The message type.
         *
         * \param[in] colorString A string representing the HTML color code.
         */
        void setMessageTypeColor(MessageType messageType, const QString& colorString);

        /**
         * Slot you can use to include or exclude the message type as a string.
         *
         * \param[in] nowIncludeMessageType If true, the message type will be included.  If false, the message type
         *                                  will be excluded.
         */
        void setMessageTypeIncluded(bool nowIncludeMessageType = true);

        /**
         * Slot you can use to exclude or include the message type as a string.
         *
         * \param[in] nowExcludeMessageType If true, the message type will be excluded.  If false, the message type
         *                                  will be included.
         */
        void setMessageTypeExcluded(bool nowExcludeMessageType = true);

        /**
         * Slot you can use to include or exclude the time stamp as a string.
         *
         * \param[in] nowIncludeTimeStamp If true, the time stamp will be included.  If false, the time stamp will be
         *                                excluded.
         */
        void setTimeStampIncluded(bool nowIncludeTimeStamp = true);

        /**
         * Slot you can use to exclude or include the time stamp as a string.
         *
         * \param[in] nowExcludeTimeStamp If true, the time stamp will be excluded.  If false, the time stamp will be
         *                                included.
         */
        void setTimeStampExcluded(bool nowExcludeTimeStamp = true);

        /**
         * Slot you can use to include or exclude the thread ID as a string.
         *
         * \param[in] nowIncludeThreadId If true, the thread ID will be included.  If false, the thread ID will be
         *                                excluded.
         */
        void setThreadIdIncluded(bool nowIncludeThreadId = true);

        /**
         * Slot you can use to exclude or include the thread ID as a string.
         *
         * \param[in] nowExcludeThreadId If true, the thread ID will be excluded.  If false, the thread ID will be
         *                                included.
         */
        void setThreadIdExcluded(bool nowExcludeThreadId = true);

        /**
         * Method you can use to dump a message directly to the console.
         *
         * \param[in] threadId    The thread ID to assign to the message.
         *
         * \param[in] messageType The message type.
         *
         * \param[in] message     The text to report.
         */
        void reportImmediate(unsigned threadId, MessageType messageType, const QString& message);

        /**
         * Method you can use to dump a message directly to the console.  This version will report no thread ID.
         *
         * \param[in] messageType The message type.
         *
         * \param[in] message     The text to report.
         */
        void reportImmediate(MessageType messageType, const QString& message);

    private:
        /**
         * Method that is called when the model is started.  You can overload this method to perform any
         * needed per-run initialization.
         *
         * \param[in] numberThreads The number of threads being instantiated.
         */
        void initialize(unsigned numberThreads) final;

        /**
         * Method that is called when the console is about to post a new message.  This call will block until a new
         * message can be posted.
         *
         * \param[in] threadId    The thread ID.
         *
         * \param[in] messageType The message type.
         *
         * \return Returns true if the message should be reported.  Returns false if the message should be ignored.
         */
        bool startMessage(unsigned threadId, MessageType messageType) final;

        /**
         * Method that is called when the console has finished posting a message.
         *
         * \param[in] threadId The thread ID.
         */
        void endMessage(unsigned threadId) final;

        /**
         * Message that is called to receive a text string.  This call will block until the entire contents can be
         * accepted.
         *
         * \param[in] threadId The thread ID.
         *
         * \param[in] newText  A nul terminated text string to be reported.
         */
        void append(unsigned threadId, const char* newText) final;

        /**
         * Method that is called to receive a boolean value.   This call will block until the entire contents can be
         * accepted.
         *
         * \param[in] threadId The thread ID.
         *
         * \param[in] value    The value to be reported.
         */
        void append(unsigned threadId, M::Boolean value) final;

        /**
         * Method that is called to receive an signed integer value.   This call will block until the entire contents
         * can be accepted.
         *
         * \param[in] threadId The thread ID.
         *
         * \param[in] value    The value to be reported.
         */
        void append(unsigned threadId, int value) final;

        /**
         * Method that is called to receive an unsigned integer value.   This call will block until the entire contents
         * can be accepted.
         *
         * \param[in] threadId The thread ID.
         *
         * \param[in] value    The value to be reported.
         */
        void append(unsigned threadId, unsigned value) final;

        /**
         * Method that is called to receive an unsigned integer value.   This call will block until the entire contents
         * can be accepted.
         *
         * \param[in] threadId The thread ID.
         *
         * \param[in] value    The value to be reported.
         */
        void append(unsigned threadId, unsigned long long value) final;

        /**
         * Method that is called to receive an integer value.   This call will block until the entire contents can be
         * accepted.
         *
         * \param[in] threadId The thread ID.
         *
         * \param[in] value    The value to be reported.
         */
        void append(unsigned threadId, M::Integer value) final;

        /**
         * Method that is called to receive a real value.   This call will block until the entire contents can be
         * accepted.
         *
         * \param[in] threadId The thread ID.
         *
         * \param[in] value    The value to be reported.
         */
        void append(unsigned threadId, M::Real value) final;

        /**
         * Method that is called to receive a complex value.   This call will block until the entire contents can be
         * accepted.
         *
         * \param[in] threadId The thread ID.
         *
         * \param[in] value    The value to be reported.
         */
        void append(unsigned threadId, M::Complex value) final;

        /**
         * Method that is called to receive a set value.   This call will block until the entire contents can be
         * accepted.
         *
         * \param[in] threadId The thread ID.
         *
         * \param[in] value    The value to be reported.
         */
        void append(unsigned threadId, const Model::Set& value) final;

        /**
         * Method that is called to receive a set value.   This call will block until the entire contents can be
         * accepted.
         *
         * \param[in] threadId The thread ID.
         *
         * \param[in] value    The value to be reported.
         */
        void append(unsigned threadId, const M::Set& value) final;

        /**
         * Method that is called to receive a tuple value.   This call will block until the entire contents can be
         * accepted.
         *
         * \param[in] threadId The thread ID.
         *
         * \param[in] value    The value to be reported.
         */
        void append(unsigned threadId, const Model::Tuple& value) final;

        /**
         * Method that is called to receive a tuple value.   This call will block until the entire contents can be
         * accepted.
         *
         * \param[in] threadId The thread ID.
         *
         * \param[in] value    The value to be reported.
         */
        void append(unsigned threadId, const M::Tuple& value) final;

        /**
         * Method that is called to receive a variant value.   This call will block until the entire contents can be
         * accepted.
         *
         * \param[in] threadId The thread ID.
         *
         * \param[in] value    The value to be reported.
         */
        void append(unsigned threadId, const Model::Variant& value) final;

        /**
         * Method that is called to receive a variant value.   This call will block until the entire contents can be
         * accepted.
         *
         * \param[in] threadId The thread ID.
         *
         * \param[in] value    The value to be reported.
         */
        void append(unsigned threadId, const M::Variant& value) final;

        /**
         * Method that is called to set the base to use for integer values.
         *
         * \param[in] threadId The thread ID.
         *
         * \param[in] newBase The new base to be used.
         */
        void setBase(unsigned threadId, unsigned newBase) final;

        /**
         * Method that is called to set the width to use for inserted values.
         *
         * \param[in] threadId The thread ID.
         *
         * \param[in] newWidth The new width to be used.
         */
        void setWidth(unsigned threadId, unsigned newWidth) final;

        /**
         * Method that is called to set the pad character to use for inserted values.
         *
         * \param[in] threadId The thread ID.
         *
         * \param[in] newPad   The new pad character.
         */
        void setPad(unsigned threadId, char newPad) final;

        /**
         * Method that is called to indicate values should be rendered in upper-case.
         *
         * \param[in] threadId The thread ID.
         */
        void setUpperCase(unsigned threadId) final;

        /**
         * Method that is called to indicate values should be rendered in lower-case.
         *
         * \param[in] threadId The thread ID.
         */
        void setLowerCase(unsigned threadId) final;

        /**
         * Method that is called to indicate case rendering should be reset.
         *
         * \param[in] threadId The thread ID.
         */
        void resetCase(unsigned threadId) final;

    private:
        /**
         * Enumeration of support character font modes.
         */
        enum class CaseSetting {
            /**
             * Indicates no case forcing.
             */
            DEFAULT_CASE,

            /**
             * Indicates force to upper case.
             */
            UPPER_CASE,

            /**
             * Indicates force lower case.
             */
            LOWER_CASE
        };

        /**
         * Structure used to track attributes.
         */
        struct ThreadAttributes {
            unsigned    base;
            unsigned    width;
            char        pad;
            CaseSetting caseSetting;
        };

        /**
         * Method that forces the case, pad, and width attributes on a string.
         *
         * \param[in] s          The string to be converted.
         *
         * \param[in] attributes The attributes to apply.
         *
         * \return Returns the converted string.
         */
        static QString applyAttributes(const QString& s, const ThreadAttributes& attributes);

        /**
         * Mutex used to control access to shared resources.
         */
        QMutex accessMutex;

        /**
         * String list holding the buffer contents.
         */
        QStringList bufferContents;

        /**
         * The number of threads reported by the model during start-up.
         */
        unsigned currentNumberThreads;

        /**
         * List of buffers, per thread.
         */
        QString* threadBuffer;

        /**
         * Array of thread specific attributes.
         */
        ThreadAttributes* threadAttributes;

        /**
         * List of message types, per thread.
         */
        MessageType* messageTypes;

        /**
         * An atomic integer used to track relative ordering of messages.
         */
        std::atomic_int64_t runningSequenceNumber;

        /**
         * Mask of active message types.
         */
        std::uint64_t activeMessageTypes;

        /**
         * The current output mode.
         */
        OutputMode currentOutputMode;

        /**
         * Map of message colors by message type.
         */
        QMap<MessageType, QString> currentMessageColors;

        /**
         * Boolean value indicating if we should include the message type as a string.
         */
        bool currentIncludeMessageType;

        /**
         * Boolean value indicating if we should include the time stamp as a string.
         */
        bool currentIncludeTimeStamp;

        /**
         * Boolean value that indicates if we should report the thread ID.
         */
        bool currentIncludeThreadId;
};

#endif
