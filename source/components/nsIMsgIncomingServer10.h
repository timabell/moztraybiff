/*
	Taken from a Mozilla Thunderbird 1.0 build.
	Kept for compatibility when building against a 1.5 tree.
*/

#ifndef __gen_nsIMsgIncomingServer10_h__
#define __gen_nsIMsgIncomingServer10_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

#ifndef __gen_MailNewsTypes2_h__
#include "MailNewsTypes2.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif
class nsIFileSpec; /* forward declaration */

class nsIMsgFolder; /* forward declaration */

class nsIMsgFolderCache; /* forward declaration */

class nsIMsgWindow; /* forward declaration */

class nsIMsgFilterList; /* forward declaration */

class nsIMsgRetentionSettings; /* forward declaration */

class nsIMsgDownloadSettings; /* forward declaration */

class nsISpamSettings; /* forward declaration */

class nsIMsgFilterPlugin; /* forward declaration */

class nsIUrlListener; /* forward declaration */


/* starting interface:    nsIMsgIncomingServer10 */
#define NS_IMSGINCOMINGSERVER10_IID_STR "ad3ca01a-eff4-4a2b-8c4f-ae1a766d7e7b"

#define NS_IMSGINCOMINGSERVER10_IID \
  {0xad3ca01a, 0xeff4, 0x4a2b, \
    { 0x8c, 0x4f, 0xae, 0x1a, 0x76, 0x6d, 0x7e, 0x7b }}

