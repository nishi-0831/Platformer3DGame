#pragma once
#include "Axis.h"
typedef long LONG;
namespace mtgb
{
	struct InputConfig
	{

		LONG xRange; // x軸の値の範囲
		LONG yRange; // y軸の値の範囲
		LONG zRange; // z軸の値の範囲

		/// <summary>
		/// マウス移動量の上限
		/// この値をもとに移動量を0～1に正規化する
		/// </summary>
		LONG maxMouseMovement;
		float deadZone; // 入力を無視する閾値

		/// <summary>
		/// <para> x,y,z軸のアナログスティックから取得する値の範囲の設定 </para>
		/// 下限が負、上限が正
		/// 設定した値が100なら-100～100の値で取れる
		/// </summary>
		/// <param name="_range">範囲となる絶対値</param>
		void SetRange(LONG _range);

		/// <summary>
		/// <para> 指定した軸のアナログスティックから取得する値の範囲の設定 </para>
		/// <para> 下限が負、上限が正 </para>
		/// <para> 設定した値が100なら-100～100の値で取れる </para>
		/// </summary>
		/// <param name="_range">範囲となる絶対値</param>
		/// <param name="_axis">設定する軸</param>
		void SetRange(LONG _range, Axis _axis);

		/// <summary>
		/// 指定した値(割合)をデッドゾーンに設定
		/// </summary>
		/// <param name="_deadZone">0～1の範囲で指定してください</param>
		void SetDeadZone(float _deadZone);

		/// <summary>
		/// デッドゾーンを適用して0～1の範囲に正規化する
		/// </summary>
		/// <param name="value">適用する値</param>
		/// <returns>適用された値</returns>
		float ApplyDeadZone(float _value) const;

		/// <summary>
		/// マウス移動量の上限を設定する
		/// この値をもとに移動量を0～1に正規化する
		/// </summary>
		/// <param name="_value"></param>
		void SetMaxMouseMovement(float _value);

		/// <summary>
		/// マウス移動量を正規化して返す
		/// </summary>
		/// <param name="_movement"></param>
		/// <returns></returns>
		float NormalizeMouseMovement(float _movement) const;
	};
} // namespace mtgb