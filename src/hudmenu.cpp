/**
 * vim: set ts=4 sw=4 tw=99 noet:
 * =============================================================================
 * CustomhudManager_s2m
 * Copyright (C) 2026 Michal "Slynx (˙·٠● S l y n x ●٠·˙)" Přikryl.
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
 * As a special exception, Michal "Slynx (˙·٠● S l y n x ●٠·˙)" Přikryl gives
 * you permission to link the code of this program (as well as its derivative
 * works) to "Counter-Strike 2," "Source 2," "Steam," and any Game MODs or
 * server software running on software by Valve Corporation. You must obey the
 * GNU General Public License in all respects for all other code used.
 *
 * Additionally, this exception applies to all derivative works unless
 * otherwise stated in LICENSE.txt.
 *
 * Authors:
 *   - Michal "Slynx (˙·٠● S l y n x ●٠·˙)" Přikryl
 *
 * Project: CustomhudManager_s2m
 */
#include "hudmenu.h"

#include "hud.h"
#include "plugin.h"

#include <source2mod/schema/entity/classes/CCSCustomHudLayout.h>
#include <source2mod/schema/entity/classes/CCSPlayerController.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <string>
#include <unordered_map>

namespace hudmenu
{
    namespace
    {
        constexpr const char* kRowPanel[HudMenuInstance::kRows] = {
            "menu_row_0", "menu_row_1", "menu_row_2", "menu_row_3", "menu_row_4", "menu_row_5",
        };
        constexpr const char* kRowKey[HudMenuInstance::kRows] = {
            "menu_row_0_key", "menu_row_1_key", "menu_row_2_key", "menu_row_3_key", "menu_row_4_key", "menu_row_5_key",
        };
        constexpr const char* kRowText[HudMenuInstance::kRows] = {
            "menu_row_0_text", "menu_row_1_text", "menu_row_2_text", "menu_row_3_text", "menu_row_4_text", "menu_row_5_text",
        };
        constexpr const char* kRowSub[HudMenuInstance::kRows] = {
            "menu_row_0_sub", "menu_row_1_sub", "menu_row_2_sub", "menu_row_3_sub", "menu_row_4_sub", "menu_row_5_sub",
        };
        constexpr const char* kShow = "show";
        constexpr const char* kDisabled = "disabled";

        /// The instance each slot has open, and its serial. The core owns the
        /// instances; this is only how a click and a handler's aftermath find
        /// theirs. An instance registers in its constructor and leaves in its
        /// destructor -- only if it is still the one registered, since the
        /// core builds the replacement before it destroys the old one.
        std::unordered_map<int, HudMenuInstance*> s_bySlot;
        std::unordered_map<int, uint64_t> s_serialBySlot;
        uint64_t s_nextSerial = 0;

        // The engine's game time, as the HUD elements use it (hud.cpp).
        float Now()
        {
            static float last = 0.0f;
            if (CGlobalVars* globals = g_pSource2ModAPI ? g_pSource2ModAPI->GetGlobalVars() : nullptr)
                last = globals->curtime;
            return last;
        }

        void HashIn(size_t& seed, size_t value)
        {
            seed ^= value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
        }
    }

    bool IsOpen(int slot, uint64_t serial)
    {
        auto it = s_serialBySlot.find(slot);
        return it != s_serialBySlot.end() && it->second == serial;
    }

    HudMenuInstance::HudMenuInstance(CCSPlayerController* player, HudMenu* menu)
        : IMenuInstance(player, menu), hudMenu_(menu), path_{ { menu, 0 } }, serial_(++s_nextSerial),
          slot_(player ? player->GetSlot() : -1), openedAt_(Now())
    {
        if (slot_ >= 0)
        {
            s_bySlot[slot_] = this;
            s_serialBySlot[slot_] = serial_;
        }
    }

    HudMenuInstance::~HudMenuInstance()
    {
        Hide();

        if (slot_ >= 0)
        {
            auto it = s_bySlot.find(slot_);
            if (it != s_bySlot.end() && it->second == this)
            {
                s_bySlot.erase(it);
                s_serialBySlot.erase(slot_);
            }
        }
    }

    bool HudMenuInstance::EffectiveCapture() const
    {
        if (hudMenu_->CaptureInput)
            return true;

        if (!hudMenu_->CaptureWhenDead)
            return false;

        CCSPlayerController* player = slot_ >= 0 ? CCSPlayerController::FromSlot(slot_) : nullptr;
        return player && !player->m_bPawnIsAlive();
    }

