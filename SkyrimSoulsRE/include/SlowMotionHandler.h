#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	class SlowMotionHandler : public ConsoleRE::BSTEventSink<ConsoleRE::MenuOpenCloseEvent>
	{
	public:
		static bool isInSlowMotion;
		static float currentSlowMotionMultiplier;

		static void EnableSlowMotion();
		static void DisableSlowMotion();

		ConsoleRE::BSEventNotifyControl ProcessEvent(const ConsoleRE::MenuOpenCloseEvent* a_event, ConsoleRE::BSTEventSource<ConsoleRE::MenuOpenCloseEvent>* a_dispatcher) override;
	};

	extern SlowMotionHandler g_slowMotionHandler;
}
