/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref Application instance.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QSize>
#include <QSizeF>
#include <QFont>
#include <QDateTime>
#include <QSslError>

#include <eqt_programmatic_application.h>

#include "app_common.h"

class QString;
class QNetworkReply;
class QNetworkProxy;
class QAuthenticator;
class QSslPreSharedKeyAuthenticator;
class QScreen;
class QTimer;

namespace Ud {
    class UsageData;
}

class SplashScreen;
class Registrar;
class PlugInManager;
class DocumentFileDialog;
class ImageFileDialog;
class Clipboard;
class BuildExecuteStateMachine;
class ConsoleDevice;
class ToolButtonSizingDialog;

#if (defined(Q_OS_DARWIN))

    class MacOSLinkerLocator;

#elif (!defined(Q_OS_WIN) && !defined(Q_OS_LINUX))

    #error Unknown platform

#endif

/**
 * An application base class.  This class provides a thin wrapper on the Eqt::Application class providing support
 * for tracking usage data and managing licensing.
 */
class APP_PUBLIC_API Application:public EQt::ProgrammaticApplication {
    Q_OBJECT

    public:
        /**
         * Environment variable used to indicate whether the application should support debug options.
         */
        static const char environmentVariableEnableDebug[];

        /**
         * Environment variable used to determine the directory structure.
         */
        static const char environmentVariableEnvironmentType[];

        /**
         * The name of the application global math font.
         */
        static const char globalMathFontName[];

        /**
         * Constructor.
         *
         * \param[in,out] applicationCount  The number of command line parameters.
         *
         * \param[in,out] applicationValues The values passed on the command line.
         */
        Application(int& applicationCount, char** applicationValues);

        ~Application() override;

        /**
         * Returns the instance of this application.
         *
         * \return A pointer to the application instance.
         */
        static inline Application* instance() {
            return dynamic_cast<Application*>(QApplication::instance());
        }

        /**
         * Returns a string indicating the application build as a numeric value.
         *
         * \return Returns a string holding the application build as a numeric value.  Values will be always
         *         increasing.
         */
        static unsigned long long applicationBuildNumber();

        /**
         * Returns a string indicating the application build.
         *
         * \return Returns a string holding the application build.  Strings will be based on the Julian build date.
         */
        static QString applicationBuildString();

        /**
         * Returns a pointer to the application's Ud::UsageData class instance.
         *
         * \return Return a pointer to an instance of Ud::UsageData
         */
        static Ud::UsageData* usageData();

        /**
         * Determines if the usage data has been configured by the application already.
         *
         * \return Returns true if the usage data has been configured.  Returns false if the usage data has not been
         *         configured.
         */
        static bool usageDataIsConfigured();

        /**
         * Returns a pointer to the application global registrar.
         *
         * \return Returns a pointer to the application global registrar.
         */
        static Registrar* registrar();

        /**
         * Returns a pointer to the application's plug-in manager.
         *
         * \return Returns a pointer to the application global plug-in manager.
         */
        static PlugInManager* plugInManager();

        /**
         * Returns a pointer to the application's document file dialog.
         *
         * \return Returns a pointer to the application's global document file dialog.
         */
        static DocumentFileDialog* documentFileDialog();

        /**
         * Returns a pointer to the application's image file dialog.
         *
         * \return Returns a pointer to the application's global image file dialog.
         */
        static ImageFileDialog* imageFileDialog();

        /**
         * Returns a pointer to the application's clipboard.
         *
         * \return Returns a pointer to the application's global clipboard.
         */
        static Clipboard* clipboard();

        /**
         * Returns a pointer to the application's build/execute state machine instance.
         */
        static BuildExecuteStateMachine* buildExecuteStateMachine();

        /**
         * Returns a pointer to the application's global console device instance.
         */
        static ConsoleDevice* consoleDevice();

        /**
         * Returns the physical dots per inch of the display the application is running on.
         *
         * \return The dots per inch in the horizontal and vertical direction.
         */
        static QSize physicalDpi();

        /**
         * Returns the logical dots per inch of the display the application is running on.  This value is used for
         * font rendering.
         *
         * \return The dots per inch in the horizontal and vertical direction.
         */
        static QSize logicalDpi();

        /**
         * Method that provides a scale factor to apply to font point sizes to make fonts correctly sized on the
         * display.
         *
         * This method exists to overcome the fact that platforms don't properly scale fonts based on the actual
         * screen dimensions and screen DPI.
         *
         * \return Returns a scale factor that should be applied to fonts to make then render as close to the correct
         *         size as possible for the display.
         */
        static double fontScaleFactor();

