/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref Application class.
***********************************************************************************************************************/

#include <QtGlobal>
#include <QApplication>
#include <QTimer>
#include <QStyle>
#include <QSharedPointer>
#include <QString>
#include <QSize>
#include <QDateTime>
#include <QGridLayout>
#include <QSpacerItem>
#include <QByteArray>
#include <QUrl>
#include <QNetworkInterface>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QSslPreSharedKeyAuthenticator>
#include <QSslError>
#include <QScreen>
#include <QMessageBox>
#include <QFontDatabase>

#include <cstdlib>
#include <iostream>

#include <QDebug>

#include <wh_web_hook.h>

#include <ud_usage_data.h>

#include <eqt_application.h>
#include <eqt_graphics_item.h>
#include <eqt_programmatic_application.h>
#include <eqt_global_setting.h>
#include <eqt_recent_files_data.h>
#include <eqt_tool_button.h>
#include <eqt_message_dialog.h>

#include <ld_format_structures.h>
#include <ld_environment.h>
#include <ld_configure.h>
#include <ld_element.h>
#include <ld_root_element.h>
#include <ld_character_format.h>
#include <ld_brace_conditional_format.h>

#if (defined(Q_OS_DARWIN))

    #include "macos_linker_locator.h"

#elif (!defined(Q_OS_WIN) && !defined(Q_OS_LINUX))

    #error Unknown platform

#endif

#include "application_settings.h"
#include "metatypes.h"
#include "version.h"
#include "scm_version.h"
#include "main_window.h"
#include "splash_screen.h"
#include "registrar.h"
#include "plug_in_manager.h"
#include "document_file_dialog.h"
#include "image_file_dialog.h"
#include "clipboard.h"
#include "build_execute_state_machine.h"
#include "console_device.h"
#include "configure.h"
#include "scene_units.h"
#include "tool_button_sizing_dialog.h"
#include "application.h"

const char Application::environmentVariableEnableDebug[]     = "INESONIC_DEBUG";
const char Application::environmentVariableEnvironmentType[] = "INESONIC_ENVIRONMENT_MODE";
const char Application::globalMathFontName[]                 = "STIXMath";

#if (defined(Q_OS_WIN))

    static const char platformStyleSheet[] =
        "QAbstractScrollArea#pageView {"
        "    margin: 0px;"
        "    border-width: 0px;"
        "    padding: 0px;"
        "}\n"
        "QToolButton:hover {"
        "    border: 2px solid #A3C2D3;"
        "    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #FFFFFF, stop: 1 #EDF7FC);"
        "}\n"
        "QToolButton {"
        "    border: 2px solid #DBDBDB;"
        "    background-color: solid #FBFBFB;"
        "}\n";

#elif (defined(Q_OS_LINUX))

    static const char platformStyleSheet[] =
        "QAbstractScrollArea#pageView {"
        "    margin: 0px;"
        "    border-width: 0px;"
        "    padding: 0px;"
        "    background-color: rgb(0,0,0,0);"
        "}\n"
        "QToolButton:hover {"
        "    border: 2px solid #A3C2D3;"
        "    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #FFFFFF, stop: 1 #EDF7FC);"
        "}\n"
        "QToolButton {"
        "    border: 2px solid #DBDBDB;"
        "    background-color: solid #FBFBFB;"
        "}\n";

#elif (defined(Q_OS_DARWIN))

    static const char platformStyleSheet[] =
        "QGroupBox#groupBox.Home.File { border: 1px; }\n"
        "QGraphicsView#pageView {"
        "    margin: 0px;"
        "    border-width: 0px;"
        "    padding: 0px;"
        "}\n"
        "QGraphicsView#editor {"
        "    background-color: rgb(128,128,0);\n"
        "}\n";
//        "QToolButton {"
//        "    border: 2px solid #A3C2D3;"
//        "    border-radius: 10px;"
//        "    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #FFFFFF, stop: 1 #EBF6FE);"
//        "}\n";

#else

    #error Unknown platform

#endif

const unsigned Application::numberBaseInitializationSteps = 8;

