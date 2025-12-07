// MeshRenderer.generated.cpp
# include "MeshRenderer.generated.h"
# include "MeshRenderer.h"
# include "TypeRegistry.h"

namespace mtgb
{
	// ImGui�\��������o�^
	struct MeshRenderer_Register
	{
		MeshRenderer_Register()
		{
			RegisterShowFuncHolder::Set<MeshRenderer>([]( MeshRenderer* _target, const char* _name)
				{
								TypeRegistry::Instance().CallFunc(&_target->meshFileName, "meshFileName");
								TypeRegistry::Instance().CallFunc(&_target->meshHandle, "meshHandle");
								TypeRegistry::Instance().CallFunc(&_target->layer, "layer");
								TypeRegistry::Instance().CallFunc(&_target->shaderType, "shaderType");
				});
		}
	};

	static MeshRenderer_Register meshrenderer_register;
}
