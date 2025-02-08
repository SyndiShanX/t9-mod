#pragma once
#include "common.hpp"
#include "game/function_types.hpp"
#include "memory/scanned_result.hpp"

namespace Client {
	namespace Game {
		class Pointers {
		public:
			class SignatureCalculator {
			public:
				using Res = Memory::ScannedResult<void>;

				SignatureCalculator(const std::string& signature, std::function<Res(Res)> mod)
					: m_Signature(signature)
					, m_Mod(std::move(mod))
				{}

				SignatureCalculator(const std::string& signature)
					: SignatureCalculator(signature, [](Res r) { return r; })
				{}

				std::string m_Signature;
				std::function<Res(Res)> m_Mod;
			};

			class PointerCalculator {
			public:
				PointerCalculator(std::vector<SignatureCalculator> calculators, std::string name, void** pointer)
					: m_Calculators(calculators)
					, m_Name(name)
					, m_Pointer(pointer)
				{}

				std::vector<SignatureCalculator> m_Calculators;
				std::string m_Name;
				void** m_Pointer;
			};

			class PointerList {
			public:
				PointerList(std::string _module, std::vector<PointerCalculator> pointers) {
					this->m_Module = _module;
					this->m_Pointers = pointers;
				}

				std::string m_Module{};
				std::vector<PointerCalculator> m_Pointers{};

				void Apply();
			};

			explicit Pointers();
			PointerList GetPointerList();
			PointerList GetNTPointerList();

			void PatchAuth();
			void SetMode(int mode, int menu);

			Functions::CL_DisconnectT* m_CL_Disconnect{};
			Functions::CL_DrawTextPhysicalT* m_CL_DrawTextPhysical{};
			Functions::Com_SessionMode_SetNetworkModeT* m_Com_SessionMode_SetNetworkMode{};
			Functions::Dvar_SetBoolFromSourceT* m_Dvar_SetBoolFromSource{};
			Functions::Dvar_ShowOverStackT* m_Dvar_ShowOverStack{};
			Functions::LiveUser_GetUserDataForControllerT* m_LiveUser_GetUserDataForController{};
			Functions::LobbyBase_SetNetworkModeT* m_LobbyBase_SetNetworkMode{};
			Functions::Unk_SetScreenT* m_Unk_SetScreen{};
			Functions::Unk_SetUsernameT* m_Unk_SetUsername{};

			std::uintptr_t** m_Dvar_NoDW{};
			bool* m_Scr_Initialized{};
			std::uintptr_t* m_unk_AuthManager{};
			char* m_unk_Config_1{};
			char* m_unk_Config_2{};
			T9::Font_s** m_unk_WatermarkFont{};

			Functions::RtlDispatchExceptionT* m_RtlDispatchException{};
		};
	}
	inline std::unique_ptr<Game::Pointers> g_Pointers{};
}

#define MATERIAL_WHITE Client::g_Pointers->m_DB_FindXAssetHeader(Client::T9::XAssetType::ASSET_TYPE_MATERIAL, "white", true, -1).material
