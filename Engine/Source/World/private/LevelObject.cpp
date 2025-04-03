#include <LevelObject.h>

namespace GameEngine::World
{
	void LevelObject::AddComponent(const ComponentName& name, const ComponentDesc& desc)
	{
		Component component{ name, desc };
		m_ComponentList.insert({component.GetID(), component});
	}
}