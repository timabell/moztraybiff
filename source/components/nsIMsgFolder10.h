/*
	Taken from a Mozilla Thunderbird 1.0 build.
	Kept for compatibility when building against a 1.5 tree.
*/

#ifndef __gen_nsIMsgFolder10_h__
#define __gen_nsIMsgFolder10_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

#ifndef __gen_nsICollection_h__
#include "nsICollection.h"
#endif

#ifndef __gen_nsIFolderListener_h__
#include "nsIFolderListener.h"
#endif

#ifndef __gen_nsIMsgThread_h__
#include "nsIMsgThread.h"
#endif

#ifndef __gen_nsIMsgIncomingServer_h__
#include "nsIMsgIncomingServer.h"
#endif

#ifndef __gen_nsISupportsArray_h__
#include "nsISupportsArray.h"
#endif

#ifndef __gen_nsIMsgCopyServiceListener_h__
#include "nsIMsgCopyServiceListener.h"
#endif

#ifndef __gen_nsIUrlListener_h__
#include "nsIUrlListener.h"
#endif

#ifndef __gen_nsIEnumerator_h__
#include "nsIEnumerator.h"
#endif

#ifndef __gen_nsISimpleEnumerator_h__
#include "nsISimpleEnumerator.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif
#include "nsIMsgDatabase.h"
#include "nsMsgKeyArray.h"
class nsIMsgDBHdr; /* forward declaration */

class nsIMsgWindow; /* forward declaration */

class nsIMsgDatabase; /* forward declaration */

class nsIDBFolderInfo; /* forward declaration */

class nsMsgKeyArray; /* forward declaration */

class nsIMsgFilterList; /* forward declaration */

class nsIMsgFolderCacheElement; /* forward declaration */

class nsITransport; /* forward declaration */

class nsIFileSpec; /* forward declaration */

class nsIOutputStream; /* forward declaration */

class nsIInputStream; /* forward declaration */

typedef PRInt32 nsMsgBiffState;

typedef PRInt32 nsMsgDispositionState;


/* starting interface:    nsIMsgFolder10 */
#define NS_IMSGFOLDER10_IID_STR "85e39ff0-b248-11d2-b7ef-00805f05ffa5"

#define NS_IMSGFOLDER10_IID \
  {0x85e39ff0, 0xb248, 0x11d2, \
    { 0xb7, 0xef, 0x00, 0x80, 0x5f, 0x05, 0xff, 0xa5 }}

