#include "Menus/StatsMenuEx.h"

#include "../../../OrbisUtil/Third-Party/herumi/xbayk/6.00/xbyak.h"

namespace SkyrimSoulsRE
{
    ConsoleRE::UI_MESSAGE_RESULTS StatsMenuEx::ProcessMessage_Hook(ConsoleRE::UIMessage& a_message)
    {
        if (a_message.type == ConsoleRE::UI_MESSAGE_TYPE::kHide)
        {
            isSleeping = false;
        }

        return _ProcessMessage(this, a_message);
    }

    void StatsMenuEx::AdvanceMovie_Hook(float a_interval, uint32_t a_currentTime)
    {
        HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(ConsoleRE::UI::GetSingleton()->GetMenu(ConsoleRE::HUDMenu::MENU_NAME).get());
        if (hudMenu)
        {
            hudMenu->UpdateHUD();
        }

        return _AdvanceMovie(this, a_interval, a_currentTime);
    }

    ConsoleRE::IMenu* StatsMenuEx::Creator()
    {
        StatsMenuEx* menu = static_cast<StatsMenuEx*>(CreateMenu(ConsoleRE::StatsMenu::MENU_NAME));
        return menu;
    }

    void OpenStatsMenuAfterSleep_Hook(ConsoleRE::UIMessageQueue* a_queue, const ConsoleRE::BSFixedString& a_menuName, ConsoleRE::UI_MESSAGE_TYPE a_type, ConsoleRE::IUIMessageData* a_data)
    {
        using FlagBD9 = ConsoleRE::PlayerCharacter::FlagBD9;

        ConsoleRE::PlayerCharacter* player = ConsoleRE::PlayerCharacter::GetSingleton();
        if (player->playerFlags.sleeping) // unkBD9.all(FlagBD9::kIsSleeping))
        {
            StatsMenuEx::isSleeping = true;
        }

        return a_queue->AddMessage(a_menuName, a_type, a_data);
    }

	// HERE:
    void StatsMenuEx::InstallHook()
    {
        auto& trampoline = API::GetTrampoline();

        // Fix level up when sleeping using survival mode
        // We replace a tail-call jump with a normal call, so the rest of the function can execute as well (this will remove the sleeping flag).
        REL::safe_write(Offsets::Menus::StatsMenu::OpenStatsMenuAfterSleep_Hook.address() + 0xA3, uint32_t(0x90909090));
        REL::safe_write(Offsets::Menus::StatsMenu::OpenStatsMenuAfterSleep_Hook.address() + 0xA7, uint8_t(0x90));
		REL::safe_write(Offsets::Menus::StatsMenu::OpenStatsMenuAfterSleep_Hook.address() + 0xA8, uint16_t(0x9090));
		REL::safe_write(Offsets::Menus::StatsMenu::OpenStatsMenuAfterSleep_Hook.address() + 0xAA, uint16_t(0x9090));
		REL::safe_write(Offsets::Menus::StatsMenu::OpenStatsMenuAfterSleep_Hook.address() + 0xAC, uint8_t(0x90));
        trampoline.write_call<5>(Offsets::Menus::StatsMenu::OpenStatsMenuAfterSleep_Hook.address() + 0xAD, OpenStatsMenuAfterSleep_Hook);

        // Make StatsMenu check our sleeping variable
        struct SleepCheck_Code : Xbyak::CodeGenerator
        {
            SleepCheck_Code(uintptr_t a_trueAddress, uintptr_t a_falseAddress)
            {
                Xbyak::Label trueAddress;
                Xbyak::Label falseAddress;
                Xbyak::Label falseLabel;

                mov(rdi, uintptr_t(&StatsMenuEx::isSleeping));
                mov(dil, byte[rdi]);
                cmp(dil, 1);
                jne(falseLabel);
                jmp(ptr[rip + trueAddress]);

                L(falseLabel);
                jmp(ptr[rip + falseAddress]);

                L(trueAddress);
                dq(a_trueAddress);

                L(falseAddress);
                dq(a_falseAddress);
            }
        };

        SleepCheck_Code code{ uintptr_t(Offsets::Menus::StatsMenu::ProcessMessage.address() + 0x10A7/*0xF45*/), uintptr_t(Offsets::Menus::StatsMenu::ProcessMessage.address() + 0x1091/*0xFA9*/) };
        void* codeLoc = API::GetTrampoline().allocate(code);
        trampoline.write_branch<6>(Offsets::Menus::StatsMenu::ProcessMessage.address() + 0x105B /*0xF3C*/, codeLoc);

        // Fix for menu not appearing (???)
		uint8_t arr[6]{ 0xE9, 0xDF, 0x04, 0x00, 0x00, 0x90 }; // jmp loc_A65D5A;nop
		REL::safe_write(Offsets::Menus::StatsMenu::ProcessMessage.address() + 0x86, arr);
		
        // Prevent setting kFreezeFrameBackground flag
		REL::safe_write(Offsets::Menus::StatsMenu::ProcessMessage.address() + 0x801, uint32_t(0x90909090));
		REL::safe_write(Offsets::Menus::StatsMenu::ProcessMessage.address() + 0x805, uint8_t(0x90));

        // Keep the menu updated
        REL::safe_write(Offsets::Menus::StatsMenu::ProcessMessage.address() + 0x10A5, uint16_t(0x9090));

        // Fix for controls not working
		REL::safe_write(Offsets::Menus::StatsMenu::CanProcess.address() + 0x50, uint8_t(0xEB));

        // Hook ProcessMessage and AdvanceMovie
        REL::Relocation<uintptr_t> vTable(Offsets::Menus::StatsMenu::Vtbl);
        _ProcessMessage = vTable.write_vfunc(0x5, &StatsMenuEx::ProcessMessage_Hook);
        _AdvanceMovie = vTable.write_vfunc(0x6, &StatsMenuEx::AdvanceMovie_Hook);
    }
}
