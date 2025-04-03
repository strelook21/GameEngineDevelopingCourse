#include <LevelEditor/ECS/ecsLevelEditor.h>

namespace
{
	void ParsePosition(
		const std::string& positionDesc,
		GameEngine::EntitySystem::EditorECS::Position& position
	)
	{
		//assert(std::ranges::count(positionDesc, ',') == 2);

		const char* compValue = positionDesc.c_str();
		char* end;

		float f = std::strtof(compValue, &end);
		position.x = f;
		compValue = end + 1;

		f = std::strtof(compValue, &end);
		position.y = f;
		compValue = end + 1;

		f = std::strtof(compValue, &end);
		position.z = f;
	}
}

namespace GameEngine::EntitySystem::LevelEditorECS
{
	void RegisterLevelEditorEcsSystems(flecs::world& world, World::Level& level)
	{
		world.system<const PositionDesc, EntitySystem::EditorECS::Position>()
			.each([&](const PositionDesc& positionDesc, EntitySystem::EditorECS::Position& position)
				{
					ParsePosition(level.GetLevelObject(positionDesc.objectID)->GetComponent(positionDesc.componentID)->second, position);
				});
	}
}