#include <iostream>
#include "DMALibrary/Memory/Memory.h"

int main()
{
	if (!mem.Init("r5apex.exe", true, false))
	{
		std::cout << "Failed to initilize DMA" << std::endl;
		return 1;
	}
}