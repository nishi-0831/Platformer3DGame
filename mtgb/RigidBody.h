#pragma once
#include "StatefulComponent.h"
#include "IComponentMemento.h"

#include "RigidBodyCP.h"
#include "Vector3.h"
#include "RigidBodyData.h"
#include <functional>

namespace mtgb
{
	class RigidBodyCP;
	class Transform;

	class RigidBody : public RigidBodyData, public StatefulComponent<RigidBody, RigidBodyCP, RigidBodyData, ComponentMemento<RigidBody, RigidBodyData>>
	{
		friend RigidBodyCP;
	public:
		using StatefulComponent<RigidBody, RigidBodyCP, RigidBodyData, ComponentMemento<RigidBody, RigidBodyData>>::StatefulComponent;

		RigidBody(const EntityId _entityId);
		~RigidBody();
		inline RigidBody& operator=(const RigidBody& _other)
		{
			if (&_other == this)
			{
				return *this;
			}

			this->velocity = _other.velocity;
			this->onHit_ = _other.onHit_;
			this->isNeedUpdate = _other.isNeedUpdate;
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
		//Vector3 velocity;  // ���x

	private:
		//bool isNeedUpdate;
		std::function<void(const EntityId)> onHit_;
		Transform* pTransform_;  // TODO: ��Ȃ�Transform
	};

	using RigidBodyMemento = ComponentMemento<RigidBody, RigidBodyData>;
}
