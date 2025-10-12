#pragma once
#include <functional>
#include "GameObjectLayer.h"
namespace mtgb
{
	/// <summary>
	/// UI�`�掞�Ɏw�肷��ݒ�
	/// </summary>
	struct UIParams
	{
		int depth = 0;// �������قǉ��A�傫���قǎ�O
		GameObjectLayerFlag layerFlag = mtbit::operator|(GameObjectLayer::A, GameObjectLayer::B);// �`�悷�郌�C���[
	};
	struct UIDrawCommand
	{
		UIParams params;
		std::function<void()> drawFunction; //�`��֐�

		bool operator<(const UIDrawCommand& other) const
		{
			return params.depth < other.params.depth;
		}
	};
}