#include "ConsoleCommandHooks.h"

namespace SkyrimSoulsRE::ConsoleCommandHooks
{
	class BGSSaveLoadManagerEx : public ConsoleRE::BGSSaveLoadManager
	{
	public:
		bool Save_Impl(int32_t a_deviceID, uint32_t a_outputStats, const char* a_fileName)
		{
			return this->ConsoleRE::BGSSaveLoadManager::Save_Impl(a_deviceID, a_outputStats, a_fileName);
		}
	};

	bool Save_Hook(ConsoleRE::BGSSaveLoadManager* a_this, int32_t a_deviceID, uint32_t a_outputStats, const char* a_fileName)
	{
		class SaveTask : public UnpausedTask
		{
			int32_t deviceID;
			uint32_t outputStats;
			std::string fileName;
		public:

			SaveTask(int32_t a_deviceID, uint32_t a_outputStats, std::string a_fileName)
			{
				this->deviceID = a_deviceID;
				this->outputStats = a_outputStats;
				this->fileName = a_fileName;
				this->usesDelay = true;
				this->beginTime = std::chrono::high_resolution_clock::now();
				this->delayTimeMS = std::chrono::milliseconds(Settings::GetSingleton()->saveDelayMS);
			}

			void Run() override
			{
				BGSSaveLoadManagerEx* saveLoadManager = static_cast<BGSSaveLoadManagerEx*>(ConsoleRE::BGSSaveLoadManager::GetSingleton());
				if (saveLoadManager->Save_Impl(deviceID, outputStats, fileName.c_str()))
				{
					ConsoleRE::ConsoleLog::GetSingleton()->Print("Saved.");
				}
				else
				{
					ConsoleRE::ConsoleLog::GetSingleton()->Print("Save failed.");
				}
				
				ConsoleRE::UI::GetSingleton()->numPausesGame--;
			}
		};

		// Create save screenshot
		reinterpret_cast<void(*)()>(Offsets::Misc::CreateSaveScreenshot.address())();
		ConsoleRE::UI::GetSingleton()->numPausesGame++;

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(new SaveTask(a_deviceID, a_outputStats, std::string(a_fileName)));

		return true;
	}

	bool ServeTime_Hook(ConsoleRE::PlayerCharacter* a_player)
	{
		class ServeTimeTask : public UnpausedTask
		{
		public:

			ServeTimeTask() {}

			void Run() override
			{
				ConsoleRE::PlayerCharacter* player = ConsoleRE::PlayerCharacter::GetSingleton();
				return player->ServePrisonTime();
			}
		};

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(new ServeTimeTask());

		return true;
	}

	bool CenterOnCell_Hook(ConsoleRE::PlayerCharacter* a_player, const char* a_cellName, ConsoleRE::TESObjectCELL* a_cell)
	{
		class CenterOnCellTask : public UnpausedTask
		{
			std::string cellName;
			ConsoleRE::TESObjectCELL* cell;

		public:

			CenterOnCellTask(std::string a_cellName, ConsoleRE::TESObjectCELL* a_cell)
			{
				this->cellName = a_cellName;
				this->cell = a_cell;
			}

			void Run() override
			{
				ConsoleRE::PlayerCharacter* player = ConsoleRE::PlayerCharacter::GetSingleton();		
				
				if (cellName == "")
				{
					player->CenterOnCell(cell);
				}
				else
				{
					player->CenterOnCell(cellName.c_str());
				}
			}
		};
		std::string cellName = a_cellName ? std::string(a_cellName) : "";

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(new CenterOnCellTask(cellName, a_cell));

		return true;
	}

	void InstallHook()
	{
		auto& trampoline = API::GetTrampoline();

		trampoline.write_call<5>(Offsets::ConsoleCommands::CenterOnCell_Hook.address() + 0x5F, CenterOnCell_Hook);
		trampoline.write_call<5>(Offsets::ConsoleCommands::CenterOnWorld_Hook.address() + 0x191, CenterOnCell_Hook);
		trampoline.write_call<5>(Offsets::ConsoleCommands::CenterOnExterior_Hook.address() + 0x191, CenterOnCell_Hook);
		trampoline.write_call<5>(Offsets::ConsoleCommands::ServeTime_Hook.address() + 0x11, ServeTime_Hook);
		
		REL::safe_write(Offsets::ConsoleCommands::ServeTime_Hook.address() + 0x11 + 0x5, uint8_t(0x90));
		trampoline.write_call<5>(Offsets::ConsoleCommands::SaveGame_Hook.address() + 0xF1, Save_Hook);
	}
}
