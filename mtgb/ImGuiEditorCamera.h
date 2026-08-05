#pragma once
#include "Handlers.h"
#include "MTStat/MTStat.h"
#include "ImGuiShowable.h"
#include <string>
#include "ISystem.h"
#include "ICamera.h"
namespace mtgb
{
	class Transform;
	enum class CameraOperation
	{
		TRACK,
		DOLLY, // カメラを左右に移動
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
		void SetCamera();

		void Update() override;
		void CreateCamera();
		void FrameSelected(EntityId _entityId);

	  private:
		void DoDolly();
		void DoPan();
		void DoTrack();

		void MoveCameraSphericalOnTheSpot();
		void SelectTransform();
		CameraHandleInScene hCamera_;
		mtstat::MTStat<CameraOperation> sCameraOperation_;
		float rotateSensitivity_;
		float moveSpeed_;

		std::string windowName_;
		// オブジェクトを画面の中央に表示する際の、距離に掛ける倍率
		// ちょうど収まる距離を基準に、カメラ位置を調整するのに使う
		// 1.0fでぴったり、1.2fで20%分さらに離れる
		float frameSelectedDistanceScale_;
	};
} // namespace mtgb