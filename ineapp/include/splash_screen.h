/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref SplashScreen class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef SPLASH_SCREEN_H
#define SPLASH_SCREEN_H

#include <QDialog>
#include <QString>
#include <QDate>
#include <QFont>
#include <QPixmap>
#include <QPoint>
#include <QPointF>
#include <QRect>
#include <QPen>
#include <QBrush>

#include "app_common.h"

class QWidget;
class QPaintEvent;
class QTimer;
class QPixmap;

/**
 * Modal dialog that manages the application splash screen.
 */
class APP_PUBLIC_API SplashScreen:public QDialog {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent wizard dialog.
         */
        SplashScreen(QWidget* parent = Q_NULLPTR);

        /**
         * Constructor
         *
         * \param[in] numberInitializationSteps The number of initialization steps.
         *
         * \param[in] parent                    Pointer to the parent wizard dialog.
         */
        SplashScreen(unsigned numberInitializationSteps, QWidget* parent = Q_NULLPTR);

        /**
         * Constructor
         *
         * \param[in] numberInitializationSteps The number of initialization steps.
         *
         * \param[in] newCustomerName           The new displayed customer name.
         *
         * \param[in] newCompanyName            The new displayed company name.
         *
         * \param[in] newLicenseeName           The new displayed licensee name.
         *
         * \param[in] newExpirationDateTime     The new expiration date.
         *
         * \param[in] parent                    Pointer to the parent wizard dialog.
         */
        SplashScreen(
            unsigned       numberInitializationSteps,
            const QString& newCustomerName,
            const QString& newCompanyName,
            const QString& newLicenseeName,
            const QDate&   newExpirationDateTime,
            QWidget*         parent = Q_NULLPTR
        );

        ~SplashScreen() override;

        /**
         * Method you can use to determine the current number of initialization steps.
         *
         * \return Returns the current number of initialization steps.
         */
        unsigned numberInitializationSteps() const;

        /**
         * Method you can use to determine the current initialization step.  A value of 0 indicates that no
         * step has been performed.
         *
         * \return Returns the current initialization step number.
         */
        unsigned currentInitializationStep() const;

        /**
         * Method you can use to determine the currently displayed description.
         *
         * \return Returns the currently displayed description.
         */
        QString description() const;

        /**
         * Method you can use to determine the currently displayed customer name.
         *
         * \return Returns the currently displayed customer name.
         */
        QString customerName() const;

        /**
         * Method you can use to determine the currently displayed company name.
         *
         * \return Returns the currently displayed company name.
         */
        QString companyName() const;

        /**
         * Method you can use to determine the currently displayed licensee name.
         *
         * \return Returns the currently displayed licensee name.
         */
        QString licenseeName() const;

        /**
         * Method you can use to determine the displayed expiration date.
         *
         * \return Returns the current expiration date.
         */
        QDate expirationDate() const;

    public slots:
        /**
         * Slot you can trigger to set the number of initialization steps.
         *
         * \param[in] newNumberInitializationSteps The new number of application initialization steps.
         */
        void setNumberInitializationSteps(unsigned newNumberInitializationSteps);

        /**
         * Slot you can trigger to indicate a new initialization step.
         *
         * \param[in] description The description for this initialization step.
         */
        void startedInitializationStep(const QString& description);

        /**
         * Slot you can trigger to indicate the initialization is completed.  This will close and destroy the dialog
         * after a short delay.
         */
        void initializationCompleted();

        /**
         * Slot you can trigger to set the currently displayed customer name.
         *
         * \param[in] newCustomerName The new displayed customer name.
         */
        void setCustomerName(const QString& newCustomerName);

        /**
         * Slot you can trigger to set the currently displayed company name.
         *
         * \param[in] newCompanyName The new displayed company name.
         */
        void setCompanyName(const QString& newCompanyName);

        /**
         * Slot you can trigger to set the currently displayed licensee name.
         *
         * \param[in] newLicenseeName The new displayed licensee name.
         */
        void setLicenseeName(const QString& newLicenseeName);

        /**
         * Slot you can trigger to set th displayed expiration date.
         *
         * \param[in] newExpirationDate The new expiration date.
         */
        void setExpirationDate(const QDate& newExpirationDate);

    protected:
        /**
         * The window paint event handler.
         *
         * \param[in] event The event that triggered the call to this method.
         */
        void paintEvent(QPaintEvent* event) override;

    private slots:
        /**
         * Slot that is triggered when the dialog should close.
         */
        void closeDialog();

    private:
        /**
         * The dialog close delay in milliseconds.
         */
        static const unsigned dialogCloseDelay = 1000;

        /**
         * The relative position of the tag line.
         */
        static const QPointF tagLineLowerRightEdge;

        /**
         * The font height relative to the size of the graphics.
         */
        static const float fontRelativeHeight;

        /**
         * The tag line pen.
         */
        static const QPen tagLinePen;

        /**
         * The license text pen.
         */
        static const QPen licenseTextPen;

        /**
         * The progress bar completed brush.
         */
        static const QBrush progressBarCompletedBrush;

        /**
         * The progress bar pending brush.
         */
        static const QBrush progressBarPendingBrush;

        /**
         * The progress bar description text pen.
         */
        static const QPen progressBarTextPen;

        /**
         * Method that is used by the constructors to configure this dialog.
         *
         * \param[in] numberInitializationSteps The number of initialization steps.
         *
         * \param[in] newCustomerName           The new displayed customer name.
         *
         * \param[in] newCompanyName            The new displayed company name.
         *
         * \param[in] newLicenseeName           The new displayed licensee name.
         *
         * \param[in] newExpirationDateTime     The new expiration date.
         */
        void configure(
            unsigned       numberInitializationSteps = 1,
            const QString& newCustomerName = QString(),
            const QString& newCompanyName = QString(),
            const QString& newLicenseeName = QString(),
            const QDate&   newExpirationDateTime = QDate()
        );

        /**
         * Method that configures a font based on a required height.
         *
         * \param[in] fontName       The required font family.
         *
         * \param[in] requiredHeight The required font height, in pixels.
         *
         * \return Returns the configured font.
         */
        static QFont selectFont(const QString& fontName, float requiredHeight);

        /**
         * Method that updates the user information settings.
         */
        void updateUserInformation();

        /**
         * Pixmap to be drawn on the splash screen.
         */
        QPixmap backgroundPixmap;

        /**
         * License text baseline.
         */
        QPoint textBaseline;

        /**
         * Rectangle for the progress bar.
         */
        QRect progressBarRectangle;

        /**
         * Timer used to close/hide the dialog.
         */
        QTimer* closeTimer;

        /**
         * The current step.
         */
        QString currentDescription;

        /**
         * The current initialization step number.
         */
        unsigned currentInitializationStepNumber;

        /**
         * The number of initialization steps.
         */
        unsigned currentNumberInitializationSteps;

        /**
         * The tag-line text, pre-translated.
         */
        QString currentTagLineText;

        /**
         * The tag-line font.
         */
        QFont currentTagLineFont;

        /**
         * The pre-calculated position of the tag-line.
         */
        QPoint currentTagLinePosition;

        /**
         * The customer name label.
         */
        QString currentCustomerName;

        /**
         * The company name label.
         */
        QString currentCompanyName;

        /**
         * The licensee name label.
         */
        QString currentLicenseeName;

        /**
         * The expiration/renewal date label.
         */
        QString currentExpirationDateText;

        /**
         * The currently reported expiration date.
         */
        QDate currentExpirationDate;

        /**
         * The current customer data font.
         */
        QFont currentCustomerDataFont;
};

#endif
