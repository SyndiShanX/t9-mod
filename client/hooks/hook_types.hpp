#pragma once
#include "memory/minhook.hpp"
#include <global/string_literal.hpp>

namespace Client::HookPlate {
	template <Common::StringLiteral Name, typename Type, typename ...Args>
	struct StdcallHook {
		using funcType = Type __stdcall(Args...);
		static funcType hkCallback;
		static inline funcType* m_Original;

		static inline const char* GetName() {
			return Name.m_Value;
		}
	};

	template <Common::StringLiteral Name, typename Type, typename ...Args>
	struct CdeclHook {
		using funcType = Type __cdecl(Args...);
		static funcType hkCallback;
		static inline funcType* m_Original;

		static inline const char* GetName() {
			return Name.m_Value;
		}
	};

	template <Common::StringLiteral Name, typename Type, typename ...Args>
	struct FastcallHook {
		using funcType = Type __fastcall(Args...);
		static funcType hkCallback;
		static inline funcType* m_Original;

		static inline const char* GetName() {
			return Name.m_Value;
		}
	};

	template <Common::StringLiteral Name, typename TypeDecl>
	struct FnHook {
		using funcType = TypeDecl;
		static funcType* hkCallback;
		static inline funcType* m_Original;

		static inline const char* GetName() {
			return Name.m_Value;
		}
	};

	class EventHandlerStore {
	public:
		class EventHandler {
		public:
			template <typename T>
			EventHandler(std::string name, std::uint64_t identifier, T* address, std::function<void()> callback)
				: m_Name(name)
				, m_Identifier(identifier)
				, m_Address(reinterpret_cast<std::uint64_t*>(address))
				, m_Callback(callback)
			{}

			std::string m_Name;
			std::uint64_t m_Identifier;
			std::uint64_t* m_Address;
			std::function<void()> m_Callback;

			std::uint64_t m_Original = NULL;

			bool IsActive() {
				return this->m_Original != NULL;
			}

			void Enable() {
				if (this->IsActive()) {
					return;
				}

				this->m_Original = *this->m_Address;
				*this->m_Address = this->m_Identifier;
			}

			void Disable() {
				if (!this->IsActive()) {
					return;
				}

				*this->m_Address = this->m_Original;
				this->m_Original = NULL;
			}
		};

		EventHandler* FindHandler(std::uint64_t identifier) {
			auto it = std::find_if(this->m_Handlers.begin(), this->m_Handlers.end(), [identifier](const EventHandler& handler) {
				return handler.m_Identifier == identifier;
			});

			if (it != this->m_Handlers.end()) {
				return it._Ptr;
			}
			return nullptr;
		}

		template <typename T>
		void AddHandler(std::string name, std::uint64_t identifier, T* address, std::function<void()> callback) {
			LOG("Handlers", DEBUG, "Adding handler {}", name);
			EventHandler handler = EventHandler(name, identifier, address, callback);
			LOG("Handlers", DEBUG, "Enabling handler {}", name);
			handler.Enable();
			LOG("Handlers", DEBUG, "Pushing handler {}", name);
			this->m_Handlers.push_back(handler);
		}

		void RemoveHandler(std::uint64_t identifier) {
			std::size_t idx = 0;
			for (const auto& handler : this->m_Handlers) {
				if (handler.m_Identifier == identifier) {
					this->m_Handlers.erase(this->m_Handlers.begin() + idx);
					break;
				}
				idx++;
			}
		}

		void EnableHandler(std::uint64_t identifier) {
			EventHandler* handler = this->FindHandler(identifier);
			if (handler != nullptr) {
				handler->Enable();
			}
		}

		void DisableHandler(std::uint64_t identifier) {
			EventHandler* handler = this->FindHandler(identifier);
			if (handler != nullptr) {
				handler->Disable();
			}
		}

		std::vector<EventHandler> m_Handlers{};
	};
}