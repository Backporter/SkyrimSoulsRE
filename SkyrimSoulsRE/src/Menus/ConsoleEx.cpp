#include "Menus/ConsoleEx.h"

namespace SkyrimSoulsRE
{
	ConsoleRE::IMenu* ConsoleEx::Creator()
	{
		ConsoleRE::Console* menu = static_cast<ConsoleRE::Console*>(CreateMenu(ConsoleRE::Console::MENU_NAME));
		return menu;
	}

	void ConsoleEx::InstallHook()
	{

	}
}