Application::Application(
        int&   applicationCount,
        char** applicationValues
    ):EQt::ProgrammaticApplication(
        applicationCount,
        applicationValues,
        APPLICATION_NAME,
        VERSION,
        ORGANIZATION_NAME,
        ORGANIZATION_DOMAIN
    ) {
    currentRegistrar                = Q_NULLPTR;
    currentPlugInManager            = Q_NULLPTR;
    currentDocumentFileDialog       = Q_NULLPTR;
    currentImageFileDialog          = Q_NULLPTR;
    currentClipboard                = Q_NULLPTR;
    currentBuildExecuteStateMachine = Q_NULLPTR;
    currentConsoleDevice            = Q_NULLPTR;
    currentUsageData                = Q_NULLPTR;

    const std::uint8_t timeDeltaSecret[] = TIME_DELTA_SECRET;
    Wh::WebHook::setTimestampSecret(
        QByteArray(reinterpret_cast<const char*>(timeDeltaSecret), sizeof(timeDeltaSecret))
    );
    Wh::WebHook::setTimestampUrl(QUrl(TIME_DELTA_WEBHOOK_URL));

    registerMetaTypes();
    checkDebugSupport();
    setupEnvironment();
    customizeForPlatform();

    Q_INIT_RESOURCE(ineapp);

    screenResizeTimer = new QTimer(this);
    screenResizeTimer->setSingleShot(true);

    unsigned numberPlugIns = (
          Ld::Environment::plugInDirectories().count()
        + Ld::Environment::userPlugInDirectories().count()
    );

    #if (defined(Q_OS_DARWIN))

        macosLinkerLocator = new MacOSLinkerLocator(this);

    #elif (!defined(Q_OS_WIN) && !defined(Q_OS_LINUX))

        #error Unknown platform

    #endif

    splashScreen = new SplashScreen(
        numberBaseInitializationSteps + numberPlugIns,
        "Customer Name", //licenseManager->customerName(),
        "Company Name", //licenseManager->companyName(),
        QString(), // Licensee name (add for group licenses)
        "Perpetual" // licenseManager->expirationDateTime().date()
    );

    currentToolButtonSizingDialog = new ToolButtonSizingDialog;

    connect(this, &UniqueApplication::startUserInterface, this, &Application::startUserInterface);

    connect(
        this,
        SIGNAL(instanceLoaded(const QStringList&)),
        this,
        SLOT(instanceLoaded(const QStringList&))
    );

    connect(
        Application::networkAccessManager(),
        SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
        this,
        SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*))
    );

    connect(
        Application::networkAccessManager(),
        SIGNAL(preSharedKeyAuthenticationRequired(QNetworkReply*, QSslPreSharedKeyAuthenticator*)),
        this,
        SLOT(preSharedKeyAuthenticationRequired(QNetworkReply*, QSslPreSharedKeyAuthenticator*))
    );

    connect(
        Application::networkAccessManager(),
        SIGNAL(proxyAuthenticationRequired(const QNetworkProxy, QAuthenticator*)),
        this,
        SLOT(proxyAuthenticationRequired(const QNetworkProxy, QAuthenticator*))
    );

    connect(
        Application::networkAccessManager(),
        SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError>)),
        this,
        SLOT(sslErrors(QNetworkReply*, const QList<QSslError>))
    );

    connect(this, SIGNAL(primaryScreenChanged(QScreen*)), this, SLOT(primaryScreenHasChanged(QScreen*)));
    currentPrimaryScreen = primaryScreen();

    connect(
        currentPrimaryScreen,
        SIGNAL(physicalDotsPerInchChanged(qreal)), // Using qreal because Qt may use float or double depending on build.
        this,
        SLOT(physicalDotsPerInchChanged(qreal))
    );
    connect(
        currentPrimaryScreen,
        SIGNAL(logicalDotsPerInchChanged(qreal)), // Using qreal because Qt may use float or double depending on build.
        this,
        SLOT(logicalDotsPerInchChanged(qreal))
    );
    connect(
        currentPrimaryScreen,
        SIGNAL(orientationChanged(Qt::ScreenOrientation)),
        this,
        SLOT(screenOrientationChanged(Qt::ScreenOrientation))
    );
    connect(
        currentPrimaryScreen,
        SIGNAL(physicalSizeChanged(QSizeF)),
        this,
        SLOT(screenSizeChanged(const QSizeF&))
    );

    connect(screenResizeTimer, &QTimer::timeout, this, &Application::screenResized);
}


Application::~Application() {
    MainWindow::deleteAllMainWindows();

    if (currentRegistrar != Q_NULLPTR) {
        delete currentRegistrar;
    }

    if (currentDocumentFileDialog != Q_NULLPTR) {
        delete currentDocumentFileDialog;
    }

    if (currentImageFileDialog != Q_NULLPTR) {
        delete currentImageFileDialog;
    }

    if (currentToolButtonSizingDialog != Q_NULLPTR) {
        delete currentToolButtonSizingDialog;
    }
}


unsigned long long Application::applicationBuildNumber() {
    return SCM_VERSION_NUMBER;
}


QString Application::applicationBuildString() {
    unsigned long long buildNumber = Application::applicationBuildNumber();
    return QString("%1.%2").arg(buildNumber).arg(SCM_VERSION_RELEASE_EPOCH);
}


