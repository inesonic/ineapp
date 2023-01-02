/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref LineStyleWidget class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef LINE_STYLE_WIDGET_H
#define LINE_STYLE_WIDGET_H

#include <QWidget>
#include <QColor>

#include <ld_chart_line_style.h>

#include "app_common.h"

class QComboBox;
class QDoubleSpinBox;
class QTimer;
class QShowEvent;

namespace EQt {
    class ColorToolButton;
}

/**
 * Widget that allows you to modify a plot line style setting.
 */
class APP_PUBLIC_API LineStyleWidget:public QWidget {
    Q_OBJECT

    public:
        /**
         * Enumeration of supported line styles.
         */
        typedef Ld::ChartLineStyle::LineStyle LineStyle;

        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent wizard dialog.
         */
        LineStyleWidget(QWidget* parent = Q_NULLPTR);

        /**
         * Constructor
         *
         * \param[in] lineStyle The initial line style.
         *
         * \param[in] parent    Pointer to the parent wizard dialog.
         */
        LineStyleWidget(LineStyle lineStyle, QWidget* parent = Q_NULLPTR);

        /**
         * Constructor
         *
         * \param[in] lineStyle The initial line style.
         *
         * \param[in] lineWidth The initial line width.
         *
         * \param[in] parent    Pointer to the parent wizard dialog.
         */
        LineStyleWidget(LineStyle lineStyle, float lineWidth, QWidget* parent = Q_NULLPTR);

        /**
         * Constructor
         *
         * \param[in] lineStyle The initial line style.
         *
         * \param[in] lineWidth The initial line width.
         *
         * \param[in] lineColor The initial line color.
         *
         * \param[in] parent    Pointer to the parent wizard dialog.
         */
        LineStyleWidget(LineStyle lineStyle, float lineWidth, const QColor& lineColor, QWidget* parent = Q_NULLPTR);

        ~LineStyleWidget() override;

        /**
         * Method you can use to obtain the currently selected line style.
         *
         * \return Returns the currently selected line style.
         */
        LineStyle lineStyle() const;

        /**
         * Method you can use to obtain the currently entered line width.
         *
         * \return Returns the currently entered line width.
         */
        float lineWidth() const;

        /**
         * Method you can use to obtain the currently entered line color.
         *
         * \return Returns the currently entered line color.
         */
        QColor lineColor() const;

    signals:
        /**
         * Signal that is emitted when any line setting is changed.
         *
         * \param[out] newLineStyle The newly selected line style.
         *
         * \param[out] newLineWidth The newly entered line width.
         *
         * \param[out] newLineColor The newly selected line color.
         */
        void lineSettingsChanged(LineStyle newLineStyle, float newLineWidth, const QColor& newLineColor);

        /**
         * Signal that is emitted when the line style is changed.
         *
         * \param[out] newLineStyle The newly selected line style.
         */
        void lineStyleChanged(LineStyle newLineStyle);

        /**
         * Signal that is emitted when the line width is changed.
         *
         * \param[out] newLineWidth The newly entered line width.
         */
        void lineWidthChanged(float newLineWidth);

        /**
         * Signal that is emtited when the line color is changed.
         *
         * \param[out] newLineColor The newly selected line color.
         */
        void lineColorChanged(const QColor& newLineColor);

    public slots:
        /**
         * Slot you can use to change the current line style.
         *
         * \param[in] newLineStyle The new line style.
         */
        void setLineStyle(LineStyle newLineStyle);

        /**
         * Slot you can use to change the current line width.
         *
         * \param[in] newLineWidth The new line width.
         */
        void setLineWidth(float newLineWidth);

        /**
         * Slot you can use to change the current line color.
         *
         * \param[in] newLineColor The new line color.
         */
        void setLineColor(const QColor& newLineColor);

    protected:
        /**
         * Method that is triggered when the widget is shown.
         *
         * \param[in] event Event that triggered the call to this method.
         */
        void showEvent(QShowEvent* event) override;

    private slots:
        /**
         * Slot that is triggered when the user changes the line style.
         *
         * \param[in] index The index of the newly selected entry.
         */
        void lineStyleWasChanged(int index);

        /**
         * Slot that is triggered when the user changes the line width.
         *
         * \param[in] newLineWidth The new line width.
         */
        void lineWidthWasChanged(double newLineWidth);

        /**
         * Slot that is triggered when the user clicks the line color button.
         */
        void lineColorButtonClicked();

        /**
         * Slot that is triggered when this widget first becomes visible.
         */
        void nowVisible();

    private:
        /**
         * Method used by the constructors to create this widget.
         */
        void configureWidget();

        /**
         * Method that populates a grid style combo box.
         *
         * \param[in] lineWidth The new line width.
         *
         * \param[in] lineColor The new line color.
         */
        void populateLineStyleComboBox(float lineWidth, const QColor& lineColor);

        /**
         * The line style combo box.
         */
        QComboBox* lineStyleComboBox;

        /**
         * The line width spin box.
         */
        QDoubleSpinBox* lineWidthSpinBox;

        /**
         * The line color tool button.
         */
        EQt::ColorToolButton* lineColorToolButton;

        /**
         * Timer use to update geometries when the widget is made visible.
         */
        QTimer* showTimer;
};

#endif
