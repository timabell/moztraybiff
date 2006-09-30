/*
	Taken from a Mozilla Thunderbird 1.0 build.
	Kept for compatibility when building against a 1.5 tree.
*/

#ifndef nsIWidget10_h__
#define nsIWidget10_h__

#include "nsISupports.h"
#include "nsColor.h"
#include "nsIMouseListener.h"
#include "nsIMenuListener.h"
#include "nsCoord.h"

#include "prthread.h"
#include "nsEvent.h"
#include "nsString.h"

// forward declarations
class   nsIAppShell;
class   nsIToolkit;
class   nsIFontMetrics;
class   nsIToolkit;
class   nsIRenderingContext;
class   nsIEnumerator;
class   nsIDeviceContext;
class   nsIRegion;
struct  nsRect;
struct  nsFont;
class   nsIMenuBar;
class   nsIEventListener;
class   nsIRollupListener;
class   nsGUIEvent;
struct  nsColorMap;

/**
 * Callback function that processes events.
 * The argument is actually a subtype (subclass) of nsEvent which carries
 * platform specific information about the event. Platform specific code knows
 * how to deal with it.
 * The return value determines whether or not the default action should take place.
 */

typedef nsEventStatus (*PR_CALLBACK EVENT_CALLBACK)(nsGUIEvent *event);

// {18032AD5-B265-11d1-AA2A-000000000000}
#define NS_IWIDGET10_IID \
{ 0x18032ad5, 0xb265, 0x11d1, \
{ 0xaa, 0x2a, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 } }


// Hide the native window systems real window type so as to avoid
// including native window system types and api's. This is necessary
// to ensure cross-platform code.
typedef void* nsNativeWidget;

/**
 * The base class for all the widgets. It provides the interface for
 * all basic and necessary functionality.
 */
class nsIWidget10 : public nsISupports {

  public:

    NS_DEFINE_STATIC_IID_ACCESSOR(NS_IWIDGET10_IID)

    /**
     * Create and initialize a widget. 
     *
     * The widget represents a window that can be drawn into. It also is the 
     * base class for user-interface widgets such as buttons and text boxes.
     *
     * All the arguments can be NULL in which case a top level window
     * with size 0 is created. The event callback function has to be
     * provided only if the caller wants to deal with the events this
     * widget receives.  The event callback is basically a preprocess
     * hook called synchronously. The return value determines whether
     * the event goes to the default window procedure or it is hidden
     * to the os. The assumption is that if the event handler returns
     * false the widget does not see the event. The widget should not 
     * automatically clear the window to the background color. The 
     * calling code must handle paint messages and clear the background 
     * itself. 
     *
     * @param     parent or null if it's a top level window
     * @param     aRect     the widget dimension
     * @param     aHandleEventFunction the event handler callback function
     * @param     aContext
     * @param     aAppShell the parent application shell. If nsnull,
     *                      the parent window's application shell will be used.
     * @param     aToolkit
     * @param     aInitData data that is used for widget initialization
     *
     */
    NS_IMETHOD Create(nsIWidget10        *aParent,
                        const nsRect     &aRect,
                        EVENT_CALLBACK   aHandleEventFunction,
                        nsIDeviceContext *aContext,
                        nsIAppShell      *aAppShell = nsnull,
                        nsIToolkit       *aToolkit = nsnull,
                        nsWidgetInitData *aInitData = nsnull) = 0;

    /**
     * Create and initialize a widget with a native window parent
     *
     * The widget represents a window that can be drawn into. It also is the 
     * base class for user-interface widgets such as buttons and text boxes.
     *
     * All the arguments can be NULL in which case a top level window
     * with size 0 is created. The event callback function has to be
     * provided only if the caller wants to deal with the events this
     * widget receives.  The event callback is basically a preprocess
     * hook called synchronously. The return value determines whether
     * the event goes to the default window procedure or it is hidden
     * to the os. The assumption is that if the event handler returns
     * false the widget does not see the event.
     *
     * @param     aParent   native window.
     * @param     aRect     the widget dimension
     * @param     aHandleEventFunction the event handler callback function
     */
    NS_IMETHOD Create(nsNativeWidget aParent,
                        const nsRect     &aRect,
                        EVENT_CALLBACK   aHandleEventFunction,
                        nsIDeviceContext *aContext,
                        nsIAppShell      *aAppShell = nsnull,
                        nsIToolkit       *aToolkit = nsnull,
                        nsWidgetInitData *aInitData = nsnull) = 0;