        /**
         * Method that provides an application global math font suitable for use in buttons and other GUI elements.
         *
         * \return Returns the application global math font.
         */
        static const QFont& mathFont();

        /**
         * Method that indicates if debugging options should be included.
         *
         * \return Returns true if debugging options should be included.  Returns false if debugging options should
         *         be excluded.
         */
        static bool debugModeEnabled();

        /**
         * Method you can call to run the application.
         *
         * \return Returns the application exit status code.
         */
        int exec();

    signals:
        /**
         * Signal that is emitted when the button size has been changed.
         *
         * \param[out] newPhysicalDpi              The new physical screen DPI.
         *
         * \param[out] newLogicalDpi               The new logical screen DPI.
         *
         * \param[out] newToolButtonHeightInPixels The new button icon height, in pixels.
         */
        void screenSettingsChanged(
            const QSize& newPhysicalDpi,
            const QSize& newLogicalDpi,
            unsigned     newToolButtonHeightInPixels
        );

        /**
         * Signal that is emitted when the screen size or orientation is changed. This signal is delayed relative to
         * the screen settings changed signal and can be used to perform additional changes after the geometry changes
         * triggered by \ref Application::screenSettingsChanged.
         */
        void screenResized();

    private slots:
        /**
         * Sets up the user interface.  This slot tied to EQt::UniqueApplication::startUserInterface and is called
         * after the application is loaded and determines that this instance is the primary instance.
         *
         * \param[in] startupCondition Indicates the state of the application when it starts up.
         */
        void startUserInterface(EQt::UniqueApplication::StartupCondition startupCondition);

        /**
         * Slot that is triggered when the user attempts to load another instance of the application.
         *
         * \param[in] parameters The parameters passed on the other application instance's command line.
         */
        void instanceLoaded(const QStringList& parameters);

        /**
         * Slot that received notification that authentication is needed.
         *
         * \param[in] reply         The received network reply.
         *
         * \param[in] authenticator Class used to perform authentication.
         */
        void authenticationRequired(QNetworkReply* reply, QAuthenticator* authenticator);

        /**
         * Slot that is triggered when the SSL/TLS handshake is negotiating a PSK ciphersuite.  The slot indicates that
         * a PSK authentication is required.
         *
         * \param[in] reply         The network reply associated with the SSL/TLS handshake.
         *
         * \param[in] authenticator The authenticator used to perform the PSK authentication.
         */
        void preSharedKeyAuthenticationRequired(QNetworkReply* reply, QSslPreSharedKeyAuthenticator* authenticator);

        /**
         * Slot that is triggered when proxy authentication is required.
         *
         * \param[in] proxy         The proxy instance that we need to authenticate against.
         *
         * \param[in] authenticator The authenticator being used to access the internet via a network proxy.
         */
        void proxyAuthenticationRequired(const QNetworkProxy& proxy, QAuthenticator* authenticator);

        /**
         * Slot that is triggered when one or more SSL/TLS errors are detected.
         *
         * \param[in] reply  The network reply where the errors occurred.
         *
         * \param[in] errors The list of detected errors.
         */
        void sslErrors(QNetworkReply* reply, const QList<QSslError>& errors);

        /**
         * Slot that is triggered when a new plug-in is about to be loaded.
         *
         * \param[in] loadPath The path to the plug-in that is about to be loaded.
         */
        void aboutToLoadPlugIn(const QString& loadPath);

        /**
         * Slot that is triggered when the display resolution has changed.
         *
         * \param[in] screen The new primary screen.
         */
        void primaryScreenHasChanged(QScreen* screen);

        /**
         * Slot that is triggered when a screen's physical resolution is changed.
         *
         * \param[in] newDpi The new DPI setting.  Note that this value is ignored in favor of obtaining distinct DPI
         *                   settings for each dimension.
         */
        void physicalDotsPerInchChanged(qreal newDpi);

        /**
         * Slot that is triggered when a screen's logical resolution is changed.
         *
         * \param[in] newDpi The new logical DPI setting.  Note that this value is ignored in favor of obtaining
         *                   distinct DPI settings for each dimension.
         */
        void logicalDotsPerInchChanged(qreal newDpi);

        /**
         * Slot that is triggered when a screen's orientation is changed.
         *
         * \param[in] newOrientation The new screen orientation.
         */
        void screenOrientationChanged(Qt::ScreenOrientation newOrientation);

