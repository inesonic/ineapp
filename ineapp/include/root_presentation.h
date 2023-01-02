/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 Inesonic, LLC.
* All rights reserved.
********************************************************************************************************************//**
* \file
*
* This header defines the \ref RootPresentation class.
***********************************************************************************************************************/

/* .. sphinx-project ineapp */

#ifndef ROOT_PRESENTATION_H
#define ROOT_PRESENTATION_H

#include <QObject>
#include <QSharedPointer>
#include <QSizeF>
#include <QRectF>
#include <QPointF>
#include <QList>
#include <QSet>
#include <QTimer>

#include <eqt_graphics_scene.h>

#include <ld_element_structures.h>
#include <ld_format_structures.h>
#include <ld_diagnostic_structures.h>
#include <ld_element.h>
#include <ld_visual.h>
#include <ld_format.h>
#include <ld_page_format.h>
#include <ld_diagnostic.h>
#include <ld_root_element.h>
#include <ld_root_visual.h>

#include "app_common.h"
#include "root_child_location.h"
#include "page_list.h"
#include "scene_units.h"
#include "placement_status_notifier.h"
#include "placement_negotiator.h"

class QGraphicsItem;
class QPainter;
class QGraphicsView;

namespace Ld {
    class DocumentSetting;
}

class Presentation;
class PlacementTracker;
class PresentationArea;

/**
 * This class is intended to be used as a the top-level presentation of a program.  You can derived from this class to
 * manage messaging from the language definition (ld) library.  The class extends Ld::RootVisual class to bring that
 * visual into the Qt QObject and signal/slot framework.
 *
 * Unlike all the other classes derived from \ref Presentation that derive from QObject and \ref PlacementNegotiator,
 * this class derived from EQt::GraphicsScene and \ref PlacementNegotiator.  Note that while the inheritance is
 * different from other \ref Presentation classes, the class is designed to have an API that mimics other classes
 * derived from \ref Presentation and \ref PresentationWithPositionalChildren.
 */
