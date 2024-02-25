#include "PapyrusHook.h"
#include "Settings.h"

namespace SkyrimSoulsRE
{
	namespace Papyrus
	{
		bool* isInMenuMode_1;
		bool* isInMenuMode_2;

		bool IsInMenuMode()
		{
			return *isInMenuMode_1 || *isInMenuMode_2 || SkyrimSoulsRE::GetUnpausedMenuCount() || ConsoleRE::UI::GetSingleton()->IsMenuOpen(ConsoleRE::Console::MENU_NAME);
		}

		void InstallHook() 
		{
			isInMenuMode_1 = reinterpret_cast<bool*>(Offsets::Papyrus::IsInMenuMode::Value1.address());
			isInMenuMode_2 = reinterpret_cast<bool*>(Offsets::Papyrus::IsInMenuMode::Value2.address());
			
			API::GetTrampoline().write_branch<5>(Offsets::Papyrus::IsInMenuMode::Hook.address(), IsInMenuMode);
		}
	}
}
