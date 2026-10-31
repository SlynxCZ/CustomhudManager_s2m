/*
 * SourcePawn natives of customhudmanager.inc: ICustomhudManager, and HudMenu behind a handle.
 */
#include "plugin.h"
#include "source2mod/ISource2ModNatives.h"
#include "source2mod/sdk/schema/classes/CCSPlayerController.h"
#include "customhudmanager/ICustomhudManager.h"

#include <memory>
#include <string>

using namespace SourcePawn;

namespace
{
    const char* Str(IPluginContext* ctx, cell_t addr)
    {
        char* s = nullptr;
        return ctx->LocalToString(addr, &s) == SP_ERROR_NONE && s ? s : "";
    }

    ICustomhudManager* Hud()
    {
        return static_cast<ICustomhudManager*>(g_Plugin.OnSource2ModQuery(CUSTOMHUD_MANAGER_INTERFACE_VERSION, nullptr));
    }

    cell_t N_HideText(IPluginContext* ctx, const cell_t* params)
    {
        if (ICustomhudManager* hud = Hud())
            hud->HideText(CPlayerSlot(params[1]), static_cast<HudSlot>(params[2]));
        return 0;
    }

    cell_t N_ShowPrompt(IPluginContext* ctx, const cell_t* params)
    {
        if (ICustomhudManager* hud = Hud())
            hud->ShowPrompt(CPlayerSlot(params[1]), Str(ctx, params[2]), Str(ctx, params[3]), sp_ctof(params[4]));
        return 0;
    }

    cell_t N_HidePrompt(IPluginContext* ctx, const cell_t* params)
    {
        if (ICustomhudManager* hud = Hud())
            hud->HidePrompt(CPlayerSlot(params[1]));
        return 0;
    }

    cell_t N_HideAll(IPluginContext* ctx, const cell_t* params)
    {
        if (ICustomhudManager* hud = Hud())
            hud->HideAll(CPlayerSlot(params[1]));
        return 0;
    }

    cell_t N_ShowToast(IPluginContext* ctx, const cell_t* params)
    {
        if (ICustomhudManager* hud = Hud())
            hud->ShowToast(CPlayerSlot(params[1]), static_cast<HudToastStyle>(params[2]), Str(ctx, params[3]), Str(ctx, params[4]), sp_ctof(params[5]));
        return 0;
    }

    cell_t N_ClearToasts(IPluginContext* ctx, const cell_t* params)
    {
        if (ICustomhudManager* hud = Hud())
            hud->ClearToasts(CPlayerSlot(params[1]));
        return 0;
    }

    cell_t N_ShowAnnounce(IPluginContext* ctx, const cell_t* params)
    {
        if (ICustomhudManager* hud = Hud())
            hud->ShowAnnounce(CPlayerSlot(params[1]), Str(ctx, params[2]), Str(ctx, params[3]), sp_ctof(params[4]), static_cast<HudColor>(params[5]));
        return 0;
    }

    cell_t N_HideAnnounce(IPluginContext* ctx, const cell_t* params)
    {
        if (ICustomhudManager* hud = Hud())
            hud->HideAnnounce(CPlayerSlot(params[1]));
        return 0;
    }

    cell_t N_ShowCountdown(IPluginContext* ctx, const cell_t* params)
    {
        if (ICustomhudManager* hud = Hud())
            hud->ShowCountdown(CPlayerSlot(params[1]), Str(ctx, params[2]), sp_ctof(params[3]), static_cast<HudColor>(params[4]));
        return 0;
    }

    cell_t N_HideCountdown(IPluginContext* ctx, const cell_t* params)
    {
        if (ICustomhudManager* hud = Hud())
            hud->HideCountdown(CPlayerSlot(params[1]));
        return 0;
    }

    cell_t N_ShowStatus(IPluginContext* ctx, const cell_t* params)
    {
        if (ICustomhudManager* hud = Hud())
            hud->ShowStatus(CPlayerSlot(params[1]), params[2], Str(ctx, params[3]), Str(ctx, params[4]), static_cast<HudColor>(params[5]));
        return 0;
    }

    cell_t N_HideStatus(IPluginContext* ctx, const cell_t* params)
    {
        if (ICustomhudManager* hud = Hud())
            hud->HideStatus(CPlayerSlot(params[1]), params[2]);
        return 0;
    }

