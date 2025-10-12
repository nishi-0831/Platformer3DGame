#pragma once
//#include "RectF.h"
#include "RectT.h"
#include "Game.h"
#include "Vector2.h"
#include "Screen.h"

namespace
{
	// UI�݌v���̉𑜓x
	const mtgb::Vector2 CANVAS_SIZE{ 1920.0f, 1080.0f };
}

namespace mtgb
{
	/// <summary>
	/// �݌v�X�N���[����̃t�H���g�T�C�Y����`��p�t�H���g�T�C�Y�𐶐�����
	/// </summary>
	/// <param name="_to">�݌v��̃t�H���g�T�C�Y</param>
	/// <returns>�`��p�t�H���g�T�C�Y</returns>
	inline const int GenDrawScreenFontSize(const int _to)
	{
		const float RATE_Y{ _to / CANVAS_SIZE.y };
		const int SCREEN_SIZE_Y{ Game::System<Screen>().GetSize().y };

		return static_cast<int>(SCREEN_SIZE_Y * RATE_Y);
	}

	/// <summary>
	/// �݌v��̃X�N���[����`����`�掞�̃X�N���[����`�𐶐�����
	/// </summary>
	/// <param name="_rect">�݌v���̃X�N���[����`</param>
	/// <returns>�`�掞�̃X�N���[����`</returns>
	template<typename T>
	inline const mtgb::RectT<T> GenDrawScreenFrom(const mtgb::RectT<T>& _to)
	{
		return _to;
		const RectF RATE
		{
			_to.point.x / CANVAS_SIZE.x, _to.point.y / CANVAS_SIZE.y,
			_to.size.x / CANVAS_SIZE.x, _to.size.y / CANVAS_SIZE.y,
		};
		const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };

		return
		{
			RATE.point.x * SCREEN_SIZE.x, RATE.point.y * SCREEN_SIZE.y,
			RATE.size.x * SCREEN_SIZE.x, RATE.size.y * SCREEN_SIZE.y,
		};

	}

	template<typename T>
	inline const mtgb::Vector2T<T> GenDrawScreenFrom(const mtgb::Vector2T<T>& _to)
	{
		return _to;
		const Vector2 RATE{ _to.x / CANVAS_SIZE.x, _to.y / CANVAS_SIZE.y };
		const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };

		return { SCREEN_SIZE.x * RATE.x, SCREEN_SIZE.y * RATE.y };
	}
}
