#include "../../../OrbisUtil/include/RelocationManager.h"
#include "../../../OrbisUtil/include/Logger.h"
#include "../../../OrbisUtil/include/FileSystem.h"

extern "C" __declspec (dllexport) int module_start(size_t argc, const void* argv) { xUtilty::RelocationManager(); return 0; }
extern "C" __declspec (dllexport) int module_stop(size_t argc, const void* argv) { return 0; }

#include "EngineFixesChecker.h"
#include "SkyrimSoulsRE.h"
#include "version.h"

void MessageHandler(Interface::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) 
	{
	case Interface::MessagingInterface::kPluginsLoaded:
	{
		SkyrimSoulsRE::Settings* settings = SkyrimSoulsRE::Settings::GetSingleton();

		bool engineFixesPresent = false;
		if (xUtilty::SystemWrapper::GetModuleHandle("EngineFixes.prx")) 
		{
			if (SkyrimSoulsRE::EngineFixesConfig::load_config("/app0/data/SKSE/plugins/EngineFixes.toml")) 
			{
				if (SkyrimSoulsRE::EngineFixesConfig::patchMemoryManager.get() && SkyrimSoulsRE::EngineFixesConfig::fixGlobalTime.get()) 
				{
					xUtilty::Log::GetSingleton(0)->Write(xUtilty::Log::logLevel::kNone,"SSE Engine Fixes detected.");
					engineFixesPresent = true;
				}
			}
		}

		if (!engineFixesPresent) 
		{
			xUtilty::Log::GetSingleton(0)->Write(xUtilty::Log::logLevel::kNone,"SSE Engine Fixes not detected, or certain features are not enabled.");
			xUtilty::Log::GetSingleton(0)->Write(xUtilty::Log::logLevel::kNone,"To ensure best functionality, the following Engine Fixes features must be enabled : Memory Manager patch, Global Time Fix");
			
			if (!settings->hideEngineFixesWarning) 
			{
				xUtilty::Log::GetSingleton(0)->Write(xUtilty::Log::logLevel::kNone, "SSE Engine Fixes not detected, or certain features are not enabled. This will not prevent Skyrim Souls RE from running, but to ensure best functionality, the following Engine Fixes features must be enabled:\n\n- Memory Manager patch\n- Global Time Fix\n\nThe Memory Manager patch prevents the false save corruption bug that tends to happen with this mod, and the Global Time fix fixes the behaviour of some menus when using the slow-motion feature.\n\nIf you can't install SSE Engine Fixes for some reason (you're using Skyrim Together for example), you can disable this warning in the .ini.", "Skyrim Souls RE - Warning");
			}
		}

		if (xUtilty::SystemWrapper::GetModuleHandle("DialogueMovementEnabler.prx"))
		{
			xUtilty::Log::GetSingleton(0)->Write(xUtilty::Log::logLevel::kNone,"Dialogue Movement Enabler detected. Enabling compatibility.");
			settings->isUsingDME = true;
		}
		else
		{
			xUtilty::Log::GetSingleton(0)->Write(xUtilty::Log::logLevel::kNone,"Dialogue Movement Enabler not detected. Disabling compatibility.");
			settings->isUsingDME = false;
		}
		
	}
	break;
	case Interface::MessagingInterface::kDataLoad:
		SkyrimSoulsRE::InstallMenuHooks();
		break;
	}
}

namespace
{
	void InitializeLog()
	{
		//
		xUtilty::Log::GetSingleton(0)->OpenRelitive(xUtilty::FileSystem::Download, "OSEL/Plugins/SkyrimSoulsRE/SkyrimSoulsRE.log");
	}
}

extern "C" 
{
	__declspec(dllexport) bool Query(const Interface::QueryInterface* a_skse, PluginInfo* a_info)
	{
		InitializeLog();

		//
		auto soulsVersion = std::string(SKYRIMSOULSRE_VERSION_VERSTRING) + " - (" + std::string(__TIMESTAMP__) + ")";
		xUtilty::Log::GetSingleton(0)->Write(xUtilty::Log::logLevel::kNone, "Skyrim Souls RE - Updated v%s",  soulsVersion.c_str());

		//
		a_info->SetPluginName("Skyrim Souls RE");
		a_info->SetPluginVersion(SKYRIMSOULSRE_VERSION_MAJOR);

		return true;
	}

	__declspec(dllexport) bool Load(Interface::QueryInterface* a_skse)
	{
		API::Init(a_skse);
		API::AllocTrampoline(1 << 9, true);

		auto messaging = API::GetMessagingInterface();
		if (messaging->RegisterPluginCallback("SELF", MessageHandler))
		{
			xUtilty::Log::GetSingleton(0)->Write(xUtilty::Log::logLevel::kNone,"Messaging interface registration successful.");
		}
		else 
		{
			xUtilty::Log::GetSingleton(0)->Write(xUtilty::Log::logLevel::kNone,"Messaging interface registration failed.");
			// SKSE::WinAPI::MessageBox(nullptr, "Messaging interface registration failed.", "Skyrim Souls RE - Error", MESSAGEBOX_ERROR);
			return false;
		}

		SkyrimSoulsRE::LoadSettings();

		xUtilty::Log::GetSingleton(0)->Write(xUtilty::Log::logLevel::kNone,"Settings loaded.");

		SkyrimSoulsRE::InstallHooks();

		xUtilty::Log::GetSingleton(0)->Write(xUtilty::Log::logLevel::kNone,"Hooks installed.");
		xUtilty::Log::GetSingleton(0)->Write(xUtilty::Log::logLevel::kNone,"Skyrim Souls RE loaded.");

		return true;
	}

	__declspec(dllexport) bool Revert() { return true; }
};
