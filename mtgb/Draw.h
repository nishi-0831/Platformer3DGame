#pragma once
#include "Vector2Int.h"
#include "RectInt.h"
#include "RectF.h"
#include "Vector2F.h"
#include "Color.h"
#include "ISystem.h"
#include "ShaderType.h"
#include "TextAlignment.h"
#include <set>
#include "UIDrawCommand.h"
#include <type_traits>

namespace mtgb
{
	class Figure;
	class Transform;
	class FbxModel;
	class Ground;
	class PlaneUVScroll;

	/// <summary>
	/// 描画する系
	/// </summary>
	class Draw final : public ISystem
	{
		friend class RenderSystem;

	  public:
		enum struct Pivot
		{
			TOP_LEFT,
			CENTER,
			BOTTOM_MIDDLE,
		};

	  public:
		static void SetShaderOnce(ShaderType _type)
		{
			Draw::onceShaderType_ = _type;
		}
		/// <summary>
		/// シェーダがセットされているかチェックする
		/// </summary>
		/// <param name="_default">セットされていない場合のデフォルトシェーダ</param>
		static void CheckSetShader(ShaderType _default);

		static void Box(
			const Vector2Int& _begin,
			const Vector2Int& _end,
			Color _color,
			const UIParams& _uiParams = defaultUIParams_
		);

		static void Box(const RectInt& _rect, Color _color, const UIParams& _uiParams = defaultUIParams_);

		static void Image(
			ImageHandle _hImage,
			const RectF& _draw,
			const RectF& _cut,
			float _rotationZ,
			const UIParams& _uiParams = defaultUIParams_,
			Color _color			  = Color::WHITE
		);

		static void Image(
			ImageHandle _hImage,
			const RectF& _draw,
			const UIParams& _uiParams = defaultUIParams_,
			Color _color			  = Color::WHITE
		);

		static void Image(
			ImageHandle _hImage,
			const Transform& _transform,
			Color _color			  = Color::WHITE,
			const UIParams& _uiParams = defaultUIParams_
		);

		static void FBXModel(
			FBXModelHandle _hFBXModel,
			const Transform& _pTransform,
			int _frame,
			ShaderType _shaderType = ShaderType::FBX_PARTS
		);
		static void SeaUVScroll(const Transform& _transform);
		/// <summary>
		/// <para> 事前にLoadをしたテキストを描画 </para>
		/// <para> 文字列内容、サイズが変化しない場合に適している </para>
		/// <para> 矩形領域に描画される 幅と高さはLoad時に指定したもの </para>
		/// </summary>
		/// <param name="_hText">テキストのハンドル</param>
		/// <param name="_origin">矩形領域の左上</param>
		/// <param name="_alignment">テキストの配置</param>
		/// <param name="_uiParams">UIとして描画する際の設定</param>
		static void Text(
			TextHandle _hText,
			const Vector2F& _origin,
			TextAlignment _alignment  = currentDefaultTextAlignment_,
			const UIParams& _uiParams = defaultUIParams_
		);

		/// <summary>
		/// <para> 文字列内容が頻繁に変化するテキスト(タイマーやスコアなど)を即時に描画</para>
		/// <para> 矩形領域に描画される 幅と高さはウィンドウのサイズ </para>
		/// </summary>
		/// <param name="_text">描画する文字列</param>
		/// <param name="_topLeft">矩形の左上</param>
		/// <param name="_size">テキストのフォントサイズ（省略時は設定中のサイズ）</param>
		/// <param name="_alignment">テキストの配置（省略時は設定中の配置）</param>
		/// <param name="_uiParams">UIとして描画する際の設定</param>
		static void ImmediateTextW(
			std::wstring_view _text,
			Vector2F _topLeft,
			int _size				  = currentDefaultFontSize_,
			TextAlignment _alignment  = currentDefaultTextAlignment_,
			const UIParams& _uiParams = defaultUIParams_
		);

		/// <summary>
		/// <para> 文字列内容が頻繁に変化するテキスト(タイマーやスコアなど)を即時に描画</para>
		/// <para> 矩形領域に描画される </para>
		/// </summary>
		/// <param name="_text">描画する文字列</param>
		/// <param name="_rect">矩形</param>
		/// <param name="_size">テキストのフォントサイズ（省略時は設定中のサイズ）</param>
		/// <param name="_alignment">テキストの配置（省略時は設定中の配置）</param>
		/// <param name="_uiParams">UIとして描画する際の設定</param>
		static void ImmediateTextW(
			std::wstring_view _text,
			RectF _rect,
			int _size				  = currentDefaultFontSize_,
			TextAlignment _alignment  = currentDefaultTextAlignment_,
			const UIParams& _uiParams = defaultUIParams_
		);

		/// <summary>
		/// <para> 文字列内容が頻繁に変化するテキスト(タイマーやスコアなど)を即時に描画</para>
		/// <para> 矩形領域に描画される 幅と高さはウィンドウのサイズ </para>
		/// </summary>
		/// <param name="_text">描画する文字列</param>
		/// <param name="_topLeft">矩形の左上</param>
		/// <param name="_size">テキストのフォントサイズ（省略時は設定中のサイズ）</param>
		/// <param name="_alignment">テキストの配置（省略時は設定中の配置）</param>
		/// <param name="_uiParams">UIとして描画する際の設定</param>
		static void ImmediateText(
			std::string_view _text,
			Vector2F _topLeft,
			int _size				  = currentDefaultFontSize_,
			TextAlignment _alignment  = currentDefaultTextAlignment_,
			const UIParams& _uiParams = defaultUIParams_
		);

		/// <summary>
		/// <para> 文字列内容が頻繁に変化するテキスト(タイマーやスコアなど)を即時に描画</para>
		/// <para> 矩形領域に描画される </para>
		/// </summary>
		/// <param name="_text">描画する文字列</param>
		/// <param name="_rect">矩形</param>
		/// <param name="_size">テキストのフォントサイズ（省略時は設定中のサイズ）</param>
		/// <param name="_alignment">テキストの配置（省略時は設定中の配置）</param>
		/// <param name="_uiParams">UIとして描画する際の設定</param>
		static void ImmediateText(
			std::string_view _text,
			RectF _rect,
			int _size				  = currentDefaultFontSize_,
			TextAlignment _alignment  = currentDefaultTextAlignment_,
			const UIParams& _uiParams = defaultUIParams_
		);

		/// <summary>
		/// テキストの配置を設定
		/// 以降テキスト描画時に省略すると適用される
		/// </summary>
		/// <param name="_alignment">指定する配置方法</param>
		static void ChangeTextAlignment(TextAlignment _alignment);
		/// <summary>
		/// フォント、というか文字のサイズを設定
		/// 以降テキスト描画時に省略すると適用される
		/// </summary>
		/// <param name="_size"></param>
		static void ChangeFontSize(int _size);

	  public:
		Draw();
		~Draw();
		void Initialize() override;
		void Update() override;
		static void FlushUIDrawCommands(GameObjectLayerFlag _layer);
		static void ClearUICommands();
		static int CalcScaledFontSize(int _baseSize);

	  private:
		FbxModel* pFbxModel_;
		Figure* pFigure_;
		PlaneUVScroll* pSeaPlane_;

		static int currentDefaultFontSize_;
		static TextAlignment currentDefaultTextAlignment_;
		static UIParams defaultUIParams_;
		static ShaderType onceShaderType_;
		static std::multiset<UIDrawCommand> uiDrawCommands_;
	};
} // namespace mtgb
