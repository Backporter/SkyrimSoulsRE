#pragma once

namespace SkyrimSoulsRE
{
	template <class T>
	class MapInputHandlerEx : public T
	{
	public:
		bool CanProcess_Hook(ConsoleRE::InputEvent* a_event);

		static void InstallHook(REL::Offset a_vtbl);

		using CanProcess_t = decltype(&T::CanProcess);
		static inline REL::Relocation<CanProcess_t> _CanProcess;
	};

	template <class T>
	inline bool MapInputHandlerEx<T>::CanProcess_Hook(ConsoleRE::InputEvent* a_event)
	{
		ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();
		if (ui->IsMenuOpen(ConsoleRE::JournalMenu::MENU_NAME)) {
			return false;
		}
		return _CanProcess(this, a_event);
	}

	template <class T>
	inline void MapInputHandlerEx<T>::InstallHook(REL::Offset a_vtbl)
	{
		REL::Relocation<uintptr_t> vTable(a_vtbl);
		_CanProcess = vTable.write_vfunc(0x1, &MapInputHandlerEx::CanProcess_Hook);
	}
}
