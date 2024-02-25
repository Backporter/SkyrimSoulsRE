#include "Controls/CameraMovement.h"

#include "../../../OrbisUtil/Third-Party/herumi/xbayk/6.00/xbyak.h"

namespace SkyrimSoulsRE::CameraMovement
{
	bool CameraMove_Hook(bool a_result)
	{
		Settings* settings = Settings::GetSingleton();
		ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();
		ConsoleRE::PlayerControls* pc = ConsoleRE::PlayerControls::GetSingleton();
		ConsoleRE::ControlMap* controlMap = ConsoleRE::ControlMap::GetSingleton();
		ConsoleRE::MenuControls* mc = ConsoleRE::MenuControls::GetSingleton();

		if (!ui->GameIsPaused() && !ui->IsMenuOpen(ConsoleRE::DialogueMenu::MENU_NAME) && settings->enableCursorCameraMove && GetUnpausedMenuCount() && !IsFullScreenMenuOpen() && !mc->remapMode && pc->lookHandler->IsInputEventHandlingEnabled() && controlMap->IsLookingControlsEnabled())
		{
			ConsoleRE::NiPoint2* cursorPosition = reinterpret_cast<ConsoleRE::NiPoint2*>(Offsets::Misc::CursorPosition.address());
			auto* prefs = ConsoleRE::INISettingCollection::GetSingleton(); // was "INIPrefSettingCollection" but the PS4 version uses a single INI aka "Skyrim.ini" so the value wouldn't exists in that one but would exist in INISettingCollection

			uint32_t resX = prefs->GetSetting("iSize W:DISPLAY")->GetUInt32();
			uint32_t resY = prefs->GetSetting("iSize H:DISPLAY")->GetUInt32();

			float speedX = settings->cursorCameraHorizontalSpeed;
			float speedY = settings->cursorCameraVerticalSpeed;

			if (cursorPosition->x == 0)
			{
				pc->data.lookInputVec.x = -speedX;
			}
			else if (cursorPosition->x == resX)
			{
				pc->data.lookInputVec.x = speedX;
			}

			if (cursorPosition->y == 0)
			{
				pc->data.lookInputVec.y = speedY;
			}
			else if (cursorPosition->y == resY)
			{
				pc->data.lookInputVec.y = -speedY;
			}
		}

		return a_result;
	}

	void InstallHook()
	{
		struct CameraMove_Code : Xbyak::CodeGenerator
		{
			CameraMove_Code(uintptr_t a_hookAddr)
			{
				Xbyak::Label hookAddress;

				//
				pop(r14);
				pop(r15);
				pop(rbp);

				//
				mov(rdi, rax);
				jmp(ptr[rip + hookAddress]);

				L(hookAddress);
				dq(a_hookAddr);
			}
		};

		CameraMove_Code code{uintptr_t(CameraMove_Hook)};
		void* codeLoc = API::GetTrampoline().allocate(code);

		API::GetTrampoline().write_branch<5>(Offsets::Misc::ScreenEdgeCameraMoveHook.address() + 0x47A, codeLoc);
	}
}
