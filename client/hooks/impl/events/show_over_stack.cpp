#include "common.hpp"
#include "game/game.hpp"
#include "hooks/hook.hpp"

void Client::Hook::Hooks::OnShowOverStack() {
	// do stuff
	g_Pointers->PatchAuth();

	if (*g_Pointers->m_unk_WatermarkFont == nullptr) {
		return;
	}

	float color[4] = { .666f, .666f, .666f, .666f };
	g_Pointers->m_CL_DrawTextPhysical("t9-mod: " GIT_DESCRIBE, 0x7FFFFFFF, *g_Pointers->m_unk_WatermarkFont,
		4.f, 4.f + ((*g_Pointers->m_unk_WatermarkFont)->m_FontHeight / 3),
		1.f / 3.f, 1.f / 3.f,
		0.f, color, 0, 0);
}
