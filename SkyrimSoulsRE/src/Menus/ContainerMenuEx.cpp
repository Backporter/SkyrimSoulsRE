#include "Menus/ContainerMenuEx.h"

namespace SkyrimSoulsRE
{
	ConsoleRE::UI_MESSAGE_RESULTS ContainerMenuEx::ProcessMessage_Hook(ConsoleRE::UIMessage& a_message)
	{
		if (a_message.type == ConsoleRE::UI_MESSAGE_TYPE::kHide)
		{
			HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(ConsoleRE::UI::GetSingleton()->GetMenu(ConsoleRE::HUDMenu::MENU_NAME).get());
			if (hudMenu)
			{
				hudMenu->SetSkyrimSoulsMode(false);
			}
		}
		return _ProcessMessage(this, a_message);
	}

	void ContainerMenuEx::AdvanceMovie_Hook(float a_interval, uint32_t a_currentTime)
	{
		this->UpdateBottomBar();
		if (this->GetContainerMode() == ConsoleRE::ContainerMenu::ContainerMode::kPickpocket)
		{
			this->UpdatePickpocketChance();
		}

		AutoCloseManager::GetSingleton()->CheckAutoClose(ConsoleRE::ContainerMenu::MENU_NAME);

		return _AdvanceMovie(this, a_interval, a_currentTime);
	}

	int32_t ContainerMenuEx::CalcPickPocketChance(ConsoleRE::StandardItemData* a_itemData)
	{
		ConsoleRE::PlayerCharacter* player = ConsoleRE::PlayerCharacter::GetSingleton();

		ConsoleRE::Actor* targetActor = static_cast<ConsoleRE::Actor*>(ContainerMenuEx::containerRef);

		float itemWeight = a_itemData->objDesc->GetWeight();
		uint32_t count = 1;

		if (a_itemData->objDesc->object->IsAmmo() || a_itemData->objDesc->object->IsLockpick() || a_itemData->objDesc->object->IsGold())
		{
			count = a_itemData->GetCount();
			itemWeight = -1.0f;
		}

		uint32_t stealValue = targetActor->GetStealValue(a_itemData->objDesc, count, true);
		
		bool isDetected = targetActor->RequestDetectionLevel(player, static_cast<uint32_t>(ConsoleRE::DETECTION_PRIORITY::kNormal)) > 0;
		float playerSkill = player->GetActorValue(ConsoleRE::ActorValue::kPickpocket);
		float targetSkill = targetActor->GetActorValue(ConsoleRE::ActorValue::kPickpocket);

		auto chance = ConsoleRE::AIFormulas::ComputePickpocketSuccess(playerSkill, targetSkill, stealValue, itemWeight, player, targetActor, isDetected, a_itemData->objDesc->object);

		if (chance > 100) {
			chance = 100;
		}
		else if (chance < 0) {
			chance = 0;
		}

		return chance;
	}

	void ContainerMenuEx::UpdatePickpocketChance()
	{
		if (containerRef && containerRef->formType == static_cast<uint32_t>(ConsoleRE::FormType::ActorCharacter))
		{
			ConsoleRE::ItemList::Item* item = this->itemList->GetSelectedItem();
			if (item)
			{
				int32_t chance = CalcPickPocketChance(&item->data);

				ConsoleRE::GFxValue isViewingContainer;

				if (!this->uiMovie->Invoke("_root.Menu_mc.isViewingContainer", &isViewingContainer, nullptr, 0))
					return;

				std::wstring desc;
				desc = isViewingContainer.GetBool() ? toSteal : toPlace;

				std::wstring stealText(L"<font face=\'$EverywhereBoldFont\' size=\'24\' color=\'#FFFFFF\'>" + std::to_wstring(chance) + L"%</font>" + desc);

				ConsoleRE::GFxValue stealTextObj;
				ConsoleRE::GFxValue newText(stealText.c_str());
				if (this->uiMovie->GetVariable(&stealTextObj, "_root.Menu_mc.itemCardFadeHolder.StealTextInstance.PercentTextInstance"))
				{
					//SkyUI
					stealTextObj.SetMember("htmlText", newText);
				}
				else if (this->uiMovie->GetVariable(&stealTextObj, "_root.Menu_mc.ItemCardFadeHolder_mc.StealTextInstance.PercentTextInstance"))
				{
					//vanilla
					stealTextObj.SetMember("htmlText", newText);
				}
			}
		}
	}

	void ContainerMenuEx::UpdateBottomBar()
	{
		using func_t = decltype(&ContainerMenuEx::UpdateBottomBar);
		REL::Relocation<func_t> func(Offsets::Menus::ContainerMenu::UpdateBottomBar);
		return func(this);
	}