        /**
         * Slot that is triggered when the physical size of the screen has changed.
         *
         * \param[in] newSize The new screen size.
         */
        void screenSizeChanged(const QSizeF& newSize);

        /**
         * Slot that is triggered when there are no more visible windows.
         */
        void allWindowsClosed() final;

    private:
        /**
         * Value holding the number of start-up initialization steps, ignoring plug-ins.
         */
        static const unsigned numberBaseInitializationSteps;

        /**
         * Method that starts the normal application platform.
         */
        void startNormalApplication();

        /**
         * Method that performs platform specific customizations.
         */
        void customizeForPlatform();

        /**
         * Method that creates instances of GlobalSetting used throughout the application.
         */
        void createGlobalSettingData();

        /**
         * Method that is called to initialize application core.  This includes the language definition, core GUI
         * builders, and presentations associated with the language core.
         */
        void initializeCoreLanguageDefinition();

        /**
         * Method that is called to register and process plug-ins.
         */
        void initializePlugIns();

        /**
         * Method that is called to load application fonts.
         */
        void loadApplicationFonts();

        /**
         * Method that is called to finalize the language definition.  This method should only be called after the core
         * language definition is defined and all plug-ins are loaded.
         */
        void finalizeLanguageDefinition();

        /**
         * Method that is called to set the application default fonts.
         */
        void setApplicationDefaultFonts();

        /**
         * Method that calculates screen DPI and font scale factor settings from the configuration data.
         */
        void updateScreenSettings();

        /**
         * Method that calculates an optimal tool button size. The tool button will be sized to be a little taller
         * than two text line edit fields.
         */
        void updateToolButtonSizes();

        /**
         * Method that loads global settings for the application.
         */
        void loadGlobalSettings();

        /**
         * Method that saves global settings for the application.
         */
        void saveGlobalSettings();

        /**
         * Method that checks if debugging options should be enabled.
         */
        void checkDebugSupport();

        /**
         * Method that sets up the environment based on environment variables.
         */
        void setupEnvironment();

        /**
         * Method that loads fonts from a given directory and all subdirectories.  Fonts are identified by the
         * extensions ".ttf" and ".otf" (case insensitive)
         *
         * \param[in] directory The directory containing the fonts of interest.
         *
         * \return Returns true on success, returns false on error.
         */
        bool loadFontsUnder(const QString& directory);

        /**
         * Method that conditionally shows the welcome message.
         */
        void showWelcomeMessage();

        #if (defined(Q_OS_DARWIN))

            /**
             * Small class that locates the MacOS linker.
             */
            MacOSLinkerLocator* macosLinkerLocator;

        #elif (!defined(Q_OS_WIN) && !defined(Q_OS_LINUX))

            #error Unknown platform

        #endif

        /**
         * The application splash screen.
         */
        SplashScreen* splashScreen;

        /**
         * The application global math font.
         */
        QFont currentMathFont;

        /**
         * Flag indicating if debug options should be included.
         */
        bool currentIncludeDebug;

        /**
         * The current primary screen.
         */
        QScreen* currentPrimaryScreen;

        /**
         * The current physical screen DPI settings.
         */
        QSize currentScreenPhysicalDpi;

        /**
         * The current logical screen DPI settings.
         */
        QSize currentScreenLogicalDpi;

        /**
         * The current scale factor to apply to fonts to make then scale correctly on the display.
         */
        double currentFontScaleFactor;

        /**
         * Timer used to assist during screen resize updates,
         */
        QTimer* screenResizeTimer;

        /**
         * Dialog used to measure the size of tool buttons.
         */
        ToolButtonSizingDialog* currentToolButtonSizingDialog;

        /**
         * The current application usage data instance.
         */
        Ud::UsageData* currentUsageData;

        /**
         * Flag indicating if usage data has been configured.
         */
        bool currentUsageDataIsConfigured;

        /**
         * The current application registrar.
         */
        Registrar* currentRegistrar;

        /**
         * The current plug-in manager.
         */
        PlugInManager* currentPlugInManager;

        /**
         * The application global document file dialog.
         */
        DocumentFileDialog* currentDocumentFileDialog;

        /**
         * The application global image file dialog.
         */
        ImageFileDialog* currentImageFileDialog;

        /**
         * The application global clipboard.
         */
        Clipboard* currentClipboard;

        /**
         * The application global build/execute state machine.
         */
        BuildExecuteStateMachine* currentBuildExecuteStateMachine;

        /**
         * The application global console device instance.
         */
        ConsoleDevice* currentConsoleDevice;
};

#endif
