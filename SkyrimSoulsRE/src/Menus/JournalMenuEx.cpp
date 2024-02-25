#include "Menus/JournalMenuEx.h"

namespace SkyrimSoulsRE
{
	uint32_t GamepadMaskToKeycode(uint32_t a_keyMask)
	{
		return a_keyMask;
	}

	void JournalMenuEx::AdvanceMovie_Hook(float a_interval, uint32_t a_currentTime)
	{
		HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(ConsoleRE::UI::GetSingleton()->GetMenu(ConsoleRE::HUDMenu::MENU_NAME).get());
		if (hudMenu)
		{
			hudMenu->UpdateHUD();
		}
		this->UpdateClock();
		return _AdvanceMovie(this, a_interval, a_currentTime);
	}

	ConsoleRE::UI_MESSAGE_RESULTS JournalMenuEx::ProcessMessage_Hook(ConsoleRE::UIMessage& a_message)
	{
		if (a_message.type != ConsoleRE::UI_MESSAGE_TYPE::kScaleformEvent)
		{
			return _ProcessMessage(this, a_message);
		}
		
		ConsoleRE::BSUIScaleformData* data = static_cast<ConsoleRE::BSUIScaleformData*>(a_message.data);

		if (JournalMenuEx::isSaving && data->scaleformEvent->type != ConsoleRE::GFxEvent::EventType::kMouseMove)
		{
			//Block all input when saving, so the menu can't get closed, but let the cursor move around so users don't freak out
			return ConsoleRE::UI_MESSAGE_RESULTS::kIgnore;
		}

		return _ProcessMessage(this, a_message);
	}

	void JournalMenuEx::UpdateClock()
	{
		char timeDateString[200];
		ConsoleRE::Calendar::GetSingleton()->GetTimeDateString(timeDateString, 200, true);

		ConsoleRE::GFxValue dateText;
		this->uiMovie->GetVariable(&dateText, "_root.QuestJournalFader.Menu_mc.BottomBar_mc.DateText");
		if (dateText.GetType() != ConsoleRE::GFxValue::ValueType::kUndefined)
		{
			ConsoleRE::GFxValue newDate(timeDateString);
			dateText.SetMember("htmlText", newDate);
		}
	}

