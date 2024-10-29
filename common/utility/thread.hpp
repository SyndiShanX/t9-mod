#pragma once
#include "common_core.hpp"
#include "utility/nt.hpp"

namespace Common::Utility::Thread {
	bool SetName(HANDLE t, const std::string& name);
	bool SetName(DWORD id, const std::string& name);
	bool SetName(std::thread& t, const std::string& name);
	bool SetName(const std::string& name);

	template <typename ...Args>
	std::thread CreateNamedThread(const std::string& name, Args&&... args) {
		auto t = std::thread(std::forward<Args>(args)...);
		SetName(t, name);
		return t;
	}

	std::vector<DWORD> GetThreadIds();
	void ForEachThread(const std::function<void(HANDLE)>& callback);

	void SuspendOtherThreads();
	void ResumeOtherThreads();
}