Ud::UsageData* Application::usageData() {
    Application* application = Application::instance();

    if (application->currentUsageData == Q_NULLPTR) {
        static const std::uint8_t usageStatisticsHmacSecret[] = USAGE_STATISTICS_HMAC_SECRET;
        static const unsigned     usageStatisticsHmacSecretSize = sizeof(usageStatisticsHmacSecret);

        application->currentUsageData = new Ud::UsageData(
            settings(),
            networkAccessManager(),
            QByteArray(reinterpret_cast<const char*>(usageStatisticsHmacSecret), usageStatisticsHmacSecretSize),
            QUrl(USAGE_STATISTICS_WEBHOOK_URL),
            application
        );

        application->currentUsageData->setSettingsGroup("usageData");
        application->currentUsageData->setInterval(USAGE_STATISTICS_REPORT_PERIOD);
    }

    return application->currentUsageData;
}


Registrar* Application::registrar() {
    Application* application = Application::instance();

    if (application->currentRegistrar == Q_NULLPTR) {
        application->currentRegistrar = new Registrar;
    }

    return application->currentRegistrar;
}


PlugInManager* Application::plugInManager() {
    Application* application = Application::instance();

    if (application->currentPlugInManager == Q_NULLPTR) {
        application->currentPlugInManager = new PlugInManager(application);
        connect(
            application->currentPlugInManager,
            &PlugInManager::aboutToLoadPlugIn,
            application,
            &Application::aboutToLoadPlugIn
        );
    }

    return application->currentPlugInManager;
}


DocumentFileDialog* Application::documentFileDialog() {
    Application* application = Application::instance();

    if (application->currentDocumentFileDialog == Q_NULLPTR) {
        application->currentDocumentFileDialog = new DocumentFileDialog;
        application->currentDocumentFileDialog->setDefaultSuffix(FILE_EXTENSION);
        application->currentDocumentFileDialog->loadSettings();
    }

    return application->currentDocumentFileDialog;
}


ImageFileDialog* Application::imageFileDialog() {
    Application* application = Application::instance();

    if (application->currentImageFileDialog == Q_NULLPTR) {
        application->currentImageFileDialog = new ImageFileDialog;
        application->currentImageFileDialog->loadSettings();
    }

    return application->currentImageFileDialog;
}


Clipboard* Application::clipboard() {
    Application* application = Application::instance();
    if (application->currentClipboard == Q_NULLPTR) {
        application->currentClipboard = new Clipboard(application);
    }

    return application->currentClipboard;
}


BuildExecuteStateMachine* Application::buildExecuteStateMachine() {
    Application* application = Application::instance();
    if (application->currentBuildExecuteStateMachine == Q_NULLPTR) {
        application->currentBuildExecuteStateMachine = new BuildExecuteStateMachine(application);
    }

    return application->currentBuildExecuteStateMachine;
}


ConsoleDevice* Application::consoleDevice() {
    Application* application = Application::instance();
    if (application->currentConsoleDevice == Q_NULLPTR) {
        application->currentConsoleDevice = new ConsoleDevice(application);
        application->currentConsoleDevice->setOutputMode(ConsoleDevice::OutputMode::HTML_COLOR);
        application->currentConsoleDevice->setThreadIdIncluded();
        application->currentConsoleDevice->setTimeStampIncluded();
        application->currentConsoleDevice->setMessageTypeIncluded();
    }

    return application->currentConsoleDevice;
}


QSize Application::physicalDpi() {
    return instance()->currentScreenPhysicalDpi;
}


QSize Application::logicalDpi() {
    return instance()->currentScreenLogicalDpi;
}


double Application::fontScaleFactor() {
    return instance()->currentFontScaleFactor;
}


const QFont& Application::mathFont() {
    return Application::instance()->currentMathFont;
}


bool Application::debugModeEnabled() {
    return instance()->currentIncludeDebug;
}


int Application::exec() {
    int exitStatus;

    startNormalApplication();
    exitStatus = EQt::ProgrammaticApplication::exec();

    if (currentUsageData != nullptr) {
        if (exitStatus == 0) {
            currentUsageData->adjustEvent(QString("ApplicationExit_Ok"));
        } else {
            currentUsageData->adjustEvent(QString("ApplicationExit_Error"));
        }
    }

    return exitStatus;
}


void Application::startUserInterface(EQt::UniqueApplication::StartupCondition) {
    splashScreen->startedInitializationStep(tr("Initializaing core language definition"));
    processEvents();
    initializeCoreLanguageDefinition();

    initializePlugIns();

    splashScreen->startedInitializationStep(tr("Finalizing language definition"));
    processEvents();
    finalizeLanguageDefinition();

    splashScreen->startedInitializationStep(tr("Loading application fonts"));
    processEvents();
    loadApplicationFonts();
    setApplicationDefaultFonts();

    splashScreen->startedInitializationStep(tr("Loading application settings"));
    processEvents();
    createGlobalSettingData();

    splashScreen->startedInitializationStep(tr("Determining screen settings"));
    processEvents();
    updateScreenSettings();

    splashScreen->startedInitializationStep(tr("Configuring user interface"));
    processEvents();
    runBuilders();

    splashScreen->startedInitializationStep(tr("Loading global settings"));
    processEvents();
    loadGlobalSettings();

    splashScreen->startedInitializationStep(tr("Starting user interface"));
    processEvents();
    MainWindow* mainWindow = new MainWindow;
    mainWindow->show();

    splashScreen->initializationCompleted();
    splashScreen = Q_NULLPTR;

    usageData()->adjustEvent(QString("ApplicationStart"));

    showWelcomeMessage();
}


