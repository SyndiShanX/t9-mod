#include "common.hpp"
#include "hooks/hook.hpp"
#include "memory/memory.hpp"
#include "game/game.hpp"

#include <utility/memory.hpp>
#include <utility/nt.hpp>

namespace Client::Hook {
	Hooks::Hooks() {
		MH_Initialize();

		this->m_SetUnhandledExceptionFilterHK = new Memory::IAT("kernel32.dll", "SetUnhandledExceptionFilter");
		this->m_SetUnhandledExceptionFilterHK->Hook<HK_SetUnhandledExceptionFilter>();

		this->m_RtlDispatchExceptionHK = new Memory::MinHook(g_Pointers->m_RtlDispatchException);
		this->m_RtlDispatchExceptionHK->Hook<HK_RtlDispatchException>();

		this->m_CreateMutexExAHK = new Memory::MinHook(CreateMutexExA);
		this->m_CreateMutexExAHK->Hook<HK_CreateMutexExA>();

		this->m_GetThreadContextHK = new Memory::MinHook("kernelbase.dll", "GetThreadContext");
		this->m_GetThreadContextHK->Hook<HK_GetThreadContext>();

		this->m_NtQueryInformationProcessHK = new Memory::MinHook("ntdll.dll", "NtQueryInformationProcess");
		this->m_NtQueryInformationProcessHK->Hook<HK_NtQueryInformationProcess>();

		if (*reinterpret_cast<uintptr_t*>(g_Pointers->m_Dvar_ShowOverStack)) {
			g_OriginalTestPtr = *reinterpret_cast<uintptr_t*>(g_Pointers->m_Dvar_ShowOverStack);
			*reinterpret_cast<uintptr_t*>(g_Pointers->m_Dvar_ShowOverStack) = 1;
		};

		//this->m_EventHandlerStore.AddHandler("OnShowOverStack", 0x0000000000000001, g_Pointers->m_Dvar_ShowOverStack, OnShowOverStack);
	}

	Hooks::~Hooks() {
		this->DeleteHook(&this->m_RtlDispatchExceptionHK);
		this->DeleteHook(&this->m_SetUnhandledExceptionFilterHK);
	}
}
