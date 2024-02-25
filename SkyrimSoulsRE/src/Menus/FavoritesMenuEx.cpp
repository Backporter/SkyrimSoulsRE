#include "Menus/FavoritesMenuEx.h"

namespace SkyrimSoulsRE
{
	void FavoritesMenuEx::ItemSelect_Hook(ConsoleRE::FavoritesMenu* a_this, ConsoleRE::BGSEquipSlot* a_slot)
	{
		class ItemSelectTask : public UnpausedTask
		{
			ConsoleRE::BGSEquipSlot* slot;
		public:
			ItemSelectTask(ConsoleRE::BGSEquipSlot* a_slot)
			{
				this->slot = a_slot;
			}

			void Run() override
			{
				ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();
				ConsoleRE::InterfaceStrings* interfaceStrings = ConsoleRE::InterfaceStrings::GetSingleton();

				if (ui->IsMenuOpen(interfaceStrings->favoritesMenu))
				{
					ConsoleRE::IMenu* menu = ui->GetMenu(interfaceStrings->favoritesMenu).get();

					using func_t = decltype(&FavoritesMenuEx::ItemSelect_Hook);
					REL::Relocation<func_t> func(Offsets::Menus::FavoritesMenu::ItemSelect);
					func(static_cast<ConsoleRE::FavoritesMenu*>(menu), this->slot);
				}
			}
		};

		ItemSelectTask* task = new ItemSelectTask(a_slot);
		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(task);
	}

	ConsoleRE::IMenu* FavoritesMenuEx::Creator()
	{
		return CreateMenu(ConsoleRE::FavoritesMenu::MENU_NAME);
	}

	void FavoritesMenuEx::InstallHook()
	{
		// Fix for hotkeys not working
		REL::safe_write(Offsets::Menus::FavoritesMenu::CanProcess.address() + 0x19, uint16_t(0x9090));

		//
		API::GetTrampoline().write_call<5>(Offsets::Menus::FavoritesMenu::SelectItem_HookFunc_1.address() + 0x1C1, (uintptr_t)ItemSelect_Hook);
		API::GetTrampoline().write_call<5>(Offsets::Menus::FavoritesMenu::SelectItem_HookFunc_1.address() + 0x1D3, (uintptr_t)ItemSelect_Hook);
		
		//
		API::GetTrampoline().write_branch<5>(Offsets::Menus::FavoritesMenu::SelectItem_HookFunc_2.address() + 0x5E, (uintptr_t)ItemSelect_Hook);
	}
}