void Application::instanceLoaded(const QStringList& parameters) {
    qDebug() << "Parameters: " << parameters;
}


void Application::authenticationRequired(QNetworkReply*, QAuthenticator*) {
    /* TODO: Fix this to work with SSL/TLS connections. */
    qDebug() << "Fix: UsageData::authenticationRequired slot.";
}


void Application::preSharedKeyAuthenticationRequired(QNetworkReply*, QSslPreSharedKeyAuthenticator*) {
    /* TODO: Fix this to work with SSL/TLS connections. */
    qDebug() << "Information: Pre-shared key authentication required.";
}


void Application::proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*) {
    /* TODO: Fix this to properly deal with proxy servers. */
    qDebug() << "Information: Proxy authentication required.";
}


void Application::sslErrors(QNetworkReply*, const QList<QSslError>&) {
    /* TODO: Fix this to work with SSL/TLS connections. */
    qDebug() << "Information: SSL errors detected.";
}


void Application::aboutToLoadPlugIn(const QString& loadPath) {
    if (splashScreen != Q_NULLPTR && splashScreen->isVisible()) {
        QFileInfo fileInformation(loadPath);
        QString   plugInName = fileInformation.baseName();

        splashScreen->startedInitializationStep(tr("Loading plug-in %1").arg(plugInName));
    }
}


void Application::allWindowsClosed() {
    saveGlobalSettings();
}


void Application::primaryScreenHasChanged(QScreen*) {
    if (primaryScreen() != currentPrimaryScreen) {
        disconnect(
            currentPrimaryScreen,
            SIGNAL(physicalDotsPerInchChanged(float)),
            this,
            SLOT(physicalDotPerInchChanged(float))
        );
        disconnect(
            currentPrimaryScreen,
            SIGNAL(logicalDotsPerInchChanged(float)),
            this,
            SLOT(logicalDotsPerInchChanged(float))
        );
        disconnect(
            currentPrimaryScreen,
            SIGNAL(orientationChanged(Qt::ScreenOrientation)),
            this,
            SLOT(screenOrientationChanged(Qt::ScreenOrientation))
        );
        disconnect(
            currentPrimaryScreen,
            SIGNAL(physicalSizeChanged(QSizeF)),
            this,
            SLOT(screenSizeChanged(const QSizeF&))
        );

        currentPrimaryScreen = primaryScreen();

        connect(
            currentPrimaryScreen,
            SIGNAL(physicalDotsPerInchChanged(float)),
            this,
            SLOT(physicalDotPerInchChanged(float))
        );
        connect(
            currentPrimaryScreen,
            SIGNAL(logicalDotsPerInchChanged(float)),
            this,
            SLOT(logicalDotsPerInchChanged(float))
        );
        connect(
            currentPrimaryScreen,
            SIGNAL(orientationChanged(Qt::ScreenOrientation)),
            this,
            SLOT(screenOrientationChanged(Qt::ScreenOrientation))
        );
        connect(
            currentPrimaryScreen,
            SIGNAL(physicalSizeChanged(QSizeF)),
            this,
            SLOT(screenSizeChanged(const QSizeF&))
        );
    }

    updateScreenSettings();
}


void Application::physicalDotsPerInchChanged(qreal) {
    updateScreenSettings();
}


void Application::logicalDotsPerInchChanged(qreal) {
    updateScreenSettings();
}


void Application::screenOrientationChanged(Qt::ScreenOrientation) {
    updateScreenSettings();
}


void Application::screenSizeChanged(const QSizeF&) {
    updateScreenSettings();
}


void Application::startNormalApplication() {
    #if (defined(Q_OS_DARWIN))

        bool success = macosLinkerLocator->locateLinker();
        if (!success) {
            ::exit(1);
        }

    #elif (!defined(Q_OS_WIN) && !defined(Q_OS_LINUX))

        #error Unknown platform

    #endif

    QString splashScreenEnabled = qEnvironmentVariable("INESONIC_SPLASH_SCREEN", "ENABLED");
    if (splashScreenEnabled.toLower() != QString("disabled")) {
        splashScreen->show();
    }
}


