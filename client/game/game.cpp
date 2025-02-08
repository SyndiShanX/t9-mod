#include "common.hpp"
#include "game/game.hpp"
#include "memory/memory.hpp"
#include <utility/memory.hpp>
#include <utility/nt.hpp>

#define SETUP_POINTER(name) #name, (void**)&this->m_##name
#define SETUP_MOD(mod) [](Memory::ScannedResult<void> r) { return r.##mod##; }

namespace Client::Game {
	void Pointers::PointerList::Apply() {
		for (const PointerCalculator& ptr : this->m_Pointers) {
			std::size_t sigNum = 0;
			for (const SignatureCalculator& sig : ptr.m_Calculators) {
				sigNum++;
				Memory::ScannedResult<void> res = Memory::SigScan(sig.m_Signature, this->m_Module,
					ptr.m_Calculators.size() > 1 ? std::format("{} [{}]", ptr.m_Name, sigNum) : ptr.m_Name);
				if (res) {
					sig.m_Mod(res).Apply(ptr.m_Pointer);
					break;
				}
			}
		}
	}

	Pointers::PointerList Pointers::GetPointerList() {
		return {
			g_LaunchInfo.second, {
				// CL_Disconnect
				{
					{{ "40 53 56 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 4D 8B F8" }},
					SETUP_POINTER(CL_Disconnect)
				},
				// CL_DrawTextPhysical
				{
					{{ "48 83 EC 78 F3 0F 10 84 24 ? ? ? ? F3 0F 10 8C 24 ? ? ? ? 8B 84 24 ? ? ? ? F3 0F 11 44 24 ? F3 0F 10 84 24 ? ? ? ? C6 44 24 ? ? C7 44 24 ? ? ? ? ? 89 44 24 48 48 8B 84 24 ? ? ? ? 48 89 44 24 ? F3 0F 11 44 24 ? F3 0F 10 84 24 ? ? ? ? F3 0F 11 4C 24 ? F3 0F 10 8C 24 ? ? ? ? F3 0F 11 44 24 ? F3 0F 11 4C 24 ? E8 ? ? ? ? 48 83 C4 78 C3" }},
					SETUP_POINTER(CL_DrawTextPhysical)
				},
				// Com_SessionMode_SetNetworkMode
				{
					{{ "8B 05 ? ? ? ? 8B D0 33 D1 83 E2 0F 33 C2 89 05 ? ? ? ? C3" }},
					SETUP_POINTER(Com_SessionMode_SetNetworkMode)
				},
				// Dvar_SetBoolFromSource
				{
					{
						{ "48 85 C9 0F 84 ? ? ? ? 48 89 5C 24 ? 57 48 83 EC 60 0F 57 C0 44 0F B6 CA 48 B8 ? ? ? ? ? ? ? ? 41 8B F8 48 8B D9 0F 11 44 24 ? 0F 11 44 24 ? 48 85 01 0F 84 ? ? ? ?" },
						{ "48 85 C9 0F 84 ? ? ? ? 4C 8B DC 56 57 48 83 EC ? 0F 57 C0 44 0F B6 CA 48 B8 ? ? ? ? ? ? ? ? 41 8B F0 48 8B F9 0F 11 44 24 ? 0F 11 44 24 ? 48 85 01 0F 84 ? ? ? ?" }
					},
					SETUP_POINTER(Dvar_SetBoolFromSource)
				},
				// Dvar_ShowOverStack
				{
					{{ "48 8B 0D ? ? ? ? 41 0F 28 F2 F3 0F 5E F0 C7 44 24 ? ? ? ? ? 0F 28 05 ? ? ? ? 0F 28 FE 44 0F 28 C6 F3 0F 59 3D ? ? ? ? 44 0F 28 CE F3 0F 59 35 ? ? ? ?", SETUP_MOD(Add(3).Rip()) }},
					SETUP_POINTER(Dvar_ShowOverStack)
				},
				// LiveUser_GetUserDataForController
				{
					{{ "E8 ? ? ? ? 48 8B E8 33 C0 48 89 85 ? ? ? ? 48 8D 4D 10 48 89 85 ? ? ? ? 48 89 85 ? ? ? ? 89 85 ? ? ? ?", SETUP_MOD(Add(1).Rip()) }},
					SETUP_POINTER(LiveUser_GetUserDataForController)
				},
				// LobbyBase_SetNetworkMode
				{
					{{ "40 53 48 83 EC 20 8B D9 89 0D ? ? ? ? E8 ? ? ? ? 8B CB E8 ? ? ? ?" }},
					SETUP_POINTER(LobbyBase_SetNetworkMode)
				},
				// Unk_SetScreen
				{
					{{ "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 27 0F B6 FA 48 63 D9 8B 05 ? ? ? ?" }},
					SETUP_POINTER(Unk_SetScreen)
				},
				// Unk_SetUsername
				{
					{{ "48 89 5C 24 ? 57 48 83 EC 20 48 8B D9 4C 8B C2 BA ? ? ? ? 48 81 C1 ? ? ? ?" }},
					SETUP_POINTER(Unk_SetUsername)
				},
				// Dvar_NoDW
				{
					{{ "48 8B 0D ? ? ? ? E8 ? ? ? ? 83 F8 01 75 ? 48 8B 0F 8D 68 ? 44 8B F8 48 85 C9", SETUP_MOD(Add(3).Rip()) }},
					SETUP_POINTER(Dvar_NoDW)
				},
				// Scr_Initialized
				{
					{{ "38 ? ? ? ? ? 0F 85 ? ? ? ? 48 ? 4F B6 E2 AE 25 2E C1 17", SETUP_MOD(Add(2).Rip()) }},
					SETUP_POINTER(Scr_Initialized)
				},
				// unk_AuthManager
				{
					{{ "4C 8B 35 ? ? ? ? 4C 89 BC 24 ? ? ? ? 49 BF", SETUP_MOD(Add(3).Rip()) }},
					SETUP_POINTER(unk_AuthManager)
				},
				// unk_Config_1
				{
					{{ "80 3D ? ? ? ? ? 48 8D 15 ? ? ? ? 48 8B 3F 48 8B C8 48 0F 45 3D ? ? ? ? E8 ? ? ? ?", SETUP_MOD(Add(2).Rip().Add(1)) }},
					SETUP_POINTER(unk_Config_1)
				},
				// unk_Config_2
				{
					{{ "80 3D ? ? ? ? ? 75 58 33 C9 48 89 5C 24 ?", SETUP_MOD(Add(2).Rip().Add(1)) }},
					SETUP_POINTER(unk_Config_2)
				},
				// unk_WatermarkFont
				{
					{{ "48 8B 05 ? ? ? ? 48 8B 5C 24 ? 48 83 C4 30 5F C3", SETUP_MOD(Add(3).Rip()) }},
					SETUP_POINTER(unk_WatermarkFont)
				}
			}
		};
	}

