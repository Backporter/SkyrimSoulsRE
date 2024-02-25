#include "AudioHooks.h"

namespace SkyrimSoulsRE::Audio
{
	void QuaternionToMatrix(ConsoleRE::NiPoint3& a_v1, ConsoleRE::NiPoint3& a_v2, ConsoleRE::NiQuaternion a_quaternion)
	{
		// https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm
		// ( - | v1.x | v2.x )
		// ( - | v1.y | v2.y )
		// ( - | v1.z | v2.z )

		a_v1.x = 2 * a_quaternion.x * a_quaternion.y - 2 * a_quaternion.z * a_quaternion.w;
		a_v1.y = 1 - 2 * a_quaternion.x * a_quaternion.x - 2 * a_quaternion.z * a_quaternion.z;
		a_v1.z = 2 * a_quaternion.y * a_quaternion.z + 2 * a_quaternion.x * a_quaternion.w;

		a_v2.x = 2 * a_quaternion.x * a_quaternion.z + 2 * a_quaternion.y * a_quaternion.w;
		a_v2.y = 2 * a_quaternion.y * a_quaternion.z - 2 * a_quaternion.x * a_quaternion.w;
		a_v2.z = 1 - 2 * a_quaternion.x * a_quaternion.x - 2 * a_quaternion.y * a_quaternion.y;
	}

	void SetListenerPosition_Hook(ConsoleRE::BSAudioManager* a_audioManager, ConsoleRE::NiPoint3* a_pos)
	{
		ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();
		if (ui->IsMenuOpen(ConsoleRE::MapMenu::MENU_NAME))
		{
			ConsoleRE::PlayerCamera* camera = ConsoleRE::PlayerCamera::GetSingleton();
			ConsoleRE::TESCameraState* currentState = camera->currentState.get();
			currentState->GetTranslation(*a_pos);
		}

		using func_t = decltype(&SetListenerPosition_Hook);
		REL::Relocation<func_t> func{ Offsets::BSAudioManager::SetListenerPosition };
		return func(a_audioManager, a_pos);
	}

	void SetListenerRotation_Hook(ConsoleRE::BSAudioManager* a_audioManager, ConsoleRE::NiPoint3* a_unk1, ConsoleRE::NiPoint3* a_unk2)
	{
		ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();
		if (ui->IsMenuOpen(ConsoleRE::MapMenu::MENU_NAME))
		{
			ConsoleRE::PlayerCamera* camera = ConsoleRE::PlayerCamera::GetSingleton();
			ConsoleRE::TESCameraState* currentState = camera->currentState.get();

			ConsoleRE::NiQuaternion rot;
			currentState->GetRotation(rot);
			QuaternionToMatrix(*a_unk1, *a_unk2, rot);
		}

		using func_t = decltype(&SetListenerRotation_Hook);
		REL::Relocation<func_t> func{ Offsets::BSAudioManager::SetListenerRotation };
		return func(a_audioManager, a_unk1, a_unk2);
	}

	void InstallHook()
	{
		API::GetTrampoline().write_call<5>(Offsets::BSAudioManager::Hook.address() + 0xD2, (uintptr_t)SetListenerPosition_Hook);
		API::GetTrampoline().write_call<5>(Offsets::BSAudioManager::Hook.address() + 0x11A, (uintptr_t)SetListenerRotation_Hook);
	}
}