class NS_NO_VTABLE nsIMsgFolder10 : public nsICollection {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IMSGFOLDER10_IID)

  enum { nsMsgBiffState_NewMail = 0 };

  enum { nsMsgBiffState_NoMail = 1 };

  enum { nsMsgBiffState_Unknown = 2 };

  /* nsISimpleEnumerator getMessages (in nsIMsgWindow aMsgWindow); */
  NS_IMETHOD GetMessages(nsIMsgWindow *aMsgWindow, nsISimpleEnumerator **_retval) = 0;

  /* void startFolderLoading (); */
  NS_IMETHOD StartFolderLoading(void) = 0;

  /* void endFolderLoading (); */
  NS_IMETHOD EndFolderLoading(void) = 0;

  /* void updateFolder (in nsIMsgWindow aWindow); */
  NS_IMETHOD UpdateFolder(nsIMsgWindow *aWindow) = 0;

  /* readonly attribute wstring prettiestName; */
  NS_IMETHOD GetPrettiestName(PRUnichar * *aPrettiestName) = 0;

  /**
   * URL for this folder
   */
  /* readonly attribute string folderURL; */
  NS_IMETHOD GetFolderURL(char * *aFolderURL) = 0;

  /**
   * should probably move to the server
   */
  /* readonly attribute boolean showDeletedMessages; */
  NS_IMETHOD GetShowDeletedMessages(PRBool *aShowDeletedMessages) = 0;

  /**
   * this folder's parent server
   */
  /* readonly attribute nsIMsgIncomingServer server; */
  NS_IMETHOD GetServer(nsIMsgIncomingServer10 * *aServer) = 0;

  /**
   * is this folder the "phantom" server folder?
   */
  /* readonly attribute boolean isServer; */
  NS_IMETHOD GetIsServer(PRBool *aIsServer) = 0;

  /* readonly attribute boolean canSubscribe; */
  NS_IMETHOD GetCanSubscribe(PRBool *aCanSubscribe) = 0;

  /* readonly attribute boolean canFileMessages; */
  NS_IMETHOD GetCanFileMessages(PRBool *aCanFileMessages) = 0;

  /* readonly attribute boolean noSelect; */
  NS_IMETHOD GetNoSelect(PRBool *aNoSelect) = 0;

  /* readonly attribute boolean imapShared; */
  NS_IMETHOD GetImapShared(PRBool *aImapShared) = 0;

  /* readonly attribute boolean canDeleteMessages; */
  NS_IMETHOD GetCanDeleteMessages(PRBool *aCanDeleteMessages) = 0;

  /**
   * does this folder allow subfolders?
   * for example, newsgroups cannot have subfolders, and the INBOX
   * on some IMAP servers cannot have subfolders
   */
  /* readonly attribute boolean canCreateSubfolders; */
  NS_IMETHOD GetCanCreateSubfolders(PRBool *aCanCreateSubfolders) = 0;

  /**
   * can you change the name of this folder?
   * for example, newsgroups
   * and some special folders can't be renamed
   */
  /* readonly attribute boolean canRename; */
  NS_IMETHOD GetCanRename(PRBool *aCanRename) = 0;

  /* readonly attribute boolean canCompact; */
  NS_IMETHOD GetCanCompact(PRBool *aCanCompact) = 0;

  /**
   * the phantom server folder
   */
  /* readonly attribute nsIMsgFolder10 rootFolder; */
  NS_IMETHOD GetRootFolder(nsIMsgFolder10 * *aRootFolder) = 0;

  /**
   * function to get the filter list on folder's server 
   * (or in the case of news, the filter list for this newsgroup)'
   */
  /* nsIMsgFilterList getFilterList (in nsIMsgWindow msgWindow); */
  NS_IMETHOD GetFilterList(nsIMsgWindow *msgWindow, nsIMsgFilterList **_retval) = 0;

  /* void setFilterList (in nsIMsgFilterList filterList); */
  NS_IMETHOD SetFilterList(nsIMsgFilterList *filterList) = 0;

  /* void ForceDBClosed (); */
  NS_IMETHOD ForceDBClosed(void) = 0;

  /* void Delete (); */
  NS_IMETHOD Delete(void) = 0;

  /* void deleteSubFolders (in nsISupportsArray folders, in nsIMsgWindow msgWindow); */
  NS_IMETHOD DeleteSubFolders(nsISupportsArray *folders, nsIMsgWindow *msgWindow) = 0;

  /* void propagateDelete (in nsIMsgFolder10 folder, in boolean deleteStorage, in nsIMsgWindow msgWindow); */
  NS_IMETHOD PropagateDelete(nsIMsgFolder10 *folder, PRBool deleteStorage, nsIMsgWindow *msgWindow) = 0;

  /* void recursiveDelete (in boolean deleteStorage, in nsIMsgWindow msgWindow); */
  NS_IMETHOD RecursiveDelete(PRBool deleteStorage, nsIMsgWindow *msgWindow) = 0;

  /* void createSubfolder (in wstring folderName, in nsIMsgWindow msgWindow); */
  NS_IMETHOD CreateSubfolder(const PRUnichar *folderName, nsIMsgWindow *msgWindow) = 0;

  /* nsIMsgFolder10 addSubfolder (in AString folderName); */
  NS_IMETHOD AddSubfolder(const nsAString & folderName, nsIMsgFolder10 **_retval) = 0;

  /* void createStorageIfMissing (in nsIUrlListener urlListener); */
  NS_IMETHOD CreateStorageIfMissing(nsIUrlListener *urlListener) = 0;

  /* void compact (in nsIUrlListener aListener, in nsIMsgWindow aMsgWindow); */
  NS_IMETHOD Compact(nsIUrlListener *aListener, nsIMsgWindow *aMsgWindow) = 0;

  /* void compactAll (in nsIUrlListener aListener, in nsIMsgWindow aMsgWindow, in nsISupportsArray aFolderArray, in boolean aCompactOfflineAlso, in nsISupportsArray aOfflineFolderArray); */
  NS_IMETHOD CompactAll(nsIUrlListener *aListener, nsIMsgWindow *aMsgWindow, nsISupportsArray *aFolderArray, PRBool aCompactOfflineAlso, nsISupportsArray *aOfflineFolderArray) = 0;

  /* void compactAllOfflineStores (in nsIMsgWindow aMsgWindow, in nsISupportsArray aOfflineFolderArray); */
  NS_IMETHOD CompactAllOfflineStores(nsIMsgWindow *aMsgWindow, nsISupportsArray *aOfflineFolderArray) = 0;

  /* void emptyTrash (in nsIMsgWindow aMsgWindow, in nsIUrlListener aListener); */
  NS_IMETHOD EmptyTrash(nsIMsgWindow *aMsgWindow, nsIUrlListener *aListener) = 0;

  /**
   * change the name of the folder
   *
   * @param name the new name of the folder
   */
  /* void rename (in wstring name, in nsIMsgWindow msgWindow); */
  NS_IMETHOD Rename(const PRUnichar *name, nsIMsgWindow *msgWindow) = 0;

  /* void renameSubFolders (in nsIMsgWindow msgWindow, in nsIMsgFolder10 oldFolder); */
  NS_IMETHOD RenameSubFolders(nsIMsgWindow *msgWindow, nsIMsgFolder10 *oldFolder) = 0;

  /**
   * looks in immediate children of this folder for the given name
   *
   * @param name the name of the target subfolder
   */
  /* boolean containsChildNamed (in wstring name); */
  NS_IMETHOD ContainsChildNamed(const PRUnichar *name, PRBool *_retval) = 0;

  /* boolean isAncestorOf (in nsIMsgFolder10 folder); */
  NS_IMETHOD IsAncestorOf(nsIMsgFolder10 *folder, PRBool *_retval) = 0;

  /* wstring generateUniqueSubfolderName (in wstring prefix, in nsIMsgFolder10 otherFolder); */
  NS_IMETHOD GenerateUniqueSubfolderName(const PRUnichar *prefix, nsIMsgFolder10 *otherFolder, PRUnichar **_retval) = 0;

  /* void updateSummaryTotals (in boolean force); */
  NS_IMETHOD UpdateSummaryTotals(PRBool force) = 0;

  /* void summaryChanged (); */
  NS_IMETHOD SummaryChanged(void) = 0;

  /**
   * get the total number of unread messages in this folder,
   * or in all subfolders
   *
   * @param deep if true, descends into all subfolders and gets a grand total
   */
  /* long getNumUnread (in boolean deep); */
  NS_IMETHOD GetNumUnread(PRBool deep, PRInt32 *_retval) = 0;

  /**
   * get the total number of messages in this folder,
   * or in all subfolders
   *
   * @param deep if true, descends into all subfolders and gets a grand total
   */
  /* long getTotalMessages (in boolean deep); */
  NS_IMETHOD GetTotalMessages(PRBool deep, PRInt32 *_retval) = 0;

  /**
  * does this folder have new messages
  *
  */
  /* attribute boolean hasNewMessages; */
  NS_IMETHOD GetHasNewMessages(PRBool *aHasNewMessages) = 0;
  NS_IMETHOD SetHasNewMessages(PRBool aHasNewMessages) = 0;

  /**
   * return the first new message in the folder
   *
   */
  /* readonly attribute nsIMsgDBHdr firstNewMessage; */
  NS_IMETHOD GetFirstNewMessage(nsIMsgDBHdr * *aFirstNewMessage) = 0;

  /**
   * clear new status flag of all of the new messages
   *
   */
  /* void clearNewMessages (); */
  NS_IMETHOD ClearNewMessages(void) = 0;

  /* readonly attribute unsigned long expungedBytes; */
  NS_IMETHOD GetExpungedBytes(PRUint32 *aExpungedBytes) = 0;

  /**
   * can this folder be deleted?
   * for example, special folders cannot be deleted
   */
  /* readonly attribute boolean deletable; */
  NS_IMETHOD GetDeletable(PRBool *aDeletable) = 0;

  /**
   * should we be displaying recipients instead of the sender?
   * for example, in the Sent folder, recipients are more relevant
   * than the sender
   */
  /* readonly attribute boolean displayRecipients; */
  NS_IMETHOD GetDisplayRecipients(PRBool *aDisplayRecipients) = 0;

  /**
   * used to determine if it will take a long time to download all
   * the headers in this folder - so that we can do folder notifications
   * synchronously instead of asynchronously
   */
  /* readonly attribute boolean manyHeadersToDownload; */
  NS_IMETHOD GetManyHeadersToDownload(PRBool *aManyHeadersToDownload) = 0;

  /* readonly attribute boolean requiresCleanup; */
  NS_IMETHOD GetRequiresCleanup(PRBool *aRequiresCleanup) = 0;

  /* void clearRequiresCleanup (); */
  NS_IMETHOD ClearRequiresCleanup(void) = 0;

  /**
   * this should go into a news-specific interface
   */
  /* readonly attribute boolean knowsSearchNntpExtension; */
  NS_IMETHOD GetKnowsSearchNntpExtension(PRBool *aKnowsSearchNntpExtension) = 0;

  /**
   * this should go into a news-specific interface
   */
  /* readonly attribute boolean allowsPosting; */
  NS_IMETHOD GetAllowsPosting(PRBool *aAllowsPosting) = 0;

  /* readonly attribute string relativePathName; */
  NS_IMETHOD GetRelativePathName(char * *aRelativePathName) = 0;

  /**
   * size of this folder on disk (not including .msf file)
   * for imap, it's the sum of the size of the messages
   */
  /* attribute unsigned long sizeOnDisk; */
  NS_IMETHOD GetSizeOnDisk(PRUint32 *aSizeOnDisk) = 0;
  NS_IMETHOD SetSizeOnDisk(PRUint32 aSizeOnDisk) = 0;

  /* readonly attribute string username; */
  NS_IMETHOD GetUsername(char * *aUsername) = 0;

  /* readonly attribute string hostname; */
  NS_IMETHOD GetHostname(char * *aHostname) = 0;

  /* void setFlag (in unsigned long flag); */
  NS_IMETHOD SetFlag(PRUint32 flag) = 0;

  /* void clearFlag (in unsigned long flag); */
  NS_IMETHOD ClearFlag(PRUint32 flag) = 0;

  /* boolean getFlag (in unsigned long flag); */
  NS_IMETHOD GetFlag(PRUint32 flag, PRBool *_retval) = 0;

  /* void setPrefFlag (); */
  NS_IMETHOD SetPrefFlag(void) = 0;

  /* void toggleFlag (in unsigned long flag); */
  NS_IMETHOD ToggleFlag(PRUint32 flag) = 0;

  /* void onFlagChange (in unsigned long flag); */
  NS_IMETHOD OnFlagChange(PRUint32 flag) = 0;

  /* attribute unsigned long flags; */
  NS_IMETHOD GetFlags(PRUint32 *aFlags) = 0;
  NS_IMETHOD SetFlags(PRUint32 aFlags) = 0;

  /* nsIMsgFolder10 getFoldersWithFlag (in unsigned long flags, in unsigned long resultsize, out unsigned long numFolders); */
  NS_IMETHOD GetFoldersWithFlag(PRUint32 flags, PRUint32 resultsize, PRUint32 *numFolders, nsIMsgFolder10 **_retval) = 0;

  /* nsISupportsArray getAllFoldersWithFlag (in unsigned long aFlag); */
  NS_IMETHOD GetAllFoldersWithFlag(PRUint32 aFlag, nsISupportsArray **_retval) = 0;

  /* void getExpansionArray (in nsISupportsArray expansionArray); */
  NS_IMETHOD GetExpansionArray(nsISupportsArray *expansionArray) = 0;

  /* string getUriForMsg (in nsIMsgDBHdr msgHdr); */
  NS_IMETHOD GetUriForMsg(nsIMsgDBHdr *msgHdr, char **_retval) = 0;

  /* void deleteMessages (in nsISupportsArray messages, in nsIMsgWindow msgWindow, in boolean deleteStorage, in boolean isMove, in nsIMsgCopyServiceListener listener, in boolean allowUndo); */
  NS_IMETHOD DeleteMessages(nsISupportsArray *messages, nsIMsgWindow *msgWindow, PRBool deleteStorage, PRBool isMove, nsIMsgCopyServiceListener *listener, PRBool allowUndo) = 0;

  /* void copyMessages (in nsIMsgFolder10 srcFolder, in nsISupportsArray messages, in boolean isMove, in nsIMsgWindow msgWindow, in nsIMsgCopyServiceListener listener, in boolean isFolder, in boolean allowUndo); */
  NS_IMETHOD CopyMessages(nsIMsgFolder10 *srcFolder, nsISupportsArray *messages, PRBool isMove, nsIMsgWindow *msgWindow, nsIMsgCopyServiceListener *listener, PRBool isFolder, PRBool allowUndo) = 0;

  /* void copyFolder (in nsIMsgFolder10 srcFolder, in boolean isMoveFolder, in nsIMsgWindow msgWindow, in nsIMsgCopyServiceListener listener); */
  NS_IMETHOD CopyFolder(nsIMsgFolder10 *srcFolder, PRBool isMoveFolder, nsIMsgWindow *msgWindow, nsIMsgCopyServiceListener *listener) = 0;

  /* void copyFileMessage (in nsIFileSpec fileSpec, in nsIMsgDBHdr msgToReplace, in boolean isDraft, in nsIMsgWindow msgWindow, in nsIMsgCopyServiceListener listener); */
  NS_IMETHOD CopyFileMessage(nsIFileSpec *fileSpec, nsIMsgDBHdr *msgToReplace, PRBool isDraft, nsIMsgWindow *msgWindow, nsIMsgCopyServiceListener *listener) = 0;

  /* void acquireSemaphore (in nsISupports semHolder); */
  NS_IMETHOD AcquireSemaphore(nsISupports *semHolder) = 0;

  /* void releaseSemaphore (in nsISupports semHolder); */
  NS_IMETHOD ReleaseSemaphore(nsISupports *semHolder) = 0;

  /* boolean testSemaphore (in nsISupports semHolder); */
  NS_IMETHOD TestSemaphore(nsISupports *semHolder, PRBool *_retval) = 0;

  /* readonly attribute boolean locked; */
  NS_IMETHOD GetLocked(PRBool *aLocked) = 0;

  /* void getNewMessages (in nsIMsgWindow aWindow, in nsIUrlListener aListener); */
  NS_IMETHOD GetNewMessages(nsIMsgWindow *aWindow, nsIUrlListener *aListener) = 0;

  /**
   * write out summary data for this folder
   * to the given folder cache (i.e. panacea.dat)
   */
  /* void writeToFolderCache (in nsIMsgFolderCache folderCache, in boolean deep); */
  NS_IMETHOD WriteToFolderCache(nsIMsgFolderCache *folderCache, PRBool deep) = 0;

  /**
   * the charset of this folder
   */
  /* attribute string charset; */
  NS_IMETHOD GetCharset(char * *aCharset) = 0;
  NS_IMETHOD SetCharset(const char * aCharset) = 0;

  /* attribute boolean charsetOverride; */
  NS_IMETHOD GetCharsetOverride(PRBool *aCharsetOverride) = 0;
  NS_IMETHOD SetCharsetOverride(PRBool aCharsetOverride) = 0;

  /* attribute unsigned long biffState; */
  NS_IMETHOD GetBiffState(PRUint32 *aBiffState) = 0;
  NS_IMETHOD SetBiffState(PRUint32 aBiffState) = 0;

  /**
   * the number of new messages since this folder was last visited
   * @param deep if true, descends into all subfolders and gets a grand total
   */
  /* long getNumNewMessages (in boolean deep); */
  NS_IMETHOD GetNumNewMessages(PRBool deep, PRInt32 *_retval) = 0;

  /* void setNumNewMessages (in long numNewMessages); */
  NS_IMETHOD SetNumNewMessages(PRInt32 numNewMessages) = 0;

  /**
   * verbose description of the new messages in this folder
   */
  /* readonly attribute wstring newMessagesNotificationDescription; */
  NS_IMETHOD GetNewMessagesNotificationDescription(PRUnichar * *aNewMessagesNotificationDescription) = 0;

  /* attribute boolean gettingNewMessages; */
  NS_IMETHOD GetGettingNewMessages(PRBool *aGettingNewMessages) = 0;
  NS_IMETHOD SetGettingNewMessages(PRBool aGettingNewMessages) = 0;

  /**
   * local path of this folder
   */
  /* attribute nsIFileSpec path; */
  NS_IMETHOD GetPath(nsIFileSpec * *aPath) = 0;
  NS_IMETHOD SetPath(nsIFileSpec * aPath) = 0;

  /* readonly attribute string baseMessageURI; */
  NS_IMETHOD GetBaseMessageURI(char * *aBaseMessageURI) = 0;

  /* string generateMessageURI (in nsMsgKey msgKey); */
  NS_IMETHOD GenerateMessageURI(nsMsgKey msgKey, char **_retval) = 0;

  enum { nsMsgDispositionState_Replied = 0 };

  enum { nsMsgDispositionState_Forwarded = 1 };

  /* void addMessageDispositionState (in nsIMsgDBHdr aMessage, in nsMsgDispositionState aDispositionFlag); */
  NS_IMETHOD AddMessageDispositionState(nsIMsgDBHdr *aMessage, nsMsgDispositionState aDispositionFlag) = 0;

  /* void markMessagesRead (in nsISupportsArray messages, in boolean markRead); */
  NS_IMETHOD MarkMessagesRead(nsISupportsArray *messages, PRBool markRead) = 0;

  /* void markAllMessagesRead (); */
  NS_IMETHOD MarkAllMessagesRead(void) = 0;

  /* void markMessagesFlagged (in nsISupportsArray messages, in boolean markFlagged); */
  NS_IMETHOD MarkMessagesFlagged(nsISupportsArray *messages, PRBool markFlagged) = 0;

  /* void markThreadRead (in nsIMsgThread thread); */
  NS_IMETHOD MarkThreadRead(nsIMsgThread *thread) = 0;

  /* void setLabelForMessages (in nsISupportsArray messages, in nsMsgLabelValue label); */
  NS_IMETHOD SetLabelForMessages(nsISupportsArray *messages, nsMsgLabelValue label) = 0;

  /* nsIMsgDatabase getMsgDatabase (in nsIMsgWindow msgWindow); */
  NS_IMETHOD GetMsgDatabase(nsIMsgWindow *msgWindow, nsIMsgDatabase **_retval) = 0;

  /* void setMsgDatabase (in nsIMsgDatabase msgDatabase); */
  NS_IMETHOD SetMsgDatabase(nsIMsgDatabase *msgDatabase) = 0;

  /* nsIMsgDatabase getDBFolderInfoAndDB (out nsIDBFolderInfo folderInfo); */
  NS_IMETHOD GetDBFolderInfoAndDB(nsIDBFolderInfo **folderInfo, nsIMsgDatabase **_retval) = 0;

  /* nsIMsgDBHdr GetMessageHeader (in nsMsgKey msgKey); */
  NS_IMETHOD GetMessageHeader(nsMsgKey msgKey, nsIMsgDBHdr **_retval) = 0;

  /* readonly attribute boolean supportsOffline; */
  NS_IMETHOD GetSupportsOffline(PRBool *aSupportsOffline) = 0;

  /* boolean shouldStoreMsgOffline (in nsMsgKey msgKey); */
  NS_IMETHOD ShouldStoreMsgOffline(nsMsgKey msgKey, PRBool *_retval) = 0;

  /* boolean hasMsgOffline (in nsMsgKey msgKey); */
  NS_IMETHOD HasMsgOffline(nsMsgKey msgKey, PRBool *_retval) = 0;

  /* nsIInputStream getOfflineFileStream (in nsMsgKey msgKey, out PRUint32 offset, out PRUint32 size); */
  NS_IMETHOD GetOfflineFileStream(nsMsgKey msgKey, PRUint32 *offset, PRUint32 *size, nsIInputStream **_retval) = 0;

  /* readonly attribute nsIOutputStream offlineStoreOutputStream; */
  NS_IMETHOD GetOfflineStoreOutputStream(nsIOutputStream * *aOfflineStoreOutputStream) = 0;

  /* readonly attribute nsIInputStream offlineStoreInputStream; */
  NS_IMETHOD GetOfflineStoreInputStream(nsIInputStream * *aOfflineStoreInputStream) = 0;

  /* void DownloadMessagesForOffline (in nsISupportsArray messages, in nsIMsgWindow window); */
  NS_IMETHOD DownloadMessagesForOffline(nsISupportsArray *messages, nsIMsgWindow *window) = 0;

  /* nsIMsgFolder10 getChildWithURI (in string uri, in boolean deep, in boolean caseInsensitive); */
  NS_IMETHOD GetChildWithURI(const char *uri, PRBool deep, PRBool caseInsensitive, nsIMsgFolder10 **_retval) = 0;

  /* void downloadAllForOffline (in nsIUrlListener listener, in nsIMsgWindow window); */
  NS_IMETHOD DownloadAllForOffline(nsIUrlListener *listener, nsIMsgWindow *window) = 0;

  /**
   *	Turn notifications on/off for various notification types. Currently only supporting
   *  allMessageCountNotifications which refers to both total and unread message counts.
   */
  enum { allMessageCountNotifications = 0U };

  /* void enableNotifications (in long notificationType, in boolean enable, in boolean dbBatching); */
  NS_IMETHOD EnableNotifications(PRInt32 notificationType, PRBool enable, PRBool dbBatching) = 0;

  /* boolean isCommandEnabled (in string command); */
  NS_IMETHOD IsCommandEnabled(const char *command, PRBool *_retval) = 0;

  /* boolean matchOrChangeFilterDestination (in nsIMsgFolder10 folder, in boolean caseInsensitive); */
  NS_IMETHOD MatchOrChangeFilterDestination(nsIMsgFolder10 *folder, PRBool caseInsensitive, PRBool *_retval) = 0;

  /* boolean confirmFolderDeletionForFilter (in nsIMsgWindow msgWindow); */
  NS_IMETHOD ConfirmFolderDeletionForFilter(nsIMsgWindow *msgWindow, PRBool *_retval) = 0;

  /* void alertFilterChanged (in nsIMsgWindow msgWindow); */
  NS_IMETHOD AlertFilterChanged(nsIMsgWindow *msgWindow) = 0;

  /* void throwAlertMsg (in string msgName, in nsIMsgWindow msgWindow); */
  NS_IMETHOD ThrowAlertMsg(const char *msgName, nsIMsgWindow *msgWindow) = 0;

  /* wstring getStringWithFolderNameFromBundle (in string msgName); */
  NS_IMETHOD GetStringWithFolderNameFromBundle(const char *msgName, PRUnichar **_retval) = 0;

  /* void notifyCompactCompleted (); */
  NS_IMETHOD NotifyCompactCompleted(void) = 0;

  /* long compareSortKeys (in nsIMsgFolder10 msgFolder); */
  NS_IMETHOD CompareSortKeys(nsIMsgFolder10 *msgFolder, PRInt32 *_retval) = 0;

  /* [noscript] void getSortKey (out octet_ptr key, out unsigned long length); */
  NS_IMETHOD GetSortKey(PRUint8 * *key, PRUint32 *length) = 0;

  /* attribute nsIMsgRetentionSettings retentionSettings; */
  NS_IMETHOD GetRetentionSettings(nsIMsgRetentionSettings * *aRetentionSettings) = 0;
  NS_IMETHOD SetRetentionSettings(nsIMsgRetentionSettings * aRetentionSettings) = 0;

  /* attribute nsIMsgDownloadSettings downloadSettings; */
  NS_IMETHOD GetDownloadSettings(nsIMsgDownloadSettings * *aDownloadSettings) = 0;
  NS_IMETHOD SetDownloadSettings(nsIMsgDownloadSettings * aDownloadSettings) = 0;

  /* boolean callFilterPlugins (in nsIMsgWindow aMsgWindow); */
  NS_IMETHOD CallFilterPlugins(nsIMsgWindow *aMsgWindow, PRBool *_retval) = 0;

  /**
   * used for order in the folder pane, folder pickers, etc.
   */
  /* attribute long sortOrder; */
  NS_IMETHOD GetSortOrder(PRInt32 *aSortOrder) = 0;
  NS_IMETHOD SetSortOrder(PRInt32 aSortOrder) = 0;

  /** 
   * used to determine if we persist the close / open state of this folder or not
   */
  /* [noscript] readonly attribute boolean persistElided; */
  NS_IMETHOD GetPersistElided(PRBool *aPersistElided) = 0;

  /* readonly attribute nsIMsgFolder10 parentMsgFolder; */
  NS_IMETHOD GetParentMsgFolder(nsIMsgFolder10 * *aParentMsgFolder) = 0;

  /* attribute nsIDBFolderInfo dBTransferInfo; */
  NS_IMETHOD GetDBTransferInfo(nsIDBFolderInfo * *aDBTransferInfo) = 0;
  NS_IMETHOD SetDBTransferInfo(nsIDBFolderInfo * aDBTransferInfo) = 0;

  /* string getStringProperty (in string propertyName); */
  NS_IMETHOD GetStringProperty(const char *propertyName, char **_retval) = 0;

  /* void setStringProperty (in string propertyName, in string propertyValue); */
  NS_IMETHOD SetStringProperty(const char *propertyName, const char *propertyValue) = 0;

  /* attribute nsMsgKey lastMessageLoaded; */
  NS_IMETHOD GetLastMessageLoaded(nsMsgKey *aLastMessageLoaded) = 0;
  NS_IMETHOD SetLastMessageLoaded(nsMsgKey aLastMessageLoaded) = 0;

  /* readonly attribute string URI; */
  NS_IMETHOD GetURI(char * *aURI) = 0;

  /* attribute wstring name; */
  NS_IMETHOD GetName(PRUnichar * *aName) = 0;
  NS_IMETHOD SetName(const PRUnichar * aName) = 0;

  /* attribute wstring prettyName; */
  NS_IMETHOD GetPrettyName(PRUnichar * *aPrettyName) = 0;
  NS_IMETHOD SetPrettyName(const PRUnichar * aPrettyName) = 0;

  /* readonly attribute wstring abbreviatedName; */
  NS_IMETHOD GetAbbreviatedName(PRUnichar * *aAbbreviatedName) = 0;

  /* nsISupports getChildNamed (in wstring name); */
  NS_IMETHOD GetChildNamed(const PRUnichar *name, nsISupports **_retval) = 0;

  /* attribute nsIMsgFolder10 parent; */
  NS_IMETHOD GetParent(nsIMsgFolder10 * *aParent) = 0;
  NS_IMETHOD SetParent(nsIMsgFolder10 * aParent) = 0;

  /* nsIEnumerator GetSubFolders (); */
  NS_IMETHOD GetSubFolders(nsIEnumerator **_retval) = 0;

  /* readonly attribute boolean hasSubFolders; */
  NS_IMETHOD GetHasSubFolders(PRBool *aHasSubFolders) = 0;

  /* void AddFolderListener (in nsIFolderListener listener); */
  NS_IMETHOD AddFolderListener(nsIFolderListener *listener) = 0;

  /* void RemoveFolderListener (in nsIFolderListener listener); */
  NS_IMETHOD RemoveFolderListener(nsIFolderListener *listener) = 0;

  /* nsIMsgFolder10 FindSubFolder (in string escapedSubFolderName); */
  NS_IMETHOD FindSubFolder(const char *escapedSubFolderName, nsIMsgFolder10 **_retval) = 0;

  /* void NotifyPropertyChanged (in nsIAtom property, in string oldValue, in string newValue); */
  NS_IMETHOD NotifyPropertyChanged(nsIAtom *property, const char *oldValue, const char *newValue) = 0;

  /* void NotifyIntPropertyChanged (in nsIAtom property, in long oldValue, in long newValue); */
  NS_IMETHOD NotifyIntPropertyChanged(nsIAtom *property, PRInt32 oldValue, PRInt32 newValue) = 0;

  /* void NotifyBoolPropertyChanged (in nsIAtom property, in boolean oldValue, in boolean newValue); */
  NS_IMETHOD NotifyBoolPropertyChanged(nsIAtom *property, PRBool oldValue, PRBool newValue) = 0;

  /* void NotifyPropertyFlagChanged (in nsISupports item, in nsIAtom property, in unsigned long oldValue, in unsigned long newValue); */
  NS_IMETHOD NotifyPropertyFlagChanged(nsISupports *item, nsIAtom *property, PRUint32 oldValue, PRUint32 newValue) = 0;

  /* void NotifyUnicharPropertyChanged (in nsIAtom property, in wstring oldValue, in wstring newValue); */
  NS_IMETHOD NotifyUnicharPropertyChanged(nsIAtom *property, const PRUnichar *oldValue, const PRUnichar *newValue) = 0;

  /* void NotifyItemAdded (in nsISupports parentItem, in nsISupports item, in string viewString); */
  NS_IMETHOD NotifyItemAdded(nsISupports *parentItem, nsISupports *item, const char *viewString) = 0;

  /* void NotifyItemDeleted (in nsISupports parentItem, in nsISupports item, in string viewString); */
  NS_IMETHOD NotifyItemDeleted(nsISupports *parentItem, nsISupports *item, const char *viewString) = 0;

  /* void NotifyFolderEvent (in nsIAtom event); */
  NS_IMETHOD NotifyFolderEvent(nsIAtom *event) = 0;

  /* void ListDescendents (in nsISupportsArray descendents); */
  NS_IMETHOD ListDescendents(nsISupportsArray *descendents) = 0;

  /* void Shutdown (in boolean shutdownChildren); */
  NS_IMETHOD Shutdown(PRBool shutdownChildren) = 0;

  /* readonly attribute boolean inVFEditSearchScope; */
  NS_IMETHOD GetInVFEditSearchScope(PRBool *aInVFEditSearchScope) = 0;

  /* void setInVFEditSearchScope (in boolean aSearchThisFolder, in boolean aSetOnSubFolders); */
  NS_IMETHOD SetInVFEditSearchScope(PRBool aSearchThisFolder, PRBool aSetOnSubFolders) = 0;

};

#endif /* __gen_nsIMsgFolder10_h__ */