void Application::customizeForPlatform() {
    #if (defined(Q_OS_WIN))

        setAttribute(Qt::AA_DontShowIconsInMenus, true);
        setStyleSheet(platformStyleSheet);

    #elif (defined(Q_OS_LINUX))

        setStyleSheet(platformStyleSheet);

    #elif (defined(Q_OS_DARWIN))

        setAttribute(Qt::AA_DontShowIconsInMenus, true);
        setStyleSheet(platformStyleSheet);

    #else

        #error Unknown platform

    #endif
}


void Application::createGlobalSettingData() {
    Ld::FormatPointer              downcastPageFormat = Ld::PageFormat::defaultPageFormat.clone();
    QSharedPointer<Ld::PageFormat> defaultPageFormat(downcastPageFormat.dynamicCast<Ld::PageFormat>());

    EQt::GlobalSetting* pageFormatGlobalSetting = new EQt::GlobalSetting(
        "default_page_format",
        defaultPageFormat->toString(),
        this
    );

    Ld::RootElement::setApplicationDefaultPageFormat(defaultPageFormat);

    EQt::GlobalSetting* textFontGlobalSetting = new EQt::GlobalSetting(
        "default_text_font",
        Ld::CharacterFormat::applicationDefaultTextFont()->toString(),
        this
    );

    EQt::GlobalSetting* mathFontGlobalSetting = new EQt::GlobalSetting(
        "default_math_font",
        Ld::CharacterFormat::applicationDefaultMathFont()->toString(),
        this
    );

    EQt::GlobalSetting* mathIdentifierFontGlobalSetting = new EQt::GlobalSetting(
        "default_math_identifier_font",
        Ld::CharacterFormat::applicationDefaultMathIdentifierFont()->toString(),
        this
    );

    EQt::GlobalSetting* mathFunctionFontGlobalSetting = new EQt::GlobalSetting(
        "default_math_function_font",
        Ld::CharacterFormat::applicationDefaultMathFunctionFont()->toString(),
        this
    );

    EQt::GlobalSetting* braceConditionalFontGlobalSetting = new EQt::GlobalSetting(
        "brace_conditional_font",
        Ld::BraceConditionalFormat::applicationDefaultBraceConditionalFormat()->toString(),
        this
    );

    EQt::GlobalSetting* braceConditionalShowElseClause = new EQt::GlobalSetting(
        "brace_conditional_show_else_clause",
        true,
        this
    );

    EQt::GlobalSetting* defaultRngSetting = new EQt::GlobalSetting(
        "default_rng",
        Ld::RootElement::toString(Ld::RootElement::defaultRngType),
        this
    );

    connect(
        pageFormatGlobalSetting,
        static_cast<void (EQt::GlobalSetting::*)(const QString&)>(&EQt::GlobalSetting::changed),
        [=](const QString& identifier) {
            bool ok;
            Ld::PageFormat newPageFormat = Ld::PageFormat::fromString(identifier, &ok);
            if (ok) {
                QSharedPointer<Ld::PageFormat> format = newPageFormat.clone().dynamicCast<Ld::PageFormat>();
                Ld::RootElement::setApplicationDefaultPageFormat(format);
            }
        }
    );

    connect(
        textFontGlobalSetting,
        static_cast<void (EQt::GlobalSetting::*)(const QString&)>(&EQt::GlobalSetting::changed),
        [=](const QString& fontString) {
            bool ok;
            Ld::CharacterFormat newFormat(Ld::CharacterFormat::fromString(fontString, &ok));
            if (ok) {
                Ld::CharacterFormat::setApplicationDefaultTextFont(
                    newFormat.clone().dynamicCast<Ld::CharacterFormat>()
                );
            }
        }
    );

    connect(
        mathFontGlobalSetting,
        static_cast<void (EQt::GlobalSetting::*)(const QString&)>(&EQt::GlobalSetting::changed),
        [=](const QString& fontString) {
            bool ok;
            Ld::CharacterFormat newFormat(Ld::CharacterFormat::fromString(fontString, &ok));
            if (ok) {
                Ld::CharacterFormat::setApplicationDefaultMathFont(
                    newFormat.clone().dynamicCast<Ld::CharacterFormat>()
                );
            }
        }
    );

    connect(
        mathIdentifierFontGlobalSetting,
        static_cast<void (EQt::GlobalSetting::*)(const QString&)>(&EQt::GlobalSetting::changed),
        [=](const QString& fontString) {
            bool ok;
            Ld::CharacterFormat newFormat(Ld::CharacterFormat::fromString(fontString, &ok));
            if (ok) {
                Ld::CharacterFormat::setApplicationDefaultMathIdentifierFont(
                    newFormat.clone().dynamicCast<Ld::CharacterFormat>()
                );
            }
        }
    );

    connect(
        mathFunctionFontGlobalSetting,
        static_cast<void (EQt::GlobalSetting::*)(const QString&)>(&EQt::GlobalSetting::changed),
        [=](const QString& fontString) {
            bool ok;
            Ld::CharacterFormat newFormat(Ld::CharacterFormat::fromString(fontString, &ok));
            if (ok) {
                Ld::CharacterFormat::setApplicationDefaultMathFunctionFont(
                    newFormat.clone().dynamicCast<Ld::CharacterFormat>()
                );
            }
        }
    );

    connect(
        braceConditionalFontGlobalSetting,
        static_cast<void (EQt::GlobalSetting::*)(const QString&)>(&EQt::GlobalSetting::changed),
        [=](const QString& fontString) {
            bool ok;
            Ld::BraceConditionalFormat newFormat(Ld::BraceConditionalFormat::fromString(fontString, &ok));
            if (ok) {
                Ld::BraceConditionalFormat::setApplicationDefaultBraceConditionalFormat(
                    newFormat.clone().dynamicCast<Ld::BraceConditionalFormat>()
                );
            }
        }
    );

    connect(
        braceConditionalShowElseClause,
        static_cast<void (EQt::GlobalSetting::*)(bool)>(&EQt::GlobalSetting::changed),
        [=](bool includeElseConditional) {
            Ld::BraceConditionalFormat::setApplicationDefaultShowElseClause(includeElseConditional);
        }
    );

    connect(
        defaultRngSetting,
        static_cast<void (EQt::GlobalSetting::*)(const QString&)>(&EQt::GlobalSetting::changed),
        [=](const QString& rngTypeString) {
            bool ok;
            Ld::RootElement::RngType rngType = Ld::RootElement::toRngType(rngTypeString, &ok);
            if (ok) {
                Ld::RootElement::setApplicationDefaultRngType(rngType);
            }
        }
    );

    new EQt::GlobalSetting("hide_welcome_screen", false, this);

    new EQt::GlobalSetting("use_custom_physical_display_resolution", false, this);
    new EQt::GlobalSetting(
        "custom_physical_display_resolution",
        QSize(primaryScreen()->physicalDotsPerInchX(), primaryScreen()->physicalDotsPerInchY()),
        this
    );

    new EQt::GlobalSetting("use_custom_logical_display_resolution", false, this);
    new EQt::GlobalSetting(
        "custom_logical_display_resolution",
        QSize(primaryScreen()->logicalDotsPerInchX(), primaryScreen()->logicalDotsPerInchY()),
        this
    );
}


