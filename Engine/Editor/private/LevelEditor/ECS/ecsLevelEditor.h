#pragma once

#include <Level.h>
#include <flecs.h>

#include <EditorECS/ecsEditor.h>

namespace GameEngine::EntitySystem::LevelEditorECS
{
	struct PositionDesc
	{
		unsigned int objectID;
		unsigned int componentID;
	};

	void RegisterLevelEditorEcsSystems(flecs::world& world, World::Level& level);
}