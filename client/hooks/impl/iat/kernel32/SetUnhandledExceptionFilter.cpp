#include "common.hpp"
#include "hooks/hook.hpp"

#include <utility/nt.hpp>

LONG WINAPI TopLevelExceptionFilter(EXCEPTION_POINTERS* inf) {
	const char* exceptionCodeName = "<unknown>";
	switch (inf->ExceptionRecord->ExceptionCode) {
	case EXCEPTION_ACCESS_VIOLATION:
		exceptionCodeName = "Access violation";
		break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		exceptionCodeName = "Array bounds exceeded";
		break;
	case EXCEPTION_BREAKPOINT:
		exceptionCodeName = "Breakpoint";
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		exceptionCodeName = "Data type misalignment";
		break;
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		exceptionCodeName = "Float denormal operand";
		break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		exceptionCodeName = "Float / 0";
		break;
	case EXCEPTION_FLT_INEXACT_RESULT:
		exceptionCodeName = "Float inexact result";
		break;
	case EXCEPTION_FLT_INVALID_OPERATION:
		exceptionCodeName = "Float invalid operation";
		break;
	case EXCEPTION_FLT_OVERFLOW:
		exceptionCodeName = "Float overflow";
		break;
	case EXCEPTION_FLT_STACK_CHECK:
		exceptionCodeName = "Float stack check";
		break;
	case EXCEPTION_FLT_UNDERFLOW:
		exceptionCodeName = "Float underflow";
		break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		exceptionCodeName = "Illegal instruction";
		break;
	case EXCEPTION_IN_PAGE_ERROR:
		exceptionCodeName = "In-page error";
		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		exceptionCodeName = "Int / 0";
		break;
	case EXCEPTION_INT_OVERFLOW:
		exceptionCodeName = "Integer overflow";
		break;
	case EXCEPTION_INVALID_DISPOSITION:
		exceptionCodeName = "Invalid disposition";
		break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		exceptionCodeName = "Non-continuable exception";
		break;
	case EXCEPTION_PRIV_INSTRUCTION:
		exceptionCodeName = "Private instruction";
		break;
	case EXCEPTION_SINGLE_STEP:
		exceptionCodeName = "Single step";
		break;
	case EXCEPTION_STACK_OVERFLOW:
		exceptionCodeName = "Stack overflow";
		break;
	}

	const auto relLoc = [](std::size_t address) {
		Common::Utility::NT::Library assignedLib = Common::Utility::NT::Library::GetByAddress(reinterpret_cast<void*>(address));
		if (assignedLib) {
			return std::format(" ({}+0x{:016X})", assignedLib.GetName(), address - reinterpret_cast<std::size_t>(assignedLib.GetPtr()));
		}
		return std::string("");
	};
#	define AND_REL(a) a, relLoc(a)

	Common::Utility::NT::Library gameLib = Common::Utility::NT::Library();
	Common::Utility::NT::Library modLib = Common::Utility::NT::Library(Client::g_Module);

	std::string message = "Unhandled exception caught. Instead of giving you absolutely nothing:\n";
	message += std::format("{} region: 0x{:016X} -> 0x{:016X}\n", gameLib.GetName(),
		reinterpret_cast<std::size_t>(gameLib.GetPtr()), reinterpret_cast<std::size_t>(gameLib.GetPtr()) + gameLib.GetOptionalHeader()->SizeOfImage);
	message += std::format("{} region: 0x{:016X} -> 0x{:016X}\n", modLib.GetName(),
		reinterpret_cast<std::size_t>(modLib.GetPtr()), reinterpret_cast<std::size_t>(modLib.GetPtr()) + modLib.GetOptionalHeader()->SizeOfImage);
	message += "---- Exception Record ----\n";
	message += std::format("Exception code: {} (0x{:08X})\n", exceptionCodeName, inf->ExceptionRecord->ExceptionCode);
	message += std::format("Exception flags: 0x{:08X}\n", inf->ExceptionRecord->ExceptionFlags);
	message += std::format("Exception address: 0x{:016X}{}\n", AND_REL(reinterpret_cast<std::size_t>(inf->ExceptionRecord->ExceptionAddress)));
	message += std::format("Number of parameters: {}\n", inf->ExceptionRecord->NumberParameters);
	for (auto x = 0ul; x < std::min(inf->ExceptionRecord->NumberParameters, 15ul /*EXCEPTION_MAXIMUM_PARAMETERS*/); x++) {
		message += std::format(" > Parameter {}: 0x{:016X}\n", x, inf->ExceptionRecord->ExceptionInformation[x]);
	}
	message += "----- Context Record -----\n";
	message += std::format("P1Home: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->P1Home));
	message += std::format("P2Home: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->P2Home));
	message += std::format("P3Home: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->P3Home));
	message += std::format("P4Home: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->P4Home));
	message += std::format("P5Home: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->P5Home));
	message += std::format("P6Home: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->P6Home));

	message += std::format("Context flags: 0x{:08X}\n", inf->ContextRecord->ContextFlags);
	message += std::format("MxCsr: 0x{:08X}{}\n", AND_REL(inf->ContextRecord->MxCsr));

	message += std::format("SegCs: 0x{:04X}{}\n", AND_REL(inf->ContextRecord->SegCs));
	message += std::format("SegDs: 0x{:04X}{}\n", AND_REL(inf->ContextRecord->SegDs));
	message += std::format("SegEs: 0x{:04X}{}\n", AND_REL(inf->ContextRecord->SegEs));
	message += std::format("SegFs: 0x{:04X}{}\n", AND_REL(inf->ContextRecord->SegFs));
	message += std::format("SegGs: 0x{:04X}{}\n", AND_REL(inf->ContextRecord->SegGs));
	message += std::format("SegSs: 0x{:04X}{}\n", AND_REL(inf->ContextRecord->SegSs));
	message += std::format("EFlags: 0x{:08X}{}\n", AND_REL(inf->ContextRecord->EFlags));

	message += std::format("Dr0: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->Dr0));
	message += std::format("Dr1: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->Dr1));
	message += std::format("Dr2: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->Dr2));
	message += std::format("Dr3: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->Dr3));
	message += std::format("Dr6: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->Dr6));
	message += std::format("Dr7: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->Dr7));

	message += std::format("Rax: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->Rax));
	message += std::format("Rcx: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->Rcx));
	message += std::format("Rdx: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->Rdx));
	message += std::format("Rbx: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->Rbx));
	message += std::format("Rsp: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->Rsp));
	message += std::format("Rbp: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->Rbp));
	message += std::format("Rsi: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->Rsi));
	message += std::format("Rdi: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->Rdi));
	message += std::format("R8: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->R8));
	message += std::format("R9: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->R9));
	message += std::format("R10: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->R10));
	message += std::format("R11: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->R11));
	message += std::format("R12: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->R12));
	message += std::format("R13: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->R13));
	message += std::format("R14: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->R14));
	message += std::format("R15: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->R15));

	message += std::format("Rip: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->Rip));

	message += std::format("VectorControl: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->VectorControl));

	message += std::format("DebugControl: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->DebugControl));
	message += std::format("LastBranchToRip: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->LastBranchToRip));
	message += std::format("LastBranchFromRip: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->LastBranchFromRip));
	message += std::format("LastExceptionToRip: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->LastExceptionToRip));
	message += std::format("LastExceptionFromRip: 0x{:016X}{}\n", AND_REL(inf->ContextRecord->LastExceptionFromRip));

	MessageBoxA(nullptr, message.c_str(), "t9-mod exception handler", MB_OK);
	return EXCEPTION_EXECUTE_HANDLER;
}

template <>
LPTOP_LEVEL_EXCEPTION_FILTER WINAPI Client::Hook::Hooks::HK_SetUnhandledExceptionFilter::hkCallback(LPTOP_LEVEL_EXCEPTION_FILTER filter) {
	_Unreferenced_parameter_(filter);
	return SetUnhandledExceptionFilter(TopLevelExceptionFilter);
}
