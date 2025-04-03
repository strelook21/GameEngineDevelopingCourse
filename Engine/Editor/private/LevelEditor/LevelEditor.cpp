#include <LevelEditor/LevelEditor.h>
#include <LevelEditor/LevelSerializer.h>
#include <LevelEditor/ECS/ecsLevelEditor.h>

#include <map>
#include <Windows.h>

#include <Filesystem.h>
#include <ECS/ecsSystems.h>
#include <Parser/WorldParser.h>

#include <imgui.h>
#include <imgui_stdlib.h>

namespace GameEngine
{
	namespace Editor
	{
		LevelEditor::LevelEditor(flecs::world& world)
		{
			m_Level = LevelSerializer::Deserialize(Core::g_FileSystem->GetFilePath("Levels/Main.xml").generic_string());

			for (auto& [id, levelObject] : m_Level->GetLevelObjects())
			{
				m_ObjectCount++;
				flecs::entity entity = world.entity(levelObject.GetName().c_str());

				World::LevelObject::ComponentList& componentList = levelObject.GetComponents();

				World::LevelObject::ComponentList::iterator positionAttribute = std::ranges::find_if(componentList,
					[](auto& component)
					{
						return !std::strcmp(component.second.first.c_str(), "Position");
					}
				);
				World::LevelObject::ComponentList::iterator geometryAttribute = std::ranges::find_if(componentList,
					[](auto& component)
					{
						return !std::strcmp(component.second.first.c_str(), "GeometryPtr");
					}
				);

				if (positionAttribute != componentList.end() &&
					geometryAttribute != componentList.end())
				{
					assert(World::WorldParser::GetCustomComponents().contains(geometryAttribute->second.second));

					entity.set(EntitySystem::LevelEditorECS::PositionDesc{ levelObject.GetID(), positionAttribute->second.GetID() });

					// Can be set to 0 since it doesn't matter now, will be updated by the system
					entity.set(EntitySystem::EditorECS::Position{ 0.0f, 0.0f, 0.0f });
					entity.set(GeometryPtr{
						reinterpret_cast<RenderCore::Geometry*>(
							World::WorldParser::GetCustomComponents()[geometryAttribute->second.second]
							)
						});
				}
			}
			
			m_World = &world;
			EntitySystem::LevelEditorECS::RegisterLevelEditorEcsSystems(world, *m_Level);
		}

		void LevelEditor::Draw()
		{

			std::map<std::string*, float*> mapper{};

			ImGui::Begin(GetName());

			if (m_Level.has_value()) [[likely]]
			{
				for (auto& [id, levelObject] : m_Level->GetLevelObjects())
				{
					if (ImGui::TreeNode(levelObject.GetName().c_str()))
					{
						for (auto& [id, component] : levelObject.GetComponents())
						{
							if (!std::strcmp(component.first.c_str(), "Position"))
							{
								std::stringstream in_comp(component.second.c_str());
								std::string token;
								std::vector<std::string> parse;
								while (std::getline(in_comp, token, ',')) {
									parse.push_back(token);
								}
								float* f = new float[3];
								f[0] = std::stof(parse[0]);
								f[1] = std::stof(parse[1]);
								f[2] = std::stof(parse[2]);
								ImGui::InputFloat3("Position", f);
								
								//mapper[&component.second] = f;
								std::ostringstream out_comp;
								for (int i = 0; i < 3; i++)
								{
									out_comp << f[i];
									if (i < 2)
										out_comp << ",";
								}
								component.second = out_comp.str();
							}
							else
								ImGui::InputText(component.first.c_str(), &component.second);
						}

						ImGui::TreePop();
					}
				}
			}

			if (ImGui::Button("Save"))
			{
				m_SaveButtonMessageTimer.Reset();
				m_SaveButtonPressed = true;

				Save();
			}

			if (m_SaveButtonPressed)
			{
				ImGui::SameLine();
				ImGui::Text("Saved!");
			}

			if (ImGui::Button("Create Default"))
			{
				m_CreateButtonMessageTimer.Reset();
				m_CreateButtonPressed = true;

				CreateDefault();
			}

			if (m_CreateButtonPressed)
			{
				ImGui::SameLine();
				ImGui::Text("Created!");
			}

			ImGui::End();

			for (auto& [comp, f] : mapper)
			{
				std::ostringstream out_comp;
				for (int i = 0; i < 3; i++)
				{
					out_comp << f[i];
					if (i < 2)
						out_comp << ",";
				}
				*comp = out_comp.str();
			}
		}

		void LevelEditor::Update(float dt)
		{
			m_SaveButtonMessageTimer.Tick();

			if (m_SaveButtonMessageTimer.GetTotalTime() > m_TimeToShowSaveButtonMessage)
			{
				m_SaveButtonPressed = false;
			}

			m_CreateButtonMessageTimer.Tick();

			if (m_CreateButtonMessageTimer.GetTotalTime() > m_TimeToShowCreateButtonMessage)
			{
				m_CreateButtonPressed = false;
			}
		}

		void LevelEditor::Save()
		{
			assert(m_Level.has_value());
			LevelSerializer::Serialize(Core::g_FileSystem->GetFilePath("Levels/Main.xml").generic_string(), m_Level.value());
		}

		void LevelEditor::CreateDefault()
		{

			World::LevelObject newObject;
			newObject.SetName(("Obj" + std::to_string(m_ObjectCount)).c_str());
			newObject.AddComponent("Position", "0.0,0.0,0.0");
			newObject.AddComponent("GeometryPtr", "Cube");
			m_Level->AddLevelObject(newObject);

			flecs::entity entity = m_World->entity(newObject.GetName().c_str());

			World::LevelObject::ComponentList& componentList = newObject.GetComponents();

			World::LevelObject::ComponentList::iterator positionAttribute = std::ranges::find_if(componentList,
				[](auto& component)
				{
					return !std::strcmp(component.second.first.c_str(), "Position");
				}
			);
			World::LevelObject::ComponentList::iterator geometryAttribute = std::ranges::find_if(componentList,
				[](auto& component)
				{
					return !std::strcmp(component.second.first.c_str(), "GeometryPtr");
				}
			);

			if (positionAttribute != componentList.end() &&
				geometryAttribute != componentList.end())
			{
				assert(World::WorldParser::GetCustomComponents().contains(geometryAttribute->second.second));

				entity.set(EntitySystem::LevelEditorECS::PositionDesc{ newObject.GetID(), positionAttribute->second.GetID() });

				// Can be set to 0 since it doesn't matter now, will be updated by the system
				entity.set(EntitySystem::EditorECS::Position{ 0.0f, 0.0f, 0.0f });
				entity.set(GeometryPtr{
					reinterpret_cast<RenderCore::Geometry*>(
						World::WorldParser::GetCustomComponents()[geometryAttribute->second.second]
						)
					});
			}
			m_ObjectCount++;
		}
	}
}