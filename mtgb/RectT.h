#pragma once
#include "Vector2T.h"
namespace mtgb
{
	/// <summary>
	/// <para>��`�\����</para>
	/// <para>�n�_�ƃT�C�Y�������Ă���</para>
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template<typename T>
	struct RectT
	{

		RectT() = default;
		RectT(
			T _x,
			T _y,
			T _width,
			T _height) :
			x{ _x },
			y{ _y },
			width{ _width },
			height{ _height }
		{
		}
		template<typename U>
		RectT(
			U _x,
			U _y,
			U _width,
			U _height) :
			x{ static_cast<T>(_x) },
			y{ static_cast<T>(_y) },
			width{ static_cast<T>(_width)},
			height{static_cast<T>(_height)}
		{ }
		template<typename U>
		RectT(const RectT<U>& _other)
			: x{static_cast<T>(_other.x)}
			, y{static_cast<T>(_other.y)}
			, width{static_cast<T>(_other.width)}
			, height{static_cast<T>(_other.height)}
		{
		}
		RectT(const Vector2T<T>& _point,const Vector2T<T>& _size):
			point{ _point },
			size{ _size }
		{}
		RectT(const RectT& _other):
			point{_other.point},
			size{_other.size}
		{}
		RectT(Vector2T<T>&& _point, Vector2T<T>&& _size):
			point{std::move(_point)},
			size{std::move(_size)}
		{ }
		RectT(RectT&& _other) noexcept :
			point{ std::move(_other.point) },
			size{ std::move(_other.size) }
		{}
		
		RectT& operator=(const RectT& _other)
		{
			x = _other.x;
			y = _other.y;
			width = _other.width;
			height = _other.height;
			return *this;
		}
		RectT& operator=(RectT&& _other) noexcept
		{
			if (this != &_other)
			{
				x = std::move(_other.x);
				y = std::move(_other.y);
				width = std::move(_other.width);
				height = std::move(_other.height);
			}
			return *this;
		}
		

		union
		{
			struct
			{
				T x;
				T y;
			};
			Vector2T<T> point;
		};
		union
		{
			struct
			{
				T width;
				T height;
			};
			Vector2T<T> size;
		};

		/// <summary>
		/// �n�_���擾 (�ŏ��_)
		/// </summary>
		/// <returns>�n�_�̍��W</returns>
		inline Vector2T<T> GetBegin() const
		{
			return
			{
				(size.x < 0) ? (point.x + size.x) : (point.x),
				(size.y < 0) ? (point.y + size.y) : (point.y)
			};
		}
		/// <summary>
		/// �I�_���擾 (�ő�_)
		/// </summary>
		/// <returns>�I�_�̍��W</returns>
		inline Vector2T<T> GetEnd() const
		{
			return
			{
				(size.x > 0) ? (point.x + size.x) : (point.x),
				(size.y > 0) ? (point.y + size.y) : (point.y)
			};
		}

		/// <summary>
		/// ��`�𔽓]������
		/// </summary>
		/// <param name="_rect">���]����������`</param>
		/// <returns>���]���ꂽ��`</returns>
		static inline RectT Reverse(const RectT& _rect)
		{
			Vector2T<T> reversePoint{ _rect.point + _rect.size };
			Vector2T<T> reverseSize{ _rect.size * -1 };

			return { reversePoint, reverseSize };
		}
		/// <summary>
		/// ��`��x���Ŕ��]������
		/// </summary>
		/// <param name="_rect">x���Ŕ��]����������`</param>
		/// <returns>���]���ꂽ��`</returns>
		static inline RectT ReverseAxisX(const RectT& _rect)
		{
			Vector2T<T> reversePoint
			{
				_rect.point.x,
				_rect.point.y + _rect.size.y
			};
			Vector2T<T> reverseSize
			{
				_rect.size.x,
				_rect.size.y * -1,
			};

			return { reversePoint, reverseSize };
		}

		/// <summary>
		/// ���������`���C���X�^���X
		/// </summary>
		/// <param name="_begin">�n�_���W</param>
		/// <param name="_end">�I�_���W</param>
		/// <returns>��`�\����</returns>
		static inline RectT FromLine(
			const Vector2T<T>& _begin,
			const Vector2T<T>& _end)
		{
			return
			{
				_begin,
				_end - _begin
			};
		}

		/// <summary>
		/// <para> �_����`�͈̔͂Ɋ܂܂�Ă��邩 </para>
		/// <para> �E�[�A���[�͊܂܂�Ȃ� </para>
		/// </summary>
		/// <param name="_pos"></param>
		/// <param name="_rect"></param>
		/// <returns>��`�͈̔͂Ɋ܂܂�Ă���Ȃ� true</returns>
		static inline bool Contains(
			const Vector2T<T>& _pos,
			const RectT& _rect)
		{
			Vector2T<T> begin = _rect.GetBegin();
			Vector2T<T> end = _rect.GetEnd();
			return (
				_pos.x >= begin.x &&
				_pos.y >= begin.y &&
				_pos.x < end.x &&
				_pos.y < end.y
				);
		}
	};
	
}