/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref InspectorWidget class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef INSPECTOR_WIDGET_H
#define INSPECTOR_WIDGET_H

#include <QWidget>
#include <QHash>

#include <ld_data_type.h>
#include <ld_calculated_value.h>

#include "app_common.h"

namespace Ld {
    class CalculatedValuet;
};

/**
 * Base class you can use to create type specific inspector widgets.
 */
class APP_PUBLIC_API InspectorWidget:public QWidget {
    Q_OBJECT

    public:
        /**
         * Typedef for a factory function used to create type specific inspector widgets.
         *
         * \param[in] parent The parent widget.
         */
        typedef InspectorWidget* (*CreatorFunction)(QWidget* parent);

        /**
         * Static method you can use to register new inspector widget factory function.
         *
         * \param[in] valueType       The value type to associate with this factory function.
         *
         * \param[in] creatorFunction The factory function.
         *
         * \return Returns true on success, returns false if a factory function was already registered for the
         *         value type.
         */
        static bool registerCreator(Ld::DataType::ValueType valueType, CreatorFunction creatorFunction);

        /**
         * Static method you can use to obtain a widget for a given value type.
         *
         * \param[in] valueType The value type.
         *
         * \param[in] parent    The parent widget.
         *
         * \return Returns a pointer to the newly created widget.  A null pointer is returned on error.
         */
        static InspectorWidget* createWidget(Ld::DataType::ValueType valueType, QWidget* parent = Q_NULLPTR);

        /**
         * Static method you can use to obtain a widget for a given calculated value.  The calculated value will also
         * be set.
         *
         * \param[in] calculatedValue The value type.
         *
         * \param[in] parent          The parent widget.
         *
         * \return Returns a pointer to the newly created widget.  A null pointer is returned on error.
         */
        static InspectorWidget* createWidget(const Ld::CalculatedValue& calculatedValue, QWidget* parent = Q_NULLPTR);

        /**
         * Constructor
         *
         * \param[in] parent The pointer to the parent widget.
         */
        InspectorWidget(QWidget* parent = Q_NULLPTR);

        ~InspectorWidget() override;

        /**
         * Method you can call to obtain the currently displayed calculated value.
         *
         * \return Returns the currently displayed calculated value.
         */
        virtual Ld::CalculatedValue calculatedValue() const = 0;

        /**
         * method you can call to determine if the currently displayed calculated value is modified.
         *
         * \return Returns true if the currently displayed calculated value is different from the original calculated
         *         value.  Returns false if the calculated value matches the original calculated value.
         */
        virtual bool calculatedValueChanged() const = 0;

        /**
         * Method that determines if load/save buttons should be displayed.
         *
         * \return Returns true if the load/save buttons should be included.  Returns false if the load/save buttons
         *         should be excluded.
         */
        virtual bool includeLoadSaveSupport() const;

    signals:
        /**
         * Signal you should emit when the calculated value is updated.  This signal should not be emitted when the
         * calculated value is set.
         *
         * \param[out] newCalculatedValue The newly changed calculated value.
         */
        void valueChanged(const Ld::CalculatedValue& newCalculatedValue);

        /**
         * Signal you should emit when the calculated value is restored to its original value.  This signal should not
         * be emitted when the calculated value is reset.
         *
         * \param[out] originalCalculatedValue The restored calculated value.
         */
        void valueRestored(const Ld::CalculatedValue& originalCalculatedValue);

    public slots:
        /**
         * Pure virtual slot you should overload to receive the calculated value to be displayed by this widget.
         *
         * \param[in] calculatedValue The calculated value to display in this widget.
         */
        virtual void setCalculatedValue(const Ld::CalculatedValue& calculatedValue) = 0;

        /**
         * Pure virtual slot you should overload to receive notification that the calculated value should be reset.
         */
        virtual void resetCalculatedValue() = 0;

        /**
         * Slot you can trigger to load a variable from a file.
         */
        virtual void loadValue();

        /**
         * Slot you can trigger to save the current variable to a file.
         */
        virtual void saveValue();

    protected:
        /**
         * Method that is called to load the variable contents from a file.
         *
         * \param[in] filename     The filename of the file holding the contents to be loaded.
         *
         * \param[in] binaryFormat If true, the file is in binary format.  If false, the file is in text format.
         *
         * \return Returns true on success, returns false on error.  The default implementation returns false.
         */
        virtual bool loadFromFile(const QString& filename, bool binaryFormat);

        /**
         * Method that is called to save the variable contents to a file.
         *
         * \param[in] filename     The filename of the file to save the contents to.
         *
         * \param[in] binaryFormat If true, the file is in binary format.  If false, the file is in text format.
         *
         * \return Returns true on success, returns false on error.  The default implementation returns false.
         */
        virtual bool saveToFile(const QString& filename, bool binaryFormat);

    private:
        /**
         * The database of inspector widget factory functions.
         */
        static QHash<Ld::DataType::ValueType, CreatorFunction> creatorFunctionsByValueType;
};

#endif
