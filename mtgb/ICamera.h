#pragma once

namespace mtgb
{
	class Transform;
	enum class InputType
	{
		MOUSE,
		JOYPAD
	};
	class ICamera
	{
	public:
		ICamera();
		void DoOrbit();
		virtual void MoveCameraSpherical(float _distance);
		void FollowTarget();

	protected:
		/// <summary>
		/// <para> 天頂角(θ、polar) </para>
		/// <para> 鉛直方向 </para>
		/// <para> 0 -> 原点の真下(-Y) </para>
		/// <para> π/2 -> 原点と同じ高さ </para>
		/// <para> π -> 原点の真上(+Y) </para>
		/// </summary>
		float polarAngleRad_; 
		/// <summary>
		/// <para> 方位角(φ、azimuthal) </para>
		/// <para> 水平方向 </para>
		/// <para> 0 -> 原点の右(+X) </para>
		/// <para> π/2 -> 原点の後ろ(-Z) </para>
		/// <para> π -> 原点の左(-X) </para>
		/// </summary>
		float azimuthalAngleRad_; 
		float rotateSensitivity_;
		float orbitSpeed_;
		Transform* pCameraTransform_;
		Transform* pTargetTransform_;
		InputType inputType_;
		float distance_;
		bool followTarget_;
		bool adjustTargetDirection_;
		// -Y側、下側が0度、+Y側、上側が90度
		float minPolarAngleRad_, maxPolarAngleRad_;
		// +X側、右側が0度、-X側、左側が90度
		float minAzimuthalAngleRad_, maxAzimuthalAngleRad_;
		Vector3 lookAtPositionOffset_;

	};
}