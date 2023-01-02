/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref FormatDockWidget class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef FORMAT_DOCK_WIDGET_H
#define FORMAT_DOCK_WIDGET_H

#include <QString>

#include <eqt_programmatic_dock_widget.h>

class QAction;

namespace EQt {
    class ToolButton;
    class PaperSizePopupMenu;
    class ProgrammaticMainWindow;
}


/**
 * Class derived from EQt::ProgrammaticDockWidget that provides a dock for core application functions.
 */
class FormatDockWidget:public EQt::ProgrammaticDockWidget {
    Q_OBJECT

    public:
        /**
         * Constructor.
         *
         * \param[in] parent The parent object for this window.
         *
         * \param[in] flags  Flags that can be used to modify the behavior of this dock widget.
         */
        FormatDockWidget(EQt::ProgrammaticMainWindow* parent = Q_NULLPTR, Qt::WindowFlags flags = Qt::WindowFlags());

        ~FormatDockWidget() override;

        /**
         * Method that is called by the main window after all the actions are created to configure this dock widget.
         * The default implementation does nothing.
         *
         * \param[in] mainWindow Pointer to the main window object.
         */
        virtual void configureDockWidget(EQt::ProgrammaticMainWindow* mainWindow);

    private slots:
        /**
         * Slot that is triggered when the page orientation changes.
         *
         * \param[in] nowLandscape Holds true if the page orientation should now be changed to landscape mode.
         */
        void pageOrientationNowLandscape(bool nowLandscape);

    private:
        /**
         * Bulds the widget's horizontal orientation.
         *
         * \param[in] mainWindow Pointer to the main window object.
         */
        void buildHorizontalOrientation(EQt::ProgrammaticMainWindow* mainWindow);

        /**
         * Bulds the widget's vertical orientation.
         *
         * \param[in] mainWindow Pointer to the main window object.
         */
        void buildVerticalOrientation(EQt::ProgrammaticMainWindow* mainWindow);

        EQt::PaperSizePopupMenu* paperSizePopupMenu;
        EQt::ToolButton*         horizontalPageOrientationButton;
        EQt::ToolButton*         verticalPageOrientationButton;
        QAction*                 pageOrientationAction;
};

#endif
