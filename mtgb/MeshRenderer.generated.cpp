// MeshRenderer.generated.cpp
#include "MeshRenderer.generated.h"
#include "MeshRenderer.h"
#include "TypeRegistry.h"
#include "MTImGui.h"

namespace mtgb
{
	// ImGui表示処理を登録
	struct MeshRenderer_Register
	{
		MeshRenderer_Register()
		{
			RegisterShowFuncHolder::Set<MeshRenderer>(
				[](MeshRenderer* _target, const char* _name)
				{
					PropertyDisplayRegistry::Instance().ShowProperty(&_target->meshFileName, "meshFileName");
					PropertyDisplayRegistry::Instance().ShowProperty(&_target->meshHandle, "meshHandle");
					PropertyDisplayRegistry::Instance().ShowProperty(&_target->layer, "layer");
					PropertyDisplayRegistry::Instance().ShowProperty(&_target->shaderType, "shaderType");
				}
			);
			MTImGui::Instance().RegisterComponentViewer<MeshRenderer>();
		}
	};

	static MeshRenderer_Register meshrenderer_register;
} // namespace mtgb