void Application::initializeCoreLanguageDefinition() {
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    QList<QNetworkInterface>::const_iterator interfaceIterator    = interfaces.constBegin();
    QList<QNetworkInterface>::const_iterator interfaceEndIterator = interfaces.constEnd();

    std::uint64_t macAddress = 0;
    while (interfaceIterator != interfaceEndIterator && macAddress == 0) {
        QStringList bytes = interfaceIterator->hardwareAddress().split(":");
        QStringList::const_iterator byteIterator    = bytes.constBegin();
        QStringList::const_iterator byteEndIterator = bytes.constEnd();
        bool                        isOk            = true;

        while (byteIterator != byteEndIterator && isOk) {
            unsigned byte = byteIterator->toUInt(&isOk, 16);
            if (isOk && byte > 255) {
                isOk = false;
            }

            if (isOk) {
                macAddress = (macAddress << 8) | byte;
            }

            ++byteIterator;
        }

        ++interfaceIterator;
    }

    // MAC address is mangled to increase entropy in lower 32 bits.
    // MAC address is then used as the upper 32-bits of the ID, the seconds since Epoch is truncated and placed in the
    // lower 32-bits.

    std::uint64_t mangledAddress    = macAddress ^ (macAddress >> 32);
    std::uint64_t secondsSinceEpoch = QDateTime::currentSecsSinceEpoch();
    std::uint64_t uniqueSystemId    = (mangledAddress << 32) + (secondsSinceEpoch & 0xFFFFFFFF);

    Ld::Element::setAutoDeleteVisuals(false); // GUI will be responsible for destroying visuals.
    Ld::Configure::configure(uniqueSystemId, usageData());
    configure(Application::registrar());

    Ld::Configure::setDebugOutputEnabled(debugModeEnabled());

    #if (defined(Q_OS_DARWIN))

        Ld::Configure::configureLinker(macosLinkerLocator->linkerPath(), macosLinkerLocator->linkerExecutable());

    #elif (!defined(Q_OS_WIN) && !defined(Q_OS_LINUX))

        #error Unknown platform

    #endif
}


void Application::initializePlugIns() {
    QList<QString> plugInList = Ld::Environment::plugInFiles();
    plugInManager()->loadPlugIns(plugInList, registrar());
}


