#include "Controls/MenuControlsEx.h"
#include "Controls/InputHandlerEx.h"

namespace SkyrimSoulsRE
{
	bool MenuControlsEx::IsMappedToSameButton(uint32_t a_keyMask, ConsoleRE::INPUT_DEVICE a_deviceType, ConsoleRE::BSFixedString a_controlName, ConsoleRE::UserEvents::INPUT_CONTEXT_ID a_context)
	{
		ConsoleRE::ControlMap* controlMap = ConsoleRE::ControlMap::GetSingleton();

		if (a_deviceType == ConsoleRE::INPUT_DEVICE::kKeyboard)
		{
			uint32_t keyMask = controlMap->GetMappedKey(a_controlName, ConsoleRE::INPUT_DEVICE::kKeyboard, a_context);
			return a_keyMask == keyMask;
		}
		else if (a_deviceType == ConsoleRE::INPUT_DEVICE::kMouse)
		{
			uint32_t keyMask = controlMap->GetMappedKey(a_controlName, ConsoleRE::INPUT_DEVICE::kMouse, a_context);
			return a_keyMask == keyMask;
		}

		// 
		else if (a_deviceType == ConsoleRE::INPUT_DEVICE::kGamepad)
		{
			uint32_t keyMask = controlMap->GetMappedKey(a_controlName, ConsoleRE::INPUT_DEVICE::kGamepad, a_context);
			return a_keyMask == keyMask;
		}

		return false;
	}

	ConsoleRE::BSEventNotifyControl MenuControlsEx::ProcessEvent_Hook(ConsoleRE::InputEvent** a_event, ConsoleRE::BSTEventSource<ConsoleRE::InputEvent*>* a_source)
	{
		ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();
		ConsoleRE::PlayerControls* pc = ConsoleRE::PlayerControls::GetSingleton();
		ConsoleRE::ControlMap* controlMap = ConsoleRE::ControlMap::GetSingleton();
		ConsoleRE::UserEvents* userEvents = ConsoleRE::UserEvents::GetSingleton();
		Settings* settings = Settings::GetSingleton();
		
		bool dialogueMode = ui->IsMenuOpen(ConsoleRE::DialogueMenu::MENU_NAME) && !settings->isUsingDME;
		bool lookControlsEnabled = pc->lookHandler->IsInputEventHandlingEnabled() && controlMap->IsLookingControlsEnabled() && !dialogueMode;
		bool movementControlsEnabled = pc->movementHandler->IsInputEventHandlingEnabled() && controlMap->IsMovementControlsEnabled() && !dialogueMode;

		if (a_event && *a_event && !this->remapMode && !ui->GameIsPaused() && !IsFullScreenMenuOpen() && GetUnpausedMenuCount())
		{
			for (ConsoleRE::InputEvent* evn = *a_event; evn; evn = evn->next)
			{
				if (evn && evn->HasIDCode())
				{
					ConsoleRE::IDEvent* idEvent = static_cast<ConsoleRE::ButtonEvent*>(evn);

					if (settings->enableMovementInMenus)
					{
						//Forward
						if (idEvent->userEvent == userEvents->up && IsMappedToSameButton(idEvent->idCode, idEvent->device, userEvents->forward))
						{
							idEvent->userEvent = movementControlsEnabled ? userEvents->forward : "";
						}
						//Back
						if (idEvent->userEvent == userEvents->down && IsMappedToSameButton(idEvent->idCode, idEvent->device, userEvents->back))
						{
							idEvent->userEvent = movementControlsEnabled ? userEvents->back : "";
						}
						//Left
						if (idEvent->userEvent == userEvents->left && IsMappedToSameButton(idEvent->idCode, idEvent->device, userEvents->strafeLeft))
						{
							idEvent->userEvent = movementControlsEnabled ? userEvents->strafeLeft : "";
						}
						//Right
						if (idEvent->userEvent == userEvents->right && IsMappedToSameButton(idEvent->idCode, idEvent->device, userEvents->strafeRight))
						{
							idEvent->userEvent = movementControlsEnabled ? userEvents->strafeRight : "";
						}
						//SkyUI Favorites menu fix
						if (ui->IsMenuOpen(ConsoleRE::FavoritesMenu::MENU_NAME))
						{
							//Prevent SkyUI from detecting the key mask
							//Left
							if (idEvent->userEvent == userEvents->strafeLeft && IsMappedToSameButton(idEvent->idCode, idEvent->device, userEvents->strafeLeft))
							{
								idEvent->idCode = 0;
							}
							//Left
							if (idEvent->userEvent == userEvents->strafeRight && IsMappedToSameButton(idEvent->idCode, idEvent->device, userEvents->strafeRight))
							{
								idEvent->idCode = 0;
							}
							//Allow category change with LB and RB when using controllers
							if (idEvent->device == ConsoleRE::INPUT_DEVICE::kGamepad)
							{
								if (idEvent->idCode == 0x00000400) // LB
								{
									idEvent->userEvent = userEvents->left;
								}
								if (idEvent->idCode == 0x00000800) // RB
								{
									idEvent->userEvent = userEvents->right;
								}
							}
						}
						//Book menu fix
						if (ui->IsMenuOpen(ConsoleRE::BookMenu::MENU_NAME))
						{
							//Left
							if (idEvent->userEvent == userEvents->prevPage && IsMappedToSameButton(idEvent->idCode, idEvent->device, userEvents->strafeLeft))
							{
								idEvent->userEvent = movementControlsEnabled ? userEvents->strafeLeft : "";
							}
							//Right
							if (idEvent->userEvent == userEvents->nextPage && IsMappedToSameButton(idEvent->idCode, idEvent->device, userEvents->strafeRight))
							{
								idEvent->userEvent = movementControlsEnabled ? userEvents->strafeRight : "";
							}
						}

						//Toggle Walk/Run
						if (settings->enableToggleRun && IsMappedToSameButton(idEvent->idCode, idEvent->device, userEvents->toggleRun))
						{
							idEvent->userEvent = movementControlsEnabled ? userEvents->toggleRun : "";
						}

						//Controllers
						if (idEvent->userEvent == userEvents->leftStick)
						{
							idEvent->userEvent = movementControlsEnabled ? userEvents->move : "";
						}
					}

					if (settings->enableGamepadCameraMove)
					{
						//Look controls for controllers - do not allow when an item preview is maximized, so it is still possible to rotate it somehow
						ConsoleRE::Inventory3DManager* inventory3DManager = ConsoleRE::Inventory3DManager::GetSingleton();
						if (idEvent->userEvent == userEvents->rotate && inventory3DManager->zoomProgress == 0.0f)
						{
							idEvent->userEvent = lookControlsEnabled ? userEvents->look : "";
						}
					}
				}
			}
		}

		return _ProcessEvent(this, a_event, a_source);
	}

	void MenuControlsEx::InstallHook()
	{
		REL::Relocation<uintptr_t> vTable(Offsets::MenuControls::Vtbl);
		_ProcessEvent = vTable.write_vfunc(0x2, &MenuControlsEx::ProcessEvent_Hook);

		InputHandlerEx<ConsoleRE::FavoritesHandler>::InstallHook(REL::Relocation<uintptr_t>(ConsoleRE::Offset::FavoritesHandler::Vtbl), 0x2);
		InputHandlerEx<ConsoleRE::MenuOpenHandler>::InstallHook(REL::Relocation<uintptr_t>(ConsoleRE::Offset::MenuOpenHandler::Vtbl), 0x2);
	}
}