class APP_PUBLIC_API RootPresentation:public EQt::GraphicsScene,
                                      public virtual Ld::RootVisual,
                                      public PlacementNegotiator,
                                      public SceneUnits {
    Q_OBJECT

    public:
        /**
         * Constructor
         *
         * \param[in] parent Pointer to the parent object.
         */
        explicit RootPresentation(QObject* parent = Q_NULLPTR);

        ~RootPresentation() override;

        /**
         * Static method that creates an instance of a \ref RootPresentation class.
         *
         * \param[in] typeName The name used to reference this creator function.  A single creator function can be
         *                     used to create multiple visual types using this value to identify what type of
         *                     visual to be created.
         */
        static Ld::Visual* creator(const QString& typeName);

        /**
         * Method you can call to change the number of displayed columns of pages.
         *
         * \param[in] newNumberPageColumns The number of page columns to be displayed.  A value of 0 will be converted
         *                                 to a single column.
         */
        void setNumberPageColumns(unsigned newNumberPageColumns);

        /**
         * Method you can use to determine the number of currently displayed page columns.
         *
         * \return Returns the current number of displayed page columns.
         */
        unsigned numberPageColumns() const;

        /**
         * Method you can call to determine the maximum extents of the largest page, in points.
         *
         * \return Returns ta list holding the maximum horizontal extents for each active column.
         */
        QList<double> maximumHorizontalExtentPoints() const;

        /**
         * Method you can call to determine the left edge of each column, in points.  The first column (column 0) will
         * be at 0.
         *
         * \return Returns the position of the left edge of each column, in points.
         */
        QList<double> pageColumnLeftEdgePoints() const;

        /**
         * Method that can be called to draw per-view page decorations, margins, and guides on a paint device.
         *
         * \param[in] sceneArea      The area that requires decorations be drawn.
         *
         * \param[in] painter        The painter to use to draw the decorations.
         *
         * \param[in] includeMargins If true, the decorations will include page margins in a different shade.
         *
         * \param[in] includeGuides  If true, the decorations will include margin guide lines at the corners of the
         *                           active area.
         */
        void drawPageDecorations(const QRectF& sceneArea, QPainter* painter, bool includeMargins, bool includeGuides);

        /**
         * Method that can be called to determine how children are managed and tracked by this visual.
         *
         * \return Returns a value indicating how children are tracked by this element.  This version returns
         *         Ld::Element::ChildPlacement::POSITIONAL.
         */
        Ld::Element::ChildPlacement childPlacement() const final;

        /**
         * Method that is called by a child to the parent to request that the parent reposition the contents of the
         * child.
         *
         * \param[in] childPresentation The presentation that needs to be repositioned.
         */
        void requestRepositioning(Presentation* childPresentation) final;

        /**
         * Method that is called by a parent to a child to tell the child to start the placement operation,
         * recalculating sizes and positions.  This method will repeatedly call the parent's instance of
         * \ref PlacementNegotiator::requestArea followed by the parent's instance of one of
         * \ref PlacementNegotiator::allocateArea or \ref PlacementNegotiator::areaInsufficient until the data
         * managed by the child is processed and placed.
         *
         * This version will assert when called.
         *
         * \param[in] placementTracker       A pointer to the placement tracker that can be used to track and update
         *                                   status during a time-consuming placement operation.
         *
         * \param[in] parent                 Pointer to the parent presentation negotiator.  Provided so that the child
         *                                   does not have to do the bit of additional work to identify its parent.
         *
         * \param[in] childIdentifier        An identifier the parent uses to quickly identify the child.
         *
         * \param[in] nextSibling            A pointer to the next sibling under the parent.  This child can use this to
         *                                   determine if the next sibling's space requirements should be taken into
         *                                   account when allocating room for this child.
         *
         * \param[in] honorLeadingWhitespace If true, the child should honor, and not remove, whitespace at the left
         *                                   side of the string.
         *
         * \param[in] minimumTopSpacing      Spacing to apply above the first line of the placement area.  The value
         *                                   is primarily intended to be used to manage paragraph spacing.  A negative
         *                                   value indicates that there should be no imposed top spacing.
         *
         * \param[in] lineSpacing            Additional line spacing that should be optionally included by this child.
         *                                   Children can adjust the height they report for their presentation areas.
         *                                   A value of 1.0 indicates single spacing.  A value of 1.5 indicates 1 1/2
         *                                   line spacing.  A value of 2.0 indicates double spacing.
         *
         * \param[in] relativeScale          A hint value used to indicate if this child should try to scale smaller
         *                                   than usual. This value is intended for cases where an equation element
         *                                   needs to be rendered in a smaller than usual font.  A value of 1.0
         *                                   indicates normal scale.  The value is intended as a hint, children are not
         *                                   required to honor this parameter.
         */
        void recalculatePlacement(
            PlacementTracker*    placementTracker,
            PlacementNegotiator* parent,
            unsigned long        childIdentifier,
            Presentation*        nextSibling,
            bool                 honorLeadingWhitespace,
            float                minimumTopSpacing = imposeNoTopSpacing,
            float                lineSpacing = defaultLineSpacing,
            float                relativeScale = 1.0
        ) final;

        /**
         * Method that is called by a parent to a child to tell the child to start the placement operation using
         * existing data.  This method will call the parent \ref PlacementNegotiator::allocateArea instance on each
         * already analyzed placement area.
         *
         * This version will assert when called.
         *
         * \param[in] parent                  Pointer to the parent presentation negotiator.  Provided so that the child
         *                                    does not have to do the bit of additional work to identify its parent.
         *
         * \param[in] childIdentifier         An identifier the parent uses to quickly identify the child.
         *
         * \param[in] firstPresentationAreaId The zero based ID of the first presentation area to place.
         *
         * \param[in] minimumTopSpacing       Spacing to apply above the first line of the placement area.  The value
         *                                    is primarily intended to be used to manage paragraph spacing.  A negative
         *                                    value indicates that there should be no imposed top spacing.
         *
         * \param[in] lineSpacing             Additional line spacing that should be optionally included by this child.
         *                                    Children can adjust the height they report for their presentation areas.
         *                                    A value of 1.0 indicates single spacing.  A value of 1.5 indicates 1 1/2
         *                                    line spacing.  A value of 2.0 indicates double spacing.
         *
         * \param[in] relativeScale           A hint value used to indicate if this child should try to scale smaller
         *                                    than usual. This value is intended for cases where an equation element
         *                                    needs to be rendered in a smaller than usual font.  A value of 1.0
         *                                    indicates normal scale.  The value is intended as a hint, children are not
         *                                    required to honor this parameter.
         */
        void redoPlacement(
            PlacementNegotiator* parent,
            unsigned long        childIdentifier,
            unsigned long        firstPresentationAreaId = 0,
            float                minimumTopSpacing = imposeNoTopSpacing,
            float                lineSpacing = defaultLineSpacing,
            float                relativeScale = 1.0
        ) final;

        /**
         * Method called by a child to request the next available area on the current line.  Note that this method does
         * not actually modify the parent.
         *
         * \param[in]  childIdentifier An identifier this child must provide to the parent to help it quickly identify
         *                             the child.
         *
         * \param[out] spaceQualifier  A qualifier indicating the nature of the provided space.
         *
         * \return Returns the width and height of the next available area on the current line.  Values represent the
         *         maximum available space in each dimension.
         */
        QSizeF requestArea(unsigned long childIdentifier, SpaceQualifier* spaceQualifier = Q_NULLPTR) final;

        /**
         * Method called by a child to a parent to allocate the next available area.
         *
         * \param[in] childIdentifier    The identifier used by the parent to track this child.
         *
         * \param[in] presentationAreaId An identifier used by the child to track a presentation area.
         *
         * \param[in] size               The required space.  This value must be less than or equal to the space
         *                               required by PlacementNegotiator:requestArea.
         *
         * \param[in] ascent             Ascent used for text to perform vertical alignment.  Non text elements should
         *                               set this value to 0.
         *
         * \param[in] canStretch         If true, the parent can ask the child to horizontally stretch this presentation
         *                               area.  This parameter is used to justify text.
         */
        void allocateArea(
            unsigned long childIdentifier,
            unsigned long presentationAreaId,
            const QSizeF& size,
            float         ascent = 0,
            bool          canStretch = false
        ) final;

        /**
         * Method called by a child to indicate that the provided space by PlacementNegotiator::requestArea is
         * insufficient.
         *
         * \param[in] childIdentifier    The identifier used by the parent to track this child.
         *
         * \param[in] size               The last reported available space.  This value must be equal to the space
         *                               reported by PlacementNegotiator:requestArea.
         */
        void areaInsufficient(unsigned long childIdentifier, const QSizeF& size) final;

        /**
         * Method called by a parent to apply a stretch factor to a child.
         *
         * This version will assert when called.
         *
         * \param[in] presentationAreaId An identifier used by the child to track a presentation area.
         *
         * \param[in] stretchFactor      The stretch factor to be applied to this child's presentation area.  A value of
         *                               0 indicates normal unstretched width.  A value of 0.1 indicates that the area
         *                               should be stretched to 110% of it's normal width.  A value of 1.0 indicates
         *                               that the area should be twice as wide as normal.
         *
         * \return Returns the new stretched width.
         */
        void applyStretch(unsigned long presentationAreaId, float stretchFactor) final;

        /**
         * Method called by a parent to obtain a specified child graphics item, by presentation area ID.
         *
         * This version will assert when called.
         *
         * \param[in] presentationAreaId A zero based presentation area ID indicating which graphics region is of
         *                               interest.
         *
         * \return Returns a pointer to a QGraphicsItem used to represent this visual.  A null pointer is returned if
         *         the presentationAreaId is invalid.
         */
        QGraphicsItem* graphicsItem(unsigned long presentationAreaId) const final;

        /**
         * Method you can use to determine the largest area that can be made available to an object.  This method
         * returns the size of the active area of the next full page, in scene units.
         *
         * \return Returns the size of the largest area that can be made available for rendering.
         */
        QSizeF largestAvailableArea() const;

        /**
         * Method you can use to determine if presentation updates are currently pending.
         *
         * \return Returns true if updates are pending.  Returns false if there are no pending updates.
         */
        bool arePresentationUpdatesPending() const;

        /**
         * Method you can use to identify every view that is showing part or all of an element.
         *
         * \param[in] element The element to locate.
         *
         * \return Returns a list of views where the element's associated presentation is currently visible.
         */
        QList<QGraphicsView*> viewsShowingElement(Ld::ElementPointer element);

        /**
         * Method you can use to obtain the page under a specific position in the scene.
         *
         * \param[in] position The position we are interested in.
         *
         * \return Returns a list of page numbers tied to the specified presentation.
         */
        PageList::Index pageAt(const QPointF& position) const;

        /**
         * Method you can use to obtain a list of the pages containing a specified presentation.
         *
         * \param[in] presentation The presentation to obtain a list of page numbers for.
         *
         * \return Returns a list of page numbers tied to the specified presentation.
         */
        QSet<PageList::Index> pagesContainingPresentation(Presentation* presentation) const;

        /**
         * Returns the page extents within the scene, in scene units, of a given page.  This value represents the
         * entire rendered page, including borders and shadows.
         *
         * \param[in] pageIndex the zero based page index of the page of interest.
         *
         * \returns Returns a rectangle containing everything rendered for this page.  A null rectangle is returned
         *          if the page index is invalid.
         */
        QRectF extentsSceneUnits(PageList::Index pageIndex) const;

        /**
         * Returns the page boundary within the scene, in scene units, for a given page. This value represents the
         * contents of the page that represents the paper.
         *
         * \param[in] pageIndex the zero based page index of the page of interest.
         *
         * \return Returns the size of the paper as rendered in the scene, in scene units.
         */
        QRectF boundarySceneUnits(PageList::Index pageIndex) const;

        /**
         * Returns the active area of the page, in scene units, for a given page.  This value represents the portion of
         * the page within the page margins.
         *
         * \param[in] pageIndex the zero based page index of the page of interest.
         *
         * \return Returns the size of active area of the page contained within the page margins, in scene
         *         units.
         */
        QRectF activeAreaSceneUnits(PageList::Index pageIndex) const;

        /**
         * Method you can use to obtain a document setting, by name.
         *
         * \param[in] settingName The name of the requested setting.
         *
         * \return Returns a shared pointer to the requested setting.  A null pointer is returned if the setting is not
         *         currently defined.
         */
        QSharedPointer<Ld::DocumentSetting> documentSettingByName(const QString& settingName) const;

        /**
         * Template method you can use to obtain a document setting cast to the correct type.
         *
         * \param[in] T The class we want to obtain the settings for.
         *
         * \return Returns a shared pointer to the requested settings instance.  The pointer will be properly cast.
         */
        template<class T> QSharedPointer<T> documentSetting() const {
            QSharedPointer<Ld::DocumentSetting> setting = documentSettingByName(T::settingName);
            return setting.dynamicCast<T>();
        }

        /**
         * Method you can use to add a new document setting.  If the setting exists, it will be replaced.
         *
         * \param[in] newSetting A shared pointer to the requested document setting.
         */
        void addDocumentSetting(QSharedPointer<Ld::DocumentSetting> newSetting);

        /**
         * Method you can use to determine the distance between a given location and the closest graphics item managed
         * by this class.  This method assumes that graphics items follow in numerical order.
         *
         * \param[in]  location           The location to query, in scene units.
         *
         * \param[out] bestPresentation   An optional value that can be populated with the best presentation to
         *                                select for the location.
         *
         * \param[out] presentationAreaId An optional value that can be populated with the presentation area index of
         *                                the closest presentation area.
         *
         * \param[out] closestPoint       An optional value that can be populated with the closest point on the closest
         *                                presentation area.
         *
         * \return Returns the distance in scene units.
         */
        double distanceToClosestPresentationArea(
            const QPointF&       location,
            const Presentation** bestPresentation = Q_NULLPTR,
            unsigned long*       presentationAreaId = Q_NULLPTR,
            QPointF*             closestPoint = Q_NULLPTR
        ) const;

        /**
         * Method you can call to add a placement status receiver to this notifier.
         *
         * \param[in] newReceiver The receiver to receive notifications from this notifier.
         */
        void addPlacementStatusNotifierReceiver(PlacementStatusNotifier::Receiver* newReceiver);

        /**
         * Method you can call to remove a placement status receiver from this notifier.
         *
         * \param[in] receiver The receiver to disconnect from this notifier.
         */
        void removePlacementStatusNotifierReceiver(PlacementStatusNotifier::Receiver* receiver);

        /**
         * Method you can call to determine if the document contents are currently being positioned.
         *
         * \return Returns true if the contents are being repositioned.  Returns false if the display of the contents
         *         are not being changed.
         */
        bool isRepositioning() const;

        /**
         * Method you can call to determine if the document contents are currently correct.
         *
         * \return Returns true if the contents are being correctly displayed.  Returns false if either the
         *         display of contents needs to be updated or the contents are actively being updated.
         */
        bool isDisplayCoherent() const;

    signals:
        /**
         * Signal that is emitted when presentation updates first become pending.
         */
        void presentationUpdatesPending();

        /**
         * Signal that is emitted when presentation updates have completed.
         */
        void presentationUpdatesCompleted();

        /**
         * Signal that is emitted when presentation updates have been restarted due to an abort condition.
         */
        void presentationUpdatesRestarted();

        /**
         * Signal that is emitted when the maximum page extents has changed.  You can use this to adjust the lenth of
         * the visible ruler(s).
         *
         * \param[out] newMaximumHorizontalExtentsPoints The new width of the page columns, in points.
         *
         * \param[out] newPageLeftEdgePoints             The new left edges of each page within the scene, translated to
         *                                               points.
         *
         * \param[out] newLeftEdge                       The new scene left edge, in scene units.
         *
         * \param[out] newRightEdge                      The new scene right edge, in scene units.
         */
        void maximumExtentsChanged(
            QList<double> newMaximumHorizontalExtentsPoints,
            QList<double> newPageLeftEdgePoints,
            double        newLeftEdge,
            double        newRightEdge
        );

        /**
         * Signal that is emitted when a change to the program is detected.
         *
         * \param[in] changedPresentation Pointer to the changed presentation.
         */
        void presentationChanged(Presentation* changedPresentation);

        /**
         * Signal that is emitted when a new element is added.
         *
         * \param[in] newPresentation Pointer to the newly added presentation.
         */
        void presentationAdded(Presentation* newPresentation);

        /**
         * Signal that is emitted when an element is removed.
         *
         * \param[in] removedPresentation Pointer to the presentation that was removed.
         */
        void presentationRemoved(Presentation* removedPresentation);

        /**
         * Signal that is emitted when a page format is changed.
         *
         * \param[in] pageNumber The page number of the first page touched by the format change.
         *
         * \param[in] oldFormat  The old page format that is being replaced.
         *
         * \param[in] newFormat  The new format assigned to the page.
         */
        void pageFormatHasChanged(
            unsigned long                  pageNumber,
            QSharedPointer<Ld::PageFormat> oldFormat,
            QSharedPointer<Ld::PageFormat> newFormat
        );

        /**
         * Signal that is emitted when the root element and children are loaded or saved and become pristine.
         */
        void programNowPristine();

        /**
         * Signal that is emitted when a child is modified and the program is no longer pristine.
         */
        void programNowModified();

        /**
         * Signal that is emitted whenever the root element or any child is modified, including changes to references.
         * This signal will not be called when the root element is loaded or saved.
         */
        void programNowChanged();

        /**
         * Signal that is emitted when references are updated.
         */
        void referencesHaveChanged();

        /**
         * Signal that is emitted when the program is saved under its current name.
         *
         * \param[in] filename The filename the program was saved under.
         */
        void programWasSaved(const QString& filename);

        /**
         * Signal that is emitted when the program is saved under a new name.
         *
         * \param[in] filename The filename the program was saved under.
         */
        void programWasSavedAs(const QString& filename);

        /**
         * Signal that is emitted when an error is detected during a program save operation.
         *
         * \param[in] errorMessage A string indicating the reason for the failure.
         */
        void programSaveHasFailed(const QString& errorMessage);

        /**
         * Signal that is emitted when a new program is loaded under this root node.
         *
         * \param[in] filename     The filename the program was saved under.
         *
         * \param[in] majorVersion The major version number of the XML description.
         *
         * \param[in] minorVersion The minor version number of the XML description.
         */
        void programWasLoaded(const QString& filename, unsigned majorVersion, unsigned minorVersion);

        /**
         * Signal that is emitted when an error is detected during a program load operation.
         *
         * \param[in] errorMessage A string indicating the reason for the failure.
         */
        void programLoadHasFailed(const QString& errorMessage);

        /**
         * Signal that is emitted when a new and pristine program is created under this root node.
         */
        void newProgramWasCreated();

        /**
         * Signal that is emitted just before a program is closed.
         */
        void programClosed();

        /**
         * Signal that is emitted when a child is removed by the associated
         * Ld::ElementWithPositionalChildren class.  This signal is emitted by both the
         * Ld::ElementWithPositionalChildren::removeChild(Ld::ElementPointer) method and the
         * Ld::ElementWithPositionalChildren::removeChild(unsigned long) method.
         *
         * \param[in] childIndex        The zero based index of the removed child.
         *
         * \param[in] childPresentation The child presentation being removed.
         */
        void removingChildPresentation(unsigned long childIndex, Presentation* childPresentation);

        /**
         * Signal that is emitted when a child is inserted before a specified location by the
         * Ld::ElementWithPositionalChildren class.  This signal is emitted by the
         * Ld::ElementWithPositionalChildren::insertBefore method.
         *
         * \param[in] childIndex        The zero based index where the desired child should be placed.
         *
         * \param[in] childPresentation The new child presentation being placed.
         */
        void childPresentationNowInsertedBefore(unsigned long childIndex, Presentation* childPresentation);

        /**
         * Signal that is emitted when a child is inserted after a specified location by the
         * Ld::ElementWithPositionalChildren class.  This signal is emitted by the
         * Ld::ElementWithPositionalChildren::insertAfter method.
         *
         * \param[in] childIndex        The zero based index where the desired child should be placed.
         *
         * \param[in] childPresentation The new child presentation being placed.
         */
        void childPresentationNowInsertedAfter(unsigned long childIndex, Presentation* childPresentation);

        /**
         * Signal that is emitted when this object is newly coupled to an element.  You can use this signal to trigger
         * updates to the presentation based on the element contents and type.
         *
         * \param[in] element The element that this visual is now tied to.
         */
        void nowTiedToElement(Ld::ElementPointer element);

        /**
         * Signal that is emitted when this object is disconnected from an element.  You can use this signal to perform
         * a controlled clean-up of the presentation.
         */
        void nowUntiedFromElement();

        /**
         * Signal that is emitted when a Ld::Element reports that its format has been changed.  You can use this
         * signal to perform updates to the visual representation of the element.
         *
         * The signal will automatically be bound to the \ref Presentation::processFormatChange slot.
         *
         * \param[in] oldFormat A pointer to the old Ld::Format instance.  This value may be a null pointer if the
         *                      Ld::Element did not previously have an associated Ld::Format instance.
         *
         * \param[in] newFormat A pointer to the new Ld::Format instance.  This pointer will be the same as the
         *                      oldFormat pointer if the Ld::Format instance was not changed.
         */
        void formatWasChanged(Ld::FormatPointer oldFormat, Ld::FormatPointer newFormat);

        /**
         * Signal that is emitted when an Ld::Element reports that a descendant's format has been changed.  You
         * can use this signal to perform updates to the visual representation of the element.
         *
         * \param[in] element The element who's format has been changed.
         *
         * \param[in] format  The new format associated with this element.
         */
        void descendantFormatWasChanged(Ld::ElementPointer element, Ld::FormatPointer format);

        /**
         * Signal that is emitted when an Ld::Element reports that a descendant's format has been updated.  You can use
         * this signal to perform updates to the visual representation of the element.
         *
         * \param[in] element The element who's format has been changed.
         *
         * \param[in] format  The new format associated with this element.
         */
        void descendantFormatWasUpdated(Ld::ElementPointer element, Ld::FormatPointer format);

        /**
         * Signal that is emitted when the Ld::Element reports that underlying data has been updated.  You can use this
         * signal to perform updates to the visual representation of the element on data changes.
         *
         * The signal will automatically be bound to the \ref Presentation::processDataChange slot.
         *
         * The default version of this slot simply returns.
         */
        void elementDataWasChanged();

        /**
         * Signal that is emitted when a diagnostic is reported for the associated Ld::Element during a code generation
         * process.
         *
         * The signal will automatically be bound to the \ref Presentation::processDiagnostic slot.
         *
         * \param[in] diagnostic A code indicating the type of error.
         */
        void diagnosticWasReported(Ld::DiagnosticPointer diagnostic);

        /**
         * Signal that is emitted when a diagnostic tied to the assoicated Ld::Element has been cleared.
         *
         * The signal will automatically be bound to the \ref Presentation::clearDiagnosticDisplay slot.
         */
        void diagnosticWasCleared();

    public slots:
        /**
         * Slot that can be triggered to request that the entire presentation be re-generated.  You can use this to
         * force every element to be repositioned and redrawn.
         */
        void redraw();

        /**
         * Slot that can be triggered to immediately abort any rendering/repositioning operation.  Note that the
         * windows contents will be incoherent after triggering this slot.
         */
        void abortRepositioning();

    protected slots:
        /**
         * Slot that is triggered when a change to the program is detected.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * The default implementation does nothing.
         *
         * \param[in] changedPresentation Pointer to the changed presentation.
         */
        virtual void processPresentationChanged(Presentation* changedPresentation);

        /**
         * Slot that is triggered when a new element is added.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * The default implementation does nothing.
         *
         * \param[in] newPresentation Pointer to the newly added presentation.
         */
        virtual void processPresentationAdded(Presentation* newPresentation);

        /**
         * Slot that is triggered when an element is removed.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * The default implementation does nothing.
         *
         * \param[in] removedPresentation Pointer to the presentation that was removed.
         */
        virtual void processPresentationRemoved(Presentation* removedPresentation);

        /**
         * Slot that is triggered when a page format is changed.
         *
         * \param[in] pageNumber The page number of the first page touched by the format change.
         *
         * \param[in] oldFormat  The old page format that is being replaced.
         *
         * \param[in] newFormat  The new format assigned to the page.
         */
        virtual void processPageFormatChange(
            unsigned long                  pageNumber,
            QSharedPointer<Ld::PageFormat> oldFormat,
            QSharedPointer<Ld::PageFormat> newFormat
        );

        /**
         * Slot that is triggered when the root element and children are loaded or saved and become pristine.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * The default implementation does nothing.
         */
        virtual void processNowPristine();

        /**
         * Slot that is triggered when a child element is first modified and the program is no longer pristine.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * The default implementation does nothing.
         */
        virtual void processNowModified();

        /**
         * Slot that is triggered whenever the root element or any child is modified, including changes to references.
         * This slot will not be triggered when the root element is loaded or saved.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * The default implementation does nothing.
         */
        virtual void processNowChanged();

        /**
         * Slot that is triggered when references are updated.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * The default implementation does nothing.
         */
        virtual void processReferenceChanges();

        /**
         * Slot that is triggered when the program is saved under its current name.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * The default implementation does nothing.
         *
         * \param[in] filename The filename the program was saved under.
         */
        virtual void processProgramSaved(const QString& filename);

        /**
         * Slot that is triggered when the program is saved under a new name.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * The default implementation does nothing.
         *
         * \param[in] filename The filename the program was saved under.
         */
        virtual void processProgramSavedAs(const QString& filename);

        /**
         * Slot that is triggered when an error is detected during a program load operation.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * The default implementation does nothing.
         *
         * \param[in] errorMessage A value used to indicate the reason for the failure.
         */
        virtual void processProgramSaveFailed(const QString& errorMessage);

        /**
         * Slot that is triggered when a new program is loaded under this root node.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * The default implementation does nothing.
         *
         * \param[in] filename     The filename the program was saved under.
         *
         * \param[in] majorVersion The major version number of the XML description.
         *
         * \param[in] minorVersion The minor version number of the XML description.
         */
        virtual void processProgramLoaded(const QString& filename, unsigned majorVersion, unsigned minorVersion);

        /**
         * Slot that is triggered when an error is detected during a program load operation.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * The default implementation does nothing.
         *
         * \param[in] errorMessage A value used to indicate the reason for the failure.
         */
        virtual void processProgramLoadFailed(const QString& errorMessage);

        /**
         * Slot that is triggered when a new and pristine program is created under this root node.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * The default implementation does nothing.
         */
        virtual void processNewProgramCreated();

        /**
         * Slot that is triggered just before a program is closed.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * The default implementation does nothing.
         */
        virtual void processProgramClosed();

        /**
         * Slot that is triggered when a child is removed by the associated
         * Ld::ElementWithPositionalChildren class.  This slot is triggered by both the
         * Ld::ElementWithPositionalChildren::removeChild(Ld::ElementPointer) method and the
         * Ld::ElementWithPositionalChildren::removeChild(unsigned long) method.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * The default implementation does nothing.
         *
         * \param[in] childIndex        The zero based index of the removed child.
         *
         * \param[in] childPresentation The child element being removed.
         */
        virtual void processRemovingChildPresentation(unsigned long childIndex, Presentation* childPresentation);

        /**
         * Slot that is triggered when a child is inserted before a specified location by the
         * Ld::ElementWithPositionalChildren class.  This slot is triggered by the
         * Ld::ElementWithPositionalChildren::insertBefore method.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * The default implementation does nothing.
         *
         * \param[in] childIndex        The zero based index where the desired child should be placed.
         *
         * \param[in] childPresentation The new child presentation being placed.
         */
        virtual void processChildPresentationInsertedBefore(unsigned long childIndex, Presentation* childPresentation);

        /**
         * Slot that is triggered when a child is inserted after a specified location by the
         * Ld::ElementWithPositionalChildren class.  This slot is triggered by the
         * Ld::ElementWithPositionalChildren::insertAfter method.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * The default implementation does nothing.
         *
         * \param[in] childIndex        The zero based index where the desired child should be placed.
         *
         * \param[in] childPresentation The new child presentation being placed.
         */
        virtual void processChildPresentationInsertedAfter(unsigned long childIndex, Presentation* childPresentation);

        /**
         * Slot that is triggered when this object is newly coupled to an element.  You can use this slot to trigger
         * updates to the presentation based on the element contents and type.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * \param[in] element The element that this visual is now tied to.
         */
        virtual void tiedToElement(Ld::ElementPointer element);

        /**
         * Slot that is triggered when this object is decoupled from an element.  The default implementation will cause
         * this object to be destroyed.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries and to
         * provides a controlled clean-up of this presentation class.
         *
         * The default implementation calls QObject::deleteLater().
         */
        virtual void untiedFromElement();

        /**
         * Slot that is triggered when a Ld::Element reports that its format has been changed.  You can use this slot
         * to perform updates to the visual representation of the element.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * The default version of this slot simply returns.
         *
         * \param[in] oldFormat A pointer to the old Ld::Format instance.  This value may be a null pointer if the
         *                      Ld::Element did not previously have an associated Ld::Format instance.
         *
         * \param[in] newFormat A pointer to the new Ld::Format instance.  This pointer will be the same
         *                      as the oldFormat pointer if the Ld::Format instance was not changed.
         */
        virtual void processFormatChange(Ld::FormatPointer oldFormat, Ld::FormatPointer newFormat);

        /**
         * Slot that is triggered when a Ld::Element reports that a descendant's format has been changed.  The default
         * implementation performs no action.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * \param[in] changedElement The element who's format has been changed.
         *
         * \param[in] newFormat      The new format associated with this element.
         */
        virtual void processDescendantFormatChange(Ld::ElementPointer changedElement, Ld::FormatPointer newFormat);

        /**
         * Slot that is triggered when a Ld::Element reports that a descendant's format has been updated.  The default
         * implementation performs no action.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual to occur across thread boundaries.
         *
         * \param[in] changedElement The element who's format has been changed.
         *
         * \param[in] newFormat      The updated format associated with this element.
         */
        virtual void processDescendantFormatUpdate(Ld::ElementPointer changedElement, Ld::FormatPointer newFormat);

        /**
         * Slot that is triggered when an aspect of the associated Ld::Element has been changed.  This slot exists to
         * cover such things as text updates that are not covered by the addition or removal of children, nor format
         * updates.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual class to occur across thread boundaries.
         *
         * The default version of this slot simply returns.
         */
        virtual void processDataChange();

        /**
         * Slot that is triggered when a diagnostic is reported for the Ld::Element tied to this \ref Presentation.
         * You can overload this method if you wish to catch diagnostics reported for this presentation.
         *
         * The slot primarily exists to allow call-backs from the Ld::Visual class to occur across thread boundaries.
         *
         * \param[in] diagnostic A code indicating the type of error.
         */
        virtual void processDiagnostic(Ld::DiagnosticPointer diagnostic);

        /**
         * Slot that is triggered when any pending diagnostic conditions tied to this presentation are cleared.  The
         * default implementation will remove any red borders drawn around the graphics items tied to this presentation.
         */
        virtual void clearDiagnosticDisplay();

    private slots:
        /**
         * Slot that is triggered when the maximum page extents has changed.  The slot is used to forward notification
         * to the visible ruler(s).
         *
         * \param[out] newMaximumHorizontalExtentsPoints The new width of the page columns, in points.
         *
         * \param[out] newPageLeftEdgePoints             The new left edges of each page within the scene, translated to
         *                                               points.
         */
        void processPageMaximumExtentsChanged(
            const QList<double>& newMaximumHorizontalExtentsPoints,
            const QList<double>& newPageLeftEdgePoints
        );

        /**
         * Slot that is triggered when repositioning has been requested.
         */
        void performRepositioning();

    private:
        /**
         * Method that is called when a change to the program is detected.
         *
         * \param[in] changedElement Pointer to the changed element.
         */
        void elementChanged(Ld::ElementPointer changedElement) final;

        /**
         * Method that is called when a new element is added.
         *
         * \param[in] newElement Pointer to the newly added element.
         */
        void elementAdded(Ld::ElementPointer newElement) final;

        /**
         * Method that is called when an element is removed.
         *
         * \param[in] removedElement Pointer to the element that is about to be removed.
         */
        void elementRemoved(Ld::ElementPointer removedElement) final;

        /**
         * Method that is called when the root element and children are loaded or saved and become pristine.
         */
        void nowPristine() final;

        /**
         * Virtual method that is called when the root element or children are first modified.
         */
        void nowModified() final;

        /**
         * Virtual method that is called whenever the root element or any child is modified, including changes to
         * references.  This method will not be called when the root element is loaded or saved.
         */
        void nowChanged() final;

        /**
         * Method that is called when a page format is changed.
         *
         * \param[in] pageNumber The page number of the first page touched by the format change.
         *
         * \param[in] oldFormat  The old page format that is being replaced.
         *
         * \param[in] newFormat  The new format assigned to the page.
         */
        void pageFormatChanged(
            unsigned long                  pageNumber,
            QSharedPointer<Ld::PageFormat> oldFormat,
            QSharedPointer<Ld::PageFormat> newFormat
        ) final;

        /**
         * Method that is called when references are updated.
         */
        void referencesChanged() final;

        /**
         * Method that is called when the program is saved under its current name.
         *
         * \param[in] filename The filename the program was saved under.
         */
        void programSaved(const QString& filename) final;

        /**
         * Method that is called when the program is saved under a new name.
         *
         * \param[in] filename The filename the program was saved under.
         */
        void programSavedAs(const QString& filename) final;

        /**
         * Method that is called when an error is detected during a program load operation.
         *
         * \param[in] errorMessage A message explaining the reason for the failure.
         */
        void programSaveFailed(const QString& errorMessage) final;

        /**
         * Method that is called when a new program is loaded under this root node.
         *
         * \param[in] filename     The filename the program was saved under.
         *
         * \param[in] majorVersion The major version number of the XML description.
         *
         * \param[in] minorVersion The minor version number of the XML description.
         */
        void programLoaded(const QString& filename, unsigned majorVersion, unsigned minorVersion) final;

        /**
         * Method that is called when an error is detected during a program load operation.
         *
         * \param[in] errorMessage A message explaining the reason for the failure.
         */
        void programLoadFailed(const QString& errorMessage) final;

        /**
         * Method that is called when a new and pristine program is created under this root node.
         */
        void newProgramCreated() final;

        /**
         * Method that is called just before a program is closed.
         */
        void aboutToClose() final;

        /**
         * Method that is called just before a child is removed by the associated
         * Ld::ElementWithPositionalChildren class.  This method is triggered by both the
         * Ld::ElementWithPositionalChildren::removeChild(Ld::ElementPointer) method and the
         * Ld::ElementWithPositionalChildren::removeChild(unsigned long) method.
         *
         * \param[in] childIndex   The zero based index of the removed child.
         *
         * \param[in] childElement The child element being removed.
         */
        void aboutToRemoveChild(unsigned long childIndex, Ld::ElementPointer childElement) final;

        /**
         * Method that is called when a child is inserted before a specified location by the
         * Ld::ElementWithPositionalChildren class.  This method is triggered by the
         * Ld::ElementWithPositionalChildren::insertBefore method.
         *
         * \param[in] childIndex   The zero based index where the desired child should be placed.
         *
         * \param[in] childElement The new child element being placed.
         */
        void childInsertedBefore(unsigned long childIndex, Ld::ElementPointer childElement) final;

        /**
         * Method that is called when a child is inserted after a specified location by the
         * Ld::ElementWithPositionalChildren class.  This method is triggered by the
         * Ld::ElementWithPositionalChildren::insertAfter method.
         *
         * \param[in] childIndex   The zero based index where the desired child should be placed.
         *
         * \param[in] childElement The new child element being placed.
         */
        void childInsertedAfter(unsigned long childIndex, Ld::ElementPointer childElement) final;

        /**
         * Method that is called when this visual is tied to an element.
         *
         * \param[in] element The element that this visual is now tied to.
         */
        void coupledToElement(Ld::ElementPointer element) final;

        /**
         * Method that is called when this visual is disconnected from an element.
         */
        void decoupledFromElement() final;

        /**
         * Method that is called when the format tied to the associated Ld::Element changes or is updated.  The method
         * can be triggered by either the Ld::Element::setFormat method or by the Ld::Format::reportUpdate method.
         *
         * \param[in] oldFormat A pointer to the old Ld::Format instance.  This value may be a null pointer if the
         *                      Ld::Element did not previously have an associated Ld::Format instance.
         *
         * \param[in] newFormat A pointer to the new Ld::Format instance.  This pointer will be the same as the
         *                      oldFormat pointer if the Ld::Format instance was not changed.
         */
        void formatChanged(Ld::FormatPointer oldFormat, Ld::FormatPointer newFormat) final;

        /**
         * Method that is triggered whenever a descendant's format has been changed.
         *
         * \param[in] changedElement The element who's format has been changed.
         *
         * \param[in] newFormat      The new format associated with this element.
         */
        void descendantFormatChanged(Ld::ElementPointer changedElement, Ld::FormatPointer newFormat) final;

        /**
         * Method that is triggered whenever a descendant's format has been updated.
         *
         * \param[in] changedElement The element who's format has been changed.
         *
         * \param[in] newFormat      The updated format associated with this element.
         */
        void descendantFormatUpdated(Ld::ElementPointer changedElement, Ld::FormatPointer newFormat) final;

        /**
         * Method that is called when an aspect of the associated Ld::Element has been changed.  This method exists to
         * cover such things as text updates that are not covered by the addition or removal of children, nor format
         * updates.
         */
        void elementDataChanged() final;

        /**
         * Method that is called to indicate that a diagnostic was just reported for the associated element.  Note
         * that this method may be called from multiple threads.
         *
         * \param[in] diagnostic A code indicating the type of error.
         */
        void diagnosticReported(Ld::DiagnosticPointer diagnostic) final;

        /**
         * Method that is called to indicate that any pending diagnostic condition has been cleared.  Note that this
         * method may be called even if there is no reported diagnostic and may be called from multiple threads.
         */
        void diagnosticCleared() final;

        /**
         * Method that can be called to reset all the underlying cached placement data.  This method is called when
         * the entire scene needs to be redrawn.
         */
        void resetPlacement() final;

        /**
         * Method that is called when this placement negotiator needs to remove itself from the scene.  You should
         * overload this method in derived classes with code that removes and optionally destroys placement areas from
         * the parent.
         */
        void removeFromScene() final;

        /**
         * Method that is used to obtain an active area rectangle.
         *
         * \param[in] pageIndex The zero based page index of the page whose active area we want.
         */
        QRectF activeAreaRectangle(unsigned long pageIndex);

        /**
         * Method that is called to position any pending presentation areas.
         */
        void positionPresentationAreas();

        /**
         * Method that triggers immediate repositioning of a child based on a child index.
         *
         * \param[in] childIndex The zero based index of the child to reposition.
         */
        void requestRepositioning(unsigned long childIndex = 0);

        /**
         * Method that calculates the scene bounding rectangle that are currently visible.
         *
         * \return Returns the view bounding rectangles in scene coordinates.
         */
        QList<QRectF> visibleSceneBoundingRectangles() const;

        /**
         * Method that is called to calculate the placement of a child presentation.
         *
         * \param[in] placementTracker   A pointer to the placement tracker that can be used to track and update status
         *                               during a time-consuming placement operation.
         *
         * \param[out] childLocation     The child location data.
         *
         * \param[in]  childPresentation Pointer to the child presentation to recalculate placement for.
         *
         * \param[in]  childIndex        The zero based index of the child.
         *
         * \param[in]  minimumTopSpacing The minimum top spacing to impose on the child.
         */
        void recalculateChildPlacement(
            PlacementTracker*  placementTracker,
            RootChildLocation& childLocation,
            Presentation*      childPresentation,
            unsigned long      childIndex,
            float              minimumTopSpacing
        );

        /**
         * Method that is called to move a child.
         *
         * \param[out] childLocation     The child location data.
         *
         * \param[in]  childPresentation Pointer to the child presentation to recalculate placement for.
         *
         * \param[in]  minimumTopSpacing The minimum top spacing to impose on the child.
         */
        void moveChild(RootChildLocation& childLocation, Presentation* childPresentation, float minimumTopSpacing);

        /**
         * The maximum allowed error during repositioning.  This value is used to squash additional repositioning
         * operations due to tiny positioning errors introduced by round-off.
         */
        static constexpr float maximumAllowedRepositionVerticalError = 1.0E-3F;

        /**
         * Flag that indicates if presentation updates are pending.  This flag is set when a descendant is updated and
         * cleared when repositioning has completed.
         */
        bool currentPresentationUpdatesPending;

        /**
         * The page list used to track and manage pages.
         */
        PageList pageList;

        /**
         * The current maximum horizontal extents in points.  This value represents the current width of the largest
         * page in the document.
         */
        double currentMaximumHorizontalExtentPoints;

        /**
         * The current page index.
         */
        unsigned currentPageIndex;

        /**
         * The current active area rectangle.
         */
        QRectF currentActiveArea;

        /**
         * The current cursor Y position.
         */
        double cursorY;

        /**
         * Timer used to trigger repositioning of children.
         */
        QTimer* repositionTimer;

        /**
         * The placement status notifier object.
         */
        PlacementStatusNotifier* placementStatusNotifier;

        /**
         * The index of the first child to perform repositioning on.
         */
        unsigned long firstChildForRepositioning;

        /**
         * The index of the last child to perform repositioning on.
         */
        unsigned long lastChildForRepositioning;

        /**
         * The index of the last fully rendered child presentation.
         */
        unsigned long lastFullyRenderedChildPresentation;

        /**
         * Flag that is set if we should always recalculate the position of everything over the desired range of
         * children.
         */
        bool recalculateAllChildPositions;

        /**
         * Flag that indicates that a repositioning operation is in progress.
         */
        bool repositionInProgress;

        /**
         * Flag that is set if we have a pending repositioning request.
         */
        bool repositionRequestPending;

        /**
         * List of locations for each child of the root presentation.  This list is used to short-circuit processing
         * of placement.
         */
        QList<RootChildLocation> currentChildLocations;
};

#endif
