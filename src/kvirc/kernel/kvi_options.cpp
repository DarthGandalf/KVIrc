//=============================================================================
//
//   File : kvi_options.cpp
//   Creation date : Tue Jun 20 2000 11:42:00 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2000-2008 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your opinion) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================



#define _KVI_OPTIONS_CPP_

#include "kvi_options.h"
#include "kvi_defaults.h"
#include "kvi_config.h"
#include "kvi_app.h"
#include "kvi_iconmanager.h"
#include "kvi_mirccntrl.h"
#include "kvi_locale.h"
#include "kvi_confignames.h"
#include "kvi_window.h"
#include "kvi_out.h"
#include "kvi_stringconversion.h"
#include "kvi_settings.h"
#include "kvi_splash.h"
#include "kvi_frame.h"
#include "kvi_internalcmd.h"
#include "kvi_theme.h"
#include "kvi_fileutils.h"
//#include "kvi_textencoding.h"

#include <QMessageBox>
#include <QRegExp>
#include <QTextCodec>
#include <QDir>
#include <QStringList>

//xml parser
//#include <qdom.h>
//!xml parser

#include <zlib.h>

// kvi_app.cpp
extern KVIRC_API int g_iIdentDaemonRunningUsers;


#define RECT_OPTION(_txt,_val,_flags) KviRectOption(KVI_RECT_OPTIONS_PREFIX _txt, _val , _flags)

KviRectOption g_rectOptionsTable[KVI_NUM_RECT_OPTIONS]=
{
	RECT_OPTION("FrameGeometry",QRect(0,0,10,10),KviOption_sectFlagGeometry),
	RECT_OPTION("GeneralOptionsDialogGeometry",QRect(50,50,600,450),KviOption_sectFlagGeometry),
	RECT_OPTION("RegisteredUsersDialogGeometry",QRect(50,50,600,450),KviOption_sectFlagGeometry)
};

#define BOOL_OPTION(_txt,_val,_flags) KviBoolOption(KVI_BOOL_OPTIONS_PREFIX _txt,_val,_flags)

