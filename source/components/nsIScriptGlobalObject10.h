#ifndef nsIScriptGlobalObject10_h__
#define nsIScriptGlobalObject10_h__

#include "nsISupports.h"
#include "nsEvent.h"

class nsIScriptContext;
class nsIDOMDocument;
class nsIDOMEvent;
class nsPresContext;
class nsIDocShell10;
class nsIDOMWindowInternal;
class nsIScriptGlobalObjectOwner;
struct JSObject;

#define NS_ISCRIPTGLOBALOBJECT_IID \
{ 0xd326a211, 0xdc31, 0x45c6, \
 { 0x98, 0x97, 0x22, 0x11, 0xea, 0xbc, 0xd0, 0x1c } }

/**
 * The JavaScript specific global object. This often used to store
 * per-window global state.
 */

class nsIScriptGlobalObject10 : public nsISupports
{
public:
  NS_DEFINE_STATIC_IID_ACCESSOR(NS_ISCRIPTGLOBALOBJECT_IID)

  virtual void SetContext(nsIScriptContext *aContext) = 0;
  virtual nsIScriptContext *GetContext() = 0;
  virtual nsresult SetNewDocument(nsIDOMDocument *aDocument,
                                  nsISupports *aState,
                                  PRBool aRemoveEventListeners,
                                  PRBool aClearScope) = 0;
  virtual void SetDocShell(nsIDocShell10 *aDocShell) = 0;
  virtual nsIDocShell10 *GetDocShell() = 0;
  virtual void SetOpenerWindow(nsIDOMWindowInternal *aOpener)=0;

  /**
   * Let the script global object know who its owner is.
   * The script global object should not addref the owner. It
   * will be told when the owner goes away.
   * @return NS_OK if the method is successful
   */
  virtual void SetGlobalObjectOwner(nsIScriptGlobalObjectOwner* aOwner) = 0;

  /**
   * Get the owner of the script global object. The method
   * addrefs the returned reference according to regular
   * XPCOM rules, even though the internal reference itself
   * is a "weak" reference.
   */
  virtual nsIScriptGlobalObjectOwner *GetGlobalObjectOwner() = 0;

  virtual nsresult HandleDOMEvent(nsPresContext* aPresContext, 
                                  nsEvent* aEvent, 
                                  nsIDOMEvent** aDOMEvent,
                                  PRUint32 aFlags,
                                  nsEventStatus* aEventStatus)=0;

  virtual JSObject *GetGlobalJSObject() = 0;

  /**
   * Called when the global JSObject is finalized
   */

  virtual void OnFinalize(JSObject *aJSObject) = 0;

  /**
   * Called to enable/disable scripts.
   */
  virtual void SetScriptsEnabled(PRBool aEnabled, PRBool aFireTimeouts) = 0;

  /**
   * Set a new arguments array for this window. This will be set on
   * the window right away (if there's an existing document) and it
   * will also be installed on the window when the next document is
   * loaded.  If argc is nonzero, argv must be non-null.
   *
   * @param aArgc the number of args
   * @param aArgv the pointer to the args.  This may be cast to jsval* and the
   *        args are found at
   *        ((jsval*)aArgv)[0], ..., ((jsval*)aArgv)[aArgc - 1]
   */
  virtual nsresult SetNewArguments(PRUint32 aArgc, void* aArgv) = 0;
};

#endif