	ConsoleRE::IMenu* JournalMenuEx::Creator()
	{
		class MCMRemapHandler : public ConsoleRE::GFxFunctionHandler, public ConsoleRE::BSTEventSink<ConsoleRE::InputEvent*>, public UnpausedTask
		{
		private:
			ConsoleRE::GFxValue	scope;
			uint32_t	keyCode;
		public:
			ConsoleRE::BSEventNotifyControl ProcessEvent(ConsoleRE::InputEvent* const* a_event, ConsoleRE::BSTEventSource<ConsoleRE::InputEvent*>* a_eventSource) override
			{
				ConsoleRE::ButtonEvent* evn = (ConsoleRE::ButtonEvent*) * a_event;

				if (!evn || evn->eventType != ConsoleRE::INPUT_EVENT_TYPE::kButton)
					return ConsoleRE::BSEventNotifyControl::kContinue;

				ConsoleRE::INPUT_DEVICE deviceType = evn->device;

				ConsoleRE::BSInputDeviceManager* idm = static_cast<ConsoleRE::BSInputDeviceManager*>(a_eventSource);

				if ((idm->IsGamepadEnabled() ^ (deviceType == ConsoleRE::INPUT_DEVICE::kGamepad)) || evn->value == 0 || evn->heldDownSecs != 0.0)
				{
					return ConsoleRE::BSEventNotifyControl::kContinue;
				}

				a_eventSource->RemoveEventSink(this);

				uint32_t keyMask = evn->idCode;
				uint32_t keyCode;

				// Mouse
				switch (deviceType)
				{
				case ConsoleRE::INPUT_DEVICE::kMouse:
					keyCode = keyMask; // + 256;
					break;
				case ConsoleRE::INPUT_DEVICE::kGamepad:
					keyCode = GamepadMaskToKeycode(keyMask);
					break;
				default:
					keyCode = keyMask;
				}

				// Valid scan code?
				if (keyCode >= 282)
				{
					keyCode = -1;
				}	

				this->keyCode = keyCode;

				UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
				queue->AddTask(this);

				ConsoleRE::MenuControls::GetSingleton()->remapMode = false;
				ConsoleRE::PlayerControls::GetSingleton()->data.remapMode = false;

				return ConsoleRE::BSEventNotifyControl::kContinue;
			}

			void Call(ConsoleRE::GFxFunctionHandler::Params& a_args) override
			{
				scope = a_args.args[0];

				ConsoleRE::PlayerControls* playerControls = ConsoleRE::PlayerControls::GetSingleton();
				ConsoleRE::MenuControls* menuControls = ConsoleRE::MenuControls::GetSingleton();
				ConsoleRE::BSInputDeviceManager* inputDeviceManager = ConsoleRE::BSInputDeviceManager::GetSingleton();

				inputDeviceManager->AddEventSink(this);
				menuControls->remapMode = true;
				playerControls->data.remapMode = true;
			}

			void Run() override
			{
				ConsoleRE::GFxValue arg;
				arg.SetNumber(this->keyCode);
				scope.Invoke("EndRemapMode", nullptr, &arg, 1);
			}

			void Dispose() override {}
		};

		class SaveGameHandler : public ConsoleRE::GFxFunctionHandler, public UnpausedTask
		{
		private:
			double selectedIndex;
		public:
			void Call(Params& params) override
			{
				ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();
				ConsoleRE::InterfaceStrings* interfaceStrings = ConsoleRE::InterfaceStrings::GetSingleton();

				ConsoleRE::JournalMenu* menu = static_cast<ConsoleRE::JournalMenu*>(ui->GetMenu(interfaceStrings->journalMenu).get());
				assert(menu);

				ConsoleRE::GFxValue iSaveDelayTimerID;
				menu->uiMovie->GetVariable(&iSaveDelayTimerID, "_root.QuestJournalFader.Menu_mc.SystemFader.Page_mc.iSaveDelayTimerID");

				if (iSaveDelayTimerID.GetType() == ConsoleRE::GFxValue::ValueType::kUndefined)
				{
					xUtilty::Log::GetSingleton(0)->Write(xUtilty::Log::logLevel::kNone,"Unable to get save delay timer ID. Attempting to ignore it.");
				}
				else
				{
					ConsoleRE::GFxValue result;
					bool success = menu->uiMovie->Invoke("clearInterval", &result, &iSaveDelayTimerID, 1); // Not sure if this actually does something
					assert(success);
				}

				// This function is normally supposed to close the menu, and it can get called multiple times. Make sure we only save once.
				if (!isSaving)
				{
					isSaving = true;
					ConsoleRE::GFxValue selectedIndex;
					menu->uiMovie->GetVariable(&selectedIndex, "_root.QuestJournalFader.Menu_mc.SystemFader.Page_mc.SaveLoadListHolder.selectedIndex");

					if (selectedIndex.GetType() == ConsoleRE::GFxValue::ValueType::kUndefined)
					{
						xUtilty::Log::GetSingleton(0)->Write(xUtilty::Log::logLevel::kNone,"Unable to get selected index of selected save game. Aborting save and forcing Journal Menu to close.");
						ConsoleRE::DebugNotification("SAVE FAILED - report issue to Skyrim Souls RE author!");
						ConsoleRE::UIMessageQueue* uiMessageQueue = ConsoleRE::UIMessageQueue::GetSingleton();
						uiMessageQueue->AddMessage(ConsoleRE::JournalMenu::MENU_NAME, ConsoleRE::UI_MESSAGE_TYPE::kForceHide, nullptr);
						isSaving = false;
						ConsoleRE::PlaySound("UIMenuCancel");
						return;
					}

					if (!menu->PausesGame())
					{
						menu->menuFlags |= static_cast<uint32_t>(ConsoleRE::IMenu::Flag::kPausesGame);
						ConsoleRE::UI::GetSingleton()->numPausesGame++;
					}

					//Create save screenshot
					reinterpret_cast<void(*)()>(Offsets::Misc::CreateSaveScreenshot.address())();

					this->selectedIndex = selectedIndex.GetNumber();
					this->usesDelay = true;
					this->beginTime = std::chrono::high_resolution_clock::now();
					this->delayTimeMS = std::chrono::milliseconds(Settings::GetSingleton()->saveDelayMS);
					UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
					queue->AddTask(this);

				}
			}

			void Run() override
			{
				ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();

				if (ui->IsMenuOpen(ConsoleRE::JournalMenu::MENU_NAME))
				{
					ConsoleRE::JournalMenu* menu = static_cast<ConsoleRE::JournalMenu*>(ui->GetMenu(ConsoleRE::JournalMenu::MENU_NAME).get());

					ConsoleRE::GFxValue selectedIndex = this->selectedIndex;

					ConsoleRE::FxDelegateArgs args(0, menu, menu->uiMovie.get(), &selectedIndex, 1);
					menu->fxDelegate->callbacks.GetAlt("SaveGame")->callback(args);

					ConsoleRE::UIMessageQueue* uiMessageQueue = ConsoleRE::UIMessageQueue::GetSingleton();
					uiMessageQueue->AddMessage(ConsoleRE::JournalMenu::MENU_NAME, ConsoleRE::UI_MESSAGE_TYPE::kForceHide, nullptr);
					isSaving = false;
					ConsoleRE::PlaySound("UIMenuCancel");
				}
			}

			void Dispose() override {}
		};

		ConsoleRE::JournalMenu* menu = static_cast<ConsoleRE::JournalMenu*>(CreateMenu(ConsoleRE::JournalMenu::MENU_NAME));

		//fix for remapping from MCM menu
		ConsoleRE::GFxValue globals, skse;
		
		bool result = menu->uiMovie->GetVariable(&globals, "_global");
		if (result)
		{
			result = globals.GetMember("skse", &skse);
			if (result)
			{
				ConsoleRE::GFxFunctionHandler* fn = nullptr;
				fn = new MCMRemapHandler();
				ConsoleRE::GFxValue fnValue;
				menu->uiMovie.get()->CreateFunction(&fnValue, fn);
				skse.SetMember("StartRemapMode", fnValue);
			}
		}

		// Fix for game saves
		ConsoleRE::GFxValue obj;
		bool success = menu->uiMovie->GetVariable(&obj, "_root.QuestJournalFader.Menu_mc.SystemFader.Page_mc");
		assert(success);

		ConsoleRE::GFxValue func, func2;
		menu->uiMovie->CreateFunction(&func, new SaveGameHandler());
		obj.SetMember("DoSaveGame", func);

		return menu;
	}

