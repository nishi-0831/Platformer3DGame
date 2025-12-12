#include "stdafx.h"
#include "BuddiesSkyCombatStageGenerator.h"
#include <mtgb.h>
#include <fstream>
#include <ostream>
using namespace nlohmann;
void BuddiesSkyCombatStageGenerate()
{
	GameScene* currentScene = Game::System<SceneSystem>().GetActiveScene();

	std::vector<GameObject*> gameObjs;
	currentScene->GetGameObjects( &gameObjs);

	json j;
	for (GameObject* gameObj : gameObjs)
	{
		std::string className = mtgb::ExtractClassName(gameObj->GetName());
		if (className == "Box3D")
		{
			EntityId entityId = gameObj->GetEntityId();
			Transform& transform = Transform::Get(entityId);
			MeshRenderer& meshRenderer = MeshRenderer::Get(entityId);

			j["position"]["x"] = transform.position.x;
			j["position"]["y"] = transform.position.y;
			j["position"]["z"] = transform.position.z;
			j["scale"]["x"] = transform.scale.x;
			j["scale"]["y"] = transform.scale.y;
			j["scale"]["z"] = transform.scale.z;
			j["fileName"] = meshRenderer.meshFileName;
		}
	}
	TCHAR fileName[255] = "";
	OPENFILENAME ofn = { 0 };

	ofn.lStructSize = sizeof(ofn);

	ofn.hwndOwner = WinCtxRes::GetHWND(WindowContext::First);
	ofn.lpstrFilter = "JSONƒtƒ@ƒCƒ‹(*.json)\0*.json";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = 255;
	ofn.Flags = OFN_OVERWRITEPROMPT;

	if (GetSaveFileName(&ofn))
	{
		std::ofstream openFile(fileName);

		int width = 4;
		openFile << std::setw(width) << j;

		openFile.close();

	}
}