    cell_t N_ShowProgress(IPluginContext* ctx, const cell_t* params)
    {
        if (ICustomhudManager* hud = Hud())
            hud->ShowProgress(CPlayerSlot(params[1]), Str(ctx, params[2]), Str(ctx, params[3]), sp_ctof(params[4]), static_cast<HudColor>(params[5]));
        return 0;
    }

    cell_t N_HideProgress(IPluginContext* ctx, const cell_t* params)
    {
        if (ICustomhudManager* hud = Hud())
            hud->HideProgress(CPlayerSlot(params[1]));
        return 0;
    }

    cell_t N_ShowHit(IPluginContext* ctx, const cell_t* params)
    {
        if (ICustomhudManager* hud = Hud())
            hud->ShowHit(CPlayerSlot(params[1]), params[2], params[3] != 0, params[4] != 0);
        return 0;
    }

    cell_t N_AddFeed(IPluginContext* ctx, const cell_t* params)
    {
        if (ICustomhudManager* hud = Hud())
            hud->AddFeed(CPlayerSlot(params[1]), static_cast<HudToastStyle>(params[2]), Str(ctx, params[3]), Str(ctx, params[4]), sp_ctof(params[5]));
        return 0;
    }

    cell_t N_ClearFeed(IPluginContext* ctx, const cell_t* params)
    {
        if (ICustomhudManager* hud = Hud())
            hud->ClearFeed(CPlayerSlot(params[1]));
        return 0;
    }

    cell_t N_ShowOverlay(IPluginContext* ctx, const cell_t* params)
    {
        if (ICustomhudManager* hud = Hud())
            hud->ShowOverlay(CPlayerSlot(params[1]), static_cast<HudOverlay>(params[2]), Str(ctx, params[3]), sp_ctof(params[4]));
        return 0;
    }

    cell_t N_HideOverlay(IPluginContext* ctx, const cell_t* params)
    {
        if (ICustomhudManager* hud = Hud())
            hud->HideOverlay(CPlayerSlot(params[1]));
        return 0;
    }

    cell_t N_ShowTimer(IPluginContext* ctx, const cell_t* params)
    {
        if (ICustomhudManager* hud = Hud())
            hud->ShowTimer(CPlayerSlot(params[1]), Str(ctx, params[2]), Str(ctx, params[3]), Str(ctx, params[4]), static_cast<HudColor>(params[5]));
        return 0;
    }

    cell_t N_HideTimer(IPluginContext* ctx, const cell_t* params)
    {
        if (ICustomhudManager* hud = Hud())
            hud->HideTimer(CPlayerSlot(params[1]));
        return 0;
    }

    cell_t N_ShowCard(IPluginContext* ctx, const cell_t* params)
    {
        if (ICustomhudManager* hud = Hud())
            hud->ShowCard(CPlayerSlot(params[1]), Str(ctx, params[2]), Str(ctx, params[3]), Str(ctx, params[4]), static_cast<HudColor>(params[5]));
        return 0;
    }

    cell_t N_HideCard(IPluginContext* ctx, const cell_t* params)
    {
        if (ICustomhudManager* hud = Hud())
            hud->HideCard(CPlayerSlot(params[1]));
        return 0;
    }

    cell_t N_HideReveal(IPluginContext* ctx, const cell_t* params)
    {
        if (ICustomhudManager* hud = Hud())
            hud->HideReveal(CPlayerSlot(params[1]));
        return 0;
    }

    cell_t N_HideStats(IPluginContext* ctx, const cell_t* params)
    {
        if (ICustomhudManager* hud = Hud())
            hud->HideStats(CPlayerSlot(params[1]));
        return 0;
    }

    cell_t N_HideTracker(IPluginContext* ctx, const cell_t* params)
    {
        if (ICustomhudManager* hud = Hud())
            hud->HideTracker(CPlayerSlot(params[1]));
        return 0;
    }

    cell_t N_IsAvailable(IPluginContext*, const cell_t*)
    {
        ICustomhudManager* hud = Hud();
        return hud && hud->IsAvailable();
    }

    cell_t N_LayoutName(IPluginContext* ctx, const cell_t* params)
    {
        ICustomhudManager* hud = Hud();
        size_t written = 0;
        ctx->StringToLocalUTF8(params[1], params[2], hud ? hud->LayoutName() : "", &written);
        return static_cast<cell_t>(written);
    }

