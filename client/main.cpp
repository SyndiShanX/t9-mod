#include "common.hpp"
#include "game/game.hpp"
#include "hooks/hook.hpp"
#include <utility/nt.hpp>

BOOL APIENTRY DllMain(HMODULE hMod, DWORD reason, PVOID) {
	using namespace Client;
	if (reason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hMod);
		g_Module = hMod;
		g_MainThread = CreateThread(nullptr, 0, [](PVOID) -> DWORD {
			Common::g_LogService = std::make_unique<Common::LogService>();
			g_LaunchInfo = Common::GameLaunchType::GetLaunchInfo();
			Common::WinAPI::_SetConsoleTitle("t9-mod: "s + GIT_DESCRIBE + " (" + Common::GameLaunchType::GetDisplayName(g_LaunchInfo.first) + ")");
			Common::Utility::NT::Library().Unprotect();
			LOG("MainThread", INFO, "T9-Mod injected.");

			while (g_Running) {
				// no, we are not planning on unloading the mod, that will cause
				// absolute disaster. instead, we just unload it when the dll is
				// called for detach, therefore safely exiting.
				std::this_thread::sleep_for(1s);
			}

			if (g_Pointers) {
				g_Pointers.reset();
				LOG("MainThread", INFO, "Pointers uninitialized.");
			}

			if (g_Hooks) {
				g_Hooks.reset();
				LOG("MainThread", INFO, "Hooks uninitialized.");
			}

			Common::g_LogService.reset();
			return 0;
		}, nullptr, 0, &g_MainThreadId);
	}
	else if (reason == DLL_PROCESS_DETACH) {
		g_Running = false;
	}

	return TRUE;
}

bool s_CalledMainEntryPoint = false;
void MainEntryPoint() {
	if (s_CalledMainEntryPoint) {
		return;
	}
	s_CalledMainEntryPoint = true;
	using namespace Client;

	g_Pointers = std::make_unique<Game::Pointers>();
	LOG("MainThread", INFO, "Pointers initialized.");

	g_Hooks = std::make_unique<Hook::Hooks>();
	LOG("MainThread", INFO, "Hooks initialized.");

	CreateThread(nullptr, 0, [](PVOID) -> DWORD {
		LOG("Grand Theft Auto VII", DEBUG, "Waiting for Scr_Initialized to be like true or something");
		while (!(*g_Pointers->m_Scr_Initialized)) {
			std::this_thread::sleep_for(100ms);
		}

		LOG("Grand Theft Auto VII", DEBUG, "Scr_Initialized is like true or something now im gonna do some stuff");
		g_Pointers->m_Dvar_SetBoolFromSource(*g_Pointers->m_Dvar_NoDW, true, 0);
		g_Pointers->m_CL_Disconnect(0, false, "");

		LOG("Grand Theft Auto VII", DEBUG, "Did some stuff, vaporizing now.");
		return 0;
	}, nullptr, 0, nullptr);
}

extern "C" __declspec(dllexport) int /* EDiscordResult */ /* DISCORD_API */ DiscordCreate(int /* DiscordVersion */ version, struct DiscordCreateParams* params, struct IDiscordCore** result) {
	_Unreferenced_parameter_(version);
	_Unreferenced_parameter_(params);
	_Unreferenced_parameter_(result);

	MainEntryPoint();

	LOG("Proxy/DiscordCreate", INFO, "DiscordCreate called, returning 1 (ServiceUnavailable).");
	return 1 /* DiscordResult_ServiceUnavailable */;
}
