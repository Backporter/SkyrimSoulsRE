#include "Menus/SleepWaitMenuEx.h"
#include "SlowMotionHandler.h"

namespace SkyrimSoulsRE
{
	ConsoleRE::UI_MESSAGE_RESULTS SleepWaitMenuEx::ProcessMessage_Hook(ConsoleRE::UIMessage& a_message)
	{
		if (a_message.type == ConsoleRE::UI_MESSAGE_TYPE::kScaleformEvent)
		{
			return this->ConsoleRE::IMenu::ProcessMessage(a_message);
		}
		return _ProcessMessage(this, a_message);
	}

	void SleepWaitMenuEx::AdvanceMovie_Hook(float a_interval, uint32_t a_currentTime)
	{
		HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(ConsoleRE::UI::GetSingleton()->GetMenu(ConsoleRE::HUDMenu::MENU_NAME).get());
		if (hudMenu)
		{
			hudMenu->UpdateHUD();
		}
		if (!this->PausesGame())
		{
			this->UpdateClock();
		}

		return _AdvanceMovie(this, a_interval, a_currentTime);
	}

	void SleepWaitMenuEx::UpdateClock()
	{
		char timeDateString[200];
		ConsoleRE::Calendar::GetSingleton()->GetTimeDateString(timeDateString, 200, false);

		ConsoleRE::GFxValue dateText;
		this->uiMovie->GetVariable(&dateText, "_root.SleepWaitMenu_mc.CurrentTime");
		if (dateText.GetType() != ConsoleRE::GFxValue::ValueType::kUndefined)
		{
			ConsoleRE::GFxValue newDate(timeDateString);
			dateText.SetMember("htmlText", newDate);
		}
	}

	void SleepWaitMenuEx::StartSleepWait_Hook(const ConsoleRE::FxDelegateArgs& a_args)
	{
		class StartSleepWaitTask : public UnpausedTask
		{
			double sleepWaitTime;

		public:
			StartSleepWaitTask(double a_time)
			{
				this->sleepWaitTime = a_time;
				this->beginTime = std::chrono::high_resolution_clock::now();			
			}

			void Run() override
			{
				ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();

				if (ui->IsMenuOpen(ConsoleRE::SleepWaitMenu::MENU_NAME))
				{
					ConsoleRE::SleepWaitMenu* menu = static_cast<ConsoleRE::SleepWaitMenu*>(ui->GetMenu(ConsoleRE::SleepWaitMenu::MENU_NAME).get());
					ConsoleRE::GFxValue time = this->sleepWaitTime;
					ConsoleRE::FxDelegateArgs args(0, menu, menu->uiMovie.get(), &time, 1);
					_StartSleepWait(args);
				}
			}
		};

		ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();
		Settings* settings = Settings::GetSingleton();

		ConsoleRE::SleepWaitMenu* menu = static_cast<ConsoleRE::SleepWaitMenu*>(a_args.GetHandler());
		if (!menu->PausesGame())
		{
			menu->menuFlags |= static_cast<uint32_t>(ConsoleRE::IMenu::Flag::kPausesGame);
			ui->numPausesGame++;
		}

		StartSleepWaitTask* task = new StartSleepWaitTask(a_args[0].GetNumber());
		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(task);
	}

	ConsoleRE::IMenu* SleepWaitMenuEx::Creator()
	{
		ConsoleRE::SleepWaitMenu* menu = static_cast<ConsoleRE::SleepWaitMenu*>(CreateMenu(ConsoleRE::SleepWaitMenu::MENU_NAME));

		ConsoleRE::FxDelegate* dlg = menu->fxDelegate.get();
		_StartSleepWait = dlg->callbacks.GetAlt("OK")->callback;
		dlg->callbacks.GetAlt("OK")->callback = StartSleepWait_Hook;

		return menu;
	}
	void SleepWaitMenuEx::InstallHook()
	{
		//Hook AdvanceMovie
		REL::Relocation<uintptr_t> vTable(Offsets::Menus::SleepWaitMenu::Vtbl);
		_AdvanceMovie = vTable.write_vfunc(0x6, &SleepWaitMenuEx::AdvanceMovie_Hook);

		//Hook ProcessMessage
		_ProcessMessage = vTable.write_vfunc(0x5, &SleepWaitMenuEx::ProcessMessage_Hook);
	}
}