    // ShowText(slot, hudSlot, text, seconds, color, size, offsetX, offsetY)
    cell_t N_ShowText(IPluginContext* ctx, const cell_t* params)
    {
        ICustomhudManager* hud = Hud();
        if (!hud)
            return 0;
        HudTextStyle style;
        style.color = static_cast<HudColor>(params[5]);
        style.size = static_cast<HudSize>(params[6]);
        style.offsetX = params[7];
        style.offsetY = params[8];
        hud->ShowText(CPlayerSlot(params[1]), static_cast<HudSlot>(params[2]), Str(ctx, params[3]), sp_ctof(params[4]), style);
        return 0;
    }

    // ShowReveal(slot, tag, title, sub, info, extraLabel, extra, color, seconds)
    cell_t N_ShowReveal(IPluginContext* ctx, const cell_t* params)
    {
        ICustomhudManager* hud = Hud();
        if (!hud)
            return 0;
        HudReveal reveal;
        reveal.tag = Str(ctx, params[2]);
        reveal.title = Str(ctx, params[3]);
        reveal.sub = Str(ctx, params[4]);
        reveal.info = Str(ctx, params[5]);
        reveal.extraLabel = Str(ctx, params[6]);
        reveal.extra = Str(ctx, params[7]);
        reveal.color = static_cast<HudColor>(params[8]);
        reveal.seconds = sp_ctof(params[9]);
        hud->ShowReveal(CPlayerSlot(params[1]), reveal);
        return 0;
    }

    // What SetStatRow / SetTrackerRow gathered for the next ShowStats / ShowTracker.
    struct StatRow
    {
        std::string label, value;
        HudColor color = HudColor::White;
    };
    StatRow g_statRows[HudStats::kRows];

    struct TrackerRow
    {
        std::string title, sub, value;
        float progress = -1.0f;
        bool done = false;
    };
    TrackerRow g_trackerRows[HudTracker::kRows];

    cell_t N_SetStatRow(IPluginContext* ctx, const cell_t* params)
    {
        if (params[1] < 0 || params[1] >= HudStats::kRows)
            return ctx->ThrowNativeError("Row %d is not 0 to %d", params[1], HudStats::kRows - 1);
        StatRow& row = g_statRows[params[1]];
        row.label = Str(ctx, params[2]);
        row.value = Str(ctx, params[3]);
        row.color = static_cast<HudColor>(params[4]);
        return 0;
    }

    // ShowStats(slot, title, color, bar, barColor, footLeftLabel, footLeftValue, footLeftColor, footRightLabel, footRightValue, footRightColor)
    cell_t N_ShowStats(IPluginContext* ctx, const cell_t* params)
    {
        ICustomhudManager* hud = Hud();
        if (!hud)
            return 0;
        HudStats stats;
        stats.title = Str(ctx, params[2]);
        stats.color = static_cast<HudColor>(params[3]);
        for (int i = 0; i < HudStats::kRows; i++)
        {
            stats.rows[i].label = g_statRows[i].label.empty() ? nullptr : g_statRows[i].label.c_str();
            stats.rows[i].value = g_statRows[i].value.c_str();
            stats.rows[i].color = g_statRows[i].color;
        }
        stats.bar = sp_ctof(params[4]);
        stats.barColor = static_cast<HudColor>(params[5]);
        const char* leftLabel = Str(ctx, params[6]);
        stats.footLeftLabel = *leftLabel ? leftLabel : nullptr;
        stats.footLeftValue = Str(ctx, params[7]);
        stats.footLeftColor = static_cast<HudColor>(params[8]);
        const char* rightLabel = Str(ctx, params[9]);
        stats.footRightLabel = *rightLabel ? rightLabel : nullptr;
        stats.footRightValue = Str(ctx, params[10]);
        stats.footRightColor = static_cast<HudColor>(params[11]);
        hud->ShowStats(CPlayerSlot(params[1]), stats);
        for (StatRow& row : g_statRows)
            row = {};
        return 0;
    }

    cell_t N_SetTrackerRow(IPluginContext* ctx, const cell_t* params)
    {
        if (params[1] < 0 || params[1] >= HudTracker::kRows)
            return ctx->ThrowNativeError("Row %d is not 0 to %d", params[1], HudTracker::kRows - 1);
        TrackerRow& row = g_trackerRows[params[1]];
        row.title = Str(ctx, params[2]);
        row.sub = Str(ctx, params[3]);
        row.value = Str(ctx, params[4]);
        row.progress = sp_ctof(params[5]);
        row.done = params[6] != 0;
        return 0;
    }

