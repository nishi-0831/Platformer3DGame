#pragma once
#include <string>
#include "Handlers.h"
#include "MTStat/MTStat.h"
#include "Editor/ImGuiShowable.h"
#include "Core/ISystem.h"
#include "Camera/ICamera.h"
#include "Editor/SelectionMode.h"
namespace mtgb
{
	class Transform;
	enum class CameraOperation
	{
		TRACK, // カメラを前後に動かす
		DOLLY, // カメラを平行に移動
		PAN,   // カメラ自身を軸として回転
		ORBIT  // ターゲットを軸として回転
	};
	class ImGuiEditorCamera : public ImGuiShowable, public ISystem, public ICamera
	{
	  public:
		ImGuiEditorCamera();
		~ImGuiEditorCamera();
		void ShowImGui() override;
		void Initialize() override;
		/// <summary>
		/// エディタのカメラを有効にする。
		/// 有効にすることでレイキャストや描画がエディタのカメラ基準になる
		/// </summary>
		void SetCamera();
		/// <summary>
		/// シーン内にエディタのカメラを作成
		/// </summary>
		void CreateCamera();

		void Update() override;
		/// <summary>
		/// 指定されたオブジェクトに急接近する
		/// </summary>
		/// <param name="_ids"></param>
		void FrameSelected(std::span<EntityId> _ids);

	  private:
		void DoDolly();
		void DoPan();
		void DoTrack();
		/// <summary>
		/// その場でカメラを回転させる
		/// </summary>
		void MoveCameraOnTheSpot();
		/// <summary>
		/// レイを飛ばして、シーン内にあるゲームオブジェクトを選択する
		/// </summary>
		void SelectGameObject();
		/// <summary>
		/// マウスのドラッグを処理する
		/// </summary>
		void ProcessDrag();
		/// <summary>
		/// 範囲選択を行う
		/// </summary>
		void RectSelect();
		/// <summary>
		/// オブジェクトのコライダーにスナップして移動させる
		/// </summary>
		void SurfaceSnap();
		CameraHandleInScene hCamera_;
		mtstat::MTStat<CameraOperation> sCameraOperation_;
		// カメラを回転させる際の感度
		float rotateSensitivity_;
		// カメラを移動させる速度
		float moveSpeed_;
		std::string windowName_;
		// オブジェクトを画面の中央に表示する際の、距離に掛ける倍率
		// ちょうど収まる距離を基準に、カメラ位置を調整するのに使う
		// 1.0fでぴったり、1.2fで20%分さらに離れる
		float frameSelectedDistanceScale_;
		// マウスをシーンビュー内で押下したか否か
		bool mouseDownInWindow_;
		/// <summary>
		/// ドラッグしているか否か
		/// </summary>
		bool dragging_;
		/// <summary>
		/// ドラッグによって作られる矩形
		/// </summary>
		RectF dragRect_;
		SelectionMode selectionMode_;
		ImVec2 windowPos_;
		// 選択範囲となる矩形の枠の色
		ImU32 rectFrameColor_;
		// 選択範囲となる矩形を塗りつぶす色
		ImU32 rectFillColor_;
		// ドラッグ状態に遷移する条件となる、マウスの閾値
		// 長押ししてからのマウス移動量がこの値を超えるとドラッグ状態になる
		float dragThresholdMovement_;
		
		static constexpr float MIN_DISTANCE_TO_PIVOT { 0.01f };
	};
} // namespace mtgb