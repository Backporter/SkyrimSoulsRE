#include "Menus/TrainingMenuEx.h"

namespace SkyrimSoulsRE
{
	ConsoleRE::IMenu* TrainingMenuEx::Creator()
	{
		return CreateMenu(ConsoleRE::TrainingMenu::MENU_NAME);
	}

	void TrainingMenuEx::InstallHook()
	{
		
	}
}