void Application::loadApplicationFonts() {
    QStringList   fontSearchPathsList = Ld::Environment::fontSearchPaths();
    QSet<QString> fontSearchPaths(fontSearchPathsList.begin(), fontSearchPathsList.end());

    QStringList plugInFontSearchPathsList = Ld::Environment::plugInFontSearchPaths();
    fontSearchPaths += QSet<QString>(plugInFontSearchPathsList.begin(), plugInFontSearchPathsList.end());

    for (  QSet<QString>::const_iterator fontSearchPathIterator    = fontSearchPaths.constBegin(),
                                         fontSearchPathEndIterator = fontSearchPaths.constEnd()
         ; fontSearchPathIterator != fontSearchPathEndIterator
         ; ++fontSearchPathIterator
        ) {
        QString fontDirectory = *fontSearchPathIterator;
        loadFontsUnder(fontDirectory);
    }

    currentMathFont = QFont(globalMathFontName);
}


void Application::finalizeLanguageDefinition() {
    Ld::Configure::configureFinal();
}


void Application::setApplicationDefaultFonts() {
    QSharedPointer<Ld::CharacterFormat> textFormat = Ld::Format::create(Ld::CharacterFormat::formatName)
                                                     .dynamicCast<Ld::CharacterFormat>();

    textFormat->setFamily(tr("Times New Roman"));
    textFormat->setFontSize(12);

    Ld::CharacterFormat::setApplicationDefaultTextFont(textFormat);

    QSharedPointer<Ld::CharacterFormat> mathFormat = Ld::Format::create(Ld::CharacterFormat::formatName)
                                                     .dynamicCast<Ld::CharacterFormat>();

    mathFormat->setFamily(tr("STIXGeneral"));
    mathFormat->setFontSize(12);

    Ld::CharacterFormat::setApplicationDefaultMathFont(mathFormat);

    QSharedPointer<Ld::CharacterFormat> mathIdentifierFormat = Ld::Format::create(Ld::CharacterFormat::formatName)
                                                               .dynamicCast<Ld::CharacterFormat>();

    mathIdentifierFormat->setFamily(tr("STIXGeneral"));
    mathIdentifierFormat->setFontSize(12);
    mathIdentifierFormat->setItalics(true);

    Ld::CharacterFormat::setApplicationDefaultMathIdentifierFont(mathIdentifierFormat);

    QSharedPointer<Ld::CharacterFormat> mathFunctionFormat = Ld::Format::create(Ld::CharacterFormat::formatName)
                                                             .dynamicCast<Ld::CharacterFormat>();

    mathFunctionFormat->setFamily(tr("STIXGeneral"));
    mathFunctionFormat->setFontSize(12);

    Ld::CharacterFormat::setApplicationDefaultMathFunctionFont(mathFunctionFormat);

    QSharedPointer<Ld::BraceConditionalFormat>
        braceConditionalFormat = Ld::Format::create(Ld::BraceConditionalFormat::formatName)
                                 .dynamicCast<Ld::BraceConditionalFormat>();

    braceConditionalFormat->setFamily(tr("Times New Roman"));
    braceConditionalFormat->setFontSize(12);
    braceConditionalFormat->setConditionPreambleString(tr("if "));
    braceConditionalFormat->setElseConditionString(tr("otherwise"));

    Ld::BraceConditionalFormat::setApplicationDefaultBraceConditionalFormat(braceConditionalFormat);
}


void Application::updateScreenSettings() {
    QSize newPhysicalDpiSettings;
    QSize newLogicalDpiSettings;

    if (EQt::GlobalSetting::setting("use_custom_physical_display_resolution")->toBool() == true) {
        newPhysicalDpiSettings = EQt::GlobalSetting::setting("custom_physical_display_resolution")->toSize();
    } else {
        newPhysicalDpiSettings = QSize(
            currentPrimaryScreen->physicalDotsPerInchX(),
            currentPrimaryScreen->physicalDotsPerInchY()
        );
    }

    if (EQt::GlobalSetting::setting("use_custom_logical_display_resolution")->toBool() == true) {
        newLogicalDpiSettings = EQt::GlobalSetting::setting("custom_logical_display_resolution")->toSize();
    } else {
        newLogicalDpiSettings = QSize(
            currentPrimaryScreen->logicalDotsPerInchX(),
            currentPrimaryScreen->logicalDotsPerInchY()
        );
    }

    currentScreenPhysicalDpi = newPhysicalDpiSettings;
    currentScreenLogicalDpi  = newLogicalDpiSettings;

    double averagePhysical = (currentScreenPhysicalDpi.width() + currentScreenPhysicalDpi.height()) / 2.0;
    double averageLogical  = (currentScreenLogicalDpi.width() + currentScreenLogicalDpi.height()) / 2.0;

    currentFontScaleFactor = averagePhysical / averageLogical;
    SceneUnits::update(physicalDpi());

    updateToolButtonSizes();

    emit screenSettingsChanged(currentScreenPhysicalDpi, currentScreenLogicalDpi, EQt::ToolButton::globalSize());
    screenResizeTimer->start(1);
}