	void ContainerMenuEx::EquipItem_Hook(const ConsoleRE::FxDelegateArgs& a_args)
	{
		class EquipItemTask : public UnpausedTask
		{
			double		equipHand;
			bool		hasCount;
			double		count;

		public:
			EquipItemTask(double a_equipHand, bool a_hasCount, double a_count = 0)
			{
				this->equipHand = a_equipHand;
				this->hasCount = a_hasCount;
				this->count = a_count;
				this->usesDelay = false;
			}

			void Run() override
			{
				ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();

				if (ui->IsMenuOpen(ConsoleRE::ContainerMenu::MENU_NAME))
				{
					ContainerMenuEx* menu = static_cast<ContainerMenuEx*>(ui->GetMenu(ConsoleRE::ContainerMenu::MENU_NAME).get());
					ConsoleRE::ItemList::Item* selectedItem = menu->itemList->GetSelectedItem();

					if (selectedItem)
					{
						if (this->hasCount)
						{
							ConsoleRE::GFxValue arg[2];
							arg[0] = this->equipHand;
							arg[1] = this->count;
							const ConsoleRE::FxDelegateArgs args(0, menu, menu->uiMovie.get(), arg, 2);
							ContainerMenuEx::_EquipItem(args);
						}
						else
						{
							ConsoleRE::GFxValue arg = this->equipHand;
							const ConsoleRE::FxDelegateArgs args(0, menu, menu->uiMovie.get(), &arg, 2);
							ContainerMenuEx::_EquipItem(args);
						}

						if (menu->GetContainerMode() == ConsoleRE::ContainerMenu::ContainerMode::kSteal && menu->value != 0)
						{
							ConsoleRE::PlayerCharacter::GetSingleton()->StealAlarm(containerRef, selectedItem->data.objDesc->object, static_cast<int32_t>(count), selectedItem->data.objDesc->GetValue(), containerRef->GetOwner(), true);
							menu->value = 0;
						}
					}
				}
			}
		};

		EquipItemTask* task;
		if (a_args.GetArgCount() == 1)
		{
			task = new EquipItemTask(a_args[0].GetNumber(), false);
		}
		else
		{
			task = new EquipItemTask(a_args[0].GetNumber(), true, a_args[1].GetNumber());
		}
		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(task);
	}

	void ContainerMenuEx::TakeAllItems_Hook(const ConsoleRE::FxDelegateArgs& a_args)
	{
		class TakeAllItemsTask : public UnpausedTask
		{
		public:
			TakeAllItemsTask()
			{
				this->usesDelay = false;
			}

			void Run() override
			{
				ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();

				if (ui->IsMenuOpen(ConsoleRE::ContainerMenu::MENU_NAME))
				{
					ContainerMenuEx* menu = static_cast<ContainerMenuEx*>(ui->GetMenu(ConsoleRE::ContainerMenu::MENU_NAME).get());

					const ConsoleRE::FxDelegateArgs args(0, menu, menu->uiMovie.get(), nullptr, 0);
					ContainerMenuEx::_TakeAllItems(args);
				}
			}
		};
		TakeAllItemsTask* task = new TakeAllItemsTask();
		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(task);
	}

	void ContainerMenuEx::TransferItem_Hook(const ConsoleRE::FxDelegateArgs& a_args)
	{
		class TransferItemTask : public UnpausedTask
		{
			double	count;
			bool	isViewingContainer;
		public:
			TransferItemTask(double a_count, bool a_isViewingContainer)
			{
				this->count = a_count;
				this->isViewingContainer = a_isViewingContainer;
				this->usesDelay = false;
			}

			void Run() override
			{
				ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();

				if (ui->IsMenuOpen(ConsoleRE::ContainerMenu::MENU_NAME))
				{
					ContainerMenuEx* menu = static_cast<ContainerMenuEx*>(ui->GetMenu(ConsoleRE::ContainerMenu::MENU_NAME).get());

					ConsoleRE::ItemList::Item* selectedItem = menu->itemList->GetSelectedItem();

					if (selectedItem)
					{
						ConsoleRE::GFxValue arg[2];
						arg[0] = this->count;
						arg[1] = this->isViewingContainer;
						const ConsoleRE::FxDelegateArgs args(0, menu, menu->uiMovie.get(), arg, 2);
						ContainerMenuEx::_TransferItem(args);

						if (menu->GetContainerMode() == ConsoleRE::ContainerMenu::ContainerMode::kSteal && menu->value != 0)
						{
							ConsoleRE::PlayerCharacter::GetSingleton()->StealAlarm(containerRef, selectedItem->data.objDesc->object, static_cast<int32_t>(count), selectedItem->data.objDesc->GetValue(), containerRef->GetOwner(), true);
							menu->value = 0;
						}
					}
				}
			}
		};
		TransferItemTask* task = new TransferItemTask(a_args[0].GetNumber(), a_args[1].GetBool());
		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(task);
	}

