#include "stdafx.h"
#include "HPViewer.h"
#include <format>
mtgb::HPViewer::HPViewer(int _hpCount)
{
	for (int i = 0; i < _hpCount; i++)
	{
		std::string name = std::format("HP ({})", i);
		GameObject* pHpImage = new GameObject(GameObjectBuilder()
			.SetPosition({ 0,0,0 })
			.SetName(name)
			.Build());
		Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(pHpImage);

		ImageRenderer& imageRenderer = ImageRenderer::Get(pHpImage->GetEntityId());
		imageRenderer.handle_ = Image::Load("Image/Heart.png");
		pHpImages_.push_back(&imageRenderer);
	}
	pos_ = Vector2F(0, 0);
	size_ = Vector2F(30, 30);
}

mtgb::HPViewer::~HPViewer()
{
}

void mtgb::HPViewer::Update()
{
	for (int i = 0; i < pHpImages_.size(); i++)
	{
		ImageRenderer* pImageRenderer = pHpImages_[i];
		Vector2F pos = { pos_.x + size_.x * i, pos_.y };
		pImageRenderer->drawRect_ = RectF(pos, size_);
	}
}

void mtgb::HPViewer::Draw() const
{
}

void mtgb::HPViewer::Start()
{
}

void mtgb::HPViewer::TakeDamage(int _damage)
{
	if (_damage <= 0)
	{
		return;
	}
	
	while (_damage > 0)
	{
		ImageRenderer* pImageRenderer =  pHpImages_.back();
		EntityId id = pImageRenderer->GetEntityId();
		FindGameObject(id)->DestroyMe();
		pHpImages_.pop_back();

		_damage--;
	}
}
