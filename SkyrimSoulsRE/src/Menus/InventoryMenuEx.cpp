#include "Menus/InventoryMenuEx.h"

namespace SkyrimSoulsRE
{
	ConsoleRE::UI_MESSAGE_RESULTS InventoryMenuEx::ProcessMessage_Hook(ConsoleRE::UIMessage& a_message)
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
	void InventoryMenuEx::AdvanceMovie_Hook(float a_interval, uint32_t a_currentTime)
	{
		this->UpdateBottomBar();
		return _AdvanceMovie(this, a_interval, a_currentTime);
	}

	void InventoryMenuEx::UpdateBottomBar()
	{
		using func_t = decltype(&InventoryMenuEx::UpdateBottomBar);
		REL::Relocation<func_t> func(Offsets::Menus::InventoryMenu::UpdateBottomBar);
		return func(this);
	}

	void InventoryMenuEx::ItemDrop_Hook(const ConsoleRE::FxDelegateArgs& a_args)
	{
		class ItemDropTask : public UnpausedTask
		{
			double count;
		public:
			ItemDropTask(double a_count)
			{
				this->count = a_count;
			}

			void Run() override
			{
				ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();

				if (ui->IsMenuOpen(ConsoleRE::InventoryMenu::MENU_NAME))
				{
					ConsoleRE::IMenu* menu = ui->GetMenu(ConsoleRE::InventoryMenu::MENU_NAME).get();

					ConsoleRE::GFxValue arg = this->count;
					const ConsoleRE::FxDelegateArgs args(0, menu, menu->uiMovie.get(), &arg, 1);
					_ItemDrop(args);
				}
			}
		};

		ItemDropTask* task = new ItemDropTask(a_args[0].GetNumber());
		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(task);
	}

	void InventoryMenuEx::ItemSelect_Hook(const ConsoleRE::FxDelegateArgs& a_args)
	{
		class ItemSelectTask : public UnpausedTask
		{
			bool	hasSlot;
			double	slot;
		public:
			ItemSelectTask(bool a_hasSlot, double a_slot = 0.0)
			{
				this->hasSlot = a_hasSlot;
				this->slot = a_slot;			
			}

			void Run() override
			{
				ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();

				if (ui->IsMenuOpen(ConsoleRE::InventoryMenu::MENU_NAME))
				{
					ConsoleRE::IMenu* menu = ui->GetMenu(ConsoleRE::InventoryMenu::MENU_NAME).get();

					if (this->hasSlot)
					{
						ConsoleRE::GFxValue arg = this->slot;
						const ConsoleRE::FxDelegateArgs args(0, menu, menu->uiMovie.get(), &arg, 1);
						_ItemSelect(args);
					}
					else
					{
						const ConsoleRE::FxDelegateArgs args(0, menu, menu->uiMovie.get(), nullptr, 0);
						_ItemSelect(args);
					}
				}
			}
		};

		ItemSelectTask* task;

		if (a_args.GetArgCount() == 0)
		{
			task = new ItemSelectTask(false);
		}
		else
		{
			task = new ItemSelectTask(true, a_args[0].GetNumber());
		}

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(task);
	}

	ConsoleRE::IMenu* InventoryMenuEx::Creator()
	{
		ConsoleRE::InventoryMenu* menu = static_cast<ConsoleRE::InventoryMenu*>(CreateMenu(ConsoleRE::InventoryMenu::MENU_NAME));

		ConsoleRE::FxDelegate* dlg = menu->fxDelegate.get();

		_ItemSelect = dlg->callbacks.GetAlt("ItemSelect")->callback;
		dlg->callbacks.GetAlt("ItemSelect")->callback = ItemSelect_Hook;
		_ItemDrop = dlg->callbacks.GetAlt("ItemDrop")->callback;
		dlg->callbacks.GetAlt("ItemDrop")->callback = ItemDrop_Hook;

		HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(ConsoleRE::UI::GetSingleton()->GetMenu(ConsoleRE::InterfaceStrings::GetSingleton()->hudMenu).get());
		if (hudMenu)
		{
			hudMenu->SetSkyrimSoulsMode(true);
		}

		return menu;
	}

	void InventoryMenuEx::InstallHook()
	{
		//Hook AdvanceMovie
		REL::Relocation<uintptr_t> vTable(Offsets::Menus::InventoryMenu::Vtbl);
		_ProcessMessage = vTable.write_vfunc(0x5, &InventoryMenuEx::ProcessMessage_Hook);
		_AdvanceMovie = vTable.write_vfunc(0x6, &InventoryMenuEx::AdvanceMovie_Hook);
	}
};
