#pragma once
#include "ISystem.h"
#include "cmtgb.h"
#include <string>
#include "DirectWrite.h"
#include "TextAlignment.h"
#include "Vector2F.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/composite_key.hpp>

using namespace boost::multi_index;
using namespace mtgb;

// �^�O��`
struct text_layout_order {}; // ������+�T�C�Y���L�[�Ƃ���^�O
struct handle_order {}; // �n���h�����L�[�Ƃ���^�O
struct random {}; // �Y�����ŃA�N�Z�X����^�O
struct font_size_order {}; // �t�H���g�T�C�Y���L�[�Ƃ���^�O
struct layout_box_size_order {};// ���C�A�E�g�{�b�N�X�̃T�C�Y���L�[�Ƃ���^�O

// �e�L�X�g���C�A�E�g�̑��d�C���f�b�N�X�R���e�i
using TextLayoutDatas = multi_index_container<
	TextLayoutData*,
	indexed_by<
	// ������+�T�C�Y�̕����L�[�i�����̃��C�A�E�g�{�b�N�X�œ����ꍇ������̂� non_unique�j
	ordered_non_unique<tag<text_layout_order>,
	composite_key<TextLayoutData,
	member<TextLayoutData, std::wstring, &TextLayoutData::str>,
	member<TextLayoutData, int, &TextLayoutData::fontSize>>>,

	// ���C�A�E�g�{�b�N�X�̕�+�����̕����L�[�i�����e�L�X�g�œ����ꍇ������̂� non_unique�j
	ordered_non_unique<tag<layout_box_size_order>,
	composite_key<TextLayoutData,
	member<TextLayoutData,float,&TextLayoutData::width>,
	member<TextLayoutData, float, &TextLayoutData::height>>>,

	// �n���h�����L�[�Ƃ���
	ordered_unique<tag<handle_order>,
	member<TextLayoutData, int, &TextLayoutData::handle>
	>,
	// �Y�����ŃA�N�Z�X
	random_access<tag<random>>
	>
>;

// �t�H���g�t�H�[�}�b�g�̑��d�C���f�b�N�X�R���e�i
using FontFormatDatas = multi_index_container<
	FontFormatData*,
	indexed_by<
	// �t�H���g�T�C�Y���L�[
	ordered_unique<tag<font_size_order>,
	member<FontFormatData, int, &FontFormatData::fontSize>
	>
	>
>;

namespace mtgb
{

	

	class Text : public ISystem
	{
		friend class Draw;
	public:
		Text();
		~Text();

		void Initialize() override;
		void Update() override;

		void Release();
		
		/// <summary>
		/// <para> �e�L�X�g��ǂݍ���ł��̃n���h����Ԃ� </para>
		/// <para> ��������e�A�T�C�Y���ω����Ȃ��ꍇ�ɓK���Ă��� </para>
		/// </summary>
		/// <param name="str">�ǂݍ��ޕ�����</param>
		/// <param name="size">�e�L�X�g�̃t�H���g�T�C�Y</param>
		/// <returns></returns>
		static int Load(const std::string& str, int size);

		/// <summary>
		/// <para> �e�L�X�g��ǂݍ���ł��̃n���h����Ԃ� </para>
		/// <para> ��������e�A�T�C�Y���ω����Ȃ��ꍇ�ɓK���Ă��� </para>
		/// <para> �`�悳����`�̈�̕��ƍ�����\�ߎw�肷��</para>
		/// </summary>
		/// <param name="str">�ǂݍ��ޕ�����</param>
		/// <param name="fontSize">�e�L�X�g�̃t�H���g�T�C�Y</param>
		/// <param name="layoutBoxWidth">��`�̈�̕�</param>
		/// <param name="layoutBoxHeight">��`�̈�̍���</param>
		/// <returns></returns>
		static int Load(const std::string& str, int fontSize, float layoutBoxWidth, float layoutBoxHeight);

