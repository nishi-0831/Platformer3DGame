#include "Box3D.h"
#include "Transform.h"
#include "Fbx.h"
#include "Entity.h"
#include <format>
unsigned int mtgb::Box3D::generateCounter_{ 0 };

mtgb::Box3D::Box3D() 
	: GameObject()
	, pTransform_{ Component<Transform>() }
	, pMeshRenderer_{ Component<MeshRenderer>()}
	, pCollider_{ Component<Collider>() }
	, ImGuiShowable(ShowType::Inspector, Entity::entityId_)
{
	pCollider_->colliderType_ = ColliderType::TYPE_AABB;
	pCollider_->isStatic_ = false;
	pCollider_->SetExtents(pTransform_->scale * 0.5f);
	pMeshRenderer_->meshFileName = "Model/WallBox.fbx";
	pMeshRenderer_->meshHandle =  Fbx::Load( pMeshRenderer_->meshFileName);
	pMeshRenderer_->layer = AllLayer();
	pMeshRenderer_->shaderType = ShaderType::FbxParts;
	// å^èÓïÒÇ…ìoò^Ç≥ÇÍÇΩñºëOÇéÊìæ
	std::string typeName = Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(Box3D));
	name_ = std::format("{} ({})", typeName,generateCounter_++);
	displayName_ = name_;
}

mtgb::Box3D::~Box3D()
{
}

void mtgb::Box3D::Update()
{
}

void mtgb::Box3D::Draw() const
{
}

void mtgb::Box3D::ShowImGui()
{
	MTImGui::Instance().ShowComponents(Entity::entityId_);
	ImGui::Text("EntityId:%d", Entity::entityId_);
}

void mtgb::Box3D::Start()
{
	Component<MeshRenderer>()->shaderType = ShaderType::Box3D;
}

