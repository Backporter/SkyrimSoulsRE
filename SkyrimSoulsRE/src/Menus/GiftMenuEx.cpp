#include "Menus/GiftMenuEx.h"

namespace SkyrimSoulsRE
{
	ConsoleRE::UI_MESSAGE_RESULTS GiftMenuEx::ProcessMessage_Hook(ConsoleRE::UIMessage& a_message)
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

	void GiftMenuEx::AdvanceMovie_Hook(float a_interval, uint32_t a_currentTime)
	{
		this->UpdateBottomBar();

		AutoCloseManager::GetSingleton()->CheckAutoClose(ConsoleRE::GiftMenu::MENU_NAME);

		return _AdvanceMovie(this, a_interval, a_currentTime);
	}

	void GiftMenuEx::ItemSelect_Hook(const ConsoleRE::FxDelegateArgs& a_args)
	{
		class ItemSelectTask : public UnpausedTask
		{
			double	amount;
		public:
			ItemSelectTask(double a_amount)
			{
				this->amount = a_amount;
			}

			void Run() override
			{
				ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();

				if (ui->IsMenuOpen(ConsoleRE::GiftMenu::MENU_NAME))
				{
					ConsoleRE::IMenu* menu = ui->GetMenu(ConsoleRE::GiftMenu::MENU_NAME).get();

					ConsoleRE::GFxValue arg = this->amount;
					const ConsoleRE::FxDelegateArgs args(0, menu, menu->uiMovie.get(), &arg, 1);
					_ItemSelect(args);
				}
			}
		};

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(new ItemSelectTask(a_args[0].GetNumber()));
	}

	void GiftMenuEx::UpdateBottomBar()
	{
		using func_t = decltype(&GiftMenuEx::UpdateBottomBar);
		REL::Relocation<func_t> func(Offsets::Menus::GiftMenu::UpdateBottomBar);
		return func(this);
	}

	ConsoleRE::IMenu* GiftMenuEx::Creator()
	{
		ConsoleRE::GiftMenu* menu = static_cast<ConsoleRE::GiftMenu*>(CreateMenu(ConsoleRE::GiftMenu::MENU_NAME));

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
		autoCloseManager->InitAutoClose(ConsoleRE::ContainerMenu::MENU_NAME, ref, true);

		return menu;
	}

	void GiftMenuEx::InstallHook()
	{
		REL::Relocation<uintptr_t> vTable(Offsets::Menus::GiftMenu::Vtbl);
		_ProcessMessage= vTable.write_vfunc(0x5, &GiftMenuEx::ProcessMessage_Hook);
		_AdvanceMovie = vTable.write_vfunc(0x6, &GiftMenuEx::AdvanceMovie_Hook);
	}
}
