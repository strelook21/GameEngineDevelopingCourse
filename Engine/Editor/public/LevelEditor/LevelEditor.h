#pragma once

#include <Level.h>
#include <UIWindow.h>
#include <Timer.h>
#include <flecs.h>

namespace GameEngine
{
	namespace Editor
	{
		class LevelEditor final : public GUI::UIWindow
		{
		public:
			LevelEditor() = delete;
			LevelEditor(flecs::world& world);

		public:
			virtual void Draw() override;
			virtual void Update(float dt) override;
			virtual const char* GetName() const override { return "Level Editor"; }

		private:
			void Save();
			void CreateDefault();

		private:
			int m_ObjectCount = 0;
			Core::Timer m_SaveButtonMessageTimer;
			bool m_SaveButtonPressed = false;
			float m_TimeToShowSaveButtonMessage = 3.f;
			Core::Timer m_CreateButtonMessageTimer;
			bool m_CreateButtonPressed = false;
			float m_TimeToShowCreateButtonMessage = 3.f;
			flecs::world* m_World = nullptr;

			std::optional<World::Level> m_Level = std::nullopt;
		};
	}
}