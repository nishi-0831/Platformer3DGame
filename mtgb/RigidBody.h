#pragma once
#include "IComponent.h"
#include "RigidBodyCP.h"
#include "Vector3.h"
#include <functional>

namespace mtgb
{
	class RigidBodyCP;
	class Transform;

	class RigidBody : public IComponent<RigidBodyCP, RigidBody>
	{
		friend RigidBodyCP;
	public:
		using IComponent<RigidBodyCP, RigidBody>::IComponent;
		RigidBody(const EntityId _entityId);
		~RigidBody();
		inline RigidBody& operator=(const RigidBody& _other)
		{
			if (&_other == this)
			{
				return *this;
			}

			this->velocity_ = _other.velocity_;
			this->onHit_ = _other.onHit_;
			this->isNeedUpdate_ = _other.isNeedUpdate_;
			this->pTransform_ = _other.pTransform_;

			return *this;
		}

		/// <summary>
		/// ���������Ƃ��̃C�x���g�R�[���o�b�N
		/// </summary>
		/// <param name="onHit_">void(const EntityId)</param>
		void OnCollisionEnter(const std::function<void(const EntityId)>& _onHit);
		/*void OnCollisionStay();
		void OnCollisionExit();*/
	public:
		Vector3 velocity_;  // ���x

	private:
		bool isNeedUpdate_;
		std::function<void(const EntityId)> onHit_;
		Transform* pTransform_;  // TODO: ��Ȃ�Transform
	};
}
