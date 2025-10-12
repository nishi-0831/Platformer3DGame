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
		Entity() :
			entityId_{ ++entityCounter_ }  // id counter�����Z����Id������
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
		EntityId entityId_;  // �G���e�B�e�B�̎��ʎq

	private:
		static EntityId entityCounter_;  // �G���e�B�e�B���ʎq�̃J�E���^
	};
}
