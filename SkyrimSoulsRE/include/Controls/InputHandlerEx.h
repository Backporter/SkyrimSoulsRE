#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	extern uint32_t GetUnpausedMenuCount();

	template <class T>
	class InputHandlerEx : public T
	{
	public:
		using CanProcess_t = decltype(&T::CanProcess);
		static inline REL::Relocation<CanProcess_t> _CanProcess;

		bool CanProcess_Hook(ConsoleRE::InputEvent* a_event);

		static void InstallHook(REL::Relocation<uintptr_t> a_vtbl, uint64_t a_offset);
	};

	template <class T>
	inline bool InputHandlerEx<T>::CanProcess_Hook(ConsoleRE::InputEvent* a_event)
	{
		ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();
		Settings* settings = Settings::GetSingleton();

		uint32_t unpausedMenuCount = GetUnpausedMenuCount();

		if (settings->enableGamepadCameraMove && /* typeid(T) == typeid*/ std::is_same_v<T, ConsoleRE::LookHandler>) 
		{
			if (a_event->eventType == ConsoleRE::INPUT_EVENT_TYPE::kThumbstick) 
			{
				return _CanProcess(this, a_event);
			}
		}

		if (ui->IsMenuOpen(ConsoleRE::Console::MENU_NAME)) 
		{
			return false;
		}

		if (settings->enableMovementInMenus && /*typeid(T) == typeid(*/std::is_same_v<T, ConsoleRE::MovementHandler>) 
		{
			return _CanProcess(this, a_event);
		}

		if (settings->enableToggleRun && /*typeid(T) == typeid(*/std::is_same_v<T, ConsoleRE::ToggleRunHandler>) 
		{
			return _CanProcess(this, a_event);
		}

		if (unpausedMenuCount) 
		{
			return false;
		}

		return _CanProcess(this, a_event);
	}

	template <class T>
	inline void InputHandlerEx<T>::InstallHook(REL::Relocation<uintptr_t> a_vtbl, uint64_t a_offset)
	{
		_CanProcess = a_vtbl.write_vfunc(a_offset, &InputHandlerEx<T>::CanProcess_Hook);
	}
}
