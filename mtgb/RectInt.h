#pragma once
#include "cmtgb.h"
#include "Vector2Int.h"
#include "RectT.h"
namespace mtgb
{
	using RectInt = RectT<int>;
	///// <summary>
	///// <para>��`�\����</para>
	///// <para>�n�_�ƃT�C�Y�������Ă���</para>
	///// <para>�I�_�͊֐�����Ď擾�\</para>
	///// </summary>
	//struct RectInt
	//{
	//	RectInt() :
	//		x{ 0 },
	//		y{ 0 },
	//		width{ 0 },
	//		height{ 0 }
	//	{}
	//	RectInt(
	//		const int32_t _x,
	//		const int32_t _y,
	//		const int32_t _width,
	//		const int32_t _height) :
	//		x{ _x },
	//		y{ _y },
	//		width{ _width },
	//		height{ _height }
	//	{}
	//	RectInt(const Vector2Int& _point, const Vector2Int& _size) :
	//		point{ _point },
	//		size{ _size }
	//	{}

	//	union
	//	{
	//		struct
	//		{
	//			int32_t x;  // �_ x
	//			int32_t y;  // �_ y
	//		};
	//		Vector2Int point;  // ���_
	//	};
	//	union
	//	{
	//		struct
	//		{
	//			int32_t width;   // ���R��
	//			int32_t height;  // �^�e��
	//		};
	//		Vector2Int size;  // �T�C�Y
	//	};

	//	/// <summary>
	//	/// �n�_���擾 (�ŏ��_)
	//	/// </summary>
	//	/// <returns>�n�_�̍��W</returns>
	//	inline Vector2Int GetBegin() const
	//	{
	//		return
	//		{
	//			(size.x < 0) ? (point.x + size.x) : (point.x),
	//			(size.y < 0) ? (point.y + size.y) : (point.y)
	//		};
	//	}
	//	/// <summary>
	//	/// �I�_���擾 (�ő�_)
	//	/// </summary>
	//	/// <returns>�I�_�̍��W</returns>
	//	inline Vector2Int GetEnd() const
	//	{
	//		return
	//		{
	//			(size.x > 0) ? (point.x + size.x) : (point.x),
	//			(size.y > 0) ? (point.y + size.y) : (point.y)
	//		};
	//	}

	//	/// <summary>
	//	/// ��`�𔽓]������
	//	/// </summary>
	//	/// <param name="_rect">���]����������`</param>
	//	/// <returns>���]���ꂽ��`</returns>
	//	static inline RectInt Reverse(const RectInt& _rect)
	//	{
	//		Vector2Int reversePoint{ _rect.point + _rect.size };
	//		Vector2Int reverseSize{ _rect.size * -1 };

	//		return { reversePoint, reverseSize };
	//	}
	//	/// <summary>
	//	/// ��`��x���Ŕ��]������
	//	/// </summary>
	//	/// <param name="_rect">x���Ŕ��]����������`</param>
	//	/// <returns>���]���ꂽ��`</returns>
	//	static inline RectInt ReverseAxisX(const RectInt& _rect)
	//	{
	//		Vector2Int reversePoint
	//		{
	//			_rect.point.x,
	//			_rect.point.y + _rect.size.y
	//		};
	//		Vector2Int reverseSize
	//		{
	//			_rect.size.x,
	//			_rect.size.y * -1,
	//		};

	//		return { reversePoint, reverseSize };
	//	}

	//	/// <summary>
	//	/// ���������`���C���X�^���X
	//	/// </summary>
	//	/// <param name="_begin">�n�_���W</param>
	//	/// <param name="_end">�I�_���W</param>
	//	/// <returns>��`�\����</returns>
	//	static inline RectInt FromLine(
	//		const Vector2Int& _begin,
	//		const Vector2Int& _end)
	//	{
	//		return
	//		{
	//			_begin,
	//			_end - _begin
	//		};
	//	}

	//	/// <summary>
	//	/// <para> �_����`�͈̔͂Ɋ܂܂�Ă��邩 </para>
	//	/// <para> �E�[�A���[�͊܂܂�Ȃ� </para>
	//	/// </summary>
	//	/// <param name="_pos"></param>
	//	/// <param name="_rect"></param>
	//	/// <returns>��`�͈̔͂Ɋ܂܂�Ă���Ȃ� true</returns>
	//	static inline bool Contains(
	//		const Vector2Int& _pos,
	//		const RectInt& _rect)
	//	{
	//		Vector2Int begin = _rect.GetBegin();
	//		Vector2Int end = _rect.GetEnd();
	//		return (
	//			_pos.x >= begin.x &&
	//			_pos.y >= begin.y &&
	//			_pos.x < end.x &&
	//			_pos.y < end.y
	//			);
	//	}
	//};
}
