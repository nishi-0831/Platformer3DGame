#include "Box3D.h"
#include "Transform.h"
#include "Fbx.h"
#include "Entity.h"
unsigned int mtgb::Box3D::generateCounter_{ 0 };

mtgb::Box3D::Box3D() : GameObject(GameObjectBuilder()
	.SetPosition({0,0,10})
	.SetScale({1,1,1})
	.SetName("Box3D (" + std::to_string(generateCounter_++) + ")")
	.Build())
	//,pTransform_{Component<Transform>()}
//	,pMeshRenderer_{Component<MeshRenderer>()}
	//,pCollider_{Component<Collider>()}
	,ImGuiShowable(name_,ShowType::Inspector,Entity::entityId_)
{
	

}

mtgb::Box3D::~Box3D()
{
}

void mtgb::Box3D::Update()
{
}

void mtgb::Box3D::Draw() const
{
	//pCollider_->Draw();
}

void mtgb::Box3D::ShowImGui()
{
	MTImGui::Instance().ShowComponents(Entity::entityId_);
}

std::vector<IComponentMemento*> mtgb::Box3D::GetDefaultMementos(EntityId _entityId) const
{
	std::vector<IComponentMemento*> mementos;
	TransformData transformData
	{
		.position{0,0,10},
		.scale{1,1,1}
	};

	ColliderData colliderData
	{
		.type{ColliderType::TYPE_AABB},
		.isStatic{false},
		.colliderTag{},
		.center{transformData.position},
		.radius{transformData.scale.x * 0.5f},
		.extents{transformData.scale * 0.5f}
	};

	MeshRendererData meshData
	{
		.meshHandle{Fbx::Load("Model/Box.fbx")},
		.layer{AllLayer()},
		.shaderType{ShaderType::FbxParts}
	};

	mementos.push_back(new TransformMemento(_entityId,transformData));
	mementos.push_back(new ColliderMemento(_entityId, colliderData));
	mementos.push_back(new MeshRendererMemento(_entityId, meshData));

	return mementos;
}