void Application::updateToolButtonSizes() {
    unsigned recommendedButtonHeight = currentToolButtonSizingDialog->determineRecommendedButtonSize();
    EQt::ToolButton::setGlobalSize(recommendedButtonHeight);
}


void Application::loadGlobalSettings() {
    EQt::GlobalSetting::loadSettings();

    usageData()->loadSettings();
    recentFilesData()->loadSettings();
}


void Application::saveGlobalSettings() {
    if (currentDocumentFileDialog != Q_NULLPTR) {
        currentDocumentFileDialog->saveSettings();
    }

    if (currentImageFileDialog != Q_NULLPTR) {
        currentImageFileDialog->saveSettings();
    }

    recentFilesData()->saveSettings();
    usageData()->saveSettings();

    EQt::GlobalSetting::saveSettings();
}


void Application::checkDebugSupport() {
    QString debugSetting = qEnvironmentVariable(environmentVariableEnableDebug);
    if (debugSetting.isEmpty()) {
        currentIncludeDebug = false;
    } else {
        debugSetting = debugSetting.trimmed().toLower();

        bool      isInteger;
        long long integerValue = debugSetting.toLongLong(&isInteger);

        if (isInteger) {
            currentIncludeDebug = (integerValue != 0);
        } else {
            currentIncludeDebug = (debugSetting == tr("true") || debugSetting == tr("yes"));
        }
    }
}


void Application::setupEnvironment() {
    QString value = qEnvironmentVariable(environmentVariableEnvironmentType);

    Ld::Environment::Type executableType;
    if (value.isEmpty()) {
        executableType = Ld::Environment::Type::MAIN_APPLICATION_DEPLOYMENT;
    } else {
        value = value.trimmed().toLower();
        if (value == "development") {
            executableType = Ld::Environment::Type::MAIN_APPLICATION_DEVELOPMENT;
        } else if (value == "deployment") {
            executableType = Ld::Environment::Type::MAIN_APPLICATION_DEPLOYMENT;
        } else {
            executableType = Ld::Environment::Type::MAIN_APPLICATION_DEPLOYMENT;
        }
    }

    bool useHeaderFiles = false; // TODO: Do we want an environment variable for this ?
    Ld::Environment::configure(applicationFilePath(), executableType, useHeaderFiles);
}


bool Application::loadFontsUnder(const QString& directory) {
    bool success = true;
    QDir fontDirectory(directory);

    QFileInfoList                 entries         = fontDirectory.entryInfoList(
        QDir::Readable | QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot,
        QDir::Name
    );

    QFileInfoList::const_iterator fileIterator    = entries.constBegin();
    QFileInfoList::const_iterator fileEndIterator = entries.constEnd();

    while (success && fileIterator != fileEndIterator) {
        const QFileInfo& fileInfo = *fileIterator;
        if (fileInfo.isDir()) {
            success = loadFontsUnder(fileInfo.canonicalFilePath());
        } else if (fileInfo.isFile()) {
            QString extension = fileInfo.suffix().toLower();
            if (extension == tr("ttf") || extension == tr("otf")) {
                int id = QFontDatabase::addApplicationFont(fileInfo.canonicalFilePath());
                success = (id != -1);
            }
        }

        ++fileIterator;
    }

    return success;
}


void Application::showWelcomeMessage() {
    bool hideWelcomeScreen = EQt::GlobalSetting::setting("hide_welcome_screen")->toBool();

    if (!hideWelcomeScreen) {
        EQt::MessageDialog welcomeMessageDialog(
            tr("Welcome"),
            tr(
                "<p>Welcome to <b>Aion</b> !</p>"
                "<p><b>Aion</b> provides several features to help you get started quickly:</p>"
                "<ul>"
                "<li>You can access tutorial videos by selecting Help | Getting Started... or by<br/>"
                    "directing your browser to <a href=\"%1\">%1</a>.<br/></li>"
                "<li>You can view a large selection of examples by selecting Help | Examples...<br/></li>"
                "<li>You can view <b>Aion</b>'s full documentation by selecting Help | Aion Help...<br/>"
                    "or by pointing your browser to <a href=\"%2\">%2</a>.</li>"
                "</ul>"
                "<p>Enjoy !</p>"
            ).arg(HELP_GETTING_STARTED_URL, HELP_MAIN_PAGE_URL)
        );

        welcomeMessageDialog.setDoNotShowAgain(true);
        welcomeMessageDialog.exec();

        hideWelcomeScreen = welcomeMessageDialog.doNoShowAgain();
        EQt::GlobalSetting::setting("hide_welcome_screen")->setValue(hideWelcomeScreen);
    }
}
