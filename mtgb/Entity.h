#pragma once
#include "cmtgb.h"

namespace mtgb
{
	/// <summary>
	/// ECS��Entity
	/// </summary>
	class Entity
	{
	public:
		Entity() 
		{
		}

		virtual ~Entity()
		{
		}

		/// <summary>
		/// �G���e�B�e�BId���擾����
		/// </summary>
		/// <returns>�G���e�B�e�B���ʔԍ�</returns>
		inline const EntityId GetEntityId() const { return entityId_; }

	protected:
		EntityId entityId_{INVALID_ENTITY};  // �G���e�B�e�B�̎��ʎq

	private:
		//static EntityId entityCounter_;  // �G���e�B�e�B���ʎq�̃J�E���^
	};
}
