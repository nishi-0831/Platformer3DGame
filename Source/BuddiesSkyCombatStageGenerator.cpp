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

	json jsonArray = json::array();
	for (GameObject* gameObj : gameObjs)
	{
		std::string className = mtgb::ExtractClassName(gameObj->GetName());
		if (className == "Box3D")
		{
			json box3DJSON;
			EntityId entityId = gameObj->GetEntityId();
			Transform& transform = Transform::Get(entityId);
			MeshRenderer& meshRenderer = MeshRenderer::Get(entityId);

			box3DJSON["position"]["x"] = transform.position.x;
			box3DJSON["position"]["y"] = transform.position.y;
			box3DJSON["position"]["z"] = transform.position.z;
			box3DJSON["scale"]["x"] = transform.scale.x;
			box3DJSON["scale"]["y"] = transform.scale.y;
			box3DJSON["scale"]["z"] = transform.scale.z;
			box3DJSON["fileName"] = meshRenderer.meshFileName;

			jsonArray.push_back(box3DJSON);
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
		openFile << std::setw(width) << jsonArray;

		openFile.close();

	}
}
