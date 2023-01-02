/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This file implements the \ref EulaWizardPage class.
***********************************************************************************************************************/

#include <QDialog>
#include <QString>
#include <QDate>
#include <QPixmap>
#include <QScreen>
#include <QSize>
#include <QTimer>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QFontMetrics>

#include "application.h"
#include "splash_screen.h"

const QPointF SplashScreen::tagLineLowerRightEdge(0.95, 0.56);
const float   SplashScreen::fontRelativeHeight = 0.025F;
const QPen    SplashScreen::tagLinePen(QColor(16, 16, 16));
const QPen    SplashScreen::licenseTextPen(QColor(16, 16, 16));
const QBrush  SplashScreen::progressBarCompletedBrush(QColor(64, 64, 64));
const QBrush  SplashScreen::progressBarPendingBrush(QColor(128, 128, 128));
const QPen    SplashScreen::progressBarTextPen(QColor(255, 255, 255));

SplashScreen::SplashScreen(QWidget* parent):QDialog(parent, Qt::WindowType::SplashScreen | Qt::WindowStaysOnTopHint) {
    configure();
}


SplashScreen::SplashScreen(
        unsigned numberInitializationSteps,
        QWidget* parent
    ):QDialog(
        parent,
        Qt::WindowType::SplashScreen | Qt::WindowStaysOnTopHint
    ) {
    configure(numberInitializationSteps);
}


SplashScreen::SplashScreen(
        unsigned       numberInitializationSteps,
        const QString& newCustomerName,
        const QString& newCompanyName,
        const QString& newLicenseeName,
        const QDate&   newExpirationDate,
        QWidget*         parent
    ):QDialog(
        parent,
        Qt::WindowType::SplashScreen | Qt::WindowStaysOnTopHint
    ) {
    configure(
        numberInitializationSteps,
        newCustomerName,
        newCompanyName,
        newLicenseeName,
        newExpirationDate
    );
}


SplashScreen::~SplashScreen() {}


unsigned SplashScreen::numberInitializationSteps() const {
    return currentNumberInitializationSteps;
}


unsigned SplashScreen::currentInitializationStep() const {
    return currentInitializationStepNumber;
}


QString SplashScreen::description() const {
    return currentDescription;
}


QString SplashScreen::customerName() const {
    return currentCustomerName;
}


QString SplashScreen::companyName() const {
    return currentCompanyName;
}


QString SplashScreen::licenseeName() const {
    return currentLicenseeName;
}


QDate SplashScreen::expirationDate() const {
    return currentExpirationDate;
}


void SplashScreen::setNumberInitializationSteps(unsigned newNumberInitializationSteps) {
    currentNumberInitializationSteps = newNumberInitializationSteps;
    repaint();
}


void SplashScreen::startedInitializationStep(const QString& description) {
    ++currentInitializationStepNumber;
    currentDescription = description;
    repaint();
}


void SplashScreen::initializationCompleted() {
    closeTimer->start(dialogCloseDelay);
}


void SplashScreen::setCustomerName(const QString& newCustomerName) {
    currentCustomerName = newCustomerName;
    updateUserInformation();
    repaint();
}


void SplashScreen::setCompanyName(const QString& newCompanyName) {
    currentCompanyName = newCompanyName;
    updateUserInformation();
    repaint();
}


void SplashScreen::setLicenseeName(const QString& newLicenseeName) {
    currentLicenseeName = newLicenseeName;
    updateUserInformation();
    repaint();
}


void SplashScreen::setExpirationDate(const QDate& newExpirationDate) {
    currentExpirationDate = newExpirationDate;
    updateUserInformation();
    repaint();
}


void SplashScreen::paintEvent(QPaintEvent* /* event */) {
    QPainter painter(this);

    painter.drawPixmap(0, 0, backgroundPixmap);

    painter.setFont(currentTagLineFont);
    painter.setPen(tagLinePen);
    painter.drawText(currentTagLinePosition, currentTagLineText);

    painter.setFont(currentCustomerDataFont);
    painter.setPen(licenseTextPen);

    QPoint textAdjustment(0, QFontMetrics(currentCustomerDataFont).height());
    QPoint textPosition = textBaseline;

    if (!currentCustomerName.isEmpty()) {
        painter.drawText(textPosition, currentCustomerName);
        textPosition += textAdjustment;
    }

    if (!currentCompanyName.isEmpty()) {
        painter.drawText(textPosition, currentCompanyName);
        textPosition += textAdjustment;
    }

    if (!currentLicenseeName.isEmpty()) {
        painter.drawText(textPosition, currentLicenseeName);
        textPosition += textAdjustment;
    }

    painter.drawText(textPosition, currentExpirationDateText);

    float fractionComplete = (
          static_cast<float>(currentInitializationStepNumber)
        / static_cast<float>(currentNumberInitializationSteps)
    );

    painter.setPen(QPen());

    if (fractionComplete <= 0.0F) {
        painter.setBrush(progressBarPendingBrush);
        painter.drawRect(progressBarRectangle);
    } else if (fractionComplete >= 1.0F) {
        painter.setBrush(progressBarCompletedBrush);
        painter.drawRect(progressBarRectangle);
    } else {
        unsigned w = static_cast<unsigned>(progressBarRectangle.width() * fractionComplete + 0.5);
        unsigned h = static_cast<unsigned>(progressBarRectangle.height());

        QRect completedRectangle(progressBarRectangle.topLeft(), QSize(w, h));
        QRect pendingRectangle(
            QPoint(progressBarRectangle.left() + w, progressBarRectangle.top()),
            progressBarRectangle.bottomRight()
        );

        painter.setBrush(progressBarCompletedBrush);
        painter.drawRect(completedRectangle);

        painter.setBrush(progressBarPendingBrush);
        painter.drawRect(pendingRectangle);
    }

    if (!currentDescription.isEmpty()) {
        painter.setFont(Application::font());
        painter.setPen(progressBarTextPen);
        painter.drawText(progressBarRectangle, Qt::AlignmentFlag::AlignCenter, currentDescription);
    }
}


