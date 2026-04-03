#include <iostream>

#include "mem.h"

#pragma once

namespace Sigs
{
	const auto RegenerateWeaponSkins = mem.SigScan(L"client.dll", "48 83 EC ? E8 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 48 8B 10");
<<<<<<< HEAD
=======
	
	// Function to update entity subclass and model rendering
	static uintptr_t GetUpdateSubClassFunc()
	{
		static uintptr_t updateSubClassAddr = 0;
		if (updateSubClassAddr == 0)
		{
			updateSubClassAddr = mem.SigScan(L"client.dll", "4C 8B DC 53 48 81 EC ? ? ? ? 48 8B 41");
		}
		return updateSubClassAddr;
	}
	
	// Function to set entity model
	static uintptr_t GetSetModelFunc()
	{
		static uintptr_t setModelAddr = 0;
		if (setModelAddr == 0)
		{
			setModelAddr = mem.SigScan(L"client.dll", "40 53 48 83 EC ? 48 8B D9 4C 8B C2 48 8B 0D ? ? ? ? 48 8D 54 24");
		}
		return setModelAddr;
	}
>>>>>>> 65bda63 (feat: Implement knife and glove model forcing - Complete knife implementation + glove ready for patch + comprehensive docs - Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>)
}