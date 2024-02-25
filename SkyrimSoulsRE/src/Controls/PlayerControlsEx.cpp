#include "Controls/PlayerControlsEx.h"
#include "Controls/InputHandlerEx.h"

namespace SkyrimSoulsRE
{
	void PlayerControlsEx::InstallHook()
	{
		InputHandlerEx<ConsoleRE::ActivateHandler>::	InstallHook(REL::Relocation<uintptr_t>(ConsoleRE::Offset::ActivateHandler::Vtbl),		0x2);
		InputHandlerEx<ConsoleRE::AttackBlockHandler>::	InstallHook(REL::Relocation<uintptr_t>(ConsoleRE::Offset::AttackBlockHandler::Vtbl),	0x2);
		InputHandlerEx<ConsoleRE::AutoMoveHandler>::	InstallHook(REL::Relocation<uintptr_t>(ConsoleRE::Offset::AutoMoveHandler::Vtbl),		0x2);
		InputHandlerEx<ConsoleRE::JumpHandler>::		InstallHook(REL::Relocation<uintptr_t>(ConsoleRE::Offset::JumpHandler::Vtbl),			0x2);
		InputHandlerEx<ConsoleRE::LookHandler>::		InstallHook(REL::Relocation<uintptr_t>(ConsoleRE::Offset::LookHandler::Vtbl),			0x2);
		InputHandlerEx<ConsoleRE::MovementHandler>::	InstallHook(REL::Relocation<uintptr_t>(ConsoleRE::Offset::MovementHandler::Vtbl),		0x2);
		InputHandlerEx<ConsoleRE::ReadyWeaponHandler>::	InstallHook(REL::Relocation<uintptr_t>(ConsoleRE::Offset::ReadyWeaponHandler::Vtbl),	0x2);
		InputHandlerEx<ConsoleRE::RunHandler>::			InstallHook(REL::Relocation<uintptr_t>(ConsoleRE::Offset::RunHandler::Vtbl),			0x2);
		InputHandlerEx<ConsoleRE::ShoutHandler>::		InstallHook(REL::Relocation<uintptr_t>(ConsoleRE::Offset::ShoutHandler::Vtbl),			0x2);
		InputHandlerEx<ConsoleRE::SneakHandler>::		InstallHook(REL::Relocation<uintptr_t>(ConsoleRE::Offset::SneakHandler::Vtbl),			0x2);
		InputHandlerEx<ConsoleRE::SprintHandler>::		InstallHook(REL::Relocation<uintptr_t>(ConsoleRE::Offset::SprintHandler::Vtbl),			0x2);
		InputHandlerEx<ConsoleRE::TogglePOVHandler>::	InstallHook(REL::Relocation<uintptr_t>(ConsoleRE::Offset::TogglePOVHandler::Vtbl),		0x2);
		InputHandlerEx<ConsoleRE::ToggleRunHandler>::	InstallHook(REL::Relocation<uintptr_t>(ConsoleRE::Offset::ToggleRunHandler::Vtbl),		0x2);

		//
		InputHandlerEx<ConsoleRE::FirstPersonState>::	InstallHook(REL::Relocation<uintptr_t>(ConsoleRE::Offset::FirstPersonState::Vtbl),		0x10);
		InputHandlerEx<ConsoleRE::ThirdPersonState>::	InstallHook(REL::Relocation<uintptr_t>(ConsoleRE::Offset::ThirdPersonState::Vtbl),		0x17);
	}
}
