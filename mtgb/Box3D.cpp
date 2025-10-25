#include "Box3D.h"
#include "TransformMemento.h"
#include "MeshRendererMemento.h"
#include "ColliderMemento.h"
#include "Fbx.h"
#include "Entity.h"
unsigned int mtgb::Box3D::generateCounter_{ 0 };

mtgb::Box3D::Box3D() : GameObject(GameObjectBuilder()
	.SetPosition({0,0,10})
	.SetScale({1,1,1})
	.SetName("Box3D (" + std::to_string(generateCounter_++) + ")")
	.Build())
	,pTransform_{Component<Transform>()}
	,pMeshRenderer_{Component<MeshRenderer>()}
	,pCollider_{Component<Collider>()}
	,ImGuiShowable(name_,ShowType::Inspector,Entity::entityId_)
{
	pMeshRenderer_->SetMesh(Fbx::Load("Model/Box.fbx"));
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

	mementos.push_back(new TransformMemento(
		_entityId,
		Vector3(0.0f, 0.0f, 10.0f),
		Vector3(1.0f, 1.0f, 1.0f),
		Quaternion()
	));

	mementos.push_back(new MeshRendererMemento(
		_entityId,
		Fbx::Load("Model/Box.fbx"),
		AllLayer(),
		ShaderType::FbxParts
	));

	mementos.push_back(new ColliderMemento(
		_entityId,
		ColliderType::TYPE_AABB,
		false,						// is static
		ColliderTag::GAME_OBJECT));

	return mementos;
}
