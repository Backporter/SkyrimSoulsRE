#pragma once

#include <map>
#include <string>
#include <vector>

namespace SkyrimSoulsRE
{
	class Settings
	{
	private:
		static Settings* singleton;
		Settings();

	public:
		static Settings* GetSingleton();

		//Unpaused Menus
		std::map<std::string, bool> unpausedMenus;

		//AutoClose
		bool autoCloseMenus;
		float autoCloseDistance;
		float autoCloseTolerance;

		//Controls
		bool enableMovementInMenus;
		bool enableToggleRun;
		bool enableGamepadCameraMove;
		bool enableCursorCameraMove;
		float cursorCameraVerticalSpeed;
		float cursorCameraHorizontalSpeed;

		//slow motion
		std::map<std::string, bool> slowMotionMenus;
		float slowMotionMultiplier;

		//Blur
		bool disableBlur;

		//Saving
		uint32_t saveDelayMS;

		//HUD
		bool disableHUDModifications;
		float sneakMeterPosX;
		float sneakMeterPosY;

		//Combat Alert Overlay
		std::map<std::string, bool> overlayMenus;

		//Messages
		bool hideEngineFixesWarning;

		// Dialogue Movement Enabler compatibility (internal setting)
		bool isUsingDME = false;
	};

	extern void LoadSettings();
}
