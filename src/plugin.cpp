/**
 * vim: set ts=4 sw=4 tw=99 noet:
 * =============================================================================
 * CustomhudManager_s2m
 * Copyright (C) 2026 Michal "Slynx (˙·٠● S l y n x ●٠·˙)" Přikryl.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, Michal "Slynx (˙·٠● S l y n x ●٠·˙)" Přikryl gives
 * you permission to link the code of this program (as well as its derivative
 * works) to "Counter-Strike 2," "Source 2," "Steam," and any Game MODs or
 * server software running on software by Valve Corporation. You must obey the
 * GNU General Public License in all respects for all other code used.
 *
 * Additionally, this exception applies to all derivative works unless
 * otherwise stated in LICENSE.txt.
 *
 * Authors:
 *   - Michal "Slynx (˙·٠● S l y n x ●٠·˙)" Přikryl
 *
 * Project: CustomhudManager_s2m
 */
#include "plugin.h"
#include "version_gen.h"

#include "hud.h"
#include "hudmenu.h"

#include <cstring>


HudPlugin g_Plugin;
SOURCE2MOD_EXPOSE(PLUGIN_NAME, g_Plugin);

bool HudPlugin::Load(PluginId id, ISource2ModAPI* api, char* error, size_t maxlen, bool late)
{
    SOURCE2MOD_SAVEVARS();

    if (!g_pSource2ModCustomHud || !g_pSource2ModMenus || !g_pSource2ModEntities || !g_pSource2ModHooks)
    {
        if (error && maxlen)
            g_pSource2ModAPI->Format(error, maxlen, "the core is missing an interface this plugin needs (custom HUD, menus, entities, hooks): update Source2Mod");
        return false;
    }

    // OnLevelShutdown, OnSource2ModQuery.
    api->AddListener(this, this);

    // After the game's frame; before the engine lets a player go. Dropped by
    // the core when this plugin unloads.
    g_pSource2ModHooks->Hook(HookPoint::GameFrame, HookTarget::All(), { this, &HudPlugin::OnGameFrame }, true);
    g_pSource2ModHooks->Hook(HookPoint::ClientDisconnect, HookTarget::All(), { this, &HudPlugin::OnClientDisconnect });

    hud::hudManager.Init();

    return true;
}

bool HudPlugin::Unload(char* error, size_t maxlen)
{
    // The instances' code is in this library: nothing of them may stay open.
    hudmenu::CloseAll();
    hud::hudManager.Clear();
    return true;
}

void* HudPlugin::OnSource2ModQuery(const char* iface, int* ret)
{
    if (!iface)
        return nullptr;

    if (!strcmp(iface, CUSTOMHUD_MANAGER_INTERFACE_VERSION))
    {
        if (ret) *ret = SOURCE2MOD_IFACE_OK;
        return static_cast<ICustomhudManager*>(&hud::hudManager);
    }

    if (ret) *ret = SOURCE2MOD_IFACE_FAILED;

    return nullptr;
}

HookReturn<void> HudPlugin::OnGameFrame(GameFrameContext& ctx, bool post)
{
    hud::hudManager.Tick();
    return Action::Ignore;
}

void HudPlugin::OnLevelShutdown()
{
    // The layouts do not survive the level change.
    hud::hudManager.Clear();
}

HookReturn<void> HudPlugin::OnClientDisconnect(ClientDisconnectContext& ctx, bool post)
{
    hud::hudManager.OnClientDisconnect(ctx.slot);
    return Action::Ignore;
}

const char* HudPlugin::GetAuthor()
{
    return PLUGIN_AUTHOR;
}

const char* HudPlugin::GetName()
{
    return PLUGIN_DISPLAY_NAME;
}

const char* HudPlugin::GetDescription()
{
    return PLUGIN_DESCRIPTION;
}

const char* HudPlugin::GetVersion()
{
    return PLUGIN_FULL_VERSION;
}