    // ShowTracker(slot, title, tag, footer, color)
    cell_t N_ShowTracker(IPluginContext* ctx, const cell_t* params)
    {
        ICustomhudManager* hud = Hud();
        if (!hud)
            return 0;
        HudTracker tracker;
        tracker.title = Str(ctx, params[2]);
        tracker.tag = Str(ctx, params[3]);
        tracker.footer = Str(ctx, params[4]);
        tracker.color = static_cast<HudColor>(params[5]);
        for (int i = 0; i < HudTracker::kRows; i++)
        {
            tracker.rows[i].title = g_trackerRows[i].title.empty() ? nullptr : g_trackerRows[i].title.c_str();
            tracker.rows[i].sub = g_trackerRows[i].sub.c_str();
            tracker.rows[i].value = g_trackerRows[i].value.c_str();
            tracker.rows[i].progress = g_trackerRows[i].progress;
            tracker.rows[i].done = g_trackerRows[i].done;
        }
        hud->ShowTracker(CPlayerSlot(params[1]), tracker);
        for (TrackerRow& row : g_trackerRows)
            row = {};
        return 0;
    }

    // ---- HudMenu behind a handle ----

    struct ScriptHudMenu
    {
        explicit ScriptHudMenu(const char* title) : menu(title) {}
        HudMenu menu;
        cell_t handle = 0;
        std::shared_ptr<bool> alive = std::make_shared<bool>(true);
    };

    Source2ModHandleType MenuType()
    {
        static const Source2ModHandleType type = g_pSource2ModNatives->CreateHandleType("CustomHudMenu", [](void* p)
        {
            auto* m = static_cast<ScriptHudMenu*>(p);
            *m->alive = false;
            g_pSource2ModScheduler->NextFrame([m]()
            {
                g_pSource2ModMenus->CloseMenu(&m->menu);
                delete m;
            });
        });
        return type;
    }

    ScriptHudMenu* MenuOf(IPluginContext* ctx, cell_t handle)
    {
        return static_cast<ScriptHudMenu*>(g_pSource2ModNatives->ReadHandle(ctx, handle, MenuType()));
    }

    cell_t N_MenuNew(IPluginContext* ctx, const cell_t* params)
    {
        auto* m = new ScriptHudMenu(Str(ctx, params[1]));
        m->handle = g_pSource2ModNatives->CreateHandle(MenuType(), m, ctx);
        if (!m->handle)
            delete m;
        return m->handle;
    }

    // AddOption(text, handler, data, disabled)
    cell_t N_MenuAddOption(IPluginContext* ctx, const cell_t* params)
    {
        ScriptHudMenu* m = MenuOf(ctx, params[1]);
        IPluginFunction* fn = m ? ctx->GetFunctionById(static_cast<funcid_t>(params[3])) : nullptr;
        if (!fn)
            return -1;
        const cell_t option = static_cast<cell_t>(m->menu.Options().size());
        const cell_t data = params[4];
        const cell_t handle = m->handle;
        std::shared_ptr<bool> alive = m->alive;
        m->menu.AddMenuOption(Str(ctx, params[2]), [fn, option, data, handle, alive](CCSPlayerController* player, ChatMenuOption&)
        {
            if (!*alive)
                return;
            fn->PushCell(g_pSource2ModNatives->ScriptCellFromEntity(player));
            fn->PushCell(handle);
            fn->PushCell(option);
            fn->PushCell(data);
            fn->Execute(nullptr);
        }, params[5] != 0);
        return option;
    }