	ConsoleRE::BSEventNotifyControl JournalMenuEx::RemapHandler::ProcessEvent_Hook(ConsoleRE::InputEvent** a_event, ConsoleRE::BSTEventSource<ConsoleRE::InputEvent**>* a_eventSource)
	{
		if (!*a_event || (*a_event)->eventType != ConsoleRE::INPUT_EVENT_TYPE::kButton)
		{
			return ConsoleRE::BSEventNotifyControl::kContinue;
		}

		ConsoleRE::INPUT_DEVICE deviceType = (*a_event)->device;

		ConsoleRE::BSInputDeviceManager* idm = ConsoleRE::BSInputDeviceManager::GetSingleton();

		if ((idm->IsGamepadEnabled() ^ (deviceType == ConsoleRE::INPUT_DEVICE::kGamepad)) || (*a_event)->AsButtonEvent()->value == 0 || (*a_event)->AsButtonEvent()->heldDownSecs != 0.0)
		{
			return ConsoleRE::BSEventNotifyControl::kContinue;
		}

		idm->RemoveEventSink(this);

		class RemapTask : public UnpausedTask
		{
		private:
			FakeButtonEvent*	evn;
			RemapHandler*		handler;
		public:
			RemapTask(RemapHandler* a_handler, FakeButtonEvent* a_evn)
			{
				this->evn = a_evn;
				this->handler = a_handler;
			}

			void Run() override
			{
				ConsoleRE::InputEvent* inputEvent = reinterpret_cast<ConsoleRE::InputEvent*>(evn);
				_ProcessEvent(handler, &inputEvent, nullptr);
			}
		};

		ConsoleRE::ButtonEvent* buttonEvent = (*a_event)->AsButtonEvent();
		FakeButtonEvent* fakeEvent = new FakeButtonEvent();
		fakeEvent->device = buttonEvent->device;
		fakeEvent->eventType = buttonEvent->eventType;
		fakeEvent->next = nullptr;
		fakeEvent->userEvent = buttonEvent->userEvent;
		fakeEvent->idCode = buttonEvent->idCode;
		fakeEvent->value = buttonEvent->value;
		fakeEvent->heldDownSecs = buttonEvent->heldDownSecs;

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(new RemapTask(this, fakeEvent));

		return ConsoleRE::BSEventNotifyControl::kContinue;
	}

	void JournalMenuEx::InstallHook()
	{
		//Hook ProcessMessage
		REL::Relocation<uintptr_t> vTable(Offsets::Menus::JournalMenu::Vtbl);
		_ProcessMessage = vTable.write_vfunc(0x5, &JournalMenuEx::ProcessMessage_Hook);

		//Hook AdvanceMovie
		_AdvanceMovie = vTable.write_vfunc(0x6, &JournalMenuEx::AdvanceMovie_Hook);

		REL::Relocation<uintptr_t> vTable_remapHandler(Offsets::Menus::JournalMenu::RemapHandler_Vtbl);
		RemapHandler::_ProcessEvent = vTable_remapHandler.write_vfunc(0x2, &JournalMenuEx::RemapHandler::ProcessEvent_Hook);
	}
};
