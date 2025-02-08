#pragma once
#include <common_core.hpp>
#include <global/game_launch_type.hpp>
#include <logger/log_service.hpp>
#include <utility/winapi.hpp>

//#include "resource.h"

#include <bcrypt.h>
#include <winternl.h>
#include <intrin.h>

#define VT_GET(ptr, idx) (*(void***)(ptr))[idx]
#define HIGH_ORDER_LOG_HOOK 0

namespace Client {
	using namespace std::chrono_literals;
	using namespace std::string_literals;

	inline HMODULE g_Module{};
	inline HANDLE g_MainThread{};
	inline DWORD g_MainThreadId{};
	inline std::atomic_bool g_Running{ true };
	inline std::pair<int, std::string> g_LaunchInfo{ Common::GameLaunchType::UNKNOWN, "?" };

	NTSTATUS NtContinue(PCONTEXT threadContext, BOOLEAN raiseAlert);
	std::string GetWindowsUsername();

	bool RemoveEvilKeywordsFromString(const UNICODE_STRING& string);
	bool RemoveEvilKeywordsFromString(wchar_t* str, const size_t length);
	bool RemoveEvilKeywordsFromString(char* str, const size_t length);
}
