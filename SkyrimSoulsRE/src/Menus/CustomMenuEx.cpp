#include "Menus/CustomMenuEx.h"

namespace SkyrimSoulsRE
{
	ConsoleRE::IMenu* CustomMenuEx::Creator()
	{
		return CreateMenu("CustomMenu");
	}
}
