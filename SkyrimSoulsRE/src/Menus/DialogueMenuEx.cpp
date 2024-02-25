#include "Menus/DialogueMenuEx.h"
#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	//Prevent dialogue from closing when an another menu is open
	void DialogueMenuEx::UpdateAutoCloseTimer_Hook(ConsoleRE::AIProcess* a_process, float a_delta)
	{
		ConsoleRE::HighProcessData* highData = a_process->high;
		if (highData)
		{
			if (SkyrimSoulsRE::GetUnpausedMenuCount())
			{
				highData->closeDialogueTimer = 120.0;
			}
			else
			{
				highData->closeDialogueTimer += a_delta; //a_delta is negative
			}
		}
	}

	void DialogueMenuEx::AdvanceMovie_Hook(float a_interval, uint32_t a_currentTime)
	{
		uint32_t unpausedMenuCount = SkyrimSoulsRE::GetUnpausedMenuCount();
		bool isVisible = this->uiMovie->GetVisible();

		if (!ConsoleRE::UI::GetSingleton()->GameIsPaused())
		{
			if (isVisible && unpausedMenuCount)
			{
				this->uiMovie->SetVisible(false);
				this->depthPriority = 0;
			}
			else if (!isVisible && !unpausedMenuCount)
			{
				this->uiMovie->SetVisible(true);
				this->depthPriority = 3;
			}
		}

		_AdvanceMovie(this, a_interval, a_currentTime);
	}

	void DialogueMenuEx::InstallHook()
	{
		//Hook AdvanceMovie
		REL::Relocation<uintptr_t> vTable(Offsets::Menus::DialogueMenu::Vtbl);
		_AdvanceMovie = vTable.write_vfunc(0x6, &DialogueMenuEx::AdvanceMovie_Hook);

		API::GetTrampoline().write_call<5>(Offsets::Menus::DialogueMenu::UpdateAutoCloseTimer_Hook.address() + 0x52C, (uintptr_t)UpdateAutoCloseTimer_Hook);
	}
}
