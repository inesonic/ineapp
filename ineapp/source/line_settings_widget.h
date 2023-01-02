/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref LineSettingsWidget class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef LINE_SETTINGS_WIDGET_H
#define LINE_SETTINGS_WIDGET_H

#include <QWidget>
#include <QSize>
#include <QSet>

#include <util_hash_functions.h>

#include <eqt_line_sample_widget.h>

#include <ld_table_line_settings.h>

class QRadioButton;
class QDoubleSpinBox;

namespace EQt {
    class LineSampleWidget;
}

/**
 * Class that provides a widget you can use to adjust table line settings.
 */
class LineSettingsWidget:public QWidget {
    Q_OBJECT

    public:
        /**
         * Type used to represent a line style.
         */
        typedef Ld::TableLineSettings::Style LineStyle;

        /**
         * Type used to indicate the line orientation.
         */
        typedef EQt::LineSampleWidget::Orientation LineOrientation;

        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent widget.
         */
        LineSettingsWidget(QWidget* parent = Q_NULLPTR);

        /**
         * Constructor
         *
         * \param[in] lineOrientation The line orientation.
         *
         * \param[in] parent          Pointer to the parent widget.
         */
        LineSettingsWidget(LineOrientation lineOrientation, QWidget* parent = Q_NULLPTR);

        ~LineSettingsWidget() override;

        /**
         * Method you can use to determine the line orientation.
         *
         * \return Returns the current line orientation.
         */
        LineOrientation lineOrientation() const;

        /**
         * Method you can use to determine what line styles are enabled.
         *
         * \return Returns a set of enabled line styles.  By default all line styles are enabled.
         */
        QSet<LineStyle> enabledLineStyles() const;

        /**
         * Method you can use to determine the selected line style.
         *
         * \return Returns the selected line style.  The value LineStyle::INVALID is returned if either no line style
         *         is selected or multiple line styles are selected.
         */
        LineStyle lineStyle() const;

        /**
         * Method you can use to determine the selected line width.
         *
         * \return Returns the selected line width.  A zero value is returned if no line style is selected.
         */
        float lineWidth() const;

    public slots:
        /**
         * Slot you can use to set the line orientation.
         *
         * \param[in] newLineOrientation The new line orientation.
         */
        void setLineOrientation(LineOrientation newLineOrientation);

        /**
         * Slot you can use to set the enabled line styles.
         *
         * \param[in] newEnabledLineStyles a set containing the enabled line styles.
         */
        void setEnabledLineStyles(const QSet<LineStyle>& newEnabledLineStyles);

        /**
         * Slot you can use to set the selected line styles.
         *
         * \param[in] newLineStyles A set holding the selected line styles.
         */
        void setLineStyles(const QSet<LineStyle>& newLineStyles);

        /**
         * Slot you can use to set the selected line style.
         *
         * \param[in] newLineStyle The newly selected line style.
         */
        void setLineStyle(LineStyle newLineStyle);

        /**
         * Slot you can use to set the line width.
         *
         * \param[in] newLineWidth The new line width.  A zero or negative line width will set the line width to an
         *                         invalid value.
         */
        void setLineWidth(float newLineWidth);

        /**
         * Slot you can use to mark the line width as invalid.
         */
        void setLineWidthInvalid();

    signals:
        /**
         * Signal that is emitted when the line style is changed.
         *
         * \param[out] newLineStyle The newly selected line style.
         */
        void lineStyleChanged(LineStyle newLineStyle);

        /**
         * Signal that is emitted when the line width is changed.
         *
         * \param[out] newLineWidth The newly selected line width.
         */
        void lineWidthChanged(float newLineWidth);

    private slots:
        /**
         * Slot that is triggered when the no line radio button is clicked.
         */
        void noLineRadioButtonClicked();

        /**
         * Slot that is triggered when the single line radio button is clicked.
         */
        void singleLineRadioButtonClicked();

        /**
         * Slot that is triggered when the double line radio button is clicked.
         */
        void doubleLineRadioButtonClicked();

        /**
         * Slot that is triggered when the line width is changed.
         *
         * \param[in] newLineWidth The newly selected line width.
         */
        void lineWidthValueChanged(double newLineWidth);

    private:
        /**
         * Value that indicates the width of the line samples.
         */
        static const unsigned lineSampleWidgetWidth = 26;

        /**
         * Method that configures this widget.
         */
        void configureWidget();

        /**
         * Pointer to the single line sample widget.
         */
        EQt::LineSampleWidget* singleLineSampleWidget;

        /**
         * Pointer to the double line sample widget.
         */
        EQt::LineSampleWidget* doubleLineSampleWidget;

        /**
         * Pointer to the radio button used to select no line.
         */
        QRadioButton* noLineRadioButton;

        /**
         * Pointer to the radio button used to select a single line.
         */
        QRadioButton* singleLineRadioButton;

        /**
         * Pointer to the radio button used to select a double line.
         */
        QRadioButton* doubleLineRadioButton;

        /**
         * Pointer to the spin box used to select the line width.
         */
        QDoubleSpinBox* lineWidthSpinBox;
};

#if (!defined(Q_OS_WIN))

    /**
     * Hash function for the Qt QColor type.
     *
     * \param[in] lineStyle The color value to be hashed.
     *
     * \param[in] seed      An optional hash seed.
     */
    unsigned qHash(LineSettingsWidget::LineStyle lineStyle, Util::HashSeed seed = 0);

#endif

#endif
