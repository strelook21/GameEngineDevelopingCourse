#pragma once

#include <World/export.h>
#include <map>

namespace GameEngine::World
{
	class WORLD_API LevelObject final
	{
	public:
		using ComponentName = std::string;
		using ComponentDesc = std::string;
		struct Component {
			Component(const ComponentName& name = "N\\A", const ComponentDesc& desc = "N\\A") : first(name), second(desc) { static unsigned int global_cID = 0; id = global_cID++; }

			ComponentName first;
			ComponentDesc second;

			unsigned int GetID() const { return id; }

		private:
			unsigned int id = 0;
		};
		using ComponentList = std::map<unsigned int, Component>;

	public:
		LevelObject() { static unsigned int global_oID = 0; m_ID = global_oID++; }
		~LevelObject() = default;

	public:
		void SetName(const char* name) { m_Name = name; }
		const std::string& GetName() const { return m_Name; }
		void AddComponent(const ComponentName& name, const ComponentDesc& desc);
		const ComponentList& GetComponents() const { return m_ComponentList; }
		ComponentList& GetComponents() { return m_ComponentList; }
		Component* GetComponent(unsigned int id) { return &m_ComponentList[id]; }
		unsigned int GetID() const { return m_ID;  }

	private:
		std::string m_Name = "None";
		ComponentList m_ComponentList;
		unsigned int m_ID = 0;
	};
}