		/// <summary>
		/// <para> �e�L�X�g��ǂݍ���ł��̃n���h����Ԃ� </para>
		/// <para> ��������e�A�T�C�Y���ω����Ȃ��ꍇ�ɓK���Ă��� </para>
		/// <para> �`�悳����`�̈�̕��ƍ�����\�ߎw�肷��</para>
		/// </summary>
		/// <param name="str">�ǂݍ��ޕ�����</param>
		/// <param name="fontSize">�e�L�X�g�̃t�H���g�T�C�Y</param>
		/// <param name="layoutBoxSize">��`�̈�̕��A����</param>
		/// <returns></returns>
		static int Load(const std::string& str, int fontSize,Vector2F layoutBoxSize);
		

#if false
		/// <summary>
		/// �n���h���ɉ������e�L�X�g��`��
		/// ���S�ɓ����������`�悵������ꍇ�ɓK���Ă���
		/// </summary>
		/// <param name="handle"></param>
		/// <param name="x">�e�L�X�g�̍��[</param>
		/// <param name="y">�e�L�X�g�̏�[</param>
		void Draw(int handle, float x, float y);

		/// <summary>
		/// �����`��
		/// �����񂾂����p�ɂɕω�����ꍇ�ɓK���Ă���
		/// </summary>
		/// <param name="text">�`�悷��e�L�X�g</param>
		/// <param name="x">�e�L�X�g�̍��[</param>
		/// <param name="y">�e�L�X�g�̏�[</param>
		void ImmediateDraw(const std::string& text, float x, float y);
		void ImmediateDraw(const std::string& text, float x, float y,int size);
		void ImmediateDraw(const std::string& text, RectInt rect,int size);


		//void ImmediateDraw(const std::wstring& text, float x, float y);

		/// <summary>
		/// �T�C�Y���w�肵�đ����`��
		/// �T�C�Y���قȂ��IDWriteTextFormat��V�����쐬���ăL���b�V������̂ŃC�[�W���O�Ȃǂɂ͎g��Ȃ��ł�
		/// </summary>
		/// <param name="text"></param>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="size"></param>
		void ImmediateDraw(const std::wstring& text, float x, float y,int size);
		void ImmediateDraw(const std::wstring& text, float x, float y);
		void ImmediateDraw(const std::wstring& text, float x,float y,float width,float height,int size);


		/// <summary>
		/// �ȍ~�̃f�t�H���g�̃t�H���g�T�C�Y��ύX����
		/// </summary>
		/// <param name="size"></param>
		void ChangeFontSize(int size);

		void ChangeTextAlignment(TextAlignment alignment);
#endif
	private:

		/// <summary>
		/// �V�K�܂��͊����̃e�L�X�g�̃n���h����Ԃ�
		/// </summary>
		/// <param name="text"></param>
		/// <param name="size"></param>
		/// <returns></returns>
		int GetOrCreateTextLayoutHandle(const std::wstring& text, int size);
		int GetOrCreateTextLayoutHandle(const std::wstring& text, int size,float width,float height);

		TextLayoutData* GetTextLayoutData(int handle);

		/// <summary>
		/// �w��T�C�Y��IDWriteTextFormat���擾�܂��͍쐬
		/// </summary>
		/// <param name="size">�t�H���g�T�C�Y</param>
		/// <returns>IDWriteTextFormat��PixelFontMetrics�̃y�A</returns>
		FontFormatData* GetOrCreateTextFormat(int size);

		//���݂̃f�t�H���g�̃t�H���g�T�C�Y
		int currentDefaultFontSize_;

		TextAlignment currentTextAlignment_;

		//�f�t�H���g�̃t�H���g�t�@�~���[
		const static wchar_t* DEFAULT_FONT_FAMILY_NAME;

		TextLayoutDatas* textLayoutDatas_; // �e�L�X�g���C�A�E�g�̃L���b�V��
		FontFormatDatas* fontFormatDatas_; // �t�H���g�t�H�[�}�b�g�̃L���b�V��

		int nextHandle_; // ���Ɋ��蓖�Ă�n���h���ԍ�
		
	};
}

