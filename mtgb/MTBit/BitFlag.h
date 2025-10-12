#pragma once
#include <type_traits>
#include <bitset>
#include <concepts>


namespace mtbit
{
	// �񋓌^ ���� size_t�^�̈ÖٓI�^�ϊ����ł��Ȃ��I = enum struct
	template<typename EnumStructT>
	concept IsEnumStruct = std::is_enum_v<EnumStructT> && !std::is_convertible_v<EnumStructT, std::underlying_type_t<EnumStructT>>;

	/// <summary>
	/// �X�R�[�v�t���񋓌^���g�����r�b�g�t���O
	/// </summary>
	/// <typeparam name="EnumStructT">�X�R�[�v�t���񋓌^</typeparam>
	template<typename EnumStructT>
	class BitFlag
	{
		static_assert(IsEnumStruct<EnumStructT>, "enum struct�̂ݎw��ł��܂��B");
		// �w�肳�ꂽ enum struct �̃T�C�Y���̃r�b�g��
		static constexpr size_t BIT_COUNT{ sizeof(EnumStructT) * 8 };

	public:
		/// <summary>
		/// ����\�ȃr�b�g�t���O
		/// </summary>
		class BitFlagEditor
		{
			friend BitFlag;
		public:
			~BitFlagEditor() = default;

			/// <summary>
			/// �w��̃t���O�𗧂Ă�
			/// </summary>
			/// <param name="_e">�X�R�[�v�t���񋓌^</param>
			/// <returns>�����Ċ֐����Ăяo����</returns>
			BitFlagEditor& On(const EnumStructT _e);
			/// <summary>
			/// �w��̃t���O���~�낷
			/// </summary>
			/// <param name="_e">�X�R�[�v�t���񋓌^</param>
			/// <returns>�����Ċ֐����Ăяo����</returns>
			BitFlagEditor& Off(const EnumStructT _e);
			/// <summary>
			/// �w��̃t���O��ǉ�����
			/// </summary>
			/// <param name="_other">�X�R�[�v�t���񋓌^</param>
			/// <returns>�����Ċ֐����Ăяo����</returns>
			BitFlagEditor& Add(const BitFlag _other);
			/// <summary>
			/// �w��̃t���O������
			/// </summary>
			/// <param name="_other">�X�R�[�v�t���񋓌^</param>
			/// <returns>�����Ċ֐����Ăяo����</returns>
			BitFlagEditor& Sub(const BitFlag _other);
			/// <summary>
			/// ���ׂẴt���O�𗧂Ă�
			/// </summary>
			/// <returns>�����Ċ֐����Ăяo����</returns>
			BitFlagEditor& OnAll();
			/// <summary>
			/// ���ׂẴt���O���~�낷
			/// </summary>
			/// <returns>�����Ċ֐����Ăяo����</returns>
			BitFlagEditor& OffAll();

			/// <summary>
			/// �t���O�̑�����I������
			/// </summary>
			/// <returns>�r�b�g�t���O</returns>
			BitFlag& EndEdit() { return bitFlag_; }

		private:
			BitFlagEditor(BitFlag& _bitFlag) : bitFlag_{ _bitFlag } {}

		private:
			BitFlag& bitFlag_;  // �ҏW����r�b�g�t���O
		};

	public:
		BitFlag() = default;
		BitFlag(const BitFlag&) = default;
		BitFlag(const EnumStructT _e);
		~BitFlag() = default;

		/// <summary>
		/// �t���O�̑�����J�n����
		/// </summary>
		/// <returns></returns>
		BitFlagEditor BeginEdit() { return BitFlagEditor{ *this }; }

		/// <summary>
		/// �w��t���O�������Ă��邩�ǂ���
		/// </summary>
		/// <param name="_e">�w��t���O</param>
		/// <returns>�����Ă��� true / false</returns>
		bool Has(const EnumStructT _e) const;
		/// <summary>
		/// �w�肵���t���O�̂����ꂩ�������Ă��邩�ǂ���
		/// </summary>
		/// <param name="_other">�w��t���O</param>
		/// <returns>�����Ă��� true / false</returns>
		bool Has(const BitFlag _other) const;

		/// <summary>
		/// �t���O����v���Ă��邩
		/// </summary>
		/// <param name="_other">�t���O</param>
		/// <returns>��v���Ă��� true / false</returns>
		bool Is(const BitFlag _other) const;

		/// <summary>
		/// �C���X�^���X�����
		/// </summary>
		/// <returns>�C���X�^���X</returns>
		static BitFlag New() { return BitFlag{}; }

	private:
		std::bitset<BIT_COUNT> value_;  // �r�b�g�t���O
	};

	template<typename T>
	concept BitFlagEnumStruct = IsEnumStruct<T> && requires(T t)
	{
		static_cast<mtbit::BitFlag<T>>(t);
	};
	
	template<BitFlagEnumStruct EnumStructT>
	inline mtbit::BitFlag<EnumStructT> operator|(const EnumStructT _e1, const EnumStructT _e2) 
	{
		return mtbit::BitFlag<EnumStructT>{ _e1 } | _e2;
	}
	
	template<typename EnumStructT>
	inline mtbit::BitFlag<EnumStructT> operator|(const mtbit::BitFlag<EnumStructT> _flag1, const mtbit::BitFlag<EnumStructT> _flag2)
	{
		return mtbit::BitFlag<EnumStructT>{ _flag1 }.BeginEdit().Add(_flag2).EndEdit();
	}
	
	template<typename EnumStructT>
	inline mtbit::BitFlag<EnumStructT> operator|(const EnumStructT _e, const mtbit::BitFlag<EnumStructT> _flag)
	{
		return mtbit::BitFlag<EnumStructT>{ _flag }.BeginEdit().On(_e).EndEdit();
	}
	
	template<typename EnumStructT>
	inline mtbit::BitFlag<EnumStructT> operator|(const mtbit::BitFlag<EnumStructT> _flag, const EnumStructT _e)
	{
		return mtbit::BitFlag<EnumStructT>{ _flag }.BeginEdit().On(_e).EndEdit();
	}
}

template<typename EnumStructT>
inline mtbit::BitFlag<EnumStructT>::BitFlag(const EnumStructT _e)
{
	value_.set(static_cast<size_t>(_e));
}

template<typename EnumStructT>
inline bool mtbit::BitFlag<EnumStructT>::Has(const EnumStructT _e) const
{
	return value_[static_cast<size_t>(_e)];
}

template<typename EnumStructT>
inline bool mtbit::BitFlag<EnumStructT>::Has(const BitFlag _other) const
{
	return (value_ & _other.value_).any();
}

template<typename EnumStructT>
inline bool mtbit::BitFlag<EnumStructT>::Is(const BitFlag _other) const
{
	return value_ == _other.value_;
}

template<typename EnumStructT>
inline typename mtbit::BitFlag<EnumStructT>::BitFlagEditor& mtbit::BitFlag<EnumStructT>::BitFlagEditor::On(const EnumStructT _e)
{
	bitFlag_.value_.set(static_cast<size_t>(_e));
	return *this;
}

template<typename EnumStructT>
inline typename mtbit::BitFlag<EnumStructT>::BitFlagEditor& mtbit::BitFlag<EnumStructT>::BitFlagEditor::Off(const EnumStructT _e)
{
	bitFlag_.value_.reset(static_cast<size_t>(_e));
	return *this;
}

template<typename EnumStructT>
inline typename mtbit::BitFlag<EnumStructT>::BitFlagEditor& mtbit::BitFlag<EnumStructT>::BitFlagEditor::Add(const BitFlag _other)
{
	bitFlag_.value_ |= _other.value_;
	return *this;
}

template<typename EnumStructT>
inline typename mtbit::BitFlag<EnumStructT>::BitFlagEditor& mtbit::BitFlag<EnumStructT>::BitFlagEditor::Sub(const BitFlag _other)
{
	bitFlag_.value_ &= !_other.value_;
	return *this;
}

template<typename EnumStructT>
inline typename mtbit::BitFlag<EnumStructT>::BitFlagEditor& mtbit::BitFlag<EnumStructT>::BitFlagEditor::OnAll()
{
	bitFlag_.value_.set();
	return *this;
}

template<typename EnumStructT>
inline typename mtbit::BitFlag<EnumStructT>::BitFlagEditor& mtbit::BitFlag<EnumStructT>::BitFlagEditor::OffAll()
{
	bitFlag_.value_.reset();
	return *this;
}