	ConsoleRE::IMenu* ContainerMenuEx::Creator()
	{
		ConsoleRE::ContainerMenu* menu = static_cast<ConsoleRE::ContainerMenu*>(CreateMenu(ConsoleRE::ContainerMenu::MENU_NAME));

		ConsoleRE::FxDelegate* dlg = menu->fxDelegate.get();

		_TransferItem = dlg->callbacks.GetAlt("ItemTransfer")->callback;
		dlg->callbacks.GetAlt("ItemTransfer")->callback = TransferItem_Hook;

		_EquipItem = dlg->callbacks.GetAlt("EquipItem")->callback;
		dlg->callbacks.GetAlt("EquipItem")->callback = EquipItem_Hook;

		_TakeAllItems = dlg->callbacks.GetAlt("TakeAllItems")->callback;
		dlg->callbacks.GetAlt("TakeAllItems")->callback = TakeAllItems_Hook;

		HUDMenuEx* hudMenu = static_cast<HUDMenuEx*>(ConsoleRE::UI::GetSingleton()->GetMenu(ConsoleRE::HUDMenu::MENU_NAME).get());
		if (hudMenu)
		{
			hudMenu->SetSkyrimSoulsMode(true);
		}

		uint32_t handle = menu->GetTargetRefHandle();
		ConsoleRE::TESObjectREFRPtr refptr = nullptr;
		ConsoleRE::TESObjectREFR* ref = nullptr;
		if (ConsoleRE::TESObjectREFR::LookupByHandle(handle, refptr))
		{
			ref = refptr.get();
		}

		containerRef = ref;

		AutoCloseManager* autoCloseManager = AutoCloseManager::GetSingleton();		
		autoCloseManager->InitAutoClose(ConsoleRE::ContainerMenu::MENU_NAME, ref, menu->GetContainerMode() == ConsoleRE::ContainerMenu::ContainerMode::kPickpocket);

		return menu;
	}

	void ContainerMenuEx::InstallHook()
	{
		REL::Relocation<uintptr_t> vTable(Offsets::Menus::ContainerMenu::Vtbl);
		_ProcessMessage = vTable.write_vfunc(0x5, &ContainerMenuEx::ProcessMessage_Hook);
		_AdvanceMovie = vTable.write_vfunc(0x6, &ContainerMenuEx::AdvanceMovie_Hook);
	}

	void ContainerMenuEx::ParseTranslations()
	{
		toSteal = L"$ TO STEAL";
		toPlace = L"$ TO PLACE";

		bool foundToSteal = false;
		bool foundToPlace = false;

		ConsoleRE::Setting* language = ConsoleRE::INISettingCollection::GetSingleton()->GetSetting("sLanguage:General");
		std::string path = "Interface\\";

		// Construct translation filename
		path += "Translate_";
		path += (language && language->GetType() == ConsoleRE::Setting::Type::kString) ? language->data.s : "ENGLISH";
		path += ".txt";

		xUtilty::Log::GetSingleton(0)->Write(xUtilty::Log::logLevel::kNone, ("Reading translations from " + path + "...").c_str());

		ConsoleRE::BSResourceNiBinaryStream fileStream(path.c_str());
		if (!fileStream.good())
		{
			xUtilty::Log::GetSingleton(0)->Write(xUtilty::Log::logLevel::kNone, ("Failed to read file " + path + ". Aborting.").c_str());
			return;
		}

		wchar_t bom = 0;
		bool ret = fileStream.read(&bom, 1);
		if (!ret)
		{
			xUtilty::Log::GetSingleton(0)->Write(xUtilty::Log::logLevel::kNone,"Empty translation file. Aborting.");
			return;
		}

		if (bom != L'\xFEFF')
		{
			xUtilty::Log::GetSingleton(0)->Write(xUtilty::Log::logLevel::kNone,"BOM Error, file must be encoded in UCS-2 LE. Aborting.");
			return;
		}

		while (!(foundToSteal && foundToPlace))
		{
			std::wstring str;

			bool notEOF = std::getline(fileStream, str);
			if (!notEOF) // End of file
			{
				xUtilty::Log::GetSingleton(0)->Write(xUtilty::Log::logLevel::kNone,"Unexpected end of file.");
				break;
			}

			size_t len = str.length();

			wchar_t last = str.at(len - 1);
			if (last == '\r')
				len--;

			size_t delimIdx = 0;
			for (size_t i = 0; i < len; ++i)
			{
				if (str.at(i) == L'\t')
				{
					delimIdx = i;
					break;
				}
			}

			if (delimIdx == 0)
				continue;

			std::wstring key = std::wstring{ str.substr(0, delimIdx) };
			std::wstring translation = std::wstring{ str.substr(delimIdx + 1, len - delimIdx - 1) };

			if (key == L"$ TO PLACE")
			{
				foundToPlace = true;
				toPlace = translation;
			}
			else if (key == L"$ TO STEAL")
			{
				foundToSteal = true;
				toSteal = translation;
			}
		}

		if (!foundToPlace)
		{
			xUtilty::Log::GetSingleton(0)->Write(xUtilty::Log::logLevel::kNone,"Failed to find translation for \"$ TO PLACE\".");
		}	

		if (!foundToSteal)
		{
			xUtilty::Log::GetSingleton(0)->Write(xUtilty::Log::logLevel::kNone,"Failed to find translation for \"$ TO STEAL\".");
		}	

		xUtilty::Log::GetSingleton(0)->Write(xUtilty::Log::logLevel::kNone,"Reading translations finished.");
	}
}
