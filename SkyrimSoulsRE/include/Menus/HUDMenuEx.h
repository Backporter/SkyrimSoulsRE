#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	class HUDMenuEx : public ConsoleRE::HUDMenu
	{
	public:
		static inline double stealthMeterPosX;
		static inline double stealthMeterPosY;

		void SetSkyrimSoulsMode(bool a_isEnabled);
		void UpdateHUD();

		static ConsoleRE::IMenu* Creator();
		static void InstallHook();
	};
}