class NS_NO_VTABLE nsIMsgIncomingServer10 : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IMSGINCOMINGSERVER10_IID)

  /**
   * internal pref key - guaranteed to be unique across all servers
   */
  /* attribute string key; */
  NS_IMETHOD GetKey(char * *aKey) = 0;
  NS_IMETHOD SetKey(const char * aKey) = 0;

  /**
   * pretty name - should be "userid on hostname"
   * if the pref is not set
   */
  /* attribute wstring prettyName; */
  NS_IMETHOD GetPrettyName(PRUnichar * *aPrettyName) = 0;
  NS_IMETHOD SetPrettyName(const PRUnichar * aPrettyName) = 0;

  /**
  * helper function to construct the pretty name in a server type
  * specific way - e.g., mail for foo@test.com, news on news.mozilla.org
  */
  /* readonly attribute wstring constructedPrettyName; */
  NS_IMETHOD GetConstructedPrettyName(PRUnichar * *aConstructedPrettyName) = 0;

  /**
   * hostname of the server
   */
  /* attribute string hostName; */
  NS_IMETHOD GetHostName(char * *aHostName) = 0;
  NS_IMETHOD SetHostName(const char * aHostName) = 0;

  /**
   * real hostname of the server (if server name is changed it's stored here)
   */
  /* attribute string realHostName; */
  NS_IMETHOD GetRealHostName(char * *aRealHostName) = 0;
  NS_IMETHOD SetRealHostName(const char * aRealHostName) = 0;

  /* attribute long port; */
  NS_IMETHOD GetPort(PRInt32 *aPort) = 0;
  NS_IMETHOD SetPort(PRInt32 aPort) = 0;

  /**
   * userid to log into the server
   */
  /* attribute string username; */
  NS_IMETHOD GetUsername(char * *aUsername) = 0;
  NS_IMETHOD SetUsername(const char * aUsername) = 0;

  /**
   * real username of the server (if username is changed it's stored here)
   */
  /* attribute string realUsername; */
  NS_IMETHOD GetRealUsername(char * *aRealUsername) = 0;
  NS_IMETHOD SetRealUsername(const char * aRealUsername) = 0;

  /**
   * protocol type, i.e. "pop3", "imap", "nntp", "none", etc
   * used to construct URLs
   */
  /* attribute string type; */
  NS_IMETHOD GetType(char * *aType) = 0;
  NS_IMETHOD SetType(const char * aType) = 0;

  /* readonly attribute AString accountManagerChrome; */
  NS_IMETHOD GetAccountManagerChrome(nsAString & aAccountManagerChrome) = 0;

  /**
   * the schema for the local mail store, such
   * as "mailbox", "imap", or "news"
   * used to construct URIs
   */
  /* readonly attribute string localStoreType; */
  NS_IMETHOD GetLocalStoreType(char * *aLocalStoreType) = 0;

  /* void OnUserOrHostNameChanged (in string oldName, in string newName); */
  NS_IMETHOD OnUserOrHostNameChanged(const char *oldName, const char *newName) = 0;

  /* attribute boolean rememberPassword; */
  NS_IMETHOD GetRememberPassword(PRBool *aRememberPassword) = 0;
  NS_IMETHOD SetRememberPassword(PRBool aRememberPassword) = 0;

  /* attribute string password; */
  NS_IMETHOD GetPassword(char * *aPassword) = 0;
  NS_IMETHOD SetPassword(const char * aPassword) = 0;

  /* string getPasswordWithUI (in wstring aPromptString, in wstring aPromptTitle, in nsIMsgWindow aMsgWindow, out boolean okayValue); */
  NS_IMETHOD GetPasswordWithUI(const PRUnichar *aPromptString, const PRUnichar *aPromptTitle, nsIMsgWindow *aMsgWindow, PRBool *okayValue, char **_retval) = 0;

  /* void forgetPassword (); */
  NS_IMETHOD ForgetPassword(void) = 0;

  /**
   * logon succeeded - persist password, if user chooses.
   */
  /* void storePassword (); */
  NS_IMETHOD StorePassword(void) = 0;

  /* void forgetSessionPassword (); */
  NS_IMETHOD ForgetSessionPassword(void) = 0;

  /* attribute boolean downloadOnBiff; */
  NS_IMETHOD GetDownloadOnBiff(PRBool *aDownloadOnBiff) = 0;
  NS_IMETHOD SetDownloadOnBiff(PRBool aDownloadOnBiff) = 0;

  /* attribute boolean doBiff; */
  NS_IMETHOD GetDoBiff(PRBool *aDoBiff) = 0;
  NS_IMETHOD SetDoBiff(PRBool aDoBiff) = 0;

  /* attribute long biffMinutes; */
  NS_IMETHOD GetBiffMinutes(PRInt32 *aBiffMinutes) = 0;
  NS_IMETHOD SetBiffMinutes(PRInt32 aBiffMinutes) = 0;

  /* attribute unsigned long biffState; */
  NS_IMETHOD GetBiffState(PRUint32 *aBiffState) = 0;
  NS_IMETHOD SetBiffState(PRUint32 aBiffState) = 0;

  /* attribute boolean performingBiff; */
  NS_IMETHOD GetPerformingBiff(PRBool *aPerformingBiff) = 0;
  NS_IMETHOD SetPerformingBiff(PRBool aPerformingBiff) = 0;

  /* attribute nsIFileSpec localPath; */
  NS_IMETHOD GetLocalPath(nsIFileSpec * *aLocalPath) = 0;
  NS_IMETHOD SetLocalPath(nsIFileSpec * aLocalPath) = 0;

  /* readonly attribute string serverURI; */
  NS_IMETHOD GetServerURI(char * *aServerURI) = 0;

  /* attribute nsIMsgFolder rootFolder; */
  NS_IMETHOD GetRootFolder(nsIMsgFolder * *aRootFolder) = 0;
  NS_IMETHOD SetRootFolder(nsIMsgFolder * aRootFolder) = 0;

  /* readonly attribute nsIMsgFolder rootMsgFolder; */
  NS_IMETHOD GetRootMsgFolder(nsIMsgFolder * *aRootMsgFolder) = 0;

  /* attribute boolean serverBusy; */
  NS_IMETHOD GetServerBusy(PRBool *aServerBusy) = 0;
  NS_IMETHOD SetServerBusy(PRBool aServerBusy) = 0;

  /* attribute boolean isSecure; */
  NS_IMETHOD GetIsSecure(PRBool *aIsSecure) = 0;
  NS_IMETHOD SetIsSecure(PRBool aIsSecure) = 0;

  /* attribute boolean useSecAuth; */
  NS_IMETHOD GetUseSecAuth(PRBool *aUseSecAuth) = 0;
  NS_IMETHOD SetUseSecAuth(PRBool aUseSecAuth) = 0;

  /* attribute boolean logonFallback; */
  NS_IMETHOD GetLogonFallback(PRBool *aLogonFallback) = 0;
  NS_IMETHOD SetLogonFallback(PRBool aLogonFallback) = 0;

  /* readonly attribute boolean isSecureServer; */
  NS_IMETHOD GetIsSecureServer(PRBool *aIsSecureServer) = 0;

  /* attribute string redirectorType; */
  NS_IMETHOD GetRedirectorType(char * *aRedirectorType) = 0;
  NS_IMETHOD SetRedirectorType(const char * aRedirectorType) = 0;

  /* attribute boolean emptyTrashOnExit; */
  NS_IMETHOD GetEmptyTrashOnExit(PRBool *aEmptyTrashOnExit) = 0;
  NS_IMETHOD SetEmptyTrashOnExit(PRBool aEmptyTrashOnExit) = 0;

  /* nsIMsgFilterList getFilterList (in nsIMsgWindow aMsgWindow); */
  NS_IMETHOD GetFilterList(nsIMsgWindow *aMsgWindow, nsIMsgFilterList **_retval) = 0;

  /* void setFilterList (in nsIMsgFilterList aFilterList); */
  NS_IMETHOD SetFilterList(nsIMsgFilterList *aFilterList) = 0;

  /* void SetDefaultLocalPath (in nsIFileSpec aDefaultLocalPath); */
  NS_IMETHOD SetDefaultLocalPath(nsIFileSpec *aDefaultLocalPath) = 0;

  /* void PerformBiff (in nsIMsgWindow aMsgWindow); */
  NS_IMETHOD PerformBiff(nsIMsgWindow *aMsgWindow) = 0;

  /* void getNewMessages (in nsIMsgFolder aFolder, in nsIMsgWindow aMsgWindow, in nsIUrlListener aUrlListener); */
  NS_IMETHOD GetNewMessages(nsIMsgFolder *aFolder, nsIMsgWindow *aMsgWindow, nsIUrlListener *aUrlListener) = 0;

  /* readonly attribute boolean serverRequiresPasswordForBiff; */
  NS_IMETHOD GetServerRequiresPasswordForBiff(PRBool *aServerRequiresPasswordForBiff) = 0;

  /* void performExpand (in nsIMsgWindow aMsgWindow); */
  NS_IMETHOD PerformExpand(nsIMsgWindow *aMsgWindow) = 0;

  /* void WriteToFolderCache (in nsIMsgFolderCache folderCache); */
  NS_IMETHOD WriteToFolderCache(nsIMsgFolderCache *folderCache) = 0;

  /* void CloseCachedConnections (); */
  NS_IMETHOD CloseCachedConnections(void) = 0;

  /* void shutdown (); */
  NS_IMETHOD Shutdown(void) = 0;

  /* boolean getBoolValue (in string attr); */
  NS_IMETHOD GetBoolValue(const char *attr, PRBool *_retval) = 0;

  /* void setBoolValue (in string attr, in boolean value); */
  NS_IMETHOD SetBoolValue(const char *attr, PRBool value) = 0;

  /* string getCharValue (in string attr); */
  NS_IMETHOD GetCharValue(const char *attr, char **_retval) = 0;

  /* void setCharValue (in string attr, in string value); */
  NS_IMETHOD SetCharValue(const char *attr, const char *value) = 0;

  /* wstring getUnicharValue (in string attr); */
  NS_IMETHOD GetUnicharValue(const char *attr, PRUnichar **_retval) = 0;

  /* void setUnicharValue (in string attr, in wstring value); */
  NS_IMETHOD SetUnicharValue(const char *attr, const PRUnichar *value) = 0;

  /* long getIntValue (in string attr); */
  NS_IMETHOD GetIntValue(const char *attr, PRInt32 *_retval) = 0;

  /* void setIntValue (in string attr, in long value); */
  NS_IMETHOD SetIntValue(const char *attr, PRInt32 value) = 0;

  /* nsIFileSpec getFileValue (in string attr); */
  NS_IMETHOD GetFileValue(const char *attr, nsIFileSpec **_retval) = 0;

  /* void setFileValue (in string attr, in nsIFileSpec value); */
  NS_IMETHOD SetFileValue(const char *attr, nsIFileSpec *value) = 0;

  /**
   * this is really dangerous. this destroys all pref values
   * do not call this unless you know what you're doing!
   */
  /* void clearAllValues (); */
  NS_IMETHOD ClearAllValues(void) = 0;

  /** 
   * this is also very dangerous.  this will remove the files
   * associated with this server on disk.
   */
  /* void removeFiles (); */
  NS_IMETHOD RemoveFiles(void) = 0;

  /* attribute boolean valid; */
  NS_IMETHOD GetValid(PRBool *aValid) = 0;
  NS_IMETHOD SetValid(PRBool aValid) = 0;

  /* wstring toString (); */
  NS_IMETHOD ToString(PRUnichar **_retval) = 0;

  /* void displayOfflineMsg (in nsIMsgWindow aWindow); */
  NS_IMETHOD DisplayOfflineMsg(nsIMsgWindow *aWindow) = 0;

  /* boolean equals (in nsIMsgIncomingServer10 server); */
  NS_IMETHOD Equals(nsIMsgIncomingServer10 *server, PRBool *_retval) = 0;

  /* readonly attribute boolean downloadMessagesAtStartup; */
  NS_IMETHOD GetDownloadMessagesAtStartup(PRBool *aDownloadMessagesAtStartup) = 0;

  /* readonly attribute boolean canHaveFilters; */
  NS_IMETHOD GetCanHaveFilters(PRBool *aCanHaveFilters) = 0;

  /**
   * can this server be removed from the account manager?  for
   * instance, local mail is not removable, but an imported folder is 
   */
  /* attribute boolean canDelete; */
  NS_IMETHOD GetCanDelete(PRBool *aCanDelete) = 0;
  NS_IMETHOD SetCanDelete(PRBool aCanDelete) = 0;

  /* attribute boolean loginAtStartUp; */
  NS_IMETHOD GetLoginAtStartUp(PRBool *aLoginAtStartUp) = 0;
  NS_IMETHOD SetLoginAtStartUp(PRBool aLoginAtStartUp) = 0;

  /* attribute boolean limitOfflineMessageSize; */
  NS_IMETHOD GetLimitOfflineMessageSize(PRBool *aLimitOfflineMessageSize) = 0;
  NS_IMETHOD SetLimitOfflineMessageSize(PRBool aLimitOfflineMessageSize) = 0;

  /* attribute long maxMessageSize; */
  NS_IMETHOD GetMaxMessageSize(PRInt32 *aMaxMessageSize) = 0;
  NS_IMETHOD SetMaxMessageSize(PRInt32 aMaxMessageSize) = 0;

  /* attribute nsIMsgRetentionSettings retentionSettings; */
  NS_IMETHOD GetRetentionSettings(nsIMsgRetentionSettings * *aRetentionSettings) = 0;
  NS_IMETHOD SetRetentionSettings(nsIMsgRetentionSettings * aRetentionSettings) = 0;

  /* readonly attribute boolean canBeDefaultServer; */
  NS_IMETHOD GetCanBeDefaultServer(PRBool *aCanBeDefaultServer) = 0;

  /* readonly attribute boolean canSearchMessages; */
  NS_IMETHOD GetCanSearchMessages(PRBool *aCanSearchMessages) = 0;

  /* readonly attribute boolean canEmptyTrashOnExit; */
  NS_IMETHOD GetCanEmptyTrashOnExit(PRBool *aCanEmptyTrashOnExit) = 0;

  /* attribute boolean displayStartupPage; */
  NS_IMETHOD GetDisplayStartupPage(PRBool *aDisplayStartupPage) = 0;
  NS_IMETHOD SetDisplayStartupPage(PRBool aDisplayStartupPage) = 0;

  /* attribute nsIMsgDownloadSettings downloadSettings; */
  NS_IMETHOD GetDownloadSettings(nsIMsgDownloadSettings * *aDownloadSettings) = 0;
  NS_IMETHOD SetDownloadSettings(nsIMsgDownloadSettings * aDownloadSettings) = 0;

  /* attribute long offlineSupportLevel; */
  NS_IMETHOD GetOfflineSupportLevel(PRInt32 *aOfflineSupportLevel) = 0;
  NS_IMETHOD SetOfflineSupportLevel(PRInt32 aOfflineSupportLevel) = 0;

  /* wstring generatePrettyNameForMigration (); */
  NS_IMETHOD GeneratePrettyNameForMigration(PRUnichar **_retval) = 0;

  /* readonly attribute boolean supportsDiskSpace; */
  NS_IMETHOD GetSupportsDiskSpace(PRBool *aSupportsDiskSpace) = 0;

  /**
   * If the server supports Fcc/Sent/etc, default prefs can point to 
   * the server. Otherwise, copies and folders prefs should point to
   * Local Folders.
   *
   * By default this value is set to true via global pref 'allows_specialfolders_usage'
   * (mailnews.js). For Nntp, the value is overridden to be false.
   * If ISPs want to modify this value, they should do that in their rdf file
   * by using this attribute. Please look at mozilla/mailnews/base/ispdata/aol.rdf for
   * usage example.
   */
  /* attribute boolean defaultCopiesAndFoldersPrefsToServer; */
  NS_IMETHOD GetDefaultCopiesAndFoldersPrefsToServer(PRBool *aDefaultCopiesAndFoldersPrefsToServer) = 0;
  NS_IMETHOD SetDefaultCopiesAndFoldersPrefsToServer(PRBool aDefaultCopiesAndFoldersPrefsToServer) = 0;

  /* attribute boolean canCreateFoldersOnServer; */
  NS_IMETHOD GetCanCreateFoldersOnServer(PRBool *aCanCreateFoldersOnServer) = 0;
  NS_IMETHOD SetCanCreateFoldersOnServer(PRBool aCanCreateFoldersOnServer) = 0;

  /* attribute boolean canFileMessagesOnServer; */
  NS_IMETHOD GetCanFileMessagesOnServer(PRBool *aCanFileMessagesOnServer) = 0;
  NS_IMETHOD SetCanFileMessagesOnServer(PRBool aCanFileMessagesOnServer) = 0;

  /* readonly attribute boolean canCompactFoldersOnServer; */
  NS_IMETHOD GetCanCompactFoldersOnServer(PRBool *aCanCompactFoldersOnServer) = 0;

  /* readonly attribute boolean canUndoDeleteOnServer; */
  NS_IMETHOD GetCanUndoDeleteOnServer(PRBool *aCanUndoDeleteOnServer) = 0;

  /* readonly attribute nsMsgSearchScopeValue filterScope; */
  NS_IMETHOD GetFilterScope(nsMsgSearchScopeValue *aFilterScope) = 0;

  /* readonly attribute nsMsgSearchScopeValue searchScope; */
  NS_IMETHOD GetSearchScope(nsMsgSearchScopeValue *aSearchScope) = 0;

  /** 
   * these generic getter / setters, useful for extending mailnews 
   * note, these attributes persist across sessions
   */
  /* wstring getUnicharAttribute (in string name); */
  NS_IMETHOD GetUnicharAttribute(const char *name, PRUnichar **_retval) = 0;

  /* void setUnicharAttribute (in string name, in wstring value); */
  NS_IMETHOD SetUnicharAttribute(const char *name, const PRUnichar *value) = 0;

  /* string getCharAttribute (in string name); */
  NS_IMETHOD GetCharAttribute(const char *name, char **_retval) = 0;

  /* void setCharAttribute (in string name, in string value); */
  NS_IMETHOD SetCharAttribute(const char *name, const char *value) = 0;

  /* boolean getBoolAttribute (in string name); */
  NS_IMETHOD GetBoolAttribute(const char *name, PRBool *_retval) = 0;

  /* void setBoolAttribute (in string name, in boolean value); */
  NS_IMETHOD SetBoolAttribute(const char *name, PRBool value) = 0;

  /* long getIntAttribute (in string name); */
  NS_IMETHOD GetIntAttribute(const char *name, PRInt32 *_retval) = 0;

  /* void setIntAttribute (in string name, in long value); */
  NS_IMETHOD SetIntAttribute(const char *name, PRInt32 value) = 0;

  /** 
   * If the password for the server is available either via authentication 
   * in the current session or from password manager stored entries, return
   * false. Otherwise, return true. If password is obtained from password 
   * manager, set the password member variable.
   */
  /* readonly attribute boolean passwordPromptRequired; */
  NS_IMETHOD GetPasswordPromptRequired(PRBool *aPasswordPromptRequired) = 0;

  /**
   * If we have set up to filter return receipts into
   * our Sent folder, this utility method creates
   * a filter to do that, and adds it to our filterList
   * if it doesn't exist.  If it does, it will enable it.
   *
   * If the user changes their prefs, to leave the 
   * return receipt in the inbox, this will disable 
   * the existing return receipts filter, if it exists.
   */
  /* void configureTemporaryReturnReceiptsFilter (in nsIMsgFilterList filterList); */
  NS_IMETHOD ConfigureTemporaryReturnReceiptsFilter(nsIMsgFilterList *filterList) = 0;

  /**
   * If Sent folder pref is changed we need to clear the temporary 
   * return receipt filter so that the new return receipt filter can
   * be recreated (by ConfigureTemporaryReturnReceiptsFilter()).
   */
  /* void clearTemporaryReturnReceiptsFilter (); */
  NS_IMETHOD ClearTemporaryReturnReceiptsFilter(void) = 0;

  /**
   * spam settings
   */
  /* attribute nsISpamSettings spamSettings; */
  NS_IMETHOD GetSpamSettings(nsISpamSettings * *aSpamSettings) = 0;
  NS_IMETHOD SetSpamSettings(nsISpamSettings * aSpamSettings) = 0;

  /* readonly attribute nsIMsgFilterPlugin spamFilterPlugin; */
  NS_IMETHOD GetSpamFilterPlugin(nsIMsgFilterPlugin * *aSpamFilterPlugin) = 0;

  /* nsIMsgFolder getMsgFolderFromURI (in nsIMsgFolder aFolderResource, in string aURI); */
  NS_IMETHOD GetMsgFolderFromURI(nsIMsgFolder *aFolderResource, const char *aURI, nsIMsgFolder **_retval) = 0;

  /* readonly attribute boolean isDeferredTo; */
  NS_IMETHOD GetIsDeferredTo(PRBool *aIsDeferredTo) = 0;

};

