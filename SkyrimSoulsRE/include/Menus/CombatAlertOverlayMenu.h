#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	class CombatAlertOverlayMenu : public ConsoleRE::IMenu
	{
	public:
		static constexpr const char* MENU_NAME = "CombatAlertOverlayMenu";
		static constexpr const char* FILE_NAME = "CombatAlertOverlayMenu";

		void AdvanceMovie(float a_interval, uint32_t a_currentTime) override;
		ConsoleRE::UI_MESSAGE_RESULTS ProcessMessage(ConsoleRE::UIMessage& a_message) override;

		void InitExtensions();

		static ConsoleRE::IMenu* Creator();

	private:
		CombatAlertOverlayMenu();

		ConsoleRE::GPtr<ConsoleRE::GFxMovieView> _view;
		bool _isActive = false;
	};
}
