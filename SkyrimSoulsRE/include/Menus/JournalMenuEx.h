#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	// menuDepth = 5
	// flags = kPausesGame | kUsesMenuContext | kFreezeFrameBackground | kRequiresUpdate | kTopmostRenderedMenu | kUpdateUsesCursor | kAllowSaving
	// kDisablePauseMenu if game load prevented
	// context = kJournal
	class JournalMenuEx : public ConsoleRE::JournalMenu
	{
	public:
		// Journal Menu Remap Handler
		class RemapHandler : ConsoleRE::BSTEventSink<ConsoleRE::InputEvent*>
		{
		public:
			//todo - find a better way to do this
			class FakeButtonEvent
			{
			public:
				virtual ~FakeButtonEvent() = default;  // 00

				virtual bool HasIDCode() const { return true; };							// 01
				virtual const ConsoleRE::BSFixedString& QUserEvent() const { return userEvent; };	// 02

				// members
				ConsoleRE::INPUT_DEVICE device;
				ConsoleRE::INPUT_EVENT_TYPE eventType;
				ConsoleRE::InputEvent* next;
				ConsoleRE::BSFixedString userEvent;
				uint32_t idCode;
				float value;
				float heldDownSecs;
			};
			static_assert(sizeof(FakeButtonEvent) == sizeof(ConsoleRE::ButtonEvent));

			ConsoleRE::BSEventNotifyControl ProcessEvent_Hook(ConsoleRE::InputEvent** a_event, ConsoleRE::BSTEventSource<ConsoleRE::InputEvent**>* a_eventSource);

			using ProcessEvent_t = decltype(&RemapHandler::ProcessEvent_Hook);
			static inline REL::Relocation<ProcessEvent_t> _ProcessEvent;
		};

		static inline bool isSaving = false;

		ConsoleRE::UI_MESSAGE_RESULTS ProcessMessage_Hook(ConsoleRE::UIMessage& a_message);	// 04
		void AdvanceMovie_Hook(float a_interval, uint32_t a_currentTime);	// 05

		void UpdateClock();

		static ConsoleRE::IMenu* Creator();
		static void InstallHook();

		using AdvanceMovie_t = decltype(&ConsoleRE::JournalMenu::AdvanceMovie);
		static inline REL::Relocation<AdvanceMovie_t> _AdvanceMovie;

		using ProcessMessage_t = decltype(&ConsoleRE::JournalMenu::ProcessMessage);
		static inline REL::Relocation<ProcessMessage_t> _ProcessMessage;
	};
}
