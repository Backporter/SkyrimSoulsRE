#include "Menus/HUDMenuEx.h"

namespace SkyrimSoulsRE
{
	void HUDMenuEx::SetSkyrimSoulsMode(bool a_isEnabled)
	{
		ConsoleRE::InterfaceStrings* interfaceStrings = ConsoleRE::InterfaceStrings::GetSingleton();
		ConsoleRE::UIMessageQueue* msgQueue = ConsoleRE::UIMessageQueue::GetSingleton();
		ConsoleRE::MessageDataFactoryManager* msgFactory = ConsoleRE::MessageDataFactoryManager::GetSingleton();
		Settings* settings = Settings::GetSingleton();

		auto creator = msgFactory->GetCreator(interfaceStrings->hudData);
		ConsoleRE::HUDData* messageData = static_cast<ConsoleRE::HUDData*>(creator->Create());
		messageData->type = static_cast<uint32_t>(23);
		messageData->text = "SkyrimSoulsMode";
		messageData->unk38 = a_isEnabled;

		msgQueue->AddMessage(interfaceStrings->hudMenu, ConsoleRE::UI_MESSAGE_TYPE::kUpdate, messageData);

		if (!settings->disableHUDModifications)
		{
			ConsoleRE::GFxValue StealthMeterInstance;
			ConsoleRE::GFxValue::DisplayInfo displayInfo;
			if (!this->uiMovie->GetVariable(&StealthMeterInstance, "_root.HUDMovieBaseInstance.StealthMeterInstance"))
			{
				settings->disableHUDModifications = true;
				xUtilty::Log::GetSingleton(0)->Write(xUtilty::Log::logLevel::kNone,"Incompatible HUD Menu! Disabling HUD modifications for current session. Please report this so I can add support for the mods you're using.");
				return;
			}

			StealthMeterInstance.GetDisplayInfo(&displayInfo);

			if (a_isEnabled)
			{
				HUDMenuEx::stealthMeterPosX = displayInfo.GetX();
				HUDMenuEx::stealthMeterPosY = displayInfo.GetY();

				displayInfo.SetX(settings->sneakMeterPosX); // 24
				displayInfo.SetY(settings->sneakMeterPosY); // 120
			}
			else
			{
				displayInfo.SetX(HUDMenuEx::stealthMeterPosX);
				displayInfo.SetY(HUDMenuEx::stealthMeterPosY);
			}

			StealthMeterInstance.SetDisplayInfo(displayInfo);
			this->uiMovie->SetVariable("_root.HUDMovieBaseInstance.StealthMeterInstance", &StealthMeterInstance);
		}
	}

	void HUDMenuEx::UpdateHUD()
	{
		for (ConsoleRE::HUDObject * obj : this->objects)
		{
			obj->Update();
		}
	}

	ConsoleRE::IMenu* HUDMenuEx::Creator()
	{
		ConsoleRE::HUDMenu* menu = static_cast<ConsoleRE::HUDMenu*>(CreateMenu(ConsoleRE::HUDMenu::MENU_NAME));

		// MessagesBlock.InventoryMode = true;
		// QuestUpdateBaseInstance.InventoryMode = true;

		ConsoleRE::GFxValue val = true;
		menu->uiMovie->SetVariable("_root.HUDMovieBaseInstance.StealthMeterInstance.SkyrimSoulsMode", &val);
		menu->uiMovie->SetVariable("_root.HUDMovieBaseInstance.StealthMeterInstance.SneakTextHolder.SneakTextClip.SkyrimSoulsMode", &val);
		menu->uiMovie->SetVariable("_root.HUDMovieBaseInstance.StealthMeterInstance.SneakTextHolder.SneakTextClip.SneakTextInstance.SkyrimSoulsMode", &val);
		menu->uiMovie->SetVariable("_root.HUDMovieBaseInstance.QuestUpdateBaseInstance.SkyrimSoulsMode", &val);
		menu->uiMovie->SetVariable("_root.HUDMovieBaseInstance.MessagesBlock.SkyrimSoulsMode", &val);

		return menu;
	}

	void HUDMenuEx::InstallHook()
	{
	}
}