	Pointers::PointerList Pointers::GetNTPointerList() {
		return {
			"ntdll.dll", {
				// RtlDispatchException
				{
					{
						// Windows 10
						{ "40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 65 48 8B 04 25 ? ? ? ? 33 DB" },
						// Windows 11
						{ "40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 33 F6" }
					},
					SETUP_POINTER(RtlDispatchException)
				}
			}
		};
	}

	void Pointers::PatchAuth() {
		if (this->m_unk_AuthManager && *this->m_unk_AuthManager) {
			uintptr_t authMgr = *this->m_unk_AuthManager;
			for (int i = 0; i < *reinterpret_cast<int*>(authMgr + 0x8); i++) {
				uintptr_t temp = *reinterpret_cast<uintptr_t*>(authMgr + 0x10);
				if (temp) {
					*reinterpret_cast<int*>(temp + (0xD0 * i) + 0x14) = 0;
					*reinterpret_cast<int*>(temp + (0xD0 * i) + 0x48) = 2;
					*reinterpret_cast<int*>(temp + (0xD0 * i) + 0x4C) = 3;
				}
			}
		}
		else {
			LOG("Pointers", ERROR, "Failed to patch Call of Duty: Black Ops Cold War authentication because the pointer is null.");
		}
	}

	void Pointers::SetMode(int mode, int menu) {
		this->m_Unk_SetScreen(menu, 0);
		this->m_LobbyBase_SetNetworkMode(T9::LobbyNetworkMode::LOBBY_NETWORKMODE_LAN);
		this->m_Com_SessionMode_SetNetworkMode(mode);

		*this->m_unk_Config_1 = 1;
		*this->m_unk_Config_2 = 1;
	}

	Pointers::Pointers() {
		this->GetPointerList().Apply();
		this->GetNTPointerList().Apply();

		this->m_Unk_SetUsername(this->m_LiveUser_GetUserDataForController(0), GetWindowsUsername().c_str());
		this->SetMode(0, 10);
	}
}