void SplashScreen::closeDialog() {
    deleteLater();
}


void SplashScreen::configure(
        unsigned       numberInitializationSteps,
        const QString& newCustomerName,
        const QString& newCompanyName,
        const QString& newLicenseeName,
        const QDate&   newExpirationDate
    ) {
    setWindowModality(Qt::WindowModality::ApplicationModal);

    #if (defined(Q_OS_WIN))

        QString defaultTagLineFont      = tr("Arial Black");
        QString defaultCustomerDataFont = tr("Arial");

    #elif (defined(Q_OS_LINUX))

        QString defaultTagLineFont      = tr("Lato Black");
        QString defaultCustomerDataFont = tr("Lato");

    #elif (defined(Q_OS_DARWIN))

        QString defaultTagLineFont      = tr("Arial Black");
        QString defaultCustomerDataFont = tr("Arial");

    #else

        #error Unknown platform

    #endif

    const QScreen* screen = Application::screens().at(0);
    QSize screenSize = screen->availableSize();

    backgroundPixmap = QPixmap(tr(":splash_screen_large.png")).scaledToWidth(screenSize.width() / 3);

    QFontMetrics fontMetrics(Application::font());
    unsigned     fontHeight = fontMetrics.height();

    setFixedSize(backgroundPixmap.width(), backgroundPixmap.height() + fontHeight);

    closeTimer = new QTimer(this);
    closeTimer->setSingleShot(true);
    connect(closeTimer, &QTimer::timeout, this, &SplashScreen::closeDialog);

    currentInitializationStepNumber  = 0;
    currentNumberInitializationSteps = numberInitializationSteps;

    QFontMetrics progressBarFontMetrics(Application::font());
    progressBarRectangle = QRect(
        QPoint(0, backgroundPixmap.height()),
        QSize(backgroundPixmap.width(), progressBarFontMetrics.height())
    );

    currentTagLineText     = tr("Turning Creativity Into Algorithm Intelligence");
    currentTagLineFont     = selectFont(defaultTagLineFont, fontRelativeHeight * backgroundPixmap.height());

    float tagLineWidth = QFontMetrics(currentTagLineFont).horizontalAdvance(currentTagLineText);

    currentTagLinePosition = QPoint(
        static_cast<unsigned>(tagLineLowerRightEdge.x() * backgroundPixmap.width() - tagLineWidth),
        static_cast<unsigned>(tagLineLowerRightEdge.y() * backgroundPixmap.height())
    );

    currentCustomerName     = newCustomerName;
    currentCompanyName      = newCompanyName;
    currentLicenseeName     = newLicenseeName;
    currentExpirationDate   = newExpirationDate;
    currentCustomerDataFont = selectFont(defaultCustomerDataFont, fontRelativeHeight * backgroundPixmap.height());

    updateUserInformation();
}


QFont SplashScreen::selectFont(const QString& fontName, float requiredHeight) {
    QFont font(fontName, 72.0F * requiredHeight / 96.0F);
    font.setPixelSize(requiredHeight);

    return font;
}


void SplashScreen::updateUserInformation() {
    QFontMetrics fontMetrics(currentCustomerDataFont);
    unsigned     maximumFieldWidth;
    unsigned     numberTextRows = 1;

    if (currentExpirationDate.isValid()) {
        if (currentExpirationDate > QDate(2999, 1,1)) {
            currentExpirationDateText = tr("Perpetual License");
        } else if (currentExpirationDate < QDate(1971, 1, 1)) {
            currentExpirationDateText = tr("License Invalid");
        } else {
            currentExpirationDateText = tr("License Renews On %1 (UTC)").arg(currentExpirationDate.toString());
        }
    } else {
        currentExpirationDateText = tr("Perpetual");
    }

    maximumFieldWidth = static_cast<unsigned>(fontMetrics.horizontalAdvance(currentExpirationDateText));

    if (!currentLicenseeName.isEmpty()) {
        maximumFieldWidth = std::max(
            maximumFieldWidth,
            static_cast<unsigned>(fontMetrics.horizontalAdvance(tr("Licensed to %1").arg(currentLicenseeName)))
        );
        ++numberTextRows;
    }

    if (!currentCompanyName.isEmpty()) {
        maximumFieldWidth = std::max(
            maximumFieldWidth,
            static_cast<unsigned>(fontMetrics.horizontalAdvance(currentCompanyName))
        );
        ++numberTextRows;
    }

    if (!currentCustomerName.isEmpty()) {
        maximumFieldWidth = std::max(
            maximumFieldWidth,
            static_cast<unsigned>(fontMetrics.horizontalAdvance(currentCustomerName))
        );
        ++numberTextRows;
    }

    textBaseline = QPoint(
        backgroundPixmap.width() - maximumFieldWidth,
        backgroundPixmap.height() - fontMetrics.height() * numberTextRows + fontMetrics.ascent()
    );
}
