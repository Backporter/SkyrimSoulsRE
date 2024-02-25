#include "Menus/ModManagerMenuEx.h"

namespace SkyrimSoulsRE
{
	ConsoleRE::IMenu* ModManagerMenuEx::Creator()
	{
		return CreateMenu(ConsoleRE::ModManagerMenu::MENU_NAME);
	}

	void ModManagerMenuEx::InstallHook()
	{

	}
}
