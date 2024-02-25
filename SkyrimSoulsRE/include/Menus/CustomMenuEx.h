#pragma once

#include "SkyrimSoulsRE.h"

namespace SkyrimSoulsRE
{
	class CustomMenuEx : ConsoleRE::IMenu
	{
	public:
		static ConsoleRE::IMenu* Creator();
	};
}