    void HudMenuInstance::Hide()
    {
        CCSCustomHudLayout* layout = hud::hudManager.MenuLayoutIfAny();
        CCSPlayerController* player = slot_ >= 0 ? CCSPlayerController::FromSlot(slot_) : nullptr;
        if (!layout || !player)
            return;

        layout->SetHasClass("menu_root", kShow, false, player);
        layout->SetHasClass("menu_dim", kShow, false, player);
        layout->SetInputCaptureEnabled(false, player);
    }

    void HudMenuInstance::Reset()
    {
        IMenuInstance::Reset();
        path_.resize(1);
        path_[0].page = 0;
    }

    bool HudMenuInstance::Build(Page& out)
    {
        // The evaluators are plugin code and may close this menu or open
        // another one for the player, which destroys this instance.
        CCSPlayerController* player = player_;
        const int slot = slot_;
        const uint64_t serial = serial_;
        IBaseMenu* menu = Current();

        out.title = menu->DisplayTitle(player);
        if (!IsOpen(slot, serial))
            return false;

        std::vector<int> visible;
        for (int i = 0; i < static_cast<int>(menu->Options().size()); ++i)
        {
            const ChatMenuOption& opt = menu->Options()[i];
            if (opt.Hidden)
                continue;
            if (opt.VisibleEvaluator)
            {
                const auto evaluator = opt.VisibleEvaluator;
                const bool show = evaluator(player);
                if (!IsOpen(slot, serial))
                    return false;
                if (!show)
                    continue;
            }
            visible.push_back(i);
        }

        const int total = static_cast<int>(visible.size());
        out.pages = total > 0 ? (total + kRows - 1) / kRows : 1;
        Level& level = path_.back();
        level.page = std::clamp(level.page, 0, out.pages - 1);
        out.page = level.page;

        int key = 0;
        const int first = level.page * kRows;
        for (int i = first; i < std::min(first + kRows, total); ++i)
        {
            const int index = visible[i];
            if (index >= static_cast<int>(menu->Options().size()))
                break;
            const ChatMenuOption opt = menu->Options()[index];

            Row row;
            row.index = index;
            row.submenu = opt.Submenu != nullptr;

            std::string text = opt.Text;
            if (opt.TextEvaluator)
            {
                text = opt.TextEvaluator(player);
                if (!IsOpen(slot, serial))
                    return false;
            }
            // "name\nsub": the sub part right of the row, or under the name
            // on a tile.
            const size_t nl = text.find('\n');
            row.text = nl == std::string::npos ? text : text.substr(0, nl);
            row.sub = nl == std::string::npos ? std::string() : text.substr(nl + 1);

            if (!opt.IsText)
            {
                row.key = ++key;
                row.enabled = !opt.Disabled;
                if (row.enabled && opt.DisabledEvaluator)
                {
                    const auto evaluator = opt.DisabledEvaluator;
                    row.enabled = !evaluator(player);
                    if (!IsOpen(slot, serial))
                        return false;
                }
            }
            out.rows.push_back(std::move(row));
        }

        out.back = level.page > 0 || path_.size() > 1;
        out.next = level.page < out.pages - 1;
        out.close = hudMenu_->GetExitButton();
        return true;
    }

    size_t HudMenuInstance::Signature(const Page& page) const
    {
        size_t h = std::hash<std::string>{}(page.title);
        HashIn(h, static_cast<size_t>(page.page));
        HashIn(h, static_cast<size_t>(page.pages));
        HashIn(h, path_.size());
        HashIn(h, page.rows.size());
        for (const Row& row : page.rows)
        {
            HashIn(h, std::hash<std::string>{}(row.text));
            HashIn(h, std::hash<std::string>{}(row.sub));
            HashIn(h, static_cast<size_t>(row.key));
            HashIn(h, row.enabled ? 1 : 0);
        }

        HashIn(h, page.back ? 1 : 0);
        HashIn(h, page.next ? 1 : 0);
        HashIn(h, page.close ? 1 : 0);
        HashIn(h, std::hash<std::string>{}(hudMenu_->PrevText));
        HashIn(h, std::hash<std::string>{}(hudMenu_->NextText));
        HashIn(h, std::hash<std::string>{}(hudMenu_->CloseText));
        HashIn(h, hudMenu_->DimBackground ? 1 : 0);
        HashIn(h, EffectiveCapture() ? 1 : 0);
        HashIn(h, static_cast<size_t>(Position()));
        HashIn(h, hudMenu_->Grid ? 1 : 0);
        return h;
    }

