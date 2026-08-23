#pragma once
#include <string>
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Core/GameObject/GameObjectLayer.h"
#include "GameObjectTag.h"

namespace mtgb
{
	/// <summary>
	/// <para>ゲームオブジェクトを生成するときの設定</para>
	/// <para>そのまま構造体を使用可能だが、GameObjectBuilderを通すことを推奨</para>
	/// </summary>
	struct GAME_OBJECT_DESC
	{
		std::string name {};
		Vector3 position { Vector3::Zero() };
		Vector3 scale { Vector3::One() };
		Quaternion rotate { Quaternion::Identity() };
		GameObjectLayerFlag layerFlag { GameObjectLayerFlag::New().BeginEdit().OnAll().EndEdit() };
		GameObjectTag tag { GameObjectTag::UNTAGGED };
		bool isActive { true };
		bool callUpdate { true };
		bool callDraw { true };
		// padding 10byte
	};

	// MEMO: 構造体のそのまま初期化では、順番が固定されているが、
	//     : Builderを使うことで好きなタイミングで任意の値をセットできる

	/// <summary>
	/// <para>GAME_OBJECT_DESCのビルダークラス</para>
	/// <para>メンバのセッタ関数を立て続けに呼び出し、最終的にBuild関数でGAME_OBJECT_DESCを構築</para>
	/// </summary>
	class GameObjectBuilder
	{
	  public:
		/// <summary>
		/// <para>以下のように使う</para>
		/// <para>GameObjectBuilder().SetName("Name").Set...().Build()</para>
		/// </summary>
		GameObjectBuilder() {}
		~GameObjectBuilder() {}

		GameObjectBuilder& SetName(const std::string& _name)
		{
			desc_.name = _name;
			return *this;
		}
		GameObjectBuilder& SetPosition(const Vector3& _position)
		{
			desc_.position = _position;
			return *this;
		}
		GameObjectBuilder& SetRotate(const Quaternion& _rotate)
		{
			desc_.rotate = _rotate;
			return *this;
		}
		GameObjectBuilder& SetScale(const Vector3& _scale)
		{
			desc_.scale = _scale;
			return *this;
		}
		GameObjectBuilder& SetIsActive(bool _isActive)
		{
			desc_.isActive = _isActive;
			return *this;
		}
		GameObjectBuilder& SetCallUpdate(bool _callUpdate)
		{
			desc_.callUpdate = _callUpdate;
			return *this;
		}
		GameObjectBuilder& SetCallDraw(bool _callDraw)
		{
			desc_.callDraw = _callDraw;
			return *this;
		}
		GameObjectBuilder& SetLayerFlag(GameObjectLayerFlag _flag)
		{
			desc_.layerFlag = _flag;
			return *this;
		}
		GameObjectBuilder& SetTag(GameObjectTag _tag)
		{
			desc_.tag = _tag;
			return *this;
		}

		/// <summary>
		/// 設定をビルドする
		/// </summary>
		/// <returns>完成したGAME_OBJECT_DESCのコンスト参照ポインタ</returns>
		const GAME_OBJECT_DESC& Build() const
		{
			return desc_;
		}

	  private:
		GAME_OBJECT_DESC desc_;
	};
} // namespace mtgb