    /**
     * Accessor functions to get and set the client data associated with the
     * widget.
     */
    //@{
    NS_IMETHOD  GetClientData(void*& aClientData) = 0;
    NS_IMETHOD  SetClientData(void* aClientData) = 0;
    //@}

    /**
     * Close and destroy the internal native window. 
     * This method does not delete the widget.
     */

    NS_IMETHOD Destroy(void) = 0;


    /**
     * Reparent a widget
     *
     * Change the widgets parent
     *
     * @param     aNewParent   new parent 
     */
    NS_IMETHOD SetParent(nsIWidget10* aNewParent) = 0;


    /**
     * Return the parent Widget of this Widget or nsnull if this is a 
     * top level window
     *
     * @return the parent widget or nsnull if it does not have a parent
     *
     */
    virtual nsIWidget10* GetParent(void) = 0;

    /**
     * Return an nsEnumerator over the children of this widget.
     *
     * @return an enumerator over the list of children or nsnull if it does not
     * have any children
     *
     */
    virtual nsIEnumerator*  GetChildren(void) = 0;

    /**
     * Show or hide this widget
     *
     * @param aState PR_TRUE to show the Widget, PR_FALSE to hide it
     *
     */
    NS_IMETHOD Show(PRBool aState) = 0;

    /**
     * Make the window modal
     *
     */
    NS_IMETHOD SetModal(PRBool aModal) = 0;

    /**
     * Returns whether the window is visible
     *
     */
    NS_IMETHOD IsVisible(PRBool & aState) = 0;

    /**
     * Perform platform-dependent sanity check on a potential window position.
     * This is guaranteed to work only for top-level windows.
     *
     * @param aAllowSlop: if true, allow the window to slop offscreen;
     *                    the window should be partially visible. if false,
     *                    force the entire window onscreen (or at least
     *                    the upper-left corner, if it's too large).
     * @param aX in: an x position expressed in screen coordinates.
     *           out: the x position constrained to fit on the screen(s).
     * @param aY in: an y position expressed in screen coordinates.
     *           out: the y position constrained to fit on the screen(s).
     * @return vapid success indication. but see also the parameters.
     *
     **/
    NS_IMETHOD ConstrainPosition(PRBool aAllowSlop,
                                 PRInt32 *aX,
                                 PRInt32 *aY) = 0;

    /**
     * Move this widget.
     *
     * @param aX the new x position expressed in the parent's coordinate system
     * @param aY the new y position expressed in the parent's coordinate system
     *
     **/
    NS_IMETHOD Move(PRInt32 aX, PRInt32 aY) = 0;

    /**
     * Resize this widget. 
     *
     * @param aWidth  the new width expressed in the parent's coordinate system
     * @param aHeight the new height expressed in the parent's coordinate system
     * @param aRepaint whether the widget should be repainted
     *
     */
    NS_IMETHOD Resize(PRInt32 aWidth,
                      PRInt32 aHeight,
                      PRBool   aRepaint) = 0;

    /**
     * Move or resize this widget.
     *
     * @param aX       the new x position expressed in the parent's coordinate system
     * @param aY       the new y position expressed in the parent's coordinate system
     * @param aWidth   the new width expressed in the parent's coordinate system
     * @param aHeight  the new height expressed in the parent's coordinate system
     * @param aRepaint whether the widget should be repainted if the size changes
     *
     */
    NS_IMETHOD Resize(PRInt32 aX,
                      PRInt32 aY,
                      PRInt32 aWidth,
                      PRInt32 aHeight,
                      PRBool   aRepaint) = 0;

    /**
     * Sets the widget's z-index.
     */
    NS_IMETHOD SetZIndex(PRInt32 aZIndex) = 0;

    /**
     * Gets the widget's z-index. 
     */
    NS_IMETHOD GetZIndex(PRInt32* aZIndex) = 0;

