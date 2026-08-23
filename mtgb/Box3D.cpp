#include "Box3D.h"
#include "Components/Transform/Transform.h"
#include "Model/Fbx.h"
#include "Core/Entity.h"
#include <format>
unsigned int mtgb::Box3D::generateCounter_ { 0 };

mtgb::Box3D::Box3D()
	: GameObject()
	, pTransform_ { Component<Transform>() }
	, pMeshRenderer_ { Component<MeshRenderer>() }
	, pCollider_ { Component<Collider>() }
{
	pCollider_->colliderType_ = ColliderType::TYPE_AABB;
	pCollider_->isStatic_	  = false;
	pCollider_->SetExtents(pTransform_->scale * 0.5f);
	pMeshRenderer_->meshFileName = "Model/WallBox.fbx";
	pMeshRenderer_->meshHandle	 = Fbx::Load(pMeshRenderer_->meshFileName);
	pMeshRenderer_->layer		 = AllLayer();
	pMeshRenderer_->shaderType	 = ShaderType::BOX3_D;
	// 型情報に登録された名前を取得
	std::string typeName = Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(Box3D));
	name_				 = std::format("{} ({})", typeName, generateCounter_++);
}

mtgb::Box3D::~Box3D() {}

void mtgb::Box3D::Update() {}

void mtgb::Box3D::Draw() const {}

void mtgb::Box3D::ShowImGui()
{
	GameObject::ShowImGui();
	ImGui::Text("EntityId:%lld", Entity::entityId_);
}

void mtgb::Box3D::Start() {}
