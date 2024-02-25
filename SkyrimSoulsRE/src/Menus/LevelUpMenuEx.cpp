#include "Menus/LevelUpMenuEx.h"

namespace SkyrimSoulsRE
{
	ConsoleRE::IMenu* LevelUpMenuEx::Creator()
	{
		ConsoleRE::LevelUpMenu* menu = static_cast<ConsoleRE::LevelUpMenu*>(CreateMenu(ConsoleRE::LevelUpMenu::MENU_NAME));
		return menu;
	}

	void LevelUpMenuEx::InstallHook()
	{
	}
}