    /**
     * Position this widget just behind the given widget. (Used to
     * control z-order for top-level widgets. Get/SetZIndex by contrast
     * control z-order for child widgets of other widgets.)
     * @param aPlacement top, bottom, or below a widget
     *                   (if top or bottom, param aWidget is ignored)
     * @param aWidget    widget to place this widget behind
     *                   (only if aPlacement is eZPlacementBelow).
     *                   null is equivalent to aPlacement of eZPlacementTop
     * @param aActivate  true to activate the widget after placing it
     */
    NS_IMETHOD PlaceBehind(nsTopLevelWidgetZPlacement aPlacement,
                           nsIWidget10 *aWidget, PRBool aActivate) = 0;

    /**
     * Minimize, maximize or normalize the window size.
     * Takes a value from nsSizeMode (see nsGUIEvent.h)
     */
    NS_IMETHOD SetSizeMode(PRInt32 aMode) = 0;

    /**
     * Return size mode (minimized, maximized, normalized).
     * Returns a value from nsSizeMode (see nsGUIEvent.h)
     */
    NS_IMETHOD GetSizeMode(PRInt32* aMode) = 0;

    /**
     * Enable or disable this Widget
     *
     * @param aState PR_TRUE to enable the Widget, PR_FALSE to disable it.
     *
     */
    NS_IMETHOD Enable(PRBool aState) = 0;

    /**
     * Ask whether the widget is enabled
     * @param aState returns PR_TRUE if the widget is enabled
     */
    NS_IMETHOD IsEnabled(PRBool *aState) = 0;

    /**
     * Give focus to this widget.
     */
    NS_IMETHOD SetFocus(PRBool aRaise = PR_FALSE) = 0;

    /**
     * Get this widget's outside dimensions relative to its parent widget
     *
     * @param aRect on return it holds the  x, y, width and height of this widget
     *
     */
    NS_IMETHOD GetBounds(nsRect &aRect) = 0;


    /**
     * Get this widget's outside dimensions in global coordinates. (One might think this
     * could be accomplished by stringing together other methods in this interface, but
     * then one would bloody one's nose on different coordinate system handling by different
     * platforms.)
     *
     * @param aRect on return it holds the  x, y, width and height of this widget
     *
     */
    NS_IMETHOD GetScreenBounds(nsRect &aRect) = 0;


    /**
     * Get this widget's client area dimensions, if the window has a 3D border appearance
     * this returns the area inside the border, The x and y are always zero
     *
     * @param aRect on return it holds the  x. y, width and height of the client area of this widget
     *
     */
    NS_IMETHOD GetClientBounds(nsRect &aRect) = 0;

    /**
     * Gets the width and height of the borders
     * @param aWidth the width of the border
     * @param aHeight the height of the border
     *
     */
    NS_IMETHOD GetBorderSize(PRInt32 &aWidth, PRInt32 &aHeight) = 0;

    /**
     * Get the foreground color for this widget
     *
     * @return this widget's foreground color
     *
     */
    virtual nscolor GetForegroundColor(void) = 0;

    /**
     * Set the foreground color for this widget
     *
     * @param aColor the new foreground color
     *
     */

    NS_IMETHOD SetForegroundColor(const nscolor &aColor) = 0;

    /**
     * Get the background color for this widget
     *
     * @return this widget's background color
     *
     */

    virtual nscolor GetBackgroundColor(void) = 0;

    /**
     * Set the background color for this widget
     *
     * @param aColor the new background color
     *
     */

    NS_IMETHOD SetBackgroundColor(const nscolor &aColor) = 0;

    /**
     * Get the font for this widget
     *
     * @return the font metrics 
     */

    virtual nsIFontMetrics* GetFont(void) = 0;

    /**
     * Set the font for this widget 
     *
     * @param aFont font to display. See nsFont for allowable fonts
     */

    NS_IMETHOD SetFont(const nsFont &aFont) = 0;

    /**
     * Get the cursor for this widget.
     *
     * @return this widget's cursor.
     */

    virtual nsCursor GetCursor(void) = 0;

    /**
     * Set the cursor for this widget
     *
     * @param aCursor the new cursor for this widget
     */

    NS_IMETHOD SetCursor(nsCursor aCursor) = 0;

