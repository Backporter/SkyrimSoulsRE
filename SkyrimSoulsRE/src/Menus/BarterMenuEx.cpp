#include "Menus/BarterMenuEx.h"

namespace SkyrimSoulsRE
{
	ConsoleRE::UI_MESSAGE_RESULTS BarterMenuEx::ProcessMessage_Hook(ConsoleRE::UIMessage& a_message)
	{
		if (a_message.type == ConsoleRE::UI_MESSAGE_TYPE::kHide)
		{
			HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(ConsoleRE::UI::GetSingleton()->GetMenu(ConsoleRE::InterfaceStrings::GetSingleton()->hudMenu).get());
			if (hudMenu)
			{
				hudMenu->SetSkyrimSoulsMode(false);
			}
		}
		return _ProcessMessage(this, a_message);
	}
	void BarterMenuEx::AdvanceMovie_Hook(float a_interval, uint32_t a_currentTime)
	{
		//this->UpdateBottomBar();
		AutoCloseManager::GetSingleton()->CheckAutoClose(ConsoleRE::BarterMenu::MENU_NAME);
		return _AdvanceMovie(this, a_interval, a_currentTime);
	}

	void BarterMenuEx::ItemSelect_Hook(const ConsoleRE::FxDelegateArgs& a_args)
	{
		class ItemSelectTask : public UnpausedTask
		{
			double	count;
			double	value;
			bool	isViewingVendorItems;
		public:
			ItemSelectTask(double a_count, double a_value, bool a_isViewingContainer)
			{
				this->count = a_count;
				this->value = a_value;
				this->isViewingVendorItems = a_isViewingContainer;
			}

			void Run() override
			{
				ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();

				if (ui->IsMenuOpen(ConsoleRE::BarterMenu::MENU_NAME))
				{
					BarterMenuEx* menu = static_cast<BarterMenuEx*>(ui->GetMenu(ConsoleRE::BarterMenu::MENU_NAME).get());

					ConsoleRE::GFxValue arg[3];
					arg[0] = this->count;
					arg[1] = this->value;
					arg[2] = this->isViewingVendorItems;
					const ConsoleRE::FxDelegateArgs args(0, menu, menu->uiMovie.get(), arg, 3);
					BarterMenuEx::_ItemSelect(args);
				}
			}
		};
		ItemSelectTask* task = new ItemSelectTask(a_args[0].GetNumber(), a_args[1].GetNumber(), a_args[2].GetBool());
		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(task);
	}

	void BarterMenuEx::UpdateBottomBar()
	{
		using func_t = decltype(&BarterMenuEx::UpdateBottomBar);
		REL::Relocation<func_t> func(Offsets::Menus::BarterMenu::UpdateBottomBar);
		return func(this);
	}

	ConsoleRE::IMenu* BarterMenuEx::Creator()
	{
		ConsoleRE::BarterMenu* menu = static_cast<ConsoleRE::BarterMenu*>(CreateMenu(ConsoleRE::BarterMenu::MENU_NAME));

		ConsoleRE::FxDelegate* dlg = menu->fxDelegate.get();
		_ItemSelect = dlg->callbacks.GetAlt("ItemSelect")->callback;
		dlg->callbacks.GetAlt("ItemSelect")->callback = ItemSelect_Hook;

		uint32_t handle = menu->GetTargetRefHandle();
		ConsoleRE::TESObjectREFRPtr refptr = nullptr;

		ConsoleRE::TESObjectREFR* ref = nullptr;
		if (ConsoleRE::TESObjectREFR::LookupByHandle(handle, refptr))
		{
			ref = refptr.get();
		}

		HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(ConsoleRE::UI::GetSingleton()->GetMenu(ConsoleRE::InterfaceStrings::GetSingleton()->hudMenu).get());
		if (hudMenu)
		{
			hudMenu->SetSkyrimSoulsMode(true);
		}

		AutoCloseManager* autoCloseManager = AutoCloseManager::GetSingleton();
		autoCloseManager->InitAutoClose(ConsoleRE::BarterMenu::MENU_NAME, ref, true);

		return menu;
	}

	void BarterMenuEx::InstallHook()
	{
		//Hook AdvanceMovie + ProcessMessage
		REL::Relocation<uintptr_t> vTable(Offsets::Menus::BarterMenu::Vtbl);
		_ProcessMessage = vTable.write_vfunc(0x5, &BarterMenuEx::ProcessMessage_Hook);
		_AdvanceMovie = vTable.write_vfunc(0x6, &BarterMenuEx::AdvanceMovie_Hook);
	}
}
