#pragma once

#include <World/export.h>

#include <LevelObject.h>

namespace GameEngine::World
{
	class WORLD_API Level final
	{
	public:
		using LevelObjectList = std::map<unsigned int, LevelObject>;

	public:
		Level() = delete;
		Level(std::string& name);
		~Level() = default;

	public:
		void AddLevelObject(const LevelObject& levelObject);
		const LevelObjectList& GetLevelObjects() const { return m_LevelObjectList; }
		LevelObjectList& GetLevelObjects() { return m_LevelObjectList; }
		LevelObject* GetLevelObject(unsigned int id) { return &m_LevelObjectList[id]; }

		const std::string& GetName() const { return m_Name; }

	private:
		std::string m_Name;
		LevelObjectList m_LevelObjectList;
	};
}