    /** 
     * Get the window type of this widget
     *
     * @param aWindowType the window type of the widget
     */
    NS_IMETHOD GetWindowType(nsWindowType& aWindowType) = 0;

    /**
     * Set the translucency of the top-level window containing this widget.
     * So, e.g., if you call this on the widget for an IFRAME, the top level
     * browser window containing the IFRAME actually gets set. Be careful.
     *
     * This can fail if the platform doesn't support
     * transparency/translucency. By default widgets are not
     * transparent.  This will also fail if the toplevel window is not
     * a Mozilla window, e.g., if the widget is in an embedded
     * context.
     *
     * After translucency has been enabled, the initial alpha channel
     * value for all pixels is 1, i.e., opaque.
     * If the window is resized then the alpha channel values for
     * all pixels are reset to 1.
     * @param aTranslucent true if the window may have translucent
     *   or transparent pixels
     */
    NS_IMETHOD SetWindowTranslucency(PRBool aTranslucent) = 0;

    /**
     * Get the translucency of the top-level window that contains this
     * widget.
     * @param aTranslucent true if the window may have translucent or
     *   transparent pixels
     */
    NS_IMETHOD GetWindowTranslucency(PRBool& aTranslucent) = 0;

    /**
     * Update the alpha channel for some pixels of the top-level window
     * that contains this widget.
     * The window must have been made translucent using SetWindowTranslucency.
     * @param aRect the rect to update
     * @param aAlphas the alpha values, in w x h array, row-major order,
     * in units of 1/255. nsBlender::GetAlphas is a good way to compute this array.
     */
    NS_IMETHOD UpdateTranslucentWindowAlpha(const nsRect& aRect, PRUint8* aAlphas) = 0;

    /** 
     * Hide window chrome (borders, buttons) for this widget.
     *
     */
    NS_IMETHOD HideWindowChrome(PRBool aShouldHide) = 0;

    /**
     * Put the toplevel window into or out of fullscreen mode.
     *
     */
    NS_IMETHOD MakeFullScreen(PRBool aFullScreen) = 0;

    /**
     * Validate the widget.
     *
     */
    NS_IMETHOD Validate() = 0;

    /**
     * Invalidate the widget and repaint it.
     *
     * @param aIsSynchronouse PR_TRUE then repaint synchronously. If PR_FALSE repaint later.
     * @see #Update()
     */

    NS_IMETHOD Invalidate(PRBool aIsSynchronous) = 0;

    /**
     * Invalidate a specified rect for a widget and repaints it.
     *
     * @param aIsSynchronouse PR_TRUE then repaint synchronously. If PR_FALSE repaint later.
     * @see #Update()
     */

    NS_IMETHOD Invalidate(const nsRect & aRect, PRBool aIsSynchronous) = 0;

    /**
     * Invalidate a specified region for a widget and repaints it.
     *
     * @param aIsSynchronouse PR_TRUE then repaint synchronously. If PR_FALSE repaint later.
     * @see #Update()
     */

    NS_IMETHOD InvalidateRegion(const nsIRegion* aRegion, PRBool aIsSynchronous) = 0;

    /**
     * Force a synchronous repaint of the window if there are dirty rects.
     *
     * @see Invalidate()
     */

     NS_IMETHOD Update() = 0;

    /**
     * Adds a mouse listener to this widget
     * Any existing mouse listener is replaced
     *
     * @param aListener mouse listener to add to this widget.
     */

    NS_IMETHOD AddMouseListener(nsIMouseListener * aListener) = 0;

    /**
     * Adds an event listener to this widget
     * Any existing event listener is replaced
     *
     * @param aListener event listener to add to this widget.
     */

    NS_IMETHOD AddEventListener(nsIEventListener * aListener) = 0;

    /**
     * Adds a menu listener to this widget
     * Any existing menu listener is replaced
     *
     * @param aListener menu listener to add to this widget.
     */

    NS_IMETHOD AddMenuListener(nsIMenuListener * aListener) = 0;
    
    /**
     * Return the widget's toolkit
     *
     * @return the toolkit this widget was created in. See nsToolkit.
     */

    virtual nsIToolkit* GetToolkit() = 0;    

    /**
     * Set the color map for this widget
     *
     * @param aColorMap color map for displaying this widget
     *
     */

    NS_IMETHOD SetColorMap(nsColorMap *aColorMap) = 0;

    /**
     * XXX (This is obsolete and will be removed soon, Use ScrollWidgets instead)
     * Scroll this widget. 
     *
     * @param aDx amount to scroll along the x-axis
     * @param aDy amount to scroll along the y-axis.
     * @param aClipRect clipping rectangle to limit the scroll to.
     *
     */

    NS_IMETHOD Scroll(PRInt32 aDx, PRInt32 aDy, nsRect *aClipRect) = 0;

    /**
     * Scroll the contents of the widget. 
     * All child widgets are also scrolled by offsetting their coordinates.
     * A NS_PAINT message is synchronously dispatched for the newly exposed rectangle.
     *
     * @param aDx amount to scroll along the x-axis in pixels
     * @param aDy amount to scroll along the y-axis in pixels
     *
     */

    NS_IMETHOD ScrollWidgets(PRInt32 aDx, PRInt32 aDy) = 0;

    /**
     * Scroll an area of this widget. Child widgets are not scrolled.
     * A NS_PAINT message is synchronously dispatched for the newly exposed rectangle.
     *
     * @param aRect source rectangle to scroll in the widget in pixels
     * @param aDx x offset from the source in pixels
     * @param aDy y offset from the source in pixels
     *
     */

    NS_IMETHOD ScrollRect(nsRect &aSrcRect, PRInt32 aDx, PRInt32 aDy) = 0;

    /** 
     * Internal methods
     */

    //@{
    virtual void AddChild(nsIWidget10* aChild) = 0;
    virtual void RemoveChild(nsIWidget10* aChild) = 0;
    virtual void* GetNativeData(PRUint32 aDataType) = 0;
    virtual void FreeNativeData(void * data, PRUint32 aDataType) = 0;//~~~
    virtual nsIRenderingContext* GetRenderingContext() = 0;
    virtual nsIDeviceContext* GetDeviceContext() = 0;
    virtual nsIAppShell *GetAppShell() = 0;
    //@}

    /**
     * Set border style
     * Must be called before Create.
     * @param aBorderStyle @see nsBorderStyle
     */

    NS_IMETHOD SetBorderStyle(nsBorderStyle aBorderStyle) = 0;

    /**
     * Set the widget's title.
     * Must be called after Create.
     *
     * @param aTitle string displayed as the title of the widget
     */

    NS_IMETHOD SetTitle(const nsString& aTitle) = 0;

    /**
     * Set the widget's icon.
     * Must be called after Create.
     *
     * @param anIconSpec string specifying the icon to use; convention is to pass
     *                   a resource: URL from which a platform-dependent resource
     *                   file name will be constructed
     */

    NS_IMETHOD SetIcon(const nsAString& anIconSpec) = 0;

    /**
     * Set the widget's MenuBar.
     * Must be called after Create.
     *
     * @param aMenuBar the menubar
     */

    NS_IMETHOD SetMenuBar(nsIMenuBar * aMenuBar) = 0;

    /**
     * Set the widget's MenuBar's visibility
     *
     * @param aShow PR_TRUE to show, PR_FALSE to hide
     */

    NS_IMETHOD ShowMenuBar(PRBool aShow) = 0;

    /**
     * Convert from this widget coordinates to screen coordinates.
     *
     * @param  aOldRect  widget coordinates stored in the x,y members
     * @param  aNewRect  screen coordinates stored in the x,y members
     */

    NS_IMETHOD WidgetToScreen(const nsRect& aOldRect, nsRect& aNewRect) = 0;

    /**
     * Convert from screen coordinates to this widget's coordinates.
     *
     * @param  aOldRect  screen coordinates stored in the x,y members
     * @param  aNewRect  widget's coordinates stored in the x,y members
     */

    NS_IMETHOD ScreenToWidget(const nsRect& aOldRect, nsRect& aNewRect) = 0;

    /**
     * When adjustments are to made to a whole set of child widgets, call this
     * before resizing/positioning the child windows to minimize repaints. Must
     * be followed by EndResizingChildren() after child windows have been
     * adjusted.
     *
     */

    NS_IMETHOD BeginResizingChildren(void) = 0;