/*
 * Following values for offline support have been used by
 * various files. If you are modifying any of the values
 * below, please do take care of the following files.
 * - mozilla/mailnews/base/src/nsMsgAccountManagerDS.cpp
 * - mozilla/mailnews/base/util/nsMsgIncomingServer.cpp
 * - mozilla/mailnews/imap/src/nsImapIncomingServer.cpp
 * - mozilla/mailnews/local/src/nsPop3IncomingServer.cpp
 * - mozilla/mailnews/news/src/nsNntpIncomingServer.cpp
 * - mozilla/mailnews/base/resources/content/msgAccountCentral.js
 * - mozilla/modules/libpref/src/init/mailnews.js
 * - ns/modules/libpref/src/init/mailnews-ns.js
 * - ns/mailnews/base/ispdata/aol.rdf
 * - ns/mailnews/base/ispdata/nswebmail.rdf
 */
#define OFFLINE_SUPPORT_LEVEL_NONE 0
#define OFFLINE_SUPPORT_LEVEL_REGULAR 10
#define OFFLINE_SUPPORT_LEVEL_EXTENDED 20
#define OFFLINE_SUPPORT_LEVEL_UNDEFINED -1
// Value when no port setting is found
#define PORT_NOT_SET -1
/* some useful macros to implement nsIMsgIncomingServer10 accessors */
#define NS_IMPL_SERVERPREF_STR(_class, _postfix, _prefname)	\
NS_IMETHODIMP								   	\
_class::Get##_postfix(char **retval)   			\
{											   	\
  return GetCharValue(_prefname, retval);		\
}												\
NS_IMETHODIMP	   								\
_class::Set##_postfix(const char *chvalue)			\
{												\
  return SetCharValue(_prefname, chvalue);		\
}
#define NS_IMPL_SERVERPREF_BOOL(_class, _postfix, _prefname)\
NS_IMETHODIMP								   	\
_class::Get##_postfix(PRBool *retval)   		\
{											   	\
  return GetBoolValue(_prefname, retval);		\
}												\
NS_IMETHODIMP	   								\
_class::Set##_postfix(PRBool bvalue)			\
{												\
  return SetBoolValue(_prefname, bvalue);		\
}
#define NS_IMPL_SERVERPREF_INT(_class, _postfix, _prefname)\
NS_IMETHODIMP								   	\
_class::Get##_postfix(PRInt32 *retval)   		\
{											   	\
  return GetIntValue(_prefname, retval);			\
}												\
NS_IMETHODIMP	   								\
_class::Set##_postfix(PRInt32 ivalue)			\
{												\
  return SetIntValue(_prefname, ivalue);			\
}
#define NS_IMPL_SERVERPREF_FILE(_class, _postfix, _prefname)\
NS_IMETHODIMP								   	\
_class::Get##_postfix(nsIFileSpec **retval)   		\
{											   	\
  return GetFileValue(_prefname, retval);			\
}												\
NS_IMETHODIMP	   								\
_class::Set##_postfix(nsIFileSpec* ivalue)			\
{												\
  return SetFileValue(_prefname, ivalue);			\
}

#endif /* __gen_nsIMsgIncomingServer10_h__ */
