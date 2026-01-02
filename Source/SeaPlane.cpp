#include "stdafx.h"
#include "SeaPlane.h"

unsigned int SeaPlane::generateCounter_{0};

SeaPlane::SeaPlane()
	: GameObject()
	, pTransform_{Component<Transform>()}
	, pMeshRenderer_{Component<MeshRenderer>()}
	, pCollider_{Component<Collider>()}
	, ImGuiShowable(ShowType::Inspector, Entity::entityId_)
{
	pCollider_->colliderType_ = ColliderType::TYPE_AABB;
	pCollider_->isStatic_	  = false;
	pCollider_->isTrigger_	  = true;
	pCollider_->SetExtents(pTransform_->scale * 0.5f);
	pMeshRenderer_->meshFileName = "Model/Box.fbx";
	// pMeshRenderer_->meshHandle = Fbx::Load(pMeshRenderer_->meshFileName);
	pMeshRenderer_->layer	   = AllLayer();
	pMeshRenderer_->shaderType = ShaderType::Sea;
	// 型情報に登録された名前を取得
	std::string typeName = Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(SeaPlane));
	name_				 = std::format("{} ({})", typeName, generateCounter_++);
	displayName_		 = name_;
}

SeaPlane::~SeaPlane()
{
}

void SeaPlane::Update()
{
}

void SeaPlane::Draw() const
{
}

void SeaPlane::ShowImGui()
{
	MTImGui::Instance().ShowComponents(Entity::entityId_);
	ImGui::Text("EntityId:%d", Entity::entityId_);
}

void SeaPlane::Start()
{
}