    /**
     * Call this when finished adjusting child windows. Must be preceded by
     * BeginResizingChildren().
     *
     */

    NS_IMETHOD EndResizingChildren(void) = 0;

    /**
     * Returns the preferred width and height for the widget
     *
     */
    NS_IMETHOD GetPreferredSize(PRInt32& aWidth, PRInt32& aHeight) = 0;

    /**
     * Set the preferred width and height for the widget
     *
     */
    NS_IMETHOD SetPreferredSize(PRInt32 aWidth, PRInt32 aHeight) = 0;

    /**
     * Dispatches and event to the widget
     *
     */
    NS_IMETHOD DispatchEvent(nsGUIEvent* event, nsEventStatus & aStatus) = 0;


    /**
     * For printing and lightweight widgets
     *
     */
    NS_IMETHOD Paint(nsIRenderingContext& aRenderingContext,
                     const nsRect& aDirtyRect) = 0;
   
    /**
     * Enables the dropping of files to a widget (XXX this is temporary)
     *
     */
    NS_IMETHOD EnableDragDrop(PRBool aEnable) = 0;
   
    virtual void  ConvertToDeviceCoordinates(nscoord	&aX,nscoord	&aY) = 0;

    /**
     * Enables/Disables system mouse capture.
     * @param aCapture PR_TRUE enables mouse capture, PR_FALSE disables mouse capture 
     *
     */
    NS_IMETHOD CaptureMouse(PRBool aCapture) = 0;

    /**
     * Gets the window class
     * implemented in gtk
     */
    NS_IMETHOD GetWindowClass(char *aClass) = 0;

    /**
     * Sets the window class
     * implemented in gtk
     */
    NS_IMETHOD SetWindowClass(char *aClass) = 0;

    /**
     * Enables/Disables system capture of any and all events that would cause a
     * dropdown to be rolled up, This method ignores the aConsumeRollupEvent 
     * parameter when aDoCapture is FALSE
     * @param aCapture PR_TRUE enables capture, PR_FALSE disables capture 
     * @param aConsumeRollupEvent PR_TRUE consumes the rollup event, PR_FALSE dispatches rollup event
     *
     */
    NS_IMETHOD CaptureRollupEvents(nsIRollupListener * aListener, PRBool aDoCapture, PRBool aConsumeRollupEvent) = 0;

    /**
     *   Determine whether a given event should be processed assuming we are
     * the currently active modal window.
     *   Note that the exact semantics of this method are platform-dependent.
     * The Macintosh, for instance, cares deeply that this method do exactly
     * as advertised. Gtk, for instance, handles modality in a completely
     * different fashion and does little if anything with this method.
     * @param aRealEvent event is real or a null placeholder (Macintosh)
     * @param aEvent void pointer to native event structure
     * @param aForWindow return value. PR_TRUE iff event should be processed.
     */
    NS_IMETHOD ModalEventFilter(PRBool aRealEvent, void *aEvent, PRBool *aForWindow) = 0;

    /**
     * Bring this window to the user's attention.  This is intended to be a more
     * gentle notification than popping the window to the top or putting up an
     * alert.  See, for example, Win32 FlashWindow or the NotificationManager on
     * the Mac.  The notification should be suppressed if the window is already
     * in the foreground and should be dismissed when the user brings this window
     * to the foreground.
     * @param aCycleCount Maximum number of times to animate the window per system 
     *                    conventions. If set to -1, cycles indefinitely until 
     *                    window is brought into the foreground.
     */
    NS_IMETHOD GetAttention(PRInt32 aCycleCount) = 0;

    /**
     * Get the last user input event time in milliseconds. If there are any pending
     * native toolkit input events it returns the current time. All input events are 
     * included (ie. it is *not* limited to events targeted at this nsIWidget10 instance.
     *
     * @param aTime Last user input time in milliseconds. This value can be used to compare
     * durations but can not be used for determining wall clock time. The value returned 
     * is platform dependent, but is compatible with the expression 
     * PR_IntervalToMicroseconds(PR_IntervalNow()).
     */
    NS_IMETHOD GetLastInputEventTime(PRUint32& aTime) = 0;

};

#endif // nsIWidget10_h__
