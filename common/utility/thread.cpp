#include "common_core.hpp"
#include "utility/strings.hpp"
#include "utility/thread.hpp"
#include <gsl/gsl>

namespace Common::Utility::Thread {
	bool SetName(const HANDLE t, const std::string& name) {
		const NT::Library kernel32("kernel32.dll");
		if (!kernel32) {
			return false;
		}

		const auto setDescription = kernel32.GetProc<HRESULT(WINAPI*)(HANDLE, PCWSTR)>("SetThreadDescription");
		if (!setDescription) {
			return false;
		}

		return SUCCEEDED(setDescription(t, Strings::Convert(name).data()));
	}

	bool SetName(const DWORD id, const std::string& name) {
		auto* const t = OpenThread(THREAD_SET_LIMITED_INFORMATION, FALSE, id);
		if (!t) return false;

		const auto _ = gsl::finally([t]() {
			CloseHandle(t);
		});

		return SetName(t, name);
	}

	bool SetName(std::thread& t, const std::string& name) {
		return SetName(t.native_handle(), name);
	}

	bool SetName(const std::string& name) {
		return SetName(GetCurrentThread(), name);
	}

	std::vector<DWORD> GetThreadIds() {
		auto* const h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, GetCurrentProcessId());
		if (h == INVALID_HANDLE_VALUE) {
			return {};
		}

		const auto _ = gsl::finally([h]() {
			CloseHandle(h);
		});

		THREADENTRY32 entry{};
		entry.dwSize = sizeof(entry);
		if (!Thread32First(h, &entry)) {
			return {};
		}

		std::vector<DWORD> ids{};

		do {
			const auto check_size = entry.dwSize < FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(entry.th32OwnerProcessID);
			entry.dwSize = sizeof(entry);

			if (check_size && entry.th32OwnerProcessID == GetCurrentProcessId()) {
				ids.emplace_back(entry.th32ThreadID);
			}
		} while (Thread32Next(h, &entry));

		return ids;
	}

	void ForEachThread(const std::function<void(HANDLE)>& callback) {
		const auto ids = GetThreadIds();

		for (const auto& id : ids) {
			auto* const thread = OpenThread(THREAD_ALL_ACCESS, FALSE, id);
			if (thread != nullptr) {
				const auto _ = gsl::finally([thread]() {
					CloseHandle(thread);
				});

				callback(thread);
			}
		}
	}

	void SuspendOtherThreads() {
		ForEachThread([](const HANDLE thread) {
			if (GetThreadId(thread) != GetCurrentThreadId()) {
				SuspendThread(thread);
			}
		});
	}

	void ResumeOtherThreads() {
		ForEachThread([](const HANDLE thread) {
			if (GetThreadId(thread) != GetCurrentThreadId()) {
				ResumeThread(thread);
			}
		});
	}
}
