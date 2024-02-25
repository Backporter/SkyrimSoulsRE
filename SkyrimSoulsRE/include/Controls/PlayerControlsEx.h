#pragma once

#include "../../../CSEL/source/RE/P/PlayerControls.h"

namespace SkyrimSoulsRE
{
	class PlayerControlsEx : public ConsoleRE::PlayerControls
	{
	public:
		static void InstallHook();
	};
}
