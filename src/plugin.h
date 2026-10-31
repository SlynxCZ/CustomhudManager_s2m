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
#pragma once

#include <source2mod/ISource2ModApi.h>
#include <source2mod/ISource2ModPlugin.h>

// SOURCE2MOD_SAVEVARS() fills one global per subsystem, so every subsystem header
// has to be in scope for its interface name.
#include "source2mod/ISource2ModAddresses.h"
#include "source2mod/ISource2ModCommands.h"
#include "source2mod/ISource2ModConVars.h"
#include "source2mod/ISource2ModCustomHud.h"
#include "source2mod/ISource2ModEntities.h"
#include "source2mod/ISource2ModEvents.h"
#include "source2mod/ISource2ModGameConfig.h"
#include "source2mod/ISource2ModGameSystems.h"
#include "source2mod/ISource2ModHTTP.h"
#include "source2mod/ISource2ModJSON.h"
#include "source2mod/ISource2ModMemory.h"
#include "source2mod/ISource2ModMenus.h"
#include "customhudmanager/ICustomhudManager.h"
#include "source2mod/ISource2ModModule.h"
#include "source2mod/ISource2ModSQL.h"
#include "source2mod/ISource2ModNetworkMessages.h"
#include "source2mod/ISource2ModScheduler.h"
#include "source2mod/ISource2ModSounds.h"
#include "source2mod/ISource2ModTrace.h"

SOURCE2MOD_GLOBALVARS();

/**
 * The plugin that serves ICustomhudManager: the on-screen texts and elements on
 * the s2t_hud layout, and HudMenu on the s2t_menu layout through the core's
 * menu system. Other plugins fetch the interface with GET_SOURCE2MOD_IFACE in
 * OnAllSource2ModPluginsLoaded().
 */
class HudPlugin final : public ISource2ModPlugin,
                        public ISource2ModListener
{
public:
    bool Load(ISource2ModAPI* api, char* error, size_t maxlen, bool late) override;
    bool Unload(char* error, size_t maxlen) override;

    // ISource2ModListener
    void* OnSource2ModQuery(const char* iface, int* ret) override;
    void OnLevelShutdown() override;

    // ISource2ModHooks
    HookReturn<void> OnGameFrame(GameFrameContext& ctx, bool post);
    HookReturn<void> OnClientDisconnect(ClientDisconnectContext& ctx, bool post);

    const char* GetAuthor() override;
    const char* GetName() override;
    const char* GetDescription() override;
    const char* GetVersion() override;
};

extern HudPlugin g_Plugin;

void RegisterCustomHudNatives();
