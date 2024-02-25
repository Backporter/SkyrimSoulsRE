#include "Menus/CombatAlertOverlayMenu.h"

namespace SkyrimSoulsRE
{
	ConsoleRE::IMenu* CombatAlertOverlayMenu::Creator()
	{
		return new CombatAlertOverlayMenu();
	}

	CombatAlertOverlayMenu::CombatAlertOverlayMenu()
	{
		using Context = ConsoleRE::UserEvents::INPUT_CONTEXT_ID;
		using Flag = ConsoleRE::UI_MENU_FLAGS;

		auto menu = static_cast<ConsoleRE::IMenu*>(this);
		auto scaleformManager = ConsoleRE::BSScaleformManager::GetSingleton();
		[[maybe_unused]] const auto success = scaleformManager->LoadMovieEx(menu, FILE_NAME, /*ConsoleRE::BSScaleformManager::ScaleModeType::kExactFit*/ 2, [](ConsoleRE::GFxMovieDef* a_def) -> void {});
		assert(success);
		_view = menu->uiMovie;
		_view->SetMouseCursorCount(0);
		menu->menuFlags |= 2048; // Flag::kAllowSaving;
		menu->depthPriority = 11;
		menu->inputContext = Context::kNone;
		//InitExtensions();

		ConsoleRE::PlayerCharacter* player = ConsoleRE::PlayerCharacter::GetSingleton();
		if (player)
		{
			if (player->IsInCombat())
			{
				_isActive = true;
				_view->SetVisible(true);
			}
			else if (!player->IsInCombat())
			{
				_isActive = false;
				_view->SetVisible(false);
			}
		}

		_view->SetPause(true);
	}

	void CombatAlertOverlayMenu::AdvanceMovie(float a_interval, uint32_t a_currentTime)
	{
		uint32_t currentFrame = _view->GetCurrentFrame();

		if (!GetCombatAlertOverlayCount())
		{
			ConsoleRE::UIMessageQueue* msgQueue = ConsoleRE::UIMessageQueue::GetSingleton();
			msgQueue->AddMessage(CombatAlertOverlayMenu::MENU_NAME, ConsoleRE::UI_MESSAGE_TYPE::kHide, nullptr);
		}

		ConsoleRE::PlayerCharacter* player = ConsoleRE::PlayerCharacter::GetSingleton();
		if (player && currentFrame == 120)
		{
			if (player->IsInCombat() && !_isActive)
			{
				_isActive = true;
				_view->SetVisible(true);
			}
			else if (!player->IsInCombat() && _isActive)
			{
				_isActive = false;
				_view->SetVisible(false);
			}
		}
		
		uint32_t nextFrame = currentFrame == 120 ? 1 : currentFrame + 1;
		_view->GotoFrame(nextFrame);
	}

	ConsoleRE::UI_MESSAGE_RESULTS CombatAlertOverlayMenu::ProcessMessage(ConsoleRE::UIMessage& a_message)
	{
		if (!strcasecmp(a_message.menu , /*==*/ CombatAlertOverlayMenu::MENU_NAME))
		{
			return ConsoleRE::UI_MESSAGE_RESULTS::kHandled;
		}

		return ConsoleRE::UI_MESSAGE_RESULTS::kPassOn;
	}

	void CombatAlertOverlayMenu::InitExtensions()
	{
		const ConsoleRE::GFxValue boolean(true);
		bool success;

		success = _view->SetVariable("_global.gfxExtensions", boolean);
		assert(success);
		success = _view->SetVariable("_global.noInvisibleAdvance", boolean);
		assert(success);
	}
}