KviBoolOption g_boolOptionsTable[KVI_NUM_BOOL_OPTIONS]=
{
	BOOL_OPTION("WindowsRememberProperties",true,KviOption_sectFlagWindows),
	BOOL_OPTION("IrcViewShowImages",false,KviOption_sectFlagIrcView | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("IrcViewTimestamp",true,KviOption_sectFlagIrcView),
	BOOL_OPTION("IrcViewUrlHighlighting",true,KviOption_sectFlagIrcView | KviOption_groupTheme),
	BOOL_OPTION("IrcViewWrapMargin",true,KviOption_sectFlagIrcView | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("InputHistoryCursorAtEnd",true,KviOption_sectFlagInput),

	BOOL_OPTION("AvoidParserWarnings",false,KviOption_sectFlagUserParser),
	BOOL_OPTION("UseProxyHost",false,KviOption_sectFlagConnection),
	BOOL_OPTION("ShowGeneralOptionsDialogAsToplevel",true,KviOption_sectFlagFrame),
	BOOL_OPTION("ForceBrutalQuit",false,KviOption_sectFlagIrcSocket),
	BOOL_OPTION("ShowPingPong",true,KviOption_sectFlagConnection),
	BOOL_OPTION("PopupNotifierOnNewQueryMessages",true,KviOption_sectFlagFrame),
	BOOL_OPTION("ShowExtendedServerInfo",false,KviOption_sectFlagConnection),
	BOOL_OPTION("EchoNumericTopic",true,KviOption_sectFlagConnection),
	BOOL_OPTION("ShowOwnParts",true,KviOption_sectFlagConnection),
	BOOL_OPTION("UseAntiSpamOnPrivmsg",false,KviOption_sectFlagAntiSpam),
	BOOL_OPTION("UseExtendedPrivmsgView",false,KviOption_sectFlagIrcView | KviOption_groupTheme),
	BOOL_OPTION("ShowUserAndHostInPrivmsgView",false,KviOption_sectFlagIrcView | KviOption_groupTheme),
	BOOL_OPTION("UseWordHighlighting",true,KviOption_sectFlagIrcView), /* _ALL_ newbie users, with who i was taling asks me where can they switch on */
	BOOL_OPTION("UseBigIcons",false,KviOption_sectFlagFrame), /* Almost all users asks me "How to decrease icon size?" */
	BOOL_OPTION("CleanupUnusedModules",true,KviOption_sectFlagModules),
	BOOL_OPTION("IgnoreCtcpPing",false,KviOption_sectFlagCtcp),
	BOOL_OPTION("IgnoreCtcpVersion",false,KviOption_sectFlagCtcp),
	BOOL_OPTION("UseCtcpFloodProtection",true,KviOption_sectFlagCtcp),
	BOOL_OPTION("DisableBrokenEventHandlers",true,KviOption_sectFlagUserParser),
	BOOL_OPTION("IgnoreCtcpClientinfo",false,KviOption_sectFlagCtcp),
	BOOL_OPTION("IgnoreCtcpUserinfo",false,KviOption_sectFlagCtcp),
	BOOL_OPTION("IgnoreCtcpFinger",false,KviOption_sectFlagCtcp),
	BOOL_OPTION("IgnoreCtcpSource",false,KviOption_sectFlagCtcp),
	BOOL_OPTION("IgnoreCtcpTime",false,KviOption_sectFlagCtcp),
	BOOL_OPTION("RequestMissingAvatars",true,KviOption_sectFlagAvatar),
	BOOL_OPTION("ShowCompactModeChanges",true,KviOption_sectFlagConnection),
	BOOL_OPTION("IgnoreCtcpDcc",false,KviOption_sectFlagDcc),
	BOOL_OPTION("AutoAcceptDccChat",false,KviOption_sectFlagDcc),
	BOOL_OPTION("AutoAcceptDccSend",false,KviOption_sectFlagDcc),
	BOOL_OPTION("SendZeroAckInDccRecv",false,KviOption_sectFlagDcc),
	BOOL_OPTION("UseFastDccSend",true,KviOption_sectFlagDcc),
	BOOL_OPTION("UseCompositingForTransparency",false,KviOption_sectFlagGui | KviOption_resetUpdatePseudoTransparency | KviOption_groupTheme),
	BOOL_OPTION("UseWindowListIrcContextIndicator",false,KviOption_sectFlagWindowList | KviOption_resetUpdateGui),
	BOOL_OPTION("UseGlobalApplicationFont",false,KviOption_sectFlagGui | KviOption_resetUpdateAppFont | KviOption_groupTheme),
	BOOL_OPTION("AlwaysQuitKVIrcClosingLastConsole",false,KviOption_sectFlagFrame),
	BOOL_OPTION("AlwaysDisconnectClosingConnectedConsole",false,KviOption_sectFlagFrame),
	BOOL_OPTION("FlashWindowOnHighlightedMessages",true,KviOption_sectFlagFrame),
	BOOL_OPTION("PopupNotifierOnHighlightedMessages",true,KviOption_sectFlagFrame),
	BOOL_OPTION("FlashWindowOnNotifyOnLine",true,KviOption_sectFlagConnection),
	BOOL_OPTION("IrcViewTimestampUTC",false,KviOption_sectFlagIrcView),
	BOOL_OPTION("UseGlobalPseudoTransparency",false,KviOption_sectFlagGui | KviOption_resetUpdatePseudoTransparency | KviOption_groupTheme),
	BOOL_OPTION("UseNotifyList",false,KviOption_sectFlagConnection | KviOption_resetRestartNotifyList),
	BOOL_OPTION("NotifyListChangesToActiveWindow",true,KviOption_sectFlagConnection),
	BOOL_OPTION("AutoResumeDccSendWhenAutoAccepted",false,KviOption_sectFlagDcc),
	BOOL_OPTION("LimitOutgoingTraffic",false,KviOption_sectFlagIrcSocket),
	BOOL_OPTION("SendUnknownCommandsAsRaw",true,KviOption_sectFlagUserParser),
	BOOL_OPTION("UserListViewDrawGrid",true,KviOption_sectFlagUserListView | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("ShowChannelSyncTime",true,KviOption_sectFlagConnection),
	BOOL_OPTION("RejoinChannelOnKick",false,KviOption_sectFlagConnection),
	BOOL_OPTION("KeepChannelOpenOnKick",true,KviOption_sectFlagConnection),
	BOOL_OPTION("AutoCloseDccSendOnSuccess",false,KviOption_sectFlagDcc),
	BOOL_OPTION("NotifyDccSendSuccessInConsole",false,KviOption_sectFlagDcc),
	BOOL_OPTION("CreateMinimizedDccSend",false,KviOption_sectFlagDcc),
	BOOL_OPTION("CreateMinimizedDccChat",false,KviOption_sectFlagDcc),
	BOOL_OPTION("AutoAcceptIncomingAvatars",false,KviOption_sectFlagDcc),
	BOOL_OPTION("UseNickCompletionPostfixForFirstWordOnly",true,KviOption_sectFlagInput),
	BOOL_OPTION("UseWindowListIcons",true,KviOption_sectFlagWindowList | KviOption_resetUpdateGui),
	BOOL_OPTION("CreateMinimizedDccSendWhenAutoAccepted",true,KviOption_sectFlagDcc),
	BOOL_OPTION("CreateMinimizedDccChatWhenAutoAccepted",true,KviOption_sectFlagDcc),
	BOOL_OPTION("DccGuessIpFromServerWhenLocalIsUnroutable",true,KviOption_sectFlagDcc),
	BOOL_OPTION("ShowRegisteredUsersDialogAsToplevel",true,KviOption_sectFlagFrame),
	BOOL_OPTION("AutoLogQueries",true,KviOption_sectFlagLogging),/* this options enabled by default in mIRC,XChat and irssi. People are confused while they want to see logs, but see empty dir*/
	BOOL_OPTION("AutoLogChannels",true,KviOption_sectFlagLogging),
	BOOL_OPTION("AutoLogDccChat",false,KviOption_sectFlagLogging),
	BOOL_OPTION("AutoLogConsole",false,KviOption_sectFlagLogging),

	BOOL_OPTION("SilentAntiSpam",false,KviOption_sectFlagAntiSpam),
	BOOL_OPTION("UseIncomingDccMediaTypeSavePath",true,KviOption_sectFlagDcc),
	BOOL_OPTION("NotifyListSendUserhostForOnlineUsers",true,KviOption_sectFlagConnection),
	BOOL_OPTION("UseIntelligentNotifyListManager",true,KviOption_sectFlagConnection | KviOption_resetRestartNotifyList),
	BOOL_OPTION("UseWatchListIfAvailable",true,KviOption_sectFlagConnection),
	BOOL_OPTION("UseTreeWindowList",true,KviOption_sectFlagWindowList | KviOption_resetUpdateWindowList | KviOption_groupTheme),
	BOOL_OPTION("BindIrcIPv4ConnectionsToSpecifiedAddress",false,KviOption_sectFlagConnection),
	BOOL_OPTION("BindIrcIPv6ConnectionsToSpecifiedAddress",false,KviOption_sectFlagConnection),
	BOOL_OPTION("UseAntiSpamOnNotice",false,KviOption_sectFlagAntiSpam),
	BOOL_OPTION("SetLastAvatarAsDefaultForRegisteredUsers",true,KviOption_sectFlagAvatar),
	BOOL_OPTION("CantAcceptIncomingDccConnections",false,KviOption_sectFlagDcc),
	BOOL_OPTION("IgnoreCtcpAvatar",false,KviOption_sectFlagCtcp),
	BOOL_OPTION("CtcpRepliesToActiveWindow",true,KviOption_sectFlagCtcp),
	BOOL_OPTION("AutoAcceptDccCanvas",false,KviOption_sectFlagDcc),
	BOOL_OPTION("NotifyDccSendSuccessInNotifier",true,KviOption_sectFlagDcc),
	BOOL_OPTION("KillBrokenTimers",true,KviOption_sectFlagUserParser),
	BOOL_OPTION("DccVoiceVolumeSliderControlsPCM", true, KviOption_sectFlagDcc),
	BOOL_OPTION("DccVoiceForceHalfDuplex", false, KviOption_sectFlagDcc),
	BOOL_OPTION("AutoAcceptDccVoice", false, KviOption_sectFlagDcc),
	BOOL_OPTION("CreateMinimizedDccVoice", false, KviOption_sectFlagDcc),
	BOOL_OPTION("CreateMinimizedDccVoiceWhenAutoAccepted",true,KviOption_sectFlagDcc),
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	BOOL_OPTION("UseIdentService",true,KviOption_sectFlagConnection | KviOption_resetRestartIdentd),
#else
	BOOL_OPTION("UseIdentService",false,KviOption_sectFlagConnection | KviOption_resetRestartIdentd),
#endif
	BOOL_OPTION("ShowTipAtStartup",false,KviOption_sectFlagFrame),
	BOOL_OPTION("IdentdEnableIPv6",false,KviOption_sectFlagConnection | KviOption_resetRestartIdentd),
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	BOOL_OPTION("IdentdIPv6ContainsIPv4",false,KviOption_sectFlagConnection | KviOption_resetRestartIdentd),
#else
	BOOL_OPTION("IdentdIPv6ContainsIPv4",true,KviOption_sectFlagConnection | KviOption_resetRestartIdentd),
#endif
	BOOL_OPTION("ScaleAvatars",true,KviOption_sectFlagAvatar | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("ColorNicks",false,KviOption_sectFlagIrcView | KviOption_groupTheme),
	BOOL_OPTION("BoldedNicks",true,KviOption_sectFlagIrcView | KviOption_groupTheme),
	BOOL_OPTION("DccListenOnSpecifiedInterfaceByDefault",false,KviOption_sectFlagDcc),
	BOOL_OPTION("ChangeNickAway", false, KviOption_sectFlagConnection),
	BOOL_OPTION("AutoGeneratedAwayNick",false,KviOption_sectFlagConnection), /* only ircnet uses 9-symbol nicks */
	BOOL_OPTION("ExitAwayOnInput",false,KviOption_sectFlagConnection),
	BOOL_OPTION("AlwaysHighlightNick",true,KviOption_sectFlagIrcView),
	BOOL_OPTION("ShowChannelsJoinOnIrc",true,KviOption_sectFlagFrame),
	BOOL_OPTION("ShowChannelsJoinDialogAsToplevel",true,KviOption_sectFlagFrame),
	BOOL_OPTION("UserDefinedPortRange",false,KviOption_sectFlagDcc),
	BOOL_OPTION("CreateQueryOnPrivmsg",true,KviOption_sectFlagConnection),
	BOOL_OPTION("CreateQueryOnNotice",false,KviOption_sectFlagConnection),
	BOOL_OPTION("CreateMinimizedQuery",false,KviOption_sectFlagConnection),
	BOOL_OPTION("AutoJoinOnInvite",false,KviOption_sectFlagConnection),
	BOOL_OPTION("ShowServersConnectDialogOnStart",true, KviOption_sectFlagFrame),
	BOOL_OPTION("ShowServersConnectDialogAsToplevel",true,KviOption_sectFlagFrame),
	BOOL_OPTION("AcceptBrokenFileNameDccResumeRequests",true,KviOption_sectFlagFrame),
	BOOL_OPTION("AutoReconnectOnUnexpectedDisconnect",true,KviOption_sectFlagFrame),
	BOOL_OPTION("RejoinChannelsAfterReconnect",true,KviOption_sectFlagFrame),
	BOOL_OPTION("ReopenQueriesAfterReconnect",true,KviOption_sectFlagFrame),
	BOOL_OPTION("EnableQueryTracing",true,KviOption_sectFlagConnection),
	BOOL_OPTION("OperatorMessagesToActiveWindow",false,KviOption_sectFlagConnection),
	BOOL_OPTION("InviteToActiveWindow",true,KviOption_sectFlagConnection),
	BOOL_OPTION("ShowChannelUserFlagInPrivmsgView",true,KviOption_sectFlagConnection),
	BOOL_OPTION("DccSendFakeAddressByDefault",false,KviOption_sectFlagDcc),
	BOOL_OPTION("UseWindowListActivityMeter",false,KviOption_sectFlagWindowList | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("CloseServerWidgetAfterConnect",false,KviOption_sectFlagFrame),
	BOOL_OPTION("ShowIdentityDialogAsToplevel",true,KviOption_sectFlagFrame),
	BOOL_OPTION("ShowUserChannelIcons",true,KviOption_sectFlagUserListView | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("ShowUserChannelState",false,KviOption_sectFlagUserListView | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("EnableIgnoreOnPrivMsg",true,KviOption_sectFlagConnection),
	BOOL_OPTION("VerboseIgnore",true,KviOption_sectFlagConnection),
	BOOL_OPTION("EnableIgnoreOnNotice",true,KviOption_sectFlagConnection),
	BOOL_OPTION("NotifyFailedDccHandshakes",true,KviOption_sectFlagDcc),
	BOOL_OPTION("AutoTileWindows",false,KviOption_sectFlagMdi),
	BOOL_OPTION("DrawEmoticons",false,KviOption_sectFlagIrcView | KviOption_groupTheme),
	BOOL_OPTION("IgnoreCtcpPage",false,KviOption_sectFlagCtcp),
	BOOL_OPTION("ShowDialogOnCtcpPage",true,KviOption_sectFlagCtcp),
	BOOL_OPTION("BashLikeNickCompletion",false,KviOption_sectFlagInput),
	BOOL_OPTION("UseSSLCertificate",false,KviOption_sectFlagConnection),
	BOOL_OPTION("UseSSLPrivateKey",false,KviOption_sectFlagConnection),
	BOOL_OPTION("WarnAboutPastingMultipleLines",true,KviOption_sectFlagFrame),
	BOOL_OPTION("KeepChannelsOpenOnUnexpectedDisconnect",true,KviOption_sectFlagFrame),
	BOOL_OPTION("KeepQueriesOpenOnUnexpectedDisconnect",true,KviOption_sectFlagFrame),
	BOOL_OPTION("UseLagMeterEngine",true,KviOption_sectFlagConnection | KviOption_resetRestartLagMeter),
 	BOOL_OPTION("ShowLagOnContextDisplay",true,KviOption_sectFlagFrame),
	BOOL_OPTION("DisableBanListRequestOnJoin",false,KviOption_sectFlagConnection),
	BOOL_OPTION("DisableBanExceptionListRequestOnJoin",true,KviOption_sectFlagConnection),
	BOOL_OPTION("DisableInviteListRequestOnJoin",true,KviOption_sectFlagConnection),
	BOOL_OPTION("DisableWhoRequestOnJoin",false,KviOption_sectFlagConnection),
	BOOL_OPTION("ShowDockExtension",true,KviOption_sectFlagFrame),
	BOOL_OPTION("DccAddressByIrcDns",false,KviOption_sectFlagFrame),
	BOOL_OPTION("MinimizeConsoleAfterConnect",false,KviOption_sectFlagFrame),
	BOOL_OPTION("HighlightOnlyNormalMsg",false,KviOption_sectFlagFrame),
	BOOL_OPTION("HighlightOnlyNormalMsgQueryToo",false,KviOption_sectFlagFrame),
	BOOL_OPTION("HighlightOnlyNormalMsgNickinChanToo",false,KviOption_sectFlagFrame),
	BOOL_OPTION("HighlightOnlyAtCustomHighlightLevel",false,KviOption_sectFlagFrame),
	BOOL_OPTION("MdiManagerInSdiMode",true,KviOption_sectFlagFrame),
	BOOL_OPTION("DccBrokenBouncerHack",false,KviOption_sectFlagDcc),
	BOOL_OPTION("ExternalMessagesToActiveWindow",true,KviOption_sectFlagConnection),
	BOOL_OPTION("IgnoreChannelAvatarRequestsWhenNoAvatarSet",true,KviOption_sectFlagAvatar),
	BOOL_OPTION("FlashQueryWindowOnNewMessages",true,KviOption_sectFlagFrame),
	BOOL_OPTION("ConfirmCloseWhenThereAreConnections",true,KviOption_sectFlagFrame),
	BOOL_OPTION("UseWindowsDesktopForTransparency",false,KviOption_sectFlagGui | KviOption_resetUpdatePseudoTransparency | KviOption_groupTheme),
	BOOL_OPTION("CtcpRequestsToActiveWindow",false,KviOption_sectFlagConnection),
	BOOL_OPTION("ServerRepliesToActiveWindow",true,KviOption_sectFlagConnection),
	BOOL_OPTION("ServerNoticesToActiveWindow",false,KviOption_sectFlagConnection),
	BOOL_OPTION("ServicesNoticesToActiveWindow",false,KviOption_sectFlagConnection),
	BOOL_OPTION("SortWindowListItemsByName",true,KviOption_sectFlagConnection),
	BOOL_OPTION("ShowWindowListToolTips",true,KviOption_sectFlagConnection),
	BOOL_OPTION("ShowUserListViewToolTips",true,KviOption_sectFlagConnection),
	BOOL_OPTION("WhoisRepliesToActiveWindow",true,KviOption_sectFlagConnection),
	BOOL_OPTION("PopupNotifierOnNotifyOnLine",true,KviOption_sectFlagConnection),
	BOOL_OPTION("EnableNotifier",true,KviOption_sectFlagConnection),
	BOOL_OPTION("DccSendForceIdleStep",false,KviOption_sectFlagDcc),
	BOOL_OPTION("StatusBarVisible",true,KviOption_sectFlagFrame),
	BOOL_OPTION("TrackLastReadTextViewLine",true,KviOption_sectFlagFrame),
	BOOL_OPTION("KeepChannelOpenOnPart",false,KviOption_sectFlagFrame),
	BOOL_OPTION("StripMircColorsInUserMessages",false,KviOption_sectFlagFrame | KviOption_groupTheme),
	BOOL_OPTION("DoNotSuggestRestoreDefaultScript",false,KviOption_sectFlagFrame),
	BOOL_OPTION("LimitDccSendSpeed",false,KviOption_sectFlagDcc),
	BOOL_OPTION("LimitDccRecvSpeed",false,KviOption_sectFlagDcc),
	BOOL_OPTION("IgnoreModuleVersions",false,KviOption_sectFlagFrame),
	BOOL_OPTION("EnableInputHistory",true,KviOption_sectFlagInput | KviOption_resetUpdateGui),
	BOOL_OPTION("UseSpecialColorForTimestamp",true,KviOption_sectFlagIrcView | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("EnableAwayListUpdates",true,KviOption_sectFlagFrame),
	BOOL_OPTION("ShowAvatarsInUserlist",true,KviOption_sectFlagIrcView | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("ShowUserListStatisticLabel",true,KviOption_sectFlagIrcView | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("ShowIconsInPopupMenus",true,KviOption_sectFlagIrcView | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("ScriptErrorsToDebugWindow",false,KviOption_sectFlagFrame),
	BOOL_OPTION("ShowMinimizedDebugWindow",true,KviOption_sectFlagFrame),
	BOOL_OPTION("ShowExtendedInfoInQueryLabel",true,KviOption_resetUpdateGui),
	BOOL_OPTION("UseUserListColorsAsNickColors",true,KviOption_sectFlagIrcView | KviOption_groupTheme),
	BOOL_OPTION("GzipLogs",false,KviOption_sectFlagLogging),
	BOOL_OPTION("UseFullWordHighlighting",false,KviOption_sectFlagIrcView),
	BOOL_OPTION("NotifierFlashing",true,KviOption_sectFlagFrame),
	BOOL_OPTION("CommandlineInUserFriendlyModeByDefault",false,KviOption_sectFlagFrame), // this confuses existing users and is not mentioned in the docs where there are commandline examples : we need it to be off by default for now
	BOOL_OPTION("EnableVisualEffects",true,KviOption_resetUpdateGui),
	BOOL_OPTION("DCCFileTransferReplaceOutgoingSpacesWithUnderscores",true,KviOption_sectFlagDcc),
	BOOL_OPTION("DoNotStretchAvatars",true,KviOption_sectFlagIrcView | KviOption_resetUpdateGui ),
	BOOL_OPTION("UseDifferentColorForOwnNick",false,KviOption_sectFlagIrcView | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("CloseInTray",false,KviOption_sectFlagFrame | KviOption_resetUpdateGui),
	BOOL_OPTION("StartupMinimized",false,KviOption_sectFlagFrame),
	BOOL_OPTION("SortReceivedByDccFilesByNicks",false,KviOption_sectFlagDcc),
	BOOL_OPTION("LogChannelHistory",true,KviOption_sectFlagConnection),
	BOOL_OPTION("ClassicWindowListSetMaximumButtonWidth",true,KviOption_groupTheme | KviOption_sectFlagGeometry | KviOption_resetUpdateGui),
	BOOL_OPTION("KeepChannelsOpenOnDisconnect",true,KviOption_sectFlagFrame),
	BOOL_OPTION("KeepQueriesOpenOnDisconnect",true,KviOption_sectFlagFrame),
	BOOL_OPTION("HideWindowToolButtons",true,KviOption_sectFlagFrame),
	BOOL_OPTION("HideInputToolButtons",true,KviOption_sectFlagFrame),
	BOOL_OPTION("StripControlCodesInLogs",false,KviOption_sectFlagLogging),
	BOOL_OPTION("EnableTrayIconFlashing",true,KviOption_sectFlagFrame),
	BOOL_OPTION("UseFlatClassicWindowListButtons",false,KviOption_sectFlagWindowList | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("StripMsgTypeInLogs",false,KviOption_sectFlagLogging),
	BOOL_OPTION("UseLevelBasedTrayNotification",false,KviOption_sectFlagFrame),
	BOOL_OPTION("MuteAllSounds",false,KviOption_sectFlagFrame),
	BOOL_OPTION("CreateMinimizedChannels",false,KviOption_sectFlagFrame),
	BOOL_OPTION("ShowNetworkNameForConsoleWindowListEntry",true,KviOption_sectFlagFrame | KviOption_resetUpdateGui),
	BOOL_OPTION("DrawGenderIcons",true,KviOption_sectFlagFrame | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("PrependGenderInfoToRealname",true,KviOption_sectFlagConnection),
	BOOL_OPTION("UseIdentServiceOnlyOnConnect",true,KviOption_sectFlagConnection | KviOption_resetRestartIdentd),
	BOOL_OPTION("UseSystemUrlHandlers",true,KviOption_sectFlagUrl),
	BOOL_OPTION("ScaleAvatarsOnLoad",true,KviOption_sectFlagAvatar),
	BOOL_OPTION("NotifierFading",true,KviOption_sectFlagFrame),
	BOOL_OPTION("PickRandomIpAddressForRoundRobinServers",true,KviOption_sectFlagConnection),
	BOOL_OPTION("PrependAvatarInfoToRealname",true,KviOption_sectFlagConnection),
	BOOL_OPTION("EnableAnimatedAvatars",true,KviOption_sectFlagGui | KviOption_resetUpdateGui),
	BOOL_OPTION("EnableAnimatedSmiles",true,KviOption_sectFlagGui | KviOption_resetUpdateGui),
	BOOL_OPTION("PlaceNickWithNonAlphaCharsAtEnd",true,KviOption_sectFlagGui | KviOption_resetUpdateGui),
	BOOL_OPTION("UseStartTlsIfAvailable",true,KviOption_sectFlagConnection),
	BOOL_OPTION("PasteLastLogOnChannelJoin",false,KviOption_sectFlagLogging),
	BOOL_OPTION("PasteLastLogOnQueryJoin",false,KviOption_sectFlagLogging),
	BOOL_OPTION("UseSpecifiedSmartColorForOwnNick",false,KviOption_sectFlagIrcView | KviOption_resetUpdateGui | KviOption_groupTheme),
	BOOL_OPTION("ZshLikeNickCompletion",false,KviOption_sectFlagInput),
	BOOL_OPTION("AutoAcceptDccVideo", false, KviOption_sectFlagDcc),
	BOOL_OPTION("CreateMinimizedDccVideo", false, KviOption_sectFlagDcc),
	BOOL_OPTION("CreateMinimizedDccVideoWhenAutoAccepted",true,KviOption_sectFlagDcc)
};

#define STRING_OPTION(_txt,_val,_flags) KviStringOption(KVI_STRING_OPTIONS_PREFIX _txt,_val,_flags)

// FIXME: #warning "Remove the unused string options here!"

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	#define RUN_THE_BROWSER "run \"cmd.exe /c start \\\"\\\" \\\"$0\\\"\""
#else
	#ifdef COMPILE_KDE_SUPPORT
		#define RUN_THE_BROWSER "run kfmclient openURL $0"
	#else
		#ifdef COMPILE_ON_MAC
			#define RUN_THE_BROWSER "run open $0"
		#else
			#define RUN_THE_BROWSER "run x-www-browser $0"
		#endif
	#endif
#endif

KviStringOption g_stringOptionsTable[KVI_NUM_STRING_OPTIONS]=
{
	STRING_OPTION("UNUSED:FrameCaption","",KviOption_sectFlagFrame),
	STRING_OPTION("Nickname1",KVI_DEFAULT_NICKNAME1,KviOption_sectFlagUser),
	STRING_OPTION("Nickname2",KVI_DEFAULT_NICKNAME2,KviOption_sectFlagUser),
	STRING_OPTION("Nickname3",KVI_DEFAULT_NICKNAME3,KviOption_sectFlagUser),
	STRING_OPTION("Nickname4",KVI_DEFAULT_NICKNAME4,KviOption_sectFlagUser),
	STRING_OPTION("Username",KVI_DEFAULT_USERNAME,KviOption_sectFlagUser),
	STRING_OPTION("Realname",KVI_DEFAULT_REALNAME,KviOption_sectFlagUser),
	STRING_OPTION("LocalHostIp","127.0.0.1",KviOption_sectFlagIrcSocket),
	STRING_OPTION("PartMessage",KVI_DEFAULT_PART_MESSAGE,KviOption_sectFlagConnection),
	STRING_OPTION("QuitMessage",KVI_DEFAULT_QUIT_MESSAGE,KviOption_sectFlagConnection),
	STRING_OPTION("UrlHttpCommand",RUN_THE_BROWSER,KviOption_sectFlagUrl),
	STRING_OPTION("ExtendedPrivmsgPrefix","[",KviOption_sectFlagIrcView | KviOption_groupTheme),
	STRING_OPTION("ExtendedPrivmsgPostfix","] ",KviOption_sectFlagIrcView | KviOption_groupTheme),
	STRING_OPTION("CtcpVersionPostfix","",KviOption_sectFlagCtcp),
	STRING_OPTION("CtcpUserInfoOther",KVI_DEFAULT_CTCP_USERINFO_REPLY,KviOption_sectFlagUser),
	STRING_OPTION("CtcpSourcePostfix","",KviOption_sectFlagCtcp),
	STRING_OPTION("UrlFtpCommand",RUN_THE_BROWSER,KviOption_sectFlagUrl),
	STRING_OPTION("UrlHttpsCommand",RUN_THE_BROWSER,KviOption_sectFlagUrl),
	STRING_OPTION("UrlMailtoCommand",RUN_THE_BROWSER,KviOption_sectFlagUrl),
	STRING_OPTION("PreferredMediaPlayer","auto",KviOption_sectFlagFrame),
	STRING_OPTION("UrlFileCommand","play $0",KviOption_sectFlagUrl),
	STRING_OPTION("NickCompletionPostfix",": ",KviOption_sectFlagInput),
	STRING_OPTION("UrlUnknownCommand",RUN_THE_BROWSER,KviOption_sectFlagUrl),
	STRING_OPTION("IPv4ConnectionBindAddress","",KviOption_sectFlagConnection),
	STRING_OPTION("IPv6ConnectionBindAddress","",KviOption_sectFlagConnection),
	STRING_OPTION("DccVoiceSoundDevice", "/dev/dsp",KviOption_sectFlagDcc),
	STRING_OPTION("DccVoiceMixerDevice", "/dev/mixer",KviOption_sectFlagDcc),
	STRING_OPTION("AwayMessage",KVI_DEFAULT_AWAY_MESSAGE,KviOption_sectFlagConnection),
	STRING_OPTION("IdentdUser","kvirc",KviOption_sectFlagConnection | KviOption_resetRestartIdentd),
	STRING_OPTION("DccListenDefaultInterface","ppp0",KviOption_sectFlagDcc),
	STRING_OPTION("CustomAwayNick", KVI_DEFAULT_AWAY_NICK,KviOption_sectFlagConnection),
	STRING_OPTION("DefaultDccFakeAddress","",KviOption_sectFlagDcc),
	STRING_OPTION("CtcpPageReply",KVI_DEFAULT_CTCP_PAGE_REPLY,KviOption_sectFlagCtcp),
	STRING_OPTION("SSLCertificatePath","",KviOption_sectFlagConnection | KviOption_encodePath),
	STRING_OPTION("SSLCertificatePass","",KviOption_sectFlagConnection),
	STRING_OPTION("SSLPrivateKeyPath","",KviOption_sectFlagConnection | KviOption_encodePath),
	STRING_OPTION("SSLPrivateKeyPass","",KviOption_sectFlagConnection),
	STRING_OPTION("SoundSystem","unknown",KviOption_sectFlagFrame),
	STRING_OPTION("DefaultUserMode", "", KviOption_sectFlagUser),
	STRING_OPTION("LastImageDialogPath","",KviOption_sectFlagUser | KviOption_encodePath),
	STRING_OPTION("IconThemeSubdir","",KviOption_sectFlagUser | KviOption_groupTheme | KviOption_resetReloadImages | KviOption_encodePath),
	STRING_OPTION("IncomingPath","",KviOption_sectFlagUser | KviOption_encodePath),
	STRING_OPTION("MyAvatar","",KviOption_sectFlagUser),
	STRING_OPTION("DefaultTextEncoding","",KviOption_sectFlagFrame),
	STRING_OPTION("WinampTextEncoding","",KviOption_sectFlagFrame),
	STRING_OPTION("Mp3TagsEncoding","",KviOption_sectFlagFrame),
	STRING_OPTION("CtcpUserInfoAge","",KviOption_sectFlagUser),
	STRING_OPTION("CtcpUserInfoGender","",KviOption_sectFlagUser),
	STRING_OPTION("CtcpUserInfoLocation","",KviOption_sectFlagUser),
	STRING_OPTION("CtcpUserInfoLanguages","",KviOption_sectFlagUser),
	STRING_OPTION("WordSplitters",",\"';:|.%^~!\\$#()",KviOption_sectFlagIrcView),
	STRING_OPTION("OnNewQueryOpenedSound","",KviOption_sectFlagFrame),
	STRING_OPTION("OnHighlightedMessageSound","",KviOption_sectFlagFrame),
	STRING_OPTION("OnMeKickedSound","",KviOption_sectFlagFrame),
	STRING_OPTION("OnQueryMessageSound","",KviOption_sectFlagFrame),
	STRING_OPTION("IrcViewTimestampFormat","[hh:mm:ss]",KviOption_sectFlagIrcView),
	STRING_OPTION("PreferredTorrentClient","auto",KviOption_sectFlagFrame),
	STRING_OPTION("DefaultSrvEncoding","",KviOption_sectFlagFrame),
	STRING_OPTION("LogsPath","",KviOption_sectFlagUser | KviOption_encodePath)
};

#define STRINGLIST_OPTION(_txt,_flags) \
	KviStringListOption(KVI_STRINGLIST_OPTIONS_PREFIX _txt,QStringList(),_flags)

#define STRINGLIST_OPTION_WITHDEFAULT(_txt,_flags,_def) \
	KviStringListOption(KVI_STRINGLIST_OPTIONS_PREFIX _txt,QStringList(_def),_flags)


KviStringListOption g_stringlistOptionsTable[KVI_NUM_STRINGLIST_OPTIONS]=
{
	STRINGLIST_OPTION("HighlightWords",KviOption_sectFlagIrcView),
	STRINGLIST_OPTION("SpamWords",KviOption_sectFlagAntiSpam),
	STRINGLIST_OPTION_WITHDEFAULT("RecentChannels",KviOption_sectFlagRecent,"#kvirc"),
	STRINGLIST_OPTION("RecentServers",KviOption_sectFlagRecent),
	STRINGLIST_OPTION("RecentNicknames",KviOption_sectFlagRecent),
	STRINGLIST_OPTION("ModuleExtensionToolbars",KviOption_sectFlagFrame),
	STRINGLIST_OPTION("ImageSearchPaths",KviOption_sectFlagFrame | KviOption_encodePath),
	STRINGLIST_OPTION("RecentIrcUrls",KviOption_sectFlagRecent)
};

#define MIRC_COLOR_OPTION(_num,_red,_green,_blue) \
	KviColorOption(KVI_MIRCCOLOR_OPTIONS_PREFIX _num,QColor(_red,_green,_blue), \
		KviOption_sectFlagMircColor | KviOption_resetUpdateGui | KviOption_groupTheme)

KviColorOption g_mirccolorOptionsTable[KVI_NUM_MIRCCOLOR_OPTIONS]=
{
	MIRC_COLOR_OPTION("0",255,255,255),
	MIRC_COLOR_OPTION("1",0,0,0),
	MIRC_COLOR_OPTION("2",0,0,140),
	MIRC_COLOR_OPTION("3",0,100,0),
	MIRC_COLOR_OPTION("4",230,0,0),
	MIRC_COLOR_OPTION("5",150,0,0),
	MIRC_COLOR_OPTION("6",80,0,80),
	MIRC_COLOR_OPTION("7",255,90,0),
	MIRC_COLOR_OPTION("8",255,255,0),
	MIRC_COLOR_OPTION("9",0,255,0),
	MIRC_COLOR_OPTION("10",0,150,180),
	MIRC_COLOR_OPTION("11",170,170,255),
	MIRC_COLOR_OPTION("12",15,15,255),
	MIRC_COLOR_OPTION("13",200,0,200),
	MIRC_COLOR_OPTION("14",80,80,80),
	MIRC_COLOR_OPTION("15",170,170,170)
};

#define COLOR_OPTION(_name,_red,_green,_blue,_flags) \
	KviColorOption(KVI_COLOR_OPTIONS_PREFIX _name, \
		QColor(_red,_green,_blue), \
		_flags | KviOption_resetUpdateGui | KviOption_groupTheme)

KviColorOption g_colorOptionsTable[KVI_NUM_COLOR_OPTIONS]=
{
	COLOR_OPTION("MdiBackground",0,0,0,KviOption_sectFlagMdi),
	COLOR_OPTION("WindowListNormalText",0,0,0,KviOption_sectFlagWindowList),
	COLOR_OPTION("WindowListMinimizedText",60,60,60,KviOption_sectFlagWindowList),
	COLOR_OPTION("WindowListHighlight1Text",0,16,144,KviOption_sectFlagWindowList),
	COLOR_OPTION("WindowListHighlight2Text",32,0,255,KviOption_sectFlagWindowList),
	COLOR_OPTION("WindowListProgressBar",180,180,180,KviOption_sectFlagWindowList),
	COLOR_OPTION("UserListViewAwayForeground",143,143,143,KviOption_sectFlagUserListView),
	COLOR_OPTION("UserListViewOwnForeground",255,255,0,KviOption_sectFlagUserListView),
	COLOR_OPTION("UserListViewIrcOpForeground",200,0,0,KviOption_sectFlagUserListView),
	COLOR_OPTION("NotifierBackground",255,255,255,KviOption_sectFlagNotifier),
	COLOR_OPTION("NotifierForeground",0,0,0,KviOption_sectFlagNotifier),
	COLOR_OPTION("NotifierTitleForeground",255,255,255,KviOption_sectFlagNotifier),
	COLOR_OPTION("IrcViewBackground",255,255,255,KviOption_sectFlagIrcView),
	COLOR_OPTION("InputBackgroundColor",255,255,255,KviOption_sectFlagInput),
	COLOR_OPTION("InputSelectionBackground",0,0,0,KviOption_sectFlagInput),
	COLOR_OPTION("InputSelectionForeground",255,255,255,KviOption_sectFlagInput),
	COLOR_OPTION("InputCursor",0,0,0,KviOption_sectFlagInput),
	COLOR_OPTION("InputControl",200,100,0,KviOption_sectFlagInput),
	COLOR_OPTION("InputForeground",0,0,0,KviOption_sectFlagInput),
	COLOR_OPTION("UserListViewBackground",255,255,255,KviOption_sectFlagUserListView),
	COLOR_OPTION("UserListViewOpForeground",200,0,0,KviOption_sectFlagUserListView),
	COLOR_OPTION("UserListViewVoiceForeground",0,0,200,KviOption_sectFlagUserListView),
	COLOR_OPTION("UserListViewNormalForeground",0,0,0,KviOption_sectFlagUserListView),
	COLOR_OPTION("UserListViewSelectionBackground",0,0,0,KviOption_sectFlagUserListView),
	COLOR_OPTION("UserListViewSelectionForeground",255,255,255,KviOption_sectFlagUserListView),
	COLOR_OPTION("LabelBackground",255,255,255,KviOption_sectFlagLabel),
	COLOR_OPTION("LabelForeground",0,0,0,KviOption_sectFlagLabel),
	COLOR_OPTION("GlobalTransparencyFade",255,255,255,KviOption_sectFlagGui | KviOption_resetUpdatePseudoTransparency),
	COLOR_OPTION("IrcToolBarAppletBackground",255,255,255,KviOption_sectFlagIrcToolBar),
	COLOR_OPTION("IrcToolBarAppletForegroundLowContrast",180,180,180,KviOption_sectFlagIrcToolBar),
	COLOR_OPTION("IrcToolBarAppletForegroundMidContrast",110,110,110,KviOption_sectFlagIrcToolBar),
	COLOR_OPTION("IrcToolBarAppletForegroundHighContrastInactive1",40,40,40,KviOption_sectFlagIrcToolBar),
	COLOR_OPTION("IrcToolBarAppletForegroundHighContrastInactive2",40,40,40,KviOption_sectFlagIrcToolBar),
	COLOR_OPTION("IrcToolBarAppletForegroundHighContrastActive1",0,0,0,KviOption_sectFlagIrcToolBar),
	COLOR_OPTION("IrcToolBarAppletForegroundHighContrastActive2",0,60,0,KviOption_sectFlagIrcToolBar),
	COLOR_OPTION("TreeWindowListBackground",255,255,255,KviOption_sectFlagWindowList),
	COLOR_OPTION("TreeWindowListActiveBackground",200,200,200,KviOption_sectFlagWindowList),
	COLOR_OPTION("TreeWindowListForeground",0,0,0,KviOption_sectFlagWindowList),
	COLOR_OPTION("TreeWindowListActiveForeground",0,0,0,KviOption_sectFlagWindowList),
	COLOR_OPTION("TreeWindowListHighlight1Foreground",0,16,144,KviOption_sectFlagWindowList),
	COLOR_OPTION("TreeWindowListHighlight2Foreground",32,0,255,KviOption_sectFlagWindowList),
	COLOR_OPTION("TreeWindowListProgress",150,150,150,KviOption_sectFlagWindowList),
	COLOR_OPTION("UserListViewHalfOpForeground",119,0,178,KviOption_sectFlagUserListView),
	COLOR_OPTION("WindowListHighlight3Text",128,20,255,KviOption_sectFlagWindowList),
	COLOR_OPTION("WindowListHighlight4Text",220,0,255,KviOption_sectFlagWindowList),
	COLOR_OPTION("WindowListHighlight5Text",255,0,0,KviOption_sectFlagWindowList),
	COLOR_OPTION("TreeWindowListHighlight3Foreground",128,0,255,KviOption_sectFlagWindowList),
	COLOR_OPTION("TreeWindowListHighlight4Foreground",220,0,255,KviOption_sectFlagWindowList),
	COLOR_OPTION("TreeWindowListHighlight5Foreground",255,0,0,KviOption_sectFlagWindowList),
	COLOR_OPTION("UserListViewChanAdminForeground",83,54,5,KviOption_sectFlagUserListView),
	COLOR_OPTION("UserListViewUserOpForeground",0,127,178,KviOption_sectFlagUserListView),
	COLOR_OPTION("UserListViewGrid",110,110,110,KviOption_sectFlagUserListView),
	COLOR_OPTION("UserListViewChanOwnerForeground",81,87,111,KviOption_sectFlagUserListView),
	COLOR_OPTION("IrcViewMarkLine",40,40,40,KviOption_sectFlagIrcView)
};


#define IC_COLOR_OPTION(_num,_red,_green,_blue) \
	KviColorOption( \
		KVI_ICCOLOR_OPTIONS_PREFIX _num, \
		QColor(_red,_green,_blue), \
		KviOption_sectFlagWindowList | KviOption_resetUpdateGui | KviOption_groupTheme)

KviColorOption g_iccolorOptionsTable[KVI_NUM_ICCOLOR_OPTIONS]=
{
	IC_COLOR_OPTION("0",250,250,0),
	IC_COLOR_OPTION("1",0,0,0),
	IC_COLOR_OPTION("2",0,0,200),
	IC_COLOR_OPTION("3",200,0,0),
	IC_COLOR_OPTION("4",255,255,255),
	IC_COLOR_OPTION("5",0,150,190),
	IC_COLOR_OPTION("6",0,130,0),
	IC_COLOR_OPTION("7",150,0,170)
};

#define PIXMAP_OPTION(_name,_flags) \
	KviPixmapOption( \
		KVI_PIXMAP_OPTIONS_PREFIX _name , \
		KviPixmap(), \
		_flags | KviOption_resetUpdateGui)

KviPixmapOption g_pixmapOptionsTable[KVI_NUM_PIXMAP_OPTIONS]=
{
	PIXMAP_OPTION("MdiBackground",KviOption_sectFlagMdi | KviOption_groupTheme),
	PIXMAP_OPTION("IrcViewBackground",KviOption_sectFlagIrcView | KviOption_groupTheme),
	PIXMAP_OPTION("InputBackground",KviOption_sectFlagInput | KviOption_groupTheme),
	PIXMAP_OPTION("UserListViewBackground",KviOption_sectFlagUserListView | KviOption_groupTheme),
	PIXMAP_OPTION("LabelBackground",KviOption_sectFlagLabel | KviOption_groupTheme),
	PIXMAP_OPTION("MyAvatar",KviOption_sectFlagUser),
	PIXMAP_OPTION("IrcToolBarAppletBackground",KviOption_sectFlagIrcToolBar | KviOption_groupTheme),
	PIXMAP_OPTION("TreeWindowListBackground",KviOption_sectFlagWindowList | KviOption_groupTheme),
	PIXMAP_OPTION("GlobalTransparencyBackground",KviOption_sectFlagGui | KviOption_resetUpdatePseudoTransparency | KviOption_groupTheme),
	PIXMAP_OPTION("NotifierBackground",KviOption_sectFlagNotifier | KviOption_groupTheme)
};

//#define INT_OPTION(_name,_value,_flags)
//	{
//		KVI_INT_OPTIONS_PREFIX _name ,
//		_value ,
//		_flags
//	}
//
//KviIntOption g_intOptionsTable[KVI_NUM_INT_OPTIONS]=
//{
//	INT_OPTION("IrcViewMaxBufferSize",KVI_IRCVIEW_MAX_LINES,KviOption_sectFlagIrcView),
//	INT_OPTION("IrcViewTollTipTimeoutInMsec",1800,KviOption_sectFlagIrcView),
//	INT_OPTION("IrcViewToolTipHideTimeoutInMsec",10000,KviOption_sectFlagIrcView)
//};

#define UINT_OPTION(_name,_value,_flags) \
	KviUIntOption( \
		KVI_UINT_OPTIONS_PREFIX _name , \
		_value , \
		_flags \
	)

KviUIntOption g_uintOptionsTable[KVI_NUM_UINT_OPTIONS]=
{
	UINT_OPTION("IrcSocketTimeout",60,KviOption_sectFlagIrcSocket),
	UINT_OPTION("ModuleCleanupTimerInterval",90,KviOption_sectFlagModules),
	UINT_OPTION("ModuleCleanupTimeout",180,KviOption_sectFlagModules),
	UINT_OPTION("MaxCtcpRequests",5,KviOption_sectFlagCtcp),
	UINT_OPTION("CtcpFloodCheckInterval",10,KviOption_sectFlagCtcp),
	UINT_OPTION("TotalConnectionTime",0,0),
	UINT_OPTION("DccSocketTimeout",360,KviOption_sectFlagDcc),
	UINT_OPTION("DccSendIdleStepInMSec",30,KviOption_sectFlagDcc),
	UINT_OPTION("DccSendPacketSize",4096,KviOption_sectFlagDcc),
	UINT_OPTION("GlobalTransparencyParentFadeFactor",10,KviOption_sectFlagGui | KviOption_resetUpdatePseudoTransparency | KviOption_groupTheme),
	UINT_OPTION("GlobalTransparencyChildFadeFactor",50,KviOption_sectFlagGui | KviOption_resetUpdatePseudoTransparency | KviOption_groupTheme),
	UINT_OPTION("NotifyListCheckTimeInSecs",180,KviOption_sectFlagConnection),
	UINT_OPTION("SocketQueueFlushTimeout",500,KviOption_sectFlagIrcSocket),
	UINT_OPTION("OutgoingTrafficLimitUSeconds",2000000,KviOption_sectFlagIrcSocket),
	UINT_OPTION("NotifyListIsOnDelayTimeInSecs",6,KviOption_sectFlagConnection),
	UINT_OPTION("NotifyListUserhostDelayTimeInSecs",6,KviOption_sectFlagConnection),
	UINT_OPTION("TreeWindowListMinimumWidth",24,KviOption_sectFlagWindowList | KviOption_resetUpdateWindowList),
	UINT_OPTION("AvatarOfferTimeoutInSecs",60,KviOption_sectFlagAvatar),
	UINT_OPTION("IrcViewMaxBufferSize",1024,KviOption_sectFlagIrcView),
	UINT_OPTION("IrcViewToolTipTimeoutInMsec",1200,KviOption_sectFlagIrcView),
	UINT_OPTION("IrcViewToolTipHideTimeoutInMsec",12000,KviOption_sectFlagIrcView),
	UINT_OPTION("DccVoicePreBufferSize",32000,KviOption_sectFlagDcc),
	UINT_OPTION("IdentdPort",113,KviOption_sectFlagConnection | KviOption_resetRestartIdentd),
	UINT_OPTION("AvatarScaleWidth",80,KviOption_sectFlagAvatar | KviOption_resetUpdateGui),
	UINT_OPTION("AvatarScaleHeight",80,KviOption_sectFlagAvatar | KviOption_resetUpdateGui),
	UINT_OPTION("DccMinPort",30000,KviOption_sectFlagDcc),
	UINT_OPTION("DccMaxPort",31000,KviOption_sectFlagDcc),
	UINT_OPTION("SoundOfferTimeoutInSecs",60,KviOption_sectFlagConnection),
	UINT_OPTION("MaximumRequestedAvatarSize",256000,KviOption_sectFlagAvatar),
	UINT_OPTION("TreeWindowListMaximumWidth",200,KviOption_sectFlagWindowList | KviOption_resetUpdateWindowList),
	UINT_OPTION("PasteDelay",2000,KviOption_sectFlagInput),
	UINT_OPTION("MaxDccSendTransfers",10,KviOption_sectFlagDcc),
	UINT_OPTION("MaxDccSlots",64,KviOption_sectFlagDcc),
	UINT_OPTION("TileMethod",5,KviOption_sectFlagMdi),
	UINT_OPTION("LagMeterHeartbeat",5000,KviOption_sectFlagConnection | KviOption_resetRestartLagMeter),
	UINT_OPTION("LagAlarmTime",120000,KviOption_sectFlagConnection),
	UINT_OPTION("UNUSED",0,KviOption_sectFlagFrame),
	UINT_OPTION("WindowListButtonMinWidth",100,KviOption_sectFlagWindowList),
	UINT_OPTION("OutputVerbosityLevel",KVI_VERBOSITY_LEVEL_NORMAL,KviOption_sectFlagFrame),
	UINT_OPTION("IrcContextAppletWidth",160,KviOption_sectFlagFrame),
	UINT_OPTION("MinHighlightLevel",0,KviOption_uintMinHighlightLevel),
	UINT_OPTION("UserListViewGridType",3,KviOption_sectFlagUserListView | KviOption_resetUpdateGui | KviOption_groupTheme),
	UINT_OPTION("WindowListMaximumRows",2,KviOption_sectFlagFrame),
	UINT_OPTION("MaxAutoReconnectAttempts",15,KviOption_sectFlagFrame),
	UINT_OPTION("AutoReconnectDelay",10,KviOption_sectFlagFrame),
	UINT_OPTION("MaxDccSendSpeed",0,KviOption_sectFlagFrame),
	UINT_OPTION("MaxDccRecvSpeed",0,KviOption_sectFlagFrame),
	UINT_OPTION("TimeStampForeground",KVI_DARKGRAY,KviOption_sectFlagIrcView | KviOption_resetUpdateGui | KviOption_groupTheme),
	UINT_OPTION("TimeStampBackground",KVI_TRANSPARENT,KviOption_sectFlagIrcView | KviOption_resetUpdateGui | KviOption_groupTheme),
	UINT_OPTION("UserExperienceLevel",1,KviOption_sectFlagUser),
	UINT_OPTION("ClassicWindowListMaximumButtonWidth",100,KviOption_sectFlagGeometry | KviOption_resetUpdateGui | KviOption_groupTheme),
	UINT_OPTION("DefaultBanType",7,KviOption_sectFlagIrcSocket),
	UINT_OPTION("IrcViewPixmapAlign",0,KviOption_sectFlagIrcView | KviOption_groupTheme),
	UINT_OPTION("UserListPixmapAlign",0,KviOption_sectFlagFrame | KviOption_groupTheme),
	UINT_OPTION("ToolBarAppletPixmapAlign",0,KviOption_sectFlagFrame | KviOption_groupTheme),
	UINT_OPTION("TreeWindowListPixmapAlign",0,KviOption_sectFlagFrame | KviOption_groupTheme),
	UINT_OPTION("InputPixmapAlign",0,KviOption_sectFlagFrame | KviOption_groupTheme),
	UINT_OPTION("AutoFlushLogs",30,KviOption_sectFlagFrame),
	UINT_OPTION("MinTrayLowLevelMessage",1,KviOption_sectFlagFrame),
	UINT_OPTION("MinTrayHighLevelMessage",4,KviOption_sectFlagFrame),
	UINT_OPTION("IdentdOutputMode",KviIdentdOutputMode::Quiet,KviOption_sectFlagConnection),
	UINT_OPTION("ScaleAvatarsOnLoadHeight",600,KviOption_sectFlagAvatar),
	UINT_OPTION("ScaleAvatarsOnLoadWidth",800,KviOption_sectFlagAvatar),
	UINT_OPTION("NotifierActiveTransparency",90,KviOption_sectFlagNotifier),
	UINT_OPTION("NotifierInactiveTransparency",40,KviOption_sectFlagNotifier),
	UINT_OPTION("IrcViewMarkerStyle",0,KviOption_sectFlagIrcView | KviOption_groupTheme),
	UINT_OPTION("IrcViewMarkerSize",1,KviOption_sectFlagIrcView | KviOption_groupTheme),
	UINT_OPTION("UrlMouseClickNum",2,KviOption_sectFlagUrl),
	UINT_OPTION("NotifierAutoHideTime",30,KviOption_sectFlagModules),
	UINT_OPTION("LinesToPasteOnChannelJoin",10,KviOption_sectFlagLogging),
	UINT_OPTION("DaysIntervalToPasteOnChannelJoin",10,KviOption_sectFlagLogging),
	UINT_OPTION("LinesToPasteOnQueryJoin",10,KviOption_sectFlagLogging),
	UINT_OPTION("DaysIntervalToPasteOnQueryJoin",10,KviOption_sectFlagLogging),
	UINT_OPTION("SpacesToExpandTabulationInput",4,KviOption_sectFlagInput),
	UINT_OPTION("UserIrcViewOwnForeground",4,KviOption_sectFlagIrcView | KviOption_resetUpdateGui | KviOption_groupTheme),
	UINT_OPTION("UserIrcViewOwnBackground",8,KviOption_sectFlagIrcView | KviOption_resetUpdateGui | KviOption_groupTheme),
	UINT_OPTION("NotifierPixmapAlign",0,KviOption_sectFlagNotifier | KviOption_groupTheme),
	UINT_OPTION("OutputDatetimeFormat",0,KviOption_sectFlagIrcView),
};

#define FONT_OPTION(_name,_face,_size,_flags) \
	KviFontOption( \
		KVI_FONT_OPTIONS_PREFIX _name , \
		QFont(_face,_size) , \
		_flags | KviOption_groupTheme \
	)

KviFontOption g_fontOptionsTable[KVI_NUM_FONT_OPTIONS]=
{
#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	FONT_OPTION("IrcView","Arial",9,KviOption_sectFlagIrcView | KviOption_resetUpdateGui),
	FONT_OPTION("Input","Arial",12,KviOption_sectFlagInput | KviOption_resetUpdateGui),
	FONT_OPTION("UserListView","Arial",9,KviOption_sectFlagUserListView | KviOption_resetUpdateGui),
	FONT_OPTION("Label","Arial",9,KviOption_sectFlagLabel | KviOption_resetUpdateGui),
	FONT_OPTION("Application","Arial",9,KviOption_sectFlagGui | KviOption_resetUpdateAppFont),
	FONT_OPTION("IrcToolBarApplet","Arial",9,KviOption_sectFlagIrcToolBar | KviOption_resetUpdateGui),
	FONT_OPTION("WindowList","Arial",9,KviOption_sectFlagWindowList | KviOption_resetUpdateWindowList),
	FONT_OPTION("TreeWindowList","Arial",9,KviOption_sectFlagWindowList | KviOption_resetUpdateWindowList),
	FONT_OPTION("Notifier","Arial",9,KviOption_sectFlagNotifier | KviOption_resetUpdateGui),
	FONT_OPTION("NotifierTitle","Arial",9,KviOption_sectFlagNotifier | KviOption_resetUpdateGui)
#else
	FONT_OPTION("IrcView","Monospace",10,KviOption_sectFlagIrcView | KviOption_resetUpdateGui),
	FONT_OPTION("Input","Sans Serif",12,KviOption_sectFlagInput | KviOption_resetUpdateGui),
	FONT_OPTION("UserListView","Sans Serif",10,KviOption_sectFlagUserListView | KviOption_resetUpdateGui),
	FONT_OPTION("Label","Sans Serif",10,KviOption_sectFlagLabel | KviOption_resetUpdateGui),
	FONT_OPTION("Application","Sans Serif",10,KviOption_sectFlagGui | KviOption_resetUpdateAppFont),
	FONT_OPTION("IrcToolBarApplet","Sans Serif",10,KviOption_sectFlagIrcToolBar | KviOption_resetUpdateGui),
	FONT_OPTION("WindowList","Sans Serif",10,KviOption_sectFlagWindowList | KviOption_resetUpdateWindowList),
	FONT_OPTION("TreeWindowList","Sans Serif",10,KviOption_sectFlagWindowList | KviOption_resetUpdateWindowList),
	FONT_OPTION("Notifier","Sans Serif",9,KviOption_sectFlagNotifier | KviOption_resetUpdateNotifier),
	FONT_OPTION("NotifierTitle","Sans Serif",9,KviOption_sectFlagNotifier | KviOption_resetUpdateNotifier)
#endif
};

#define MSGTYPE_OPTION(_name,_text,_icon,_levl) \
	KviMsgTypeOption( \
		KVI_MSGTYPE_OPTIONS_PREFIX _name , \
		KviMsgType(_text,_icon,KVI_BLACK,KVI_TRANSPARENT,true,_levl), \
		KviOption_sectFlagMsgType | KviOption_groupTheme \
	)

#define MSGTYPE_OPTION_SPEC(_name,_text,_icon,_fore,_back,_levl) \
	KviMsgTypeOption( \
		KVI_MSGTYPE_OPTIONS_PREFIX _name , \
		KviMsgType(_text,_icon,_fore,_back,true,_levl), \
		KviOption_sectFlagMsgType | KviOption_groupTheme \
	)



// FIXME: #warning "FIX THE ICONS HERE!!!"

KviMsgTypeOption g_msgtypeOptionsTable[KVI_NUM_MSGTYPE_OPTIONS]=
{
	MSGTYPE_OPTION("Echo",__tr_no_lookup("Normal text"),KVI_SMALLICON_NONE,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION_SPEC("Selection",__tr_no_lookup("Selection"),KVI_SMALLICON_NONE,KVI_WHITE,KVI_BLACK,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION_SPEC("Highlight",__tr_no_lookup("Highlighted text"),KVI_SMALLICON_ALERT,KVI_WHITE,KVI_BLACK,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION_SPEC("Url",__tr_no_lookup("URL foreground"),KVI_SMALLICON_NONE,KVI_BLUE,KVI_TRANSPARENT,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION_SPEC("Link",__tr_no_lookup("Link overlay foreground"),KVI_SMALLICON_NONE,KVI_BLUE,KVI_TRANSPARENT,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION_SPEC("ParserError",__tr_no_lookup("Parser error"),KVI_SMALLICON_PARSERERROR,KVI_RED,KVI_TRANSPARENT,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION_SPEC("ParserWarning",__tr_no_lookup("Parser warning"),KVI_SMALLICON_PARSERWARNING,KVI_RED,KVI_TRANSPARENT,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("HostLookup",__tr_no_lookup("Host lookup result"),KVI_SMALLICON_SERVER,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("SocketMessage",__tr_no_lookup("Socket message"),KVI_SMALLICON_SOCKETMESSAGE,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("SocketWarning",__tr_no_lookup("Socket warning"),KVI_SMALLICON_SOCKETWARNING,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION_SPEC("SocketError",__tr_no_lookup("Socket error"),KVI_SMALLICON_SOCKETERROR,KVI_RED,KVI_TRANSPARENT,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION_SPEC("SystemError",__tr_no_lookup("System error"),KVI_SMALLICON_SYSTEMERROR,KVI_RED,KVI_TRANSPARENT,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("Raw",__tr_no_lookup("Raw data to server"),KVI_SMALLICON_RAW,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("Connection",__tr_no_lookup("Connection status"),KVI_SMALLICON_WORLD,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("SystemWarning",__tr_no_lookup("System warning"),KVI_SMALLICON_SYSTEMWARNING,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("SystemMessage",__tr_no_lookup("System message"),KVI_SMALLICON_SYSTEMMESSAGE,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("UnhandledReply",__tr_no_lookup("Unhandled server reply"),KVI_SMALLICON_UNHANDLED,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("ServerInfo",__tr_no_lookup("Server information"),KVI_SMALLICON_SERVERINFO,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("Motd",__tr_no_lookup("Server Message of the Day"),KVI_SMALLICON_MOTD,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("ServerPing",__tr_no_lookup("Server ping"),KVI_SMALLICON_SERVERPING,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("Join",__tr_no_lookup("Join message"),KVI_SMALLICON_JOIN,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("Part",__tr_no_lookup("Part message"),KVI_SMALLICON_PART,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("Unrecognized",__tr_no_lookup("Unrecognized/broken message"),KVI_SMALLICON_UNRECOGNIZED,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("Topic",__tr_no_lookup("Topic message"),KVI_SMALLICON_TOPIC,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("OwnPrivateMessage",__tr_no_lookup("Own private message"),KVI_SMALLICON_OWNPRIVMSG,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("ChannelPrivateMessage",__tr_no_lookup("Channel private message"),KVI_SMALLICON_CHANPRIVMSG,KVI_MSGTYPE_LEVEL_4),
	MSGTYPE_OPTION("QueryPrivateMessage",__tr_no_lookup("Query private message"),KVI_SMALLICON_QUERYPRIVMSG,KVI_MSGTYPE_LEVEL_4),
	MSGTYPE_OPTION("CtcpReply",__tr_no_lookup("CTCP reply"),KVI_SMALLICON_CTCPREPLY,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("CtcpRequestReplied",__tr_no_lookup("CTCP request replied"),KVI_SMALLICON_CTCPREQUESTREPLIED,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("CtcpRequestIgnored",__tr_no_lookup("CTCP request ignored"),KVI_SMALLICON_CTCPREQUESTIGNORED,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("CtcpRequestFlood",__tr_no_lookup("CTCP request flood warning"),KVI_SMALLICON_CTCPREQUESTFLOOD,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("CtcpRequestUnknown",__tr_no_lookup("CTCP request unknown"),KVI_SMALLICON_CTCPREQUESTUNKNOWN,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("Action",__tr_no_lookup("User action"),KVI_SMALLICON_ACTION,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("AvatarChange",__tr_no_lookup("Avatar change"),KVI_SMALLICON_AVATAR,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("Quit",__tr_no_lookup("Quit message"),KVI_SMALLICON_QUIT,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("Split",__tr_no_lookup("Split message"),KVI_SMALLICON_SPLIT,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("QuitSplit",__tr_no_lookup("Quit on netsplit message"),KVI_SMALLICON_QUITSPLIT,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("Nick",__tr_no_lookup("Nick changes"),KVI_SMALLICON_NICK,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("Op",__tr_no_lookup("+o mode change"),KVI_SMALLICON_OP,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("Deop",__tr_no_lookup("-o mode change"),KVI_SMALLICON_DEOP,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("Voice",__tr_no_lookup("+v mode change"),KVI_SMALLICON_VOICE,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("Devoice",__tr_no_lookup("-v mode change"),KVI_SMALLICON_DEVOICE,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("Mode",__tr_no_lookup("Multiple user mode change"),KVI_SMALLICON_MODE,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("Key",__tr_no_lookup("Channel key change"),KVI_SMALLICON_KEY,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("Limit",__tr_no_lookup("Channel limit change"),KVI_SMALLICON_LIMIT,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("Ban",__tr_no_lookup("+b mode change"),KVI_SMALLICON_BAN,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("Unban",__tr_no_lookup("-b mode change"),KVI_SMALLICON_UNBAN,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("BanExcept",__tr_no_lookup("+e mode change"),KVI_SMALLICON_BANEXCEPT,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("BanUnexcept",__tr_no_lookup("-e mode change"),KVI_SMALLICON_BANUNEXCEPT,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("InviteExcept",__tr_no_lookup("+I mode change"),KVI_SMALLICON_INVITEEXCEPT,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("InviteUnexcept",__tr_no_lookup("-I mode change"),KVI_SMALLICON_INVITEUNEXCEPT,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("ChanMode",__tr_no_lookup("Multiple channel mode change"),KVI_SMALLICON_CHANMODE,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("Who",__tr_no_lookup("Who reply"),KVI_SMALLICON_WHO,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("DccRequest",__tr_no_lookup("DCC request"),KVI_SMALLICON_DCCREQUEST,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("DccMsg",__tr_no_lookup("DCC message"),KVI_SMALLICON_DCCMSG,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("DccError",__tr_no_lookup("DCC error"),KVI_SMALLICON_DCCERROR,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("NicknameProblem",__tr_no_lookup("Nickname problem"),KVI_SMALLICON_NICKNAMEPROBLEM,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("WhoisUser",__tr_no_lookup("Whois user reply"),KVI_SMALLICON_WHOISUSER,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("WhoisChannels",__tr_no_lookup("Whois channels reply"),KVI_SMALLICON_WHOISCHANNELS,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("WhoisIdle",__tr_no_lookup("Whois idle reply"),KVI_SMALLICON_WHOISIDLE,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("WhoisServer",__tr_no_lookup("Whois server reply"),KVI_SMALLICON_WHOISSERVER,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("WhoisOther",__tr_no_lookup("Whois other reply"),KVI_SMALLICON_WHOISOTHER,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("CreationTime",__tr_no_lookup("Channel creation time reply"),KVI_SMALLICON_TIME,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("NotifyOnLine",__tr_no_lookup("Notify list joins"),KVI_SMALLICON_NOTIFYONLINE,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("NotifyOffLine",__tr_no_lookup("Notify list leaves"),KVI_SMALLICON_NOTIFYOFFLINE,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("OwnPrivmsgCrypted",__tr_no_lookup("Own encrypted private message"),KVI_SMALLICON_OWNPRIVMSGCRYPTED,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("ChanPrivmsgCrypted",__tr_no_lookup("Channel encrypted private message"),KVI_SMALLICON_CHANPRIVMSGCRYPTED,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("QueryPrivmsgCrypted",__tr_no_lookup("Query encrypted private message"),KVI_SMALLICON_QUERYPRIVMSGCRYPTED,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("DccChatMsg",__tr_no_lookup("DCC chat message"),KVI_SMALLICON_DCCCHATMSG,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("DccChatMsgCrypted",__tr_no_lookup("Encrypted DCC chat message"),KVI_SMALLICON_DCCCHATMSGCRYPTED,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("Irc",__tr_no_lookup("Login operations completed"),KVI_SMALLICON_IRC,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("Kick",__tr_no_lookup("Kick action"),KVI_SMALLICON_KICK,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("Links",__tr_no_lookup("Links reply"),KVI_SMALLICON_LINKS,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("Spam",__tr_no_lookup("Spam report"),KVI_SMALLICON_SPAM,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("Icq",__tr_no_lookup("ICQ message"),KVI_SMALLICON_ICQ,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("IcqMessage",__tr_no_lookup("ICQ user-message"),KVI_SMALLICON_MESSAGE,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("IcqMessageSent",__tr_no_lookup("Outgoing ICQ user-message"),KVI_SMALLICON_MESSAGESENT,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("ChannelNotice",__tr_no_lookup("Channel notice"),KVI_SMALLICON_CHANNELNOTICE,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("ChannelNoticeCrypted",__tr_no_lookup("Encrypted channel notice"),KVI_SMALLICON_CHANNELNOTICECRYPTED,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("QueryNotice",__tr_no_lookup("Query notice"),KVI_SMALLICON_QUERYNOTICE,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("QueryNoticeCrypted",__tr_no_lookup("Encrypted query notice"),KVI_SMALLICON_QUERYNOTICECRYPTED,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("ServerNotice",__tr_no_lookup("Server notice"),KVI_SMALLICON_SERVERNOTICE,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("HalfOp",__tr_no_lookup("Halfop mode change"),KVI_SMALLICON_HALFOP,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("CtcpReplyUnknown",__tr_no_lookup("Unknown CTCP reply"),KVI_SMALLICON_CTCPREPLYUNKNOWN,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("NickServ",__tr_no_lookup("NickServ message"),KVI_SMALLICON_NICKSERV,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("ChanServ",__tr_no_lookup("ChanServ message"),KVI_SMALLICON_CHANSERV,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("Away",__tr_no_lookup("Away message"),KVI_SMALLICON_AWAY,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("Ident",__tr_no_lookup("Ident message"),KVI_SMALLICON_IDENT,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("List",__tr_no_lookup("Channel list message"),KVI_SMALLICON_LIST,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("HalfDeOp",__tr_no_lookup("Half-deop mode change"),KVI_SMALLICON_HALFDEOP,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("Invite",__tr_no_lookup("Invite message"),KVI_SMALLICON_INVITE,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("Multimedia",__tr_no_lookup("Multimedia message"),KVI_SMALLICON_MULTIMEDIA,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("QueryTrace",__tr_no_lookup("Query trace message"),KVI_SMALLICON_QUERYTRACE,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("Wallops",__tr_no_lookup("Wallops message"),KVI_SMALLICON_WALLOPS,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("JoinError",__tr_no_lookup("Join error message"),KVI_SMALLICON_NOCHANNEL,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("BroadcastPrivmsg",__tr_no_lookup("Broadcast private message"),KVI_SMALLICON_BROADCASTPRIVMSG,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("BroadcastNotice",__tr_no_lookup("Broadcast notice"),KVI_SMALLICON_BROADCASTNOTICE,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("MeKick",__tr_no_lookup("Am kicked"),KVI_SMALLICON_MEKICK,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeOp",__tr_no_lookup("Am op'd"),KVI_SMALLICON_MEOP,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeVoice",__tr_no_lookup("Am voiced"),KVI_SMALLICON_MEVOICE,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeDeOp",__tr_no_lookup("Am deop'd"),KVI_SMALLICON_MEDEOP,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeDeVoice",__tr_no_lookup("Am devoiced"),KVI_SMALLICON_MEDEVOICE,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeHalfOp",__tr_no_lookup("Am halfop'd"),KVI_SMALLICON_MEHALFOP,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeDeHalfOp",__tr_no_lookup("Am de-halfop'd"),KVI_SMALLICON_MEDEHALFOP,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeBan",__tr_no_lookup("Ban matching my mask"),KVI_SMALLICON_MEBAN,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeUnban",__tr_no_lookup("Unban matching my mask"),KVI_SMALLICON_MEUNBAN,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeBanExcept",__tr_no_lookup("Ban exception matching my mask"),KVI_SMALLICON_MEBANEXCEPT,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeBanUnExcept",__tr_no_lookup("Ban unexception matching my mask"),KVI_SMALLICON_MEBANUNEXCEPT,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeInviteExcept",__tr_no_lookup("Invite exception matching my mask"),KVI_SMALLICON_MEINVITEEXCEPT,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeInviteUnexcept",__tr_no_lookup("Invite unexception matching my mask"),KVI_SMALLICON_MEINVITEUNEXCEPT,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("Ignore",__tr_no_lookup("Ignored user message"),KVI_SMALLICON_IGNORE,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("Stats",__tr_no_lookup("Server statistics"),KVI_SMALLICON_STATS,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("SSL",__tr_no_lookup("SSL message"),KVI_SMALLICON_SSL,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("Search",__tr_no_lookup("Search"),KVI_SMALLICON_SEARCH,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("GenericSuccess",__tr_no_lookup("Generic success"),KVI_SMALLICON_CYANSQUARE,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION("GenericStatus",__tr_no_lookup("Generic status"),KVI_SMALLICON_GREENSQUARE,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("GenericVerbose",__tr_no_lookup("Generic verbose message"),KVI_SMALLICON_DKGREENSQUARE,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("GenericWarning",__tr_no_lookup("Generic warning"),KVI_SMALLICON_YELLOWSQUARE,KVI_MSGTYPE_LEVEL_3),
	MSGTYPE_OPTION_SPEC("GenericError",__tr_no_lookup("Generic error"),KVI_SMALLICON_REDSQUARE,KVI_RED,KVI_TRANSPARENT,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION_SPEC("GenericCritical",__tr_no_lookup("Generic critical error"),KVI_SMALLICON_BLACKSQUARE,KVI_RED,KVI_TRANSPARENT,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("ChanAdmin",__tr_no_lookup("Chan admin status set"),KVI_SMALLICON_CHANADMIN,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("ChanUnAdmin",__tr_no_lookup("Chan admin status unset"),KVI_SMALLICON_CHANUNADMIN,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeChanAdmin",__tr_no_lookup("Own chan admin status set"),KVI_SMALLICON_MECHANADMIN,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeChanUnAdmin",__tr_no_lookup("Own chan admin status unset"),KVI_SMALLICON_MECHANUNADMIN,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("UserOp",__tr_no_lookup("Userop mode change"),KVI_SMALLICON_USEROP,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("UserDeOp",__tr_no_lookup("User-deop mode change"),KVI_SMALLICON_DEUSEROP,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("MeUserOp",__tr_no_lookup("Am userop'd"),KVI_SMALLICON_MEUSEROP,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeDeUserOp",__tr_no_lookup("Am de-userop'd"),KVI_SMALLICON_MEDEUSEROP,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION_SPEC("Verbose",__tr_no_lookup("Verbose/Debug"),KVI_SMALLICON_BINARYTEXT,KVI_DARKGRAY,KVI_TRANSPARENT,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("ChanOwner",__tr_no_lookup("Chan owner status set"),KVI_SMALLICON_CHANOWNER,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("ChanUnOwner",__tr_no_lookup("Chan owner status unset"),KVI_SMALLICON_CHANUNOWNER,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeChanOwner",__tr_no_lookup("Own chan owner status set"),KVI_SMALLICON_MECHANOWNER,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeChanUnOwner",__tr_no_lookup("Own chan owner status unset"),KVI_SMALLICON_MECHANUNOWNER,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION_SPEC("ServerError",__tr_no_lookup("Server error"),KVI_SMALLICON_SERVERERROR,KVI_RED,KVI_TRANSPARENT,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION_SPEC("Help",__tr_no_lookup("Help"),KVI_SMALLICON_HELP,KVI_DARKGREEN,KVI_TRANSPARENT,KVI_MSGTYPE_LEVEL_1),
	MSGTYPE_OPTION("TextEncoding",__tr_no_lookup("Text Encoding Server message"),KVI_SMALLICON_TEXTENCODING,KVI_MSGTYPE_LEVEL_1),
	// TODO: add icon and check what level means
	// Pragma: the message level is a warning level that is rendered in the WindowList
	//         messages with higher level are rendered with more visible color (i.e. red)
	//         messages with very high level might flash the tray and end up in the notifier
	//         level 1 or 2 for standard torrent messages is ok
	MSGTYPE_OPTION("BitTorrent",__tr_no_lookup("BitTorrent message"),KVI_SMALLICON_MULTIMEDIA,KVI_MSGTYPE_LEVEL_2),
	MSGTYPE_OPTION("IrcOp",__tr_no_lookup("IRC Op status set"),KVI_SMALLICON_IRCOP,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("DeIrcOp",__tr_no_lookup("IRC Op status unset"),KVI_SMALLICON_DEIRCOP,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeIrcOp",__tr_no_lookup("Own IRC Op status set"),KVI_SMALLICON_MEIRCOP,KVI_MSGTYPE_LEVEL_5),
	MSGTYPE_OPTION("MeDeIrcOp",__tr_no_lookup("Own IRC Op status unset"),KVI_SMALLICON_MEDEIRCOP,KVI_MSGTYPE_LEVEL_5)
};

static const char * options_section_table[KVI_NUM_OPTION_SECT_FLAGS] =
{
	"None",          "Frame",       "Mdi",              "Windows",
	"WindowList",    "IrcView",     "MsgType",          "MircColor",
	"Input",         "UserParser",  "Connection",       "IrcSocket",
	"User",          "UserListView","Label",            "Modules",
	"Ctcp",          "Dcc",         "Gui",              "IrcToolBar",
	"Logging",       "AntiSpam",    "Avatar",           "Url",
	"Recent",        "Geometry"
};

static void config_set_section(int flag,KviConfig * cfg)
{
	int index = flag & KviOption_sectMask;
	if((index < KVI_NUM_OPTION_SECT_FLAGS) && (index >= 0))
	{
		cfg->setGroup(options_section_table[index]);
	} else cfg->setGroup(""); // Default group
}

void KviApp::loadOptions()
{
	QString buffer;
	if(getReadOnlyConfigPath(buffer,KVI_CONFIGFILE_MAIN))
	{
		KviConfig cfg(buffer,KviConfig::Read);

		int prg = 12;
		int i;

		#define READ_OPTIONS(_num,_table,_readFnc) \
		for(i=0;i<_num;i++) \
		{ \
			config_set_section(_table[i].flags,&cfg); \
			_table[i].option = cfg._readFnc(_table[i].name,_table[i].option); \
		} \
		prg += 3; \
		KVI_SPLASH_SET_PROGRESS(prg)

		READ_OPTIONS(KVI_NUM_RECT_OPTIONS,g_rectOptionsTable,readRectEntry)
		READ_OPTIONS(KVI_NUM_BOOL_OPTIONS,g_boolOptionsTable,readBoolEntry)
		READ_OPTIONS(KVI_NUM_STRING_OPTIONS,g_stringOptionsTable,readQStringEntry)
		READ_OPTIONS(KVI_NUM_COLOR_OPTIONS,g_colorOptionsTable,readColorEntry)
		READ_OPTIONS(KVI_NUM_PIXMAP_OPTIONS,g_pixmapOptionsTable,readPixmapEntry)
		READ_OPTIONS(KVI_NUM_UINT_OPTIONS,g_uintOptionsTable,readUIntEntry)
		READ_OPTIONS(KVI_NUM_FONT_OPTIONS,g_fontOptionsTable,readFontEntry)
		READ_OPTIONS(KVI_NUM_MSGTYPE_OPTIONS,g_msgtypeOptionsTable,readMsgTypeEntry)
		READ_OPTIONS(KVI_NUM_STRINGLIST_OPTIONS,g_stringlistOptionsTable,readStringListEntry)
		READ_OPTIONS(KVI_NUM_MIRCCOLOR_OPTIONS,g_mirccolorOptionsTable,readColorEntry)
		READ_OPTIONS(KVI_NUM_ICCOLOR_OPTIONS,g_iccolorOptionsTable,readColorEntry)

		for(i=0;i<KVI_NUM_STRING_OPTIONS;i++)
		{
			if(g_stringOptionsTable[i].flags & KviOption_encodePath)
				KviStringConversion::decodePath(g_stringOptionsTable[i].option);
		}

		for(i=0;i<KVI_NUM_STRINGLIST_OPTIONS;i++)
		{
			if(g_stringlistOptionsTable[i].flags & KviOption_encodePath)
				KviStringConversion::decodePath(g_stringlistOptionsTable[i].option);
		}

		#undef READ_OPTIONS
	} // else already have the defaults
}

void KviApp::saveOptions()
{
	QString buffer;

	saveRecentChannels();

	getLocalKvircDirectory(buffer,Config,KVI_CONFIGFILE_MAIN);
	KviConfig cfg(buffer,KviConfig::Write);

	int i;

	#define WRITE_OPTIONS(_num,_table) \
	for(i=0;i<_num;i++) \
	{ \
		config_set_section(_table[i].flags,&cfg); \
		cfg.writeEntry(_table[i].name,_table[i].option); \
	}

	WRITE_OPTIONS(KVI_NUM_RECT_OPTIONS,g_rectOptionsTable)
	WRITE_OPTIONS(KVI_NUM_BOOL_OPTIONS,g_boolOptionsTable)
		for(i=0;i<KVI_NUM_STRING_OPTIONS;i++)
		{
			if(g_stringOptionsTable[i].flags & KviOption_encodePath)
				KviStringConversion::encodePath(g_stringOptionsTable[i].option);
		}
	WRITE_OPTIONS(KVI_NUM_STRING_OPTIONS,g_stringOptionsTable)
		for(i=0;i<KVI_NUM_STRING_OPTIONS;i++)
		{
			if(g_stringOptionsTable[i].flags & KviOption_encodePath)
				KviStringConversion::decodePath(g_stringOptionsTable[i].option);
		}
	WRITE_OPTIONS(KVI_NUM_COLOR_OPTIONS,g_colorOptionsTable)
	WRITE_OPTIONS(KVI_NUM_PIXMAP_OPTIONS,g_pixmapOptionsTable)
	WRITE_OPTIONS(KVI_NUM_UINT_OPTIONS,g_uintOptionsTable)
	WRITE_OPTIONS(KVI_NUM_FONT_OPTIONS,g_fontOptionsTable)
	WRITE_OPTIONS(KVI_NUM_MSGTYPE_OPTIONS,g_msgtypeOptionsTable)
	for(i=0;i<KVI_NUM_STRINGLIST_OPTIONS;i++)
		{
			if(g_stringlistOptionsTable[i].flags & KviOption_encodePath)
				KviStringConversion::encodePath(g_stringlistOptionsTable[i].option);
		}
	WRITE_OPTIONS(KVI_NUM_STRINGLIST_OPTIONS,g_stringlistOptionsTable)
	for(i=0;i<KVI_NUM_STRINGLIST_OPTIONS;i++)
	{
		if(g_stringlistOptionsTable[i].flags & KviOption_encodePath)
			KviStringConversion::decodePath(g_stringlistOptionsTable[i].option);
	}
	WRITE_OPTIONS(KVI_NUM_MIRCCOLOR_OPTIONS,g_mirccolorOptionsTable)
	WRITE_OPTIONS(KVI_NUM_ICCOLOR_OPTIONS,g_iccolorOptionsTable)

	#undef WRITE_OPTIONS
}


#undef WRITE_OPTIONS

#define WRITE_OPTIONS(_num,_table) \
	for(i=0;i<_num;i++) \
	{ \
		if(_table[i].flags & KviOption_groupTheme) \
			cfg.writeEntry(_table[i].name,_table[i].option); \
	}

namespace KviTheme
{
	// utility functions for the KviTheme namespace (kvi_theme.h)
	// that are implemented here for convenience (in saving the options)
	bool save(KviThemeInfo &options)
	{
		QString szD = options.absoluteDirectory();

		if(szD.isEmpty())
		{
			options.setLastError(__tr2qs("Missing absolute directory for the theme information"));
			return false;
		}

		if(!KviFileUtils::directoryExists(szD))
		{
			if(!KviFileUtils::makeDir(szD))
			{
				options.setLastError(__tr2qs("Failed to create the theme directory"));
				return false;
			}
		}

		szD.append(KVI_PATH_SEPARATOR_CHAR);
		szD.append(KVI_THEMEINFO_FILE_NAME);

		if(!options.save(szD))
		{
			return false;
		}

		szD = options.absoluteDirectory();
		szD.append(KVI_PATH_SEPARATOR_CHAR);
		szD.append(KVI_THEMEDATA_FILE_NAME);

		KviConfig cfg(szD,KviConfig::Write);

		cfg.setGroup(KVI_THEMEDATA_CONFIG_GROUP);

		int i;

		WRITE_OPTIONS(KVI_NUM_RECT_OPTIONS,g_rectOptionsTable)
		WRITE_OPTIONS(KVI_NUM_BOOL_OPTIONS,g_boolOptionsTable)
		WRITE_OPTIONS(KVI_NUM_STRING_OPTIONS,g_stringOptionsTable)
		WRITE_OPTIONS(KVI_NUM_COLOR_OPTIONS,g_colorOptionsTable)
	 	WRITE_OPTIONS(KVI_NUM_UINT_OPTIONS,g_uintOptionsTable)
		WRITE_OPTIONS(KVI_NUM_FONT_OPTIONS,g_fontOptionsTable)
		WRITE_OPTIONS(KVI_NUM_MSGTYPE_OPTIONS,g_msgtypeOptionsTable)
		WRITE_OPTIONS(KVI_NUM_STRINGLIST_OPTIONS,g_stringlistOptionsTable)
		WRITE_OPTIONS(KVI_NUM_MIRCCOLOR_OPTIONS,g_mirccolorOptionsTable)
		WRITE_OPTIONS(KVI_NUM_ICCOLOR_OPTIONS,g_iccolorOptionsTable)

		#undef WRITE_OPTIONS

		// the pixmap options need special processing
		for(i=0;i<KVI_NUM_PIXMAP_OPTIONS;i++)
		{
			if(g_pixmapOptionsTable[i].flags & KviOption_groupTheme)
			{
				if(g_pixmapOptionsTable[i].option.pixmap())
				{
					QString szPixPath = options.absoluteDirectory();
					szPixPath.append(KVI_PATH_SEPARATOR_CHAR);
					QString szPixName = g_pixmapOptionsTable[i].name;
					szPixName += ".png";
					szPixPath += szPixName;

					if(g_pixmapOptionsTable[i].option.pixmap()->save(szPixPath,"PNG"))
					{
						cfg.writeEntry(g_pixmapOptionsTable[i].name,szPixName);
					} else {
						// we ignore this error for now
						debug("failed to save %s",szPixPath.toUtf8().data());
						cfg.writeEntry(g_pixmapOptionsTable[i].name,"");
					}
				} else {
					cfg.writeEntry(g_pixmapOptionsTable[i].name,"");
				}
			}
		}

		cfg.writeEntry("stringIconThemeSubdir",options.subdirectory());

		// find all the "kvi_bigicon" images that we can find in the main pics directory
		QString szPicsPath;

		g_pApp->getGlobalKvircDirectory(szPicsPath,KviApp::Pics);
		QDir d(szPicsPath);
		QStringList sl = d.entryList(QDir::nameFiltersFromString("kvi_bigicon_*.png"),QDir::Files);

		for(QStringList::Iterator it=sl.begin();it != sl.end();it++)
		{
			KviCachedPixmap * p = g_pIconManager->getPixmapWithCache(*it);
			if(p)
			{
				QString szPixPath = options.absoluteDirectory();
				szPixPath.append(KVI_PATH_SEPARATOR_CHAR);
				szPixPath += *it;

				if(!KviFileUtils::copyFile(p->path(),szPixPath))
				{
					options.setLastError(__tr2qs("Failed to save one of the theme images"));
					return false;
				}
			}
		}


		szD = options.absoluteDirectory();
		szD.append(KVI_PATH_SEPARATOR_CHAR);
		szD.append(KVI_SMALLICONS_SUBDIRECTORY);

		if(!KviFileUtils::makeDir(szD))
		{
			options.setLastError(__tr2qs("Failed to create the theme subdirectory"));
			return false;
		}

		// We actually need to *save* the small icons since
		// we have a compatibility mode that can load them from
		// the old format kvi_smallicon_XY.png multiimage libraries.

		for(int j=0;j<KVI_NUM_SMALL_ICONS;j++)
		{
			QPixmap * pix = g_pIconManager->getSmallIcon(j);

			QString szPixPath = options.absoluteDirectory();
			szPixPath.append(KVI_PATH_SEPARATOR_CHAR);
			szPixPath.append(KVI_SMALLICONS_SUBDIRECTORY);
			szPixPath.append(KVI_PATH_SEPARATOR_CHAR);
			szPixPath.append("kcs_");
			szPixPath.append(g_pIconManager->getSmallIconName(j));
			szPixPath.append(".png");

			if(!pix->save(szPixPath,"PNG",90))
			{
				options.setLastError(__tr2qs("Failed to save one of the theme images"));
				return false;
			}
		}

		return true;
	}

	bool load(const QString &themeDir,KviThemeInfo &buffer)
	{
		if(!buffer.loadFromDirectory(themeDir))
			return false; // makes sure that themedata exists too

		// reset the current theme subdir
		KVI_OPTION_STRING(KviOption_stringIconThemeSubdir) = "";

		// reset the splash screen pointer
		QString szPointerFile;
		g_pApp->getLocalKvircDirectory(szPointerFile,KviApp::Themes,"current-splash");
		KviFileUtils::removeFile(szPointerFile);

		QString szD = themeDir;
		KviQString::ensureLastCharIs(szD,KVI_PATH_SEPARATOR_CHAR);
		szD.append(KVI_THEMEDATA_FILE_NAME);

		KviConfig cfg(szD,KviConfig::Read);

		cfg.setGroup(KVI_THEMEDATA_CONFIG_GROUP);

		int i;
		int iResetFlags = 0;

		#undef READ_OPTIONS

		#define READ_OPTIONS(_num,_table,_readFnc) \
		for(i=0;i<_num;i++) \
		{ \
			if(_table[i].flags & KviOption_groupTheme) \
			{ \
				if(cfg.hasKey(_table[i].name)) \
				{ \
					iResetFlags |= (_table[i].flags & KviOption_resetMask); \
					_table[i].option = cfg._readFnc(_table[i].name,_table[i].option); \
				} \
			} \
		}

		#define READ_ALL_OPTIONS(_num,_table,_readFnc) \
		for(i=0;i<_num;i++) \
		{ \
			if(cfg.hasKey(_table[i].name)) \
			{ \
				iResetFlags |= (_table[i].flags & KviOption_resetMask); \
				_table[i].option = cfg._readFnc(_table[i].name,_table[i].option); \
			} \
		}

		READ_OPTIONS(KVI_NUM_RECT_OPTIONS,g_rectOptionsTable,readRectEntry)
		READ_OPTIONS(KVI_NUM_BOOL_OPTIONS,g_boolOptionsTable,readBoolEntry)
		READ_OPTIONS(KVI_NUM_STRING_OPTIONS,g_stringOptionsTable,readQStringEntry)
		READ_OPTIONS(KVI_NUM_COLOR_OPTIONS,g_colorOptionsTable,readColorEntry)
		READ_OPTIONS(KVI_NUM_UINT_OPTIONS,g_uintOptionsTable,readUIntEntry)
		READ_OPTIONS(KVI_NUM_FONT_OPTIONS,g_fontOptionsTable,readFontEntry)
		READ_ALL_OPTIONS(KVI_NUM_MSGTYPE_OPTIONS,g_msgtypeOptionsTable,readMsgTypeEntry)
		READ_OPTIONS(KVI_NUM_STRINGLIST_OPTIONS,g_stringlistOptionsTable,readStringListEntry)
		READ_OPTIONS(KVI_NUM_MIRCCOLOR_OPTIONS,g_mirccolorOptionsTable,readColorEntry)
		READ_OPTIONS(KVI_NUM_ICCOLOR_OPTIONS,g_iccolorOptionsTable,readColorEntry)

		#undef READ_OPTIONS
		#undef READ_ALL_OPTIONS
		KVI_OPTION_STRING(KviOption_stringIconThemeSubdir).trimmed();


		// the pixmap options need special processing
		for(i=0;i<KVI_NUM_PIXMAP_OPTIONS;i++)
		{
			if(g_pixmapOptionsTable[i].flags & KviOption_groupTheme)
			{
				if(cfg.hasKey(g_pixmapOptionsTable[i].name))
				{
					QString szVal = cfg.readQStringEntry(g_pixmapOptionsTable[i].name,"");
					szVal.trimmed();
					QString szBuffer;
					if(!szVal.isEmpty())
					{
						g_pApp->findImage(szBuffer,szVal);
					} else {
						szBuffer = szVal;
					}

					KviStringConversion::fromString(szBuffer,g_pixmapOptionsTable[i].option);

					// reset anyway
					iResetFlags |= g_pixmapOptionsTable[i].flags & KviOption_resetMask;
				}
			}
		}

		// create the splash screen pointer if this theme has some pixmaps in it
		if(!KVI_OPTION_STRING(KviOption_stringIconThemeSubdir).isEmpty())
			KviFileUtils::writeFile(szPointerFile,KVI_OPTION_STRING(KviOption_stringIconThemeSubdir));

		// force reloading of images anyway
		g_pApp->optionResetUpdate(iResetFlags | KviOption_resetReloadImages);

		return true;
	}
};

void KviApp::listAvailableOptions(KviWindow *wnd)
{
	int i;

	#define LIST_OPTIONS(__txt,__num,__table) \
		wnd->output(KVI_OUT_SYSTEMMESSAGE,"%c%s%c",KVI_TEXT_BOLD,__txt,KVI_TEXT_BOLD); \
		for(i = 0;i < __num;i++)wnd->outputNoFmt(KVI_OUT_SYSTEMMESSAGE,__table[i].name);

	LIST_OPTIONS(__tr("Boolean options (1/0)"),KVI_NUM_BOOL_OPTIONS,g_boolOptionsTable)
	LIST_OPTIONS(__tr("Rectangle options (x,y,width,height)"),KVI_NUM_RECT_OPTIONS,g_rectOptionsTable)
	LIST_OPTIONS(__tr("String options (string value)"),KVI_NUM_STRING_OPTIONS,g_stringOptionsTable)
	LIST_OPTIONS(__tr("Color options (#RRGGBB)"),KVI_NUM_COLOR_OPTIONS,g_colorOptionsTable)
	LIST_OPTIONS(__tr("Mirccolor options (#RRGGBB)"),KVI_NUM_MIRCCOLOR_OPTIONS,g_mirccolorOptionsTable)
	LIST_OPTIONS(__tr("Irc context color options (#RRGGBB)"),KVI_NUM_ICCOLOR_OPTIONS,g_iccolorOptionsTable)
	LIST_OPTIONS(__tr("Pixmap options (image path)"),KVI_NUM_PIXMAP_OPTIONS,g_pixmapOptionsTable)
	LIST_OPTIONS(__tr("Unsigned integer options (unsigned integer constant)"),KVI_NUM_UINT_OPTIONS,g_uintOptionsTable)
	LIST_OPTIONS(__tr("Font options (family,pointsize,style,charset,weight,flags (biusf))"),KVI_NUM_FONT_OPTIONS,g_fontOptionsTable)
	LIST_OPTIONS(__tr("Message color options (image_id,foreground,background,logFlag (0/1))"),KVI_NUM_MSGTYPE_OPTIONS,g_msgtypeOptionsTable)
	LIST_OPTIONS(__tr("String list options (comma separated list of strings)"),KVI_NUM_STRINGLIST_OPTIONS,g_stringlistOptionsTable)

	#undef LIST_OPTIONS
}

//using namespace KviStringConversion;
bool KviApp::getOptionString(const QString &optName,QString &buffer)
{

	#define GET_OPTION_STRING(__numOpt,__table,__prefix,__prefixLen) \
		if(KviQString::equalCIN(optName,__prefix,__prefixLen)) \
		{ \
			for(int i=0;i < __numOpt;i++) \
			{ \
				if(KviQString::equalCI(optName,__table[i].name)) \
				{ \
					KviStringConversion::toString(__table[i].option,buffer); \
					return true; \
				} \
			} \
			return false; \
		}

	// WARNING : stringlist prefix must go BEFORE the string prefix (otherwise it will match)

	GET_OPTION_STRING(KVI_NUM_BOOL_OPTIONS,g_boolOptionsTable,KVI_BOOL_OPTIONS_PREFIX,KVI_BOOL_OPTIONS_PREFIX_LEN)
	GET_OPTION_STRING(KVI_NUM_RECT_OPTIONS,g_rectOptionsTable,KVI_BOOL_OPTIONS_PREFIX,KVI_BOOL_OPTIONS_PREFIX_LEN)
	GET_OPTION_STRING(KVI_NUM_STRINGLIST_OPTIONS,g_stringlistOptionsTable,KVI_STRINGLIST_OPTIONS_PREFIX,KVI_STRINGLIST_OPTIONS_PREFIX_LEN)
	GET_OPTION_STRING(KVI_NUM_STRING_OPTIONS,g_stringOptionsTable,KVI_STRING_OPTIONS_PREFIX,KVI_STRING_OPTIONS_PREFIX_LEN)
	GET_OPTION_STRING(KVI_NUM_COLOR_OPTIONS,g_colorOptionsTable,KVI_COLOR_OPTIONS_PREFIX,KVI_COLOR_OPTIONS_PREFIX_LEN)
	GET_OPTION_STRING(KVI_NUM_MIRCCOLOR_OPTIONS,g_mirccolorOptionsTable,KVI_MIRCCOLOR_OPTIONS_PREFIX,KVI_MIRCCOLOR_OPTIONS_PREFIX_LEN)
	GET_OPTION_STRING(KVI_NUM_ICCOLOR_OPTIONS,g_iccolorOptionsTable,KVI_ICCOLOR_OPTIONS_PREFIX,KVI_ICCOLOR_OPTIONS_PREFIX_LEN)
	GET_OPTION_STRING(KVI_NUM_PIXMAP_OPTIONS,g_pixmapOptionsTable,KVI_PIXMAP_OPTIONS_PREFIX,KVI_PIXMAP_OPTIONS_PREFIX_LEN)
	GET_OPTION_STRING(KVI_NUM_UINT_OPTIONS,g_uintOptionsTable,KVI_UINT_OPTIONS_PREFIX,KVI_UINT_OPTIONS_PREFIX_LEN)
	GET_OPTION_STRING(KVI_NUM_FONT_OPTIONS,g_fontOptionsTable,KVI_FONT_OPTIONS_PREFIX,KVI_FONT_OPTIONS_PREFIX_LEN)
	GET_OPTION_STRING(KVI_NUM_MSGTYPE_OPTIONS,g_msgtypeOptionsTable,KVI_MSGTYPE_OPTIONS_PREFIX,KVI_MSGTYPE_OPTIONS_PREFIX_LEN)
	return false;

	#undef GET_OPTION_STRING
}

void KviApp::optionResetUpdate(int flags)
{
	if(flags & KviOption_resetReloadImages)
	{
		g_pIconManager->reloadImages();
		emit reloadImages();
	}

	if(flags & KviOption_resetUpdateAppFont)
	{
		updateApplicationFont();
	}

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	if(flags & KviOption_resetUpdatePseudoTransparency)
	{
		triggerUpdatePseudoTransparency();
	}
#endif

	if(flags & KviOption_resetRestartIdentd)
	{
		if(g_iIdentDaemonRunningUsers)
			g_pFrame->executeInternalCommand(KVI_INTERNALCOMMAND_IDENT_STOP);

		if(KVI_OPTION_BOOL(KviOption_boolUseIdentService) && !KVI_OPTION_BOOL(KviOption_boolUseIdentServiceOnlyOnConnect))
			g_pFrame->executeInternalCommand(KVI_INTERNALCOMMAND_IDENT_START);
	}

	if(flags & KviOption_resetUpdateGui)
	{
		triggerUpdateGui();
	}

	if(flags & KviOption_resetUpdateWindowList)
	{
		g_pFrame->recreateWindowList();
	}

	if(flags & KviOption_resetRestartNotifyList)
	{
		g_pApp->restartNotifyLists();
	}

	if(flags & KviOption_resetRestartLagMeter)
	{
		g_pApp->restartLagMeters();
	}

	if(flags & KviOption_resetRecentChannels)
	{
		g_pApp->buildRecentChannels();
	}

	if(flags & KviOption_resetUpdateNotifier)
	{
		emit updateNotifier();
	}
}
bool KviApp::setOptionValue(const QString &optName,const QString &value)
{
	if (!setCommonOptionValue(optName,value))
		return false;

	if(KviQString::equalCI(optName,"stringlistRecentChannels"))
	{
		buildRecentChannels();
		return true;
	}

	// The pixmap options have special treating
	if(KviQString::equalCIN(optName,KVI_PIXMAP_OPTIONS_PREFIX,KVI_PIXMAP_OPTIONS_PREFIX_LEN))
	{
		// We lookup the image path (so we allow also relative paths for this option type)
		QString szVal = value;
		szVal.trimmed();
		QString szBuffer;
		if(!szVal.isEmpty())
		{
			findImage(szBuffer,szVal);
		} else {
			szBuffer = szVal;
		}

		for(int i=0;i < KVI_NUM_PIXMAP_OPTIONS;i++)
		{
			if(KviQString::equalCI(optName,g_pixmapOptionsTable[i].name))
			{
				if(!KviStringConversion::fromString(szBuffer,g_pixmapOptionsTable[i].option))return false;
				optionResetUpdate(g_pixmapOptionsTable[i].flags);
				return true;
			}
		}
		return false;
	}

	return true;
}

bool KviApp::setCommonOptionValue(const QString &optName,const QString &value)
{

	#define SET_OPTION_VALUE(__numOpt,__table,__prefix,__prefixLen) \
		if(KviQString::equalCIN(optName,__prefix,__prefixLen)) \
		{ \
			for(int i=0;i < __numOpt;i++) \
			{ \
				if(KviQString::equalCI(optName,__table[i].name)) \
				{ \
					if(!KviStringConversion::fromString(value,__table[i].option))return false; \
					optionResetUpdate(__table[i].flags); \
					return true; \
				} \
			} \
			return false; \
		}

	SET_OPTION_VALUE(KVI_NUM_BOOL_OPTIONS,g_boolOptionsTable,KVI_BOOL_OPTIONS_PREFIX,KVI_BOOL_OPTIONS_PREFIX_LEN)
	SET_OPTION_VALUE(KVI_NUM_RECT_OPTIONS,g_rectOptionsTable,KVI_BOOL_OPTIONS_PREFIX,KVI_BOOL_OPTIONS_PREFIX_LEN)
	SET_OPTION_VALUE(KVI_NUM_STRINGLIST_OPTIONS,g_stringlistOptionsTable,KVI_STRINGLIST_OPTIONS_PREFIX,KVI_STRINGLIST_OPTIONS_PREFIX_LEN)
	SET_OPTION_VALUE(KVI_NUM_STRING_OPTIONS,g_stringOptionsTable,KVI_STRING_OPTIONS_PREFIX,KVI_STRING_OPTIONS_PREFIX_LEN)
	SET_OPTION_VALUE(KVI_NUM_COLOR_OPTIONS,g_colorOptionsTable,KVI_COLOR_OPTIONS_PREFIX,KVI_COLOR_OPTIONS_PREFIX_LEN)
	SET_OPTION_VALUE(KVI_NUM_MIRCCOLOR_OPTIONS,g_mirccolorOptionsTable,KVI_MIRCCOLOR_OPTIONS_PREFIX,KVI_MIRCCOLOR_OPTIONS_PREFIX_LEN)
	SET_OPTION_VALUE(KVI_NUM_ICCOLOR_OPTIONS,g_iccolorOptionsTable,KVI_ICCOLOR_OPTIONS_PREFIX,KVI_ICCOLOR_OPTIONS_PREFIX_LEN)
//	SET_OPTION_VALUE(KVI_NUM_PIXMAP_OPTIONS,g_pixmapOptionsTable,KVI_PIXMAP_OPTIONS_PREFIX,KVI_PIXMAP_OPTIONS_PREFIX_LEN)
	SET_OPTION_VALUE(KVI_NUM_UINT_OPTIONS,g_uintOptionsTable,KVI_UINT_OPTIONS_PREFIX,KVI_UINT_OPTIONS_PREFIX_LEN)
	SET_OPTION_VALUE(KVI_NUM_FONT_OPTIONS,g_fontOptionsTable,KVI_FONT_OPTIONS_PREFIX,KVI_FONT_OPTIONS_PREFIX_LEN)
	SET_OPTION_VALUE(KVI_NUM_MSGTYPE_OPTIONS,g_msgtypeOptionsTable,KVI_MSGTYPE_OPTIONS_PREFIX,KVI_MSGTYPE_OPTIONS_PREFIX_LEN)

	#undef SET_OPTION_VALUE
	return true;
}
