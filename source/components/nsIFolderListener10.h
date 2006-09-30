/*
	Taken from a Mozilla Thunderbird 1.0 build.
	Kept for compatibility when building against a 1.5 tree.
*/

#ifndef __gen_nsIFolderListener10_h__
#define __gen_nsIFolderListener10_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

#ifndef __gen_nsIAtom_h__
#include "nsIAtom.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif
class nsIMsgFolder; /* forward declaration */

typedef PRUint32 folderListenerNotifyFlagValue;


/* starting interface:    nsIFolderListener10 */
#define NS_IFOLDERLISTENER10_IID_STR "d4ff86da-1dd1-11b2-a6f6-9fcd348bcc34"

#define NS_IFOLDERLISTENER10_IID \
  {0xd4ff86da, 0x1dd1, 0x11b2, \
    { 0xa6, 0xf6, 0x9f, 0xcd, 0x34, 0x8b, 0xcc, 0x34 }}

class NS_NO_VTABLE nsIFolderListener10 : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IFOLDERLISTENER10_IID)

  enum { added = 1U };

  /* void OnItemAdded (in nsISupports parentItem, in nsISupports item, in string viewString); */
  NS_IMETHOD OnItemAdded(nsISupports *parentItem, nsISupports *item, const char *viewString) = 0;

  enum { removed = 2U };

  /* void OnItemRemoved (in nsISupports parentItem, in nsISupports item, in string viewString); */
  NS_IMETHOD OnItemRemoved(nsISupports *parentItem, nsISupports *item, const char *viewString) = 0;

  enum { propertyChanged = 4U };

  /* void OnItemPropertyChanged (in nsISupports item, in nsIAtom property, in string oldValue, in string newValue); */
  NS_IMETHOD OnItemPropertyChanged(nsISupports *item, nsIAtom *property, const char *oldValue, const char *newValue) = 0;

  enum { intPropertyChanged = 8U };

  /* void OnItemIntPropertyChanged (in nsISupports item, in nsIAtom property, in long oldValue, in long newValue); */
  NS_IMETHOD OnItemIntPropertyChanged(nsISupports *item, nsIAtom *property, PRInt32 oldValue, PRInt32 newValue) = 0;

  enum { boolPropertyChanged = 16U };

  /* void OnItemBoolPropertyChanged (in nsISupports item, in nsIAtom property, in boolean oldValue, in boolean newValue); */
  NS_IMETHOD OnItemBoolPropertyChanged(nsISupports *item, nsIAtom *property, PRBool oldValue, PRBool newValue) = 0;

  enum { unicharPropertyChanged = 32U };

  /* void OnItemUnicharPropertyChanged (in nsISupports item, in nsIAtom property, in wstring oldValue, in wstring newValue); */
  NS_IMETHOD OnItemUnicharPropertyChanged(nsISupports *item, nsIAtom *property, const PRUnichar *oldValue, const PRUnichar *newValue) = 0;

  enum { propertyFlagChanged = 64U };

  /* void OnItemPropertyFlagChanged (in nsISupports item, in nsIAtom property, in unsigned long oldFlag, in unsigned long newFlag); */
  NS_IMETHOD OnItemPropertyFlagChanged(nsISupports *item, nsIAtom *property, PRUint32 oldFlag, PRUint32 newFlag) = 0;

  enum { event = 128U };

  /* void OnItemEvent (in nsIMsgFolder item, in nsIAtom event); */
  NS_IMETHOD OnItemEvent(nsIMsgFolder *item, nsIAtom *event) = 0;

  enum { all = 4294967295U };

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIFOLDERLISTENER10 \
  NS_IMETHOD OnItemAdded(nsISupports *parentItem, nsISupports *item, const char *viewString); \
  NS_IMETHOD OnItemRemoved(nsISupports *parentItem, nsISupports *item, const char *viewString); \
  NS_IMETHOD OnItemPropertyChanged(nsISupports *item, nsIAtom *property, const char *oldValue, const char *newValue); \
  NS_IMETHOD OnItemIntPropertyChanged(nsISupports *item, nsIAtom *property, PRInt32 oldValue, PRInt32 newValue); \
  NS_IMETHOD OnItemBoolPropertyChanged(nsISupports *item, nsIAtom *property, PRBool oldValue, PRBool newValue); \
  NS_IMETHOD OnItemUnicharPropertyChanged(nsISupports *item, nsIAtom *property, const PRUnichar *oldValue, const PRUnichar *newValue); \
  NS_IMETHOD OnItemPropertyFlagChanged(nsISupports *item, nsIAtom *property, PRUint32 oldFlag, PRUint32 newFlag); \
  NS_IMETHOD OnItemEvent(nsIMsgFolder *item, nsIAtom *event); \

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIFOLDERLISTENER10(_to) \
  NS_IMETHOD OnItemAdded(nsISupports *parentItem, nsISupports *item, const char *viewString) { return _to OnItemAdded(parentItem, item, viewString); } \
  NS_IMETHOD OnItemRemoved(nsISupports *parentItem, nsISupports *item, const char *viewString) { return _to OnItemRemoved(parentItem, item, viewString); } \
  NS_IMETHOD OnItemPropertyChanged(nsISupports *item, nsIAtom *property, const char *oldValue, const char *newValue) { return _to OnItemPropertyChanged(item, property, oldValue, newValue); } \
  NS_IMETHOD OnItemIntPropertyChanged(nsISupports *item, nsIAtom *property, PRInt32 oldValue, PRInt32 newValue) { return _to OnItemIntPropertyChanged(item, property, oldValue, newValue); } \
  NS_IMETHOD OnItemBoolPropertyChanged(nsISupports *item, nsIAtom *property, PRBool oldValue, PRBool newValue) { return _to OnItemBoolPropertyChanged(item, property, oldValue, newValue); } \
  NS_IMETHOD OnItemUnicharPropertyChanged(nsISupports *item, nsIAtom *property, const PRUnichar *oldValue, const PRUnichar *newValue) { return _to OnItemUnicharPropertyChanged(item, property, oldValue, newValue); } \
  NS_IMETHOD OnItemPropertyFlagChanged(nsISupports *item, nsIAtom *property, PRUint32 oldFlag, PRUint32 newFlag) { return _to OnItemPropertyFlagChanged(item, property, oldFlag, newFlag); } \
  NS_IMETHOD OnItemEvent(nsIMsgFolder *item, nsIAtom *event) { return _to OnItemEvent(item, event); } \

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIFOLDERLISTENER10(_to) \
  NS_IMETHOD OnItemAdded(nsISupports *parentItem, nsISupports *item, const char *viewString) { return !_to ? NS_ERROR_NULL_POINTER : _to->OnItemAdded(parentItem, item, viewString); } \
  NS_IMETHOD OnItemRemoved(nsISupports *parentItem, nsISupports *item, const char *viewString) { return !_to ? NS_ERROR_NULL_POINTER : _to->OnItemRemoved(parentItem, item, viewString); } \
  NS_IMETHOD OnItemPropertyChanged(nsISupports *item, nsIAtom *property, const char *oldValue, const char *newValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->OnItemPropertyChanged(item, property, oldValue, newValue); } \
  NS_IMETHOD OnItemIntPropertyChanged(nsISupports *item, nsIAtom *property, PRInt32 oldValue, PRInt32 newValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->OnItemIntPropertyChanged(item, property, oldValue, newValue); } \
  NS_IMETHOD OnItemBoolPropertyChanged(nsISupports *item, nsIAtom *property, PRBool oldValue, PRBool newValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->OnItemBoolPropertyChanged(item, property, oldValue, newValue); } \
  NS_IMETHOD OnItemUnicharPropertyChanged(nsISupports *item, nsIAtom *property, const PRUnichar *oldValue, const PRUnichar *newValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->OnItemUnicharPropertyChanged(item, property, oldValue, newValue); } \
  NS_IMETHOD OnItemPropertyFlagChanged(nsISupports *item, nsIAtom *property, PRUint32 oldFlag, PRUint32 newFlag) { return !_to ? NS_ERROR_NULL_POINTER : _to->OnItemPropertyFlagChanged(item, property, oldFlag, newFlag); } \
  NS_IMETHOD OnItemEvent(nsIMsgFolder *item, nsIAtom *event) { return !_to ? NS_ERROR_NULL_POINTER : _to->OnItemEvent(item, event); } \

#endif /* __gen_nsIFolderListener10_h__ */
