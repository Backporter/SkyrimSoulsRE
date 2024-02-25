#include "Menus/MapMenuEx.h"

namespace SkyrimSoulsRE
{
	ConsoleRE::UI_MESSAGE_RESULTS MapMenuEx::ProcessMessage_Hook(ConsoleRE::UIMessage& a_message)
	{
		class ProcessMessageTask : public UnpausedTask
		{
			ConsoleRE::UIMessage* message;
		public:
			ProcessMessageTask(ConsoleRE::UIMessage* a_message)
			{
				this->message = a_message;
				this->usesDelay = false;
			}

			void Run() override
			{
				ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();

				if (ui->IsMenuOpen(ConsoleRE::MapMenu::MENU_NAME))
				{
					MapMenuEx* menu = static_cast<MapMenuEx*>(ui->GetMenu(ConsoleRE::MapMenu::MENU_NAME).get());
					menu->_ProcessMessage(menu, *message);

					ConsoleRE::UIMessageQueue* msgQueue = ConsoleRE::UIMessageQueue::GetSingleton();
					msgQueue->AddMessage(ConsoleRE::MapMenu::MENU_NAME, ConsoleRE::UI_MESSAGE_TYPE::kHide, nullptr);
				}
			}

			void Dispose() override
			{
				delete this->message;
				delete this;
			}
		};

		// Fix for disappearing first person model after closing menu
		if (a_message.type == ConsoleRE::UI_MESSAGE_TYPE::kHide)
		{
			if (closeMenu)
			{
				closeMenu = false;
				ConsoleRE::PlayerControls* pc = ConsoleRE::PlayerControls::GetSingleton();
				if (pc->autoMoveHandler->IsInputEventHandlingEnabled() && ConsoleRE::ControlMap::GetSingleton()->IsMovementControlsEnabled())
				{
					pc->data.autoMove = restoreAutoMove;
				}
				restoreAutoMove = false;
				return ConsoleRE::UI_MESSAGE_RESULTS::kHandled;
			}

			closeMenu = true;

			ConsoleRE::UIMessage* message = new ConsoleRE::UIMessage(a_message);
			if (message->data)
			{
				// Fast traveling
				restoreAutoMove = false;
				ConsoleRE::BSUIMessageData* oldData = static_cast<ConsoleRE::BSUIMessageData*>(message->data);

				ConsoleRE::InterfaceStrings* interfaceStrings = ConsoleRE::InterfaceStrings::GetSingleton();
				ConsoleRE::MessageDataFactoryManager* msgFactory = ConsoleRE::MessageDataFactoryManager::GetSingleton();

				auto creator = msgFactory->GetCreator(interfaceStrings->bsUIMessageData);
				ConsoleRE::BSUIMessageData* newData = static_cast<ConsoleRE::BSUIMessageData*>(creator->Create());
				newData->data = oldData->data;

				message->data = newData;
			}

			ProcessMessageTask* task = new ProcessMessageTask(message);

			UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
			queue->AddTask(task);

			return ConsoleRE::UI_MESSAGE_RESULTS::kIgnore;
		}

		return _ProcessMessage(this, a_message);
	}

	void MapMenuEx::AdvanceMovie_Hook(float a_interval, uint32_t a_currentTime)
	{
		class AdvanceMapMenuTask : public UnpausedTask
		{
			float interval;
			uint32_t currentTime;

		public:

			AdvanceMapMenuTask(float a_interval, uint32_t a_currentTime)
			{
				this->interval = a_interval;
				this->currentTime = a_currentTime;
				this->usesDelay = false;
			}

			void Run() override
			{
				ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();

				if (ui->IsMenuOpen(ConsoleRE::MapMenu::MENU_NAME))
				{
					MapMenuEx* menu = static_cast<MapMenuEx*>(ui->GetMenu(ConsoleRE::MapMenu::MENU_NAME).get());

					menu->UpdateClock();
					menu->_AdvanceMovie(menu, interval, currentTime);
				}
			}
		};

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(new AdvanceMapMenuTask(a_interval, a_currentTime));	
	}

	void MapMenuEx::UpdateClock()
	{
		char timeDateString[200];
		ConsoleRE::Calendar::GetSingleton()->GetTimeDateString(timeDateString, 200, true);

		ConsoleRE::GFxValue dateText;
		if (this->uiMovie->GetVariable(&dateText, "_root.bottomBar.DateText")) //SkyUI
		{
			ConsoleRE::GFxValue newDate(timeDateString);
			dateText.SetMember("htmlText", newDate);
		}
		else if (this->uiMovie->GetVariable(&dateText, "_root.Bottom.DateText")) // non-SkyUI
		{
			ConsoleRE::GFxValue newDate(timeDateString);
			dateText.SetMember("htmlText", newDate);
		}
	}

	ConsoleRE::IMenu* MapMenuEx::Creator()
	{
		ConsoleRE::MapMenu* menu = static_cast<ConsoleRE::MapMenu*>(CreateMenu(ConsoleRE::MapMenu::MENU_NAME));
		ConsoleRE::PlayerControls* pc = ConsoleRE::PlayerControls::GetSingleton();
		restoreAutoMove = pc->data.autoMove;
		pc->data.autoMove = false;
		return menu;
	}

	void MapMenuEx::InstallHook()
	{
		REL::Relocation<uintptr_t> vTable(Offsets::Menus::MapMenu::Vtbl);
		_ProcessMessage = vTable.write_vfunc(0x5, &MapMenuEx::ProcessMessage_Hook);
		_AdvanceMovie = vTable.write_vfunc(0x6, &MapMenuEx::AdvanceMovie_Hook);

		// Prevent setting kFreezeFrameBackground flag when opening local map
		REL::safe_write(Offsets::Menus::MapMenu::LocalMapUpdaterFunc.address() + 0x8B, uint32_t(0x90909090));
		REL::safe_write(Offsets::Menus::MapMenu::LocalMapUpdaterFunc.address() + 0xF7, uint16_t(0x9090));
		REL::safe_write(Offsets::Menus::MapMenu::LocalMapUpdaterFunc.address() + 0xF9, uint8_t(0x90));

		// Disable map menu background sound
		REL::safe_write(Offsets::Menus::MapMenu::Ctor.address() + 0x536, uint8_t(0xEB));

		// Fix controls while journal is open
		MapInputHandlerEx<ConsoleRE::MapMoveHandler>::InstallHook(Offsets::Menus::MapMenu::MapMoveHandler::Vtbl);
		MapInputHandlerEx<ConsoleRE::MapZoomHandler>::InstallHook(Offsets::Menus::MapMenu::MapZoomHandler::Vtbl);
		MapInputHandlerEx<ConsoleRE::MapLookHandler>::InstallHook(Offsets::Menus::MapMenu::MapLookHandler::Vtbl);
	}
}
