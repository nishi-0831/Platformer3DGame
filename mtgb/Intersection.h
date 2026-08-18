#pragma once
#include <DirectXCollision.h>
#include <optional>
#include <string>
#include <vector>
#include "RectF.h"
#include "Vector3.h"
#include "RectContainsInfo.h"
#include "WindowContext.h"
#include "GameObjectTag.h"
namespace mtgb::Intersection
{

	struct IntersectInfo
	{
		Vector3 closest;
		Vector3 push;
	};
	struct RaycastInfo
	{
		float distance;
		Vector3 normal;
		Vector3 point;
	};
	/// <summary>
	/// レイと球の交差判定
	/// </summary>
	/// <param name="_origin">原点</param>
	/// <param name="_dir">方向(内部で正規化される)</param>
	/// <param name="dist">
	/// <para> レイの原点からコライダーとの最初の交点までの距離を格納。</para>
	/// <para> レイの原点が球の内側の場合は、球を出る点までの距離</para>
	/// </param>
	/// <returns>交差している場合はtrue、していない場合はfalse</returns>
	bool IsHit(
		const DirectX::BoundingSphere& _sphere,
		const Vector3& _origin,
		const Vector3& _dir,
		float _maxDistance,
		RaycastInfo* _info
	);

	/// <summary>
	/// レイとAABBの交差判定
	/// </summary>
	/// <param name="_aabb">AABB</param>
	/// <param name="_origin">レイの原点</param>
	/// <param name="_dir">レイの方向</param>
	/// <param name="_dist">レイの原点とAABBの距離を格納</param>
	/// <returns>交差している場合はtrue、していない場合はfalse</returns>
	bool IsHit(
		const DirectX::BoundingBox& _aabb,
		const Vector3& _origin,
		const Vector3& _dir,
		float _maxDistance,
		RaycastInfo* _info
	);

	/// <summary>
	/// レイとOBBの交差判定
	/// </summary>
	/// <param name="_obb">OBB</param>
	/// <param name="_origin">レイの原点</param>
	/// <param name="_dir">レイの方向</param>
	/// <param name="_dist">レイの原点とOBBの距離を格納</param>
	/// <returns>交差している場合はtrue、していない場合はfalse</returns>
	bool IsHit(
		const DirectX::BoundingOrientedBox& _obb,
		const Vector3& _origin,
		const Vector3& _dir,
		float _maxDistance,
		RaycastInfo* _info
	);

	/// <summary>
	/// 球とAABBの交差判定
	/// </summary>
	/// <param name="_sphere">球</param>
	/// <param name="_aabb">AABB</param>
	/// <returns>交差している場合は最短距離点などが格納された構造体、していない場合はnulloptが返される</returns>
	std::optional<IntersectInfo> Intersect(const DirectX::BoundingSphere& _sphere, const DirectX::BoundingBox& _aabb);

	/// <summary>
	/// 球とOBBの交差判定
	/// </summary>
	/// <param name="_sphere">球</param>
	/// <param name="_obb">OBB</param>
	/// <returns>交差している場合は最短距離点などが格納された構造体、していない場合はnulloptが返される</returns>
	std::optional<IntersectInfo> Intersect(
		const DirectX::BoundingSphere& _sphere,
		const DirectX::BoundingOrientedBox& _obb
	);

	/// <summary>
	/// <para> ゲームオブジェクトをウィンドウのスクリーン座標系へ変換、引数で渡された矩形に含まれているか判定
	/// </para> <para>  含まれている場合はコレクションに格納される </para> <para>
	/// ウィンドウの後ろに位置する場合でも格納されるので、ウィンドウに映っているか判定する場合はzが0から1かどうか確認すること
	/// </para>
	/// </summary>
	/// <param name="_rect">含まれているかの判定に使われる矩形</param>
	/// <param name="_name">判定されるゲームオブジェクトの名前</param>
	/// <param name="_info">判定されたゲームオブジェクトの情報を格納するコレクションのポインタ</param>
	/// <param name="_context">スクリーン座標系への変換に利用するウィンドウ</param>
	void RectContains(
		const RectF& _rect,
		std::string_view _name,
		std::vector<ScreenCoordContainsInfo>* _info,
		WindowContext _context
	);

	/// <summary>
	/// <para> ゲームオブジェクトをウィンドウのスクリーン座標系へ変換、引数で渡された矩形に含まれているか判定
	/// </para> <para>  含まれている場合はコレクションに格納される </para> <para>
	/// ウィンドウの後ろに位置する場合でも格納されるので、ウィンドウに映っているか判定する場合はzが0から1かどうか確認すること
	/// </para>
	/// </summary>
	/// <param name="_rect">含まれているかの判定に使われる矩形</param>
	/// <param name="_tag">判定されるゲームオブジェクトのタグ</param>
	/// <param name="_info">判定されたゲームオブジェクトの情報を格納するコレクションのポインタ</param>
	/// <param name="_context">スクリーン座標系への変換に利用するウィンドウ</param>
	void RectContains(
		const RectF& _rect,
		GameObjectTag _tag,
		std::vector<ScreenCoordContainsInfo>* _info,
		WindowContext _context
	);
} // namespace mtgb::Intersection