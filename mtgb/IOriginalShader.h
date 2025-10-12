#pragma once
#include <string_view>
#include <d3d11.h>
#include <array>
#include <functional>


namespace mtgb
{
	/*
	* TODO:
		�E�V�F�[�_�p�X
		�E���ʗ񋓎q
		�ED3D11_RASTERIZER_DESC
		�E�����l�FVertexBuffer
		�E�����l�FIndexBuffer
		�E�����l�FConstantBuffer
		�EGPU�����~�߁FVertexBuffer
		�EGPU�����~�߁FIndexBuffer
		�EGPU�����~�߁FConstantBuffer
		�E�T���v���p(ID3D11DeviceContext*)
		
		private:
		bool isRequired;  // �K�v���̃t���O
	*/

	template<typename T>
	concept IsEnumStruct = std::is_enum_v<T> && !std::is_convertible_v<T, int>;

	/// <summary>
	/// <para>mtgb�Œ񋟂��Ȃ��Q�[�����̃V�F�[�_</para>
	/// <para>������p�����ēo�^���Ă��������B</para>
	/// </summary>
	template<IsEnumStruct EnumStructT, typename OriginalShaderT>
	class IOriginalShader
	{
		//static_assert(IsEnumStruct, "�X�R�[�v�t���񋓌^���w�肷��K�v������܂��B @IOriginalShader");
	public:/*
		/// <summary>
		/// ���_�o�b�t�@�𓮓I�ύX����ꍇ�p������
		/// </summary>
		class IWriteVertex
		{
		public:
			IWriteVertex() {}
			virtual ~IWriteVertex() {}

			
		};
		class IWriteIndex
		{
		public:
			IWriteIndex() {}
			virtual ~IWriteIndex() {}

			
		};
		class IWriteConstant
		{
		public:
			IWriteConstant() {}
			virtual ~IWriteConstant() {}

			
		};*/

	public:
		IOriginalShader() {}
		virtual ~IOriginalShader() {}

	protected:
		virtual std::string_view GetFileName() = 0;

		/// <summary>
		/// ���_�o�b�t�@�̏�����
		/// </summary>
		virtual void InitializeVertexBuffer(ID3D11Device* _pDevice);
		/// <summary>
		/// �C���f�b�N�X�o�b�t�@�̏�����
		/// </summary>
		virtual void InitializeIndexBuffer(ID3D11Device* _pDevice);
		/// <summary>
		/// �R���X�^���g�o�b�t�@�̏�����
		/// </summary>
		virtual void InitializeConstantBuffer(ID3D11Device* _pDevice);

		/// <summary>
		/// 
		/// </summary>
		/// <typeparam name="VertexT">���_�̌^</typeparam>
		/// <typeparam name="VertexesSize"></typeparam>
		/// <param name="vertexes"></param>
		template<typename VertexT, size_t VertexesSize>
		void OnWriteVertexBuffer(std::function<void(std::array<VertexT, VertexesSize>&)> vertexes);

		/// <summary>
		/// 
		/// </summary>
		/// <typeparam name="IndexSize"></typeparam>
		/// <param name="pIndexes"></param>
		template<size_t IndexSize>
		void OnWriteIndexBuffer(std::function<std::array<int, IndexSize>&> pIndexes);

		/// <summary>
		/// �R���X�^���g�o�b�t�@�ɏ�������
		/// </summary>
		/// <typeparam name="ConstantBufferT">�R���X�^���g�o�b�t�@�̌^</typeparam>
		/// <param name="_pConstantBuffer">�R���X�^���g�o�b�t�@�̃|�C���^�n��</param>
		template<typename ConstantBufferT>
		void OnWriteConstantBuffer(std::function<ConstantBufferT*> _pConstantBuffer);

		void OnWriteOther(std::function<void(ID3D11DeviceContext*)> _pDC);

	protected:
		ID3D11Buffer* pVertexBuffer_;    // ���_�̈ʒu�A�F
		ID3D11Buffer* pIndexBuffer_;     // �|���S�������ԏ���
		ID3D11Buffer* pConstantBuffer_;  //�@�V�F�[�_�̃O���[�o���ϐ�

		int indexCount_;  // �`�悷��C���f�b�N�X��
	private:
		bool isRequired_;  // override ���m�p
	};
}