    void HudMenuInstance::Draw(const Page& page)
    {
        CCSCustomHudLayout* layout = hud::hudManager.MenuLayout();
        if (!layout)
            return;

        CCSPlayerController* player = player_;

        layout->SetDialogVariableString("menu_title", "text", page.title.c_str(), player);

        uint32_t keys = 0;
        for (int r = 0; r < kRows; ++r)
        {
            if (r < static_cast<int>(page.rows.size()))
            {
                const Row& row = page.rows[r];
                const std::string key = row.key ? std::to_string(row.key) : std::string();
                // A submenu gets a '>' after its name, as on the center menu.
                const std::string text = row.submenu ? row.text + " >" : row.text;
                layout->SetDialogVariableString(kRowKey[r], "text", key.c_str(), player);
                layout->SetDialogVariableString(kRowText[r], "text", text.c_str(), player);
                layout->SetDialogVariableString(kRowSub[r], "text", row.sub.c_str(), player);
                layout->SetHasClass(kRowPanel[r], "has-sub", !row.sub.empty(), player);
                layout->SetHasClass(kRowPanel[r], kDisabled, !row.enabled, player);
                layout->SetHasClass(kRowPanel[r], kShow, true, player);
                rowKeys_[r] = row.key;
                if (row.key)
                    keys |= 1u << row.key;
            }
            else
            {
                layout->SetHasClass(kRowPanel[r], kShow, false, player);
                rowKeys_[r] = 0;
            }
        }

        layout->SetDialogVariableString("menu_prev_text", "text", hudMenu_->PrevText.c_str(), player);
        layout->SetDialogVariableString("menu_next_text", "text", hudMenu_->NextText.c_str(), player);
        layout->SetDialogVariableString("menu_close_text", "text", hudMenu_->CloseText.c_str(), player);
        layout->SetHasClass("menu_prev", kShow, page.back, player);
        layout->SetHasClass("menu_next", kShow, page.next, player);
        layout->SetHasClass("menu_close", kShow, page.close, player);
        if (page.back)
            keys |= 1u << 7;
        if (page.next)
            keys |= 1u << 8;
        if (page.close)
            keys |= 1u << 9;
        keyMask_ = keys;

        if (page.pages > 1)
        {
            const std::string footer = std::to_string(page.page + 1) + " / " + std::to_string(page.pages);
            layout->SetDialogVariableString("menu_footer", "text", footer.c_str(), player);
        }
        layout->SetHasClass("menu_footer", kShow, page.pages > 1, player);

        layout->SetHasClass("menu_dim", kShow, hudMenu_->DimBackground, player);
        layout->SetHasClass("menu_root", "grid", hudMenu_->Grid, player);

        // Where the window sits: all three set, so a position left on the
        // slot by an earlier menu does not linger next to this one's.
        const int pos = Position();
        if (pos != drawnPos_)
        {
            static constexpr const char* kPosClass[] = { "pos-left", "pos-center", "pos-right" };
            for (int i = 0; i < 3; ++i)
                layout->SetHasClass("menu_root", kPosClass[i], i == pos, player);
            drawnPos_ = pos;
        }

        layout->SetHasClass("menu_root", kShow, true, player);
        layout->SetInputCaptureEnabled(EffectiveCapture(), player);

        drawn_ = Signature(page);
    }

    void HudMenuInstance::Display()
    {
        if (!player_ || !menu_) return;

        if (g_pSource2ModMenus->GetActiveMenu(player_) != this)
        {
            Reset();
            return;
        }

        Page page;
        if (Build(page))
            Draw(page);
    }

    void HudMenuInstance::OnFrame()
    {
        if (!player_ || !menu_) return;

        const int timeout = hudMenu_->GetTimeout();
        if (timeout > 0 && Now() - openedAt_ >= static_cast<float>(timeout))
        {
            End(MenuEndReason::Timeout);
            return;
        }

        Page page;
        if (!Build(page))
            return;
        if (Signature(page) != drawn_)
            Draw(page);
    }

