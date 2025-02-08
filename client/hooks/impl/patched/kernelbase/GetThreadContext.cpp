#include "common.hpp"
#include "hooks/hook.hpp"
#include <utility/nt.hpp>

template <>
BOOL WINAPI Client::Hook::Hooks::HK_GetThreadContext::hkCallback(const HANDLE threadHandle, const LPCONTEXT context) {
	constexpr auto debug_registers_flag = (CONTEXT_DEBUG_REGISTERS & ~CONTEXT_AMD64);
	if (context && (context->ContextFlags & debug_registers_flag)) {
		auto* source = _ReturnAddress();
		const auto game = Common::Utility::NT::Library{};
		const auto sourceModule = Common::Utility::NT::Library::GetByAddress(source);

		if (sourceModule == game) {
			context->ContextFlags &= ~debug_registers_flag;
		}
	}

	return m_Original(threadHandle, context);
}
