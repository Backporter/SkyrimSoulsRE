#include "Menus/TutorialMenuEx.h"

namespace SkyrimSoulsRE
{
	ConsoleRE::IMenu* TutorialMenuEx::Creator()
	{
		return CreateMenu(ConsoleRE::TutorialMenu::MENU_NAME);
	}

	void TutorialMenuEx::InstallHook()
	{

	}
}
