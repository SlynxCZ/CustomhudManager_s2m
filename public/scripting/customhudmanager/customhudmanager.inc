/**
 * The Panorama HUD for scripts (ICustomhudManager). Everything is per player, and
 * nothing shows without the addon with the two layouts (see the plugin's README).
 */

#if defined _customhudmanager_included
 #endinput
#endif
#define _customhudmanager_included

#include <source2mod/types>
#include <source2mod/schema>

enum HudSlot
{
	HudSlot_Top = 0,
	HudSlot_TopLeft,
	HudSlot_TopRight,
	HudSlot_Left,
	HudSlot_Right,
	HudSlot_Center,
	HudSlot_Bottom,
	HudSlot_Panel
}

enum HudColor
{
	HudColor_White = 0,
	HudColor_Red,
	HudColor_Green,
	HudColor_Blue,
	HudColor_Yellow,
	HudColor_Orange,
	HudColor_Purple,
	HudColor_Cyan,
	HudColor_Grey,
	HudColor_Gold
}

enum HudSize
{
	HudSize_Small = 0,
	HudSize_Normal,
	HudSize_Large,
	HudSize_Huge
}

enum HudToastStyle
{
	HudToast_Info = 0,
	HudToast_Success,
	HudToast_Warning,
	HudToast_Danger,
	HudToast_Neutral
}

enum HudOverlay
{
	HudOverlay_Poison = 0,
	HudOverlay_Burn,
	HudOverlay_Freeze,
	HudOverlay_Heal,
	HudOverlay_Blind,
	HudOverlay_Black,
	HudOverlay_Blood
}

enum HudMenuPosition
{
	HudMenu_Left = 0,
	HudMenu_Center,
	HudMenu_Right
}

methodmap CustomHud
{
	public static native bool IsAvailable();
	/** @return Bytes written */
	public static native int LayoutName(char[] buffer, int maxlen);

	public static native void ShowText(CPlayerSlot slot, HudSlot hudSlot, const char[] text, float seconds, HudColor color = HudColor_White, HudSize size = HudSize_Normal, int offsetX = 0, int offsetY = 0);
	public static native void HideText(CPlayerSlot slot, HudSlot hudSlot);
	public static native void ShowPrompt(CPlayerSlot slot, const char[] key, const char[] text, float progress = -1.0);
	public static native void HidePrompt(CPlayerSlot slot);
	public static native void HideAll(CPlayerSlot slot);
	public static native void ShowToast(CPlayerSlot slot, HudToastStyle style, const char[] title, const char[] text, float seconds);
	public static native void ClearToasts(CPlayerSlot slot);
	public static native void ShowAnnounce(CPlayerSlot slot, const char[] title, const char[] subtitle, float seconds, HudColor color = HudColor_White);
	public static native void HideAnnounce(CPlayerSlot slot);
	public static native void ShowCountdown(CPlayerSlot slot, const char[] text, float seconds, HudColor color = HudColor_White);
	public static native void HideCountdown(CPlayerSlot slot);
	public static native void ShowStatus(CPlayerSlot slot, int chip, const char[] label, const char[] value, HudColor color = HudColor_White);
	public static native void HideStatus(CPlayerSlot slot, int chip);
	public static native void ShowProgress(CPlayerSlot slot, const char[] label, const char[] value, float progress, HudColor color = HudColor_White);
	public static native void HideProgress(CPlayerSlot slot);
	public static native void ShowHit(CPlayerSlot slot, int damage, bool headshot, bool kill);
	public static native void AddFeed(CPlayerSlot slot, HudToastStyle style, const char[] time, const char[] text, float seconds);
	public static native void ClearFeed(CPlayerSlot slot);
	public static native void ShowOverlay(CPlayerSlot slot, HudOverlay overlay, const char[] text, float seconds);
	public static native void HideOverlay(CPlayerSlot slot);
	public static native void ShowTimer(CPlayerSlot slot, const char[] tag, const char[] time, const char[] sub, HudColor color = HudColor_White);
	public static native void HideTimer(CPlayerSlot slot);
	public static native void ShowCard(CPlayerSlot slot, const char[] tag, const char[] title, const char[] sub, HudColor color = HudColor_White);
	public static native void HideCard(CPlayerSlot slot);
	public static native void HideReveal(CPlayerSlot slot);
	public static native void HideStats(CPlayerSlot slot);
	public static native void HideTracker(CPlayerSlot slot);

	/** seconds 0 or less: until HideReveal. Texts may be "". */
	public static native void ShowReveal(CPlayerSlot slot, const char[] tag, const char[] title, const char[] sub, const char[] info, const char[] extraLabel, const char[] extra, HudColor color = HudColor_White, float seconds = 6.0);

	/** A row (0-4) of the next ShowStats; rows not set stay hidden. */
	public static native void SetStatRow(int row, const char[] label, const char[] value, HudColor color = HudColor_White);
	/** bar 0 to 1 draws it, below 0 hides it; an empty foot label hides that half of the footer. */
	public static native void ShowStats(CPlayerSlot slot, const char[] title, HudColor color = HudColor_White, float bar = -1.0, HudColor barColor = HudColor_Green, const char[] footLeftLabel = "", const char[] footLeftValue = "", HudColor footLeftColor = HudColor_Blue, const char[] footRightLabel = "", const char[] footRightValue = "", HudColor footRightColor = HudColor_Orange);

	/** A row (0-3) of the next ShowTracker; progress below 0 hides its bar. */
	public static native void SetTrackerRow(int row, const char[] title, const char[] sub = "", const char[] value = "", float progress = -1.0, bool done = false);
	public static native void ShowTracker(CPlayerSlot slot, const char[] title, const char[] tag = "", const char[] footer = "", HudColor color = HudColor_Gold);
}

typedef CustomHudMenuHandler = function void (CCSPlayerController player, CustomHudMenu menu, int option, any data);

/** A menu drawn on the HUD through the core's menu system. Deleting it closes it for everyone on the next frame. */
methodmap CustomHudMenu < Handle
{
	public native CustomHudMenu(const char[] title);
	/** @return The option's index */
	public native int AddOption(const char[] text, CustomHudMenuHandler handler, any data = 0, bool disabled = false);
	public native void Open(CPlayerSlot slot);

	property HudMenuPosition Position
	{
		public native get();
		public native set(HudMenuPosition position);
	}
	property bool Grid
	{
		public native get();
		public native set(bool grid);
	}
	property bool DimBackground
	{
		public native get();
		public native set(bool dim);
	}
	property bool CaptureInput
	{
		public native get();
		public native set(bool capture);
	}
	property bool CaptureWhenDead
	{
		public native get();
		public native set(bool capture);
	}
}

public SharedPlugin __pl_CustomHud =
{
	name = "CustomHud",
	file = "customhud_manager",
#if defined REQUIRE_PLUGIN
	required = 1,
#else
	required = 0,
#endif
};
