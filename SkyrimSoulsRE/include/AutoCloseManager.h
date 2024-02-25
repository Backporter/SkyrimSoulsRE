#pragma once

#include <map>

namespace SkyrimSoulsRE
{
	class AutoCloseManager
	{
	public:
		void CheckAutoClose(ConsoleRE::BSFixedString a_menuName);
		void InitAutoClose(ConsoleRE::BSFixedString a_menuName, ConsoleRE::TESObjectREFR* a_ref, bool a_checkForDeath);

		static AutoCloseManager* GetSingleton();

	private:
		struct AutoCloseData
		{
			ConsoleRE::TESObjectREFR* target = nullptr;
			bool initiallyDisabled = false;
			float initialDistance = 0.0f;
			float minDistance = 0.0f;
			bool checkForDeath = false;
			bool dialogueMode = false;
		};

		template <class T>
		bool CheckMenuStack(AutoCloseData* a_inData, ConsoleRE::TESObjectREFR* a_ref)
		{
			ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();
			if (ui->IsMenuOpen(T::MENU_NAME) && _autoCloseDataMap.find(T::MENU_NAME.data()) != _autoCloseDataMap.end()) {
				AutoCloseData* data = _autoCloseDataMap.at(T::MENU_NAME.data());
				if (data->target == a_ref) {
					a_inData->target = data->target;
					a_inData->initialDistance = data->initialDistance;
					a_inData->minDistance = data->minDistance;
					a_inData->initiallyDisabled = data->initiallyDisabled;
					a_inData->checkForDeath = data->checkForDeath;
					a_inData->dialogueMode = data->dialogueMode;
					return true;
				}
			}
			return false;
		}

		std::map<std::string, AutoCloseData*> _autoCloseDataMap;

		static AutoCloseManager* _singleton;

		float GetDistance(ConsoleRE::NiPoint3 a_playerPos, float a_playerHeight, ConsoleRE::NiPoint3 a_refPos);
	};
}
