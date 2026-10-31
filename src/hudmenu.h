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
#include "customhudmanager/ICustomhudManager.h"

#include <cstdint>
#include <string>
#include <vector>

class CCSCustomHudLayout;
class CCSPlayerController;

namespace hudmenu
{
    /// A HudMenu on the plugin's menu layout: six rows, the navigation
    /// buttons, per-player state on one entity. Built by HudManager::OpenMenu
    /// and handed to the core (ISource2ModMenus::OpenMenu), which feeds it the
    /// keys and destroys it -- through Destroy(), so here.
    ///
    /// The options work as on the center HTML menu: per-player text, disabled
    /// and visible evaluators, hidden options, text lines (no number, never
    /// picked), submenus (Back returns), CloseOnSelect, StayOnPage, the
    /// PostSelectAction, the timeout and OnMenuEnd with the reason. A page has
    /// six rows whatever the menu's ItemsPerPage: the layout has six.
    class HudMenuInstance final : public IMenuInstance
    {
    public:
        static constexpr int kRows = 6;

        HudMenuInstance(CCSPlayerController* player, HudMenu* menu);
        ~HudMenuInstance() override;

        void Display() override;
        void OnKeyPress(CCSPlayerController* player, int key) override;
        void Close() override;
        void Reset() override;

        /// Once a frame. A HUD menu does not fade, so it is not redrawn every
        /// frame like the center HTML one; this runs the evaluators and
        /// redraws only when what would be drawn changed -- which also covers
        /// a menu rebuilt in place from one of its handlers, and the player
        /// dying (CaptureWhenDead). Ends the menu when its timeout ran out.
        void OnFrame() override;

        /// The keys are dropped only in the mode the player chose the mouse
        /// for. A dead player's temporary cursor keeps them.
        bool AcceptsKeys() const override { return !hudMenu_->CaptureInput; }

        /// The keys the page shown takes: its numbered rows, Back, Next, Close.
        uint32_t KeyMask() const override { return keyMask_; }

        /// Calls the root menu's OnMenuEnd, with the reason this instance
        /// ended for (Selected, Exit, Timeout) when it closed itself.
        void OnEnd(MenuEndReason reason) override;

        /// A click on the menu layout for this player: the button id becomes
        /// the key the row or navigation button stands for.
        void OnClick(const char* buttonId);

    protected:
        int NumPerPage() const override { return kRows; }

    private:
        struct Level
        {
            IBaseMenu* menu;
            int page;
        };

        struct Row
        {
            int index = -1;  // in the level's Options()
            int key = 0;     // 1-6, 0 for a text line
            bool enabled = false;
            bool submenu = false;
            std::string text;
            std::string sub;
        };

        /// One page as it would be drawn, with every evaluator run.
        struct Page
        {
            std::string title;
            std::vector<Row> rows;
            int page = 0;
            int pages = 1;
            bool back = false;
            bool next = false;
            bool close = false;
        };

        IBaseMenu* Current() const { return path_.back().menu; }

        /// Runs the evaluators for the current page. False when one of them
        /// closed this menu (this instance is gone then: touch nothing).
        bool Build(Page& page);

        void Draw(const Page& page);

        /// Closes the menu, reporting @p reason to OnMenuEnd.
        void End(MenuEndReason reason);

        /// Whether the layout takes the mouse right now: the menu's own
        /// setting, or the player being dead with CaptureWhenDead.
        bool EffectiveCapture() const;

        /// Hides the layout for the player; the destructor's job, so a
        /// closed, replaced or unloaded menu never stays on screen.
        void Hide();

        /// What Draw() would put on screen, hashed.
        size_t Signature(const Page& page) const;

        int Position() const { return static_cast<int>(hudMenu_->Position); }

        HudMenu* hudMenu_;
        std::vector<Level> path_;
        uint64_t serial_;
        int slot_;
        float openedAt_;
        size_t drawn_ = 0;
        /// The pos-* class last put on menu_root for this player; -1 for none yet.
        int drawnPos_ = -1;
        /// The key of each row drawn, 0 for a text line or an empty row.
        int rowKeys_[kRows] = {};
        uint32_t keyMask_ = 0;
        MenuEndReason endReason_ = MenuEndReason::Closed;
    };

    /// Opens `menu` for the player through the core's menu system.
    void Open(ISource2ModPlugin* owner, CCSPlayerController* player, HudMenu* menu);

    /// The click callback of the menu layout: routes to the player's instance.
    void OnLayoutClick(CCSPlayerController* player, const char* buttonId);

    /// Whether the menu opened under this serial is still the one the player
    /// has open -- after an option handler ran, which may have closed it.
    bool IsOpen(int slot, uint64_t serial);

    /// Closes every HUD menu that is open: the plugin is unloading and the
    /// instances' code goes with it.
    void CloseAll();
}