    void HudMenuInstance::OnKeyPress(CCSPlayerController* p, int key)
    {
        if (p != player_) return;
        if (!menu_) return;

        CCSPlayerController* player = player_;
        const int slot = slot_;
        const uint64_t serial = serial_;

        Page page;
        if (!Build(page))
            return;
        Level& level = path_.back();

        // 7 = Back, 8 = Next, 9 = Close, as the center HTML menu.
        if (key == 7 && page.back)
        {
            if (level.page > 0)
                level.page--;
            else
                path_.pop_back();
            Display();
            return;
        }
        if (key == 8 && page.next)
        {
            level.page++;
            Display();
            return;
        }
        if (key == 9 && page.close)
        {
            End(MenuEndReason::Exit);
            return;
        }
        if (key < 1 || key > kRows) return;

        // The key-th numbered row of the page: only what Build() showed.
        const Row* row = nullptr;
        for (const Row& r : page.rows)
        {
            if (r.key == key)
            {
                row = &r;
                break;
            }
        }
        if (!row || !row->enabled)
            return;

        IBaseMenu* menu = Current();
        const int index = row->index;
        if (index >= static_cast<int>(menu->Options().size()))
            return;

        if (IBaseMenu* submenu = menu->Options()[index].Submenu)
        {
            path_.push_back({ submenu, 0 });
            Display();
            return;
        }

        // The handler may close this menu or open another one for the player,
        // and either destroys this instance. Nothing of `this` may be touched
        // after the call unless it is still the open menu. Called through a
        // copy: a handler that rebuilds this same menu in place (ClearOptions
        // + AddMenuOption) destroys the option it is running from.
        ChatMenuOption opt = menu->Options()[index];
        menu->OnOptionSelected(player, opt, index);

        if (!IsOpen(slot, serial))
            return;

        if (opt.CloseOnSelect)
        {
            End(MenuEndReason::Selected);
            return;
        }
        if (!opt.StayOnPage)
        {
            switch (Current()->GetPostSelectAction())
            {
            case PostSelectAction::Close:
                End(MenuEndReason::Selected);
                return;
            case PostSelectAction::Reset:
                path_.back().page = 0;
                break;
            case PostSelectAction::Nothing:
            default:
                break;
            }
        }

        // Whatever the handler changed is on screen right away; OnFrame()
        // would catch it a frame later anyway.
        Display();
    }

    void HudMenuInstance::End(MenuEndReason reason)
    {
        endReason_ = reason;
        // CloseActiveMenu calls OnEnd and destroys this instance; the
        // destructor hides it.
        g_pSource2ModMenus->CloseActiveMenu(player_);
    }

    void HudMenuInstance::Close()
    {
        End(MenuEndReason::Closed);
    }

    void HudMenuInstance::OnEnd(MenuEndReason reason)
    {
        // The core reports what it knows (Closed for CloseActiveMenu); a menu
        // that ended itself knows better.
        if (reason == MenuEndReason::Closed)
            reason = endReason_;
        hudMenu_->OnMenuEnd(player_, reason);
    }

    void HudMenuInstance::OnClick(const char* buttonId)
    {
        if (!buttonId || !player_) return;

        int key = 0;
        if (!strncmp(buttonId, "menu_row_", 9))
        {
            const int row = atoi(buttonId + 9);
            key = row >= 0 && row < kRows ? rowKeys_[row] : 0;
        }
        else if (!strcmp(buttonId, "menu_prev"))
            key = 7;
        else if (!strcmp(buttonId, "menu_next"))
            key = 8;
        else if (!strcmp(buttonId, "menu_close"))
            key = 9;

        if (key < 1 || key > 9)
            return;

        OnKeyPress(player_, key);
    }

    // ---- free functions --------------------------------------------------------

    void Open(PluginId owner, CCSPlayerController* player, HudMenu* menu)
    {
        if (!player || !menu || !g_pSource2ModMenus) return;

        // The core takes the instance: it destroys it (Destroy(), so in this
        // library) when the menu closes, is replaced, the player leaves or
        // `owner` unloads.
        g_pSource2ModMenus->OpenMenu(owner, player, new HudMenuInstance(player, menu));
    }

    void OnLayoutClick(CCSPlayerController* player, const char* buttonId)
    {
        if (!player) return;

        auto it = s_bySlot.find(player->GetSlot());
        if (it == s_bySlot.end() || !it->second)
            return;

        // Only the menu the core still holds: an instance in the map but no
        // longer active is on its way out.
        if (g_pSource2ModMenus->GetActiveMenu(player) != it->second)
            return;

        it->second->OnClick(buttonId);
    }

    void CloseAll()
    {
        // CloseActiveMenu destroys the instance, which erases it from the map.
        std::vector<int> slots;
        slots.reserve(s_bySlot.size());
        for (const auto& kv : s_bySlot)
            slots.push_back(kv.first);

        for (int slot : slots)
        {
            if (CCSPlayerController* player = CCSPlayerController::FromSlot(slot))
                g_pSource2ModMenus->CloseActiveMenu(player);
        }

        s_bySlot.clear();
        s_serialBySlot.clear();
    }
}