    // Open(slot)
    cell_t N_MenuOpen(IPluginContext* ctx, const cell_t* params)
    {
        ScriptHudMenu* m = MenuOf(ctx, params[1]);
        ICustomhudManager* hud = Hud();
        if (m && hud)
            hud->OpenMenu(&g_Plugin, CPlayerSlot(params[2]), &m->menu);
        return 0;
    }

#define S2M_MENU_PROP(Name, Field, Cast)                                        \
    cell_t N_Menu##Name##Get(IPluginContext* ctx, const cell_t* params)         \
    {                                                                           \
        ScriptHudMenu* m = MenuOf(ctx, params[1]);                              \
        return m ? static_cast<cell_t>(m->menu.Field) : 0;                      \
    }                                                                           \
    cell_t N_Menu##Name##Set(IPluginContext* ctx, const cell_t* params)         \
    {                                                                           \
        if (ScriptHudMenu* m = MenuOf(ctx, params[1]))                          \
            m->menu.Field = Cast(params[2]);                                    \
        return 0;                                                               \
    }
    S2M_MENU_PROP(Position, Position, static_cast<HudMenuPosition>)
    S2M_MENU_PROP(Grid, Grid, static_cast<bool>)
    S2M_MENU_PROP(DimBackground, DimBackground, static_cast<bool>)
    S2M_MENU_PROP(CaptureInput, CaptureInput, static_cast<bool>)
    S2M_MENU_PROP(CaptureWhenDead, CaptureWhenDead, static_cast<bool>)
#undef S2M_MENU_PROP

    const sp_nativeinfo_t g_natives[] = {
        { "CustomHud.IsAvailable", N_IsAvailable },
        { "CustomHud.LayoutName", N_LayoutName },
        { "CustomHud.ShowText", N_ShowText },
        { "CustomHud.ShowReveal", N_ShowReveal },
        { "CustomHud.SetStatRow", N_SetStatRow },
        { "CustomHud.ShowStats", N_ShowStats },
        { "CustomHud.SetTrackerRow", N_SetTrackerRow },
        { "CustomHud.ShowTracker", N_ShowTracker },
        { "CustomHud.HideText", N_HideText },
        { "CustomHud.ShowPrompt", N_ShowPrompt },
        { "CustomHud.HidePrompt", N_HidePrompt },
        { "CustomHud.HideAll", N_HideAll },
        { "CustomHud.ShowToast", N_ShowToast },
        { "CustomHud.ClearToasts", N_ClearToasts },
        { "CustomHud.ShowAnnounce", N_ShowAnnounce },
        { "CustomHud.HideAnnounce", N_HideAnnounce },
        { "CustomHud.ShowCountdown", N_ShowCountdown },
        { "CustomHud.HideCountdown", N_HideCountdown },
        { "CustomHud.ShowStatus", N_ShowStatus },
        { "CustomHud.HideStatus", N_HideStatus },
        { "CustomHud.ShowProgress", N_ShowProgress },
        { "CustomHud.HideProgress", N_HideProgress },
        { "CustomHud.ShowHit", N_ShowHit },
        { "CustomHud.AddFeed", N_AddFeed },
        { "CustomHud.ClearFeed", N_ClearFeed },
        { "CustomHud.ShowOverlay", N_ShowOverlay },
        { "CustomHud.HideOverlay", N_HideOverlay },
        { "CustomHud.ShowTimer", N_ShowTimer },
        { "CustomHud.HideTimer", N_HideTimer },
        { "CustomHud.ShowCard", N_ShowCard },
        { "CustomHud.HideCard", N_HideCard },
        { "CustomHud.HideReveal", N_HideReveal },
        { "CustomHud.HideStats", N_HideStats },
        { "CustomHud.HideTracker", N_HideTracker },
        { "CustomHudMenu.CustomHudMenu", N_MenuNew },
        { "CustomHudMenu.AddOption", N_MenuAddOption },
        { "CustomHudMenu.Open", N_MenuOpen },
        { "CustomHudMenu.Position.get", N_MenuPositionGet },
        { "CustomHudMenu.Position.set", N_MenuPositionSet },
        { "CustomHudMenu.Grid.get", N_MenuGridGet },
        { "CustomHudMenu.Grid.set", N_MenuGridSet },
        { "CustomHudMenu.DimBackground.get", N_MenuDimBackgroundGet },
        { "CustomHudMenu.DimBackground.set", N_MenuDimBackgroundSet },
        { "CustomHudMenu.CaptureInput.get", N_MenuCaptureInputGet },
        { "CustomHudMenu.CaptureInput.set", N_MenuCaptureInputSet },
        { "CustomHudMenu.CaptureWhenDead.get", N_MenuCaptureWhenDeadGet },
        { "CustomHudMenu.CaptureWhenDead.set", N_MenuCaptureWhenDeadSet },
        { nullptr, nullptr },
    };
}

void RegisterCustomHudNatives()
{
    g_pSource2ModNatives->AddNatives(g_natives);
    g_pSource2ModNatives->RegisterLibrary("CustomHud");
}
