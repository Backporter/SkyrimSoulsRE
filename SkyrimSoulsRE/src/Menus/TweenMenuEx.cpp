#include "Menus/TweenMenuEx.h"

namespace SkyrimSoulsRE
{
	void TweenMenuEx::AdvanceMovie_Hook(float a_interval, uint32_t a_currentTime)
	{
		UpdateClock();
		return _AdvanceMovie(this, a_interval, a_currentTime);
	}

	//Tween menu clock
	void TweenMenuEx::UpdateClock()
	{
		char timeDateString[200];
		ConsoleRE::Calendar::GetSingleton()->GetTimeDateString(timeDateString, 200, true);

		ConsoleRE::GFxValue dateText;
		this->uiMovie->GetVariable(&dateText, "_root.TweenMenu_mc.BottomBarTweener_mc.BottomBar_mc.DateText");

		if (dateText.GetType() != ConsoleRE::GFxValue::ValueType::kUndefined)
		{
			ConsoleRE::GFxValue newDate(timeDateString);
			dateText.SetMember("htmlText", newDate);
		}
	}

	ConsoleRE::IMenu* TweenMenuEx::Creator()
	{
		return CreateMenu(ConsoleRE::TweenMenu::MENU_NAME);
	}

	void TweenMenuEx::InstallHook()
	{
		//Hook AdvanceMovie
		REL::Relocation<uintptr_t> vTable(Offsets::Menus::TweenMenu::Vtbl);
		_AdvanceMovie = vTable.write_vfunc(0x6, &TweenMenuEx::AdvanceMovie_Hook);

		//Fix for camera movement
		uint8_t codes[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
		REL::safe_write(Offsets::Menus::TweenMenu::Camera_Hook.address() + 0x1C0, codes, sizeof(codes));
	}
}
