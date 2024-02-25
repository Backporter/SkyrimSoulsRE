#include "Menus/MessageBoxMenuEx.h"

namespace SkyrimSoulsRE
{
	void MessageBoxMenuEx::ButtonPress_Hook(const ConsoleRE::FxDelegateArgs& a_args)
	{
		class ButtonPressTask : public UnpausedTask
		{
			double selectedIndex;
		public:
			ButtonPressTask(double a_selectedIndex)
			{
				this->selectedIndex = a_selectedIndex;
				this->beginTime = std::chrono::high_resolution_clock::now();
				this->delayTimeMS = std::chrono::milliseconds(0);
			}

			void Run() override
			{
				ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();

				if (ui->IsMenuOpen(ConsoleRE::MessageBoxMenu::MENU_NAME))
				{
					ConsoleRE::MessageBoxMenu* menu = static_cast<ConsoleRE::MessageBoxMenu*>(ui->GetMenu(ConsoleRE::MessageBoxMenu::MENU_NAME).get());
					ConsoleRE::GFxValue index = this->selectedIndex;
					ConsoleRE::FxDelegateArgs args(0, menu, menu->uiMovie.get(), &index, 1);
					_ButtonPress(args);
				}
			}
		};

		if (a_args.GetArgCount() == 1 && a_args[0].IsNumber())
		{
			UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
			ButtonPressTask* task = new ButtonPressTask(a_args[0].GetNumber());
			queue->AddTask(task);
		}
	}

	void MessageBoxMenuEx::AdvanceMovie_Hook(float a_interval, uint32_t a_currentTime)
	{
		HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(ConsoleRE::UI::GetSingleton()->GetMenu(ConsoleRE::HUDMenu::MENU_NAME).get());
		if (hudMenu)
		{
			hudMenu->UpdateHUD();
		}

		return _AdvanceMovie(this, a_interval, a_currentTime);
	}

	ConsoleRE::IMenu* MessageBoxMenuEx::Creator()
	{
		ConsoleRE::MessageBoxMenu* menu = static_cast<ConsoleRE::MessageBoxMenu*>(CreateMenu(ConsoleRE::MessageBoxMenu::MENU_NAME));

		ConsoleRE::FxDelegate* dlg = menu->fxDelegate.get();
		_ButtonPress = dlg->callbacks.GetAlt("buttonPress")->callback;
		dlg->callbacks.GetAlt("buttonPress")->callback = ButtonPress_Hook;

		return menu;
	}

	void MessageBoxMenuEx::InstallHook()
	{
		//Hook AdvanceMovie
		REL::Relocation<uintptr_t> vTable(Offsets::Menus::MessageBoxMenu::Vtbl);
		_AdvanceMovie = vTable.write_vfunc(0x6, &MessageBoxMenuEx::AdvanceMovie_Hook);
	}
}
