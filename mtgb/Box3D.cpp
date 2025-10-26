#include "Box3D.h"
#include "StatefulTransform.h"
#include "Fbx.h"
#include "Entity.h"
#include "StatefulTransform.h"
unsigned int mtgb::Box3D::generateCounter_{ 0 };

mtgb::Box3D::Box3D() : GameObject(GameObjectBuilder()
	.SetPosition({0,0,10})
	.SetScale({1,1,1})
	.SetName("Box3D (" + std::to_string(generateCounter_++) + ")")
	.Build())
	,pTransform_{Component<Transform>()}
//	,pMeshRenderer_{Component<MeshRenderer>()}
	,pCollider_{Component<Collider>()}
	,ImGuiShowable(name_,ShowType::Inspector,Entity::entityId_)
{
	//pMeshRenderer_->SetMesh(Fbx::Load("Model/Box.fbx"));
	pCollider_->type_ = TYPE_AABB;
	pCollider_->SetCenter(pTransform_->position);
	pCollider_->SetExtents(pTransform_->scale * 0.5f);

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
	TransformData data
	{
		.position{0,0,10},
		.scale{1,1,1}
	};
	mementos.emplace_back(_entityId, data);

}


