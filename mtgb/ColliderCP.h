#pragma once
#include "ISystem.h"
#include "ComponentPool.h"
#include "Collider.h"

#include <vector>
#include "WindowContext.h"
#include "RectContainsInfo.h"
#include "RectF.h"
#include "GameObjectTag.h"

namespace mtgb
{
	class Collider;
	class GameObject;
	class ColliderCP : public ComponentPool<Collider>
	{
	public:
		ColliderCP();
		~ColliderCP();

		void Start() override;
		void Update() override;

		void TestDraw() const;
		void Draw();
		EntityId RayCastHitAll(const Vector3& _origin, const Vector3& _dir, float dist);
		bool RayCastHit(const Vector3& _origin, const Vector3& _dir, float* dist, EntityId _entityId);
		void IsHitAll(const Vector3& _center, float _radius, std::vector<EntityId>* _entityIds);

		
		/// <summary>
		/// <para> �Q�[���I�u�W�F�N�g���E�B���h�E�̃X�N���[�����W�n�֕ϊ��A�����œn���ꂽ��`�Ɋ܂܂�Ă��邩���� </para>
		/// <para>  �܂܂�Ă���ꍇ�̓R���N�V�����Ɋi�[����� </para>
		/// <para>  �E�B���h�E�̌��Ɉʒu����ꍇ�ł��i�[�����̂ŁA�E�B���h�E�ɉf���Ă��邩���肷��ꍇ��z��0����1���ǂ����m�F���邱�� </para>
		/// </summary>
		/// <param name="_rect">�܂܂�Ă��邩�̔���Ɏg�����`</param>
		/// <param name="_name">���肳���Q�[���I�u�W�F�N�g�̖��O</param>
		/// <param name="_info">���肳�ꂽ�Q�[���I�u�W�F�N�g�̏����i�[����R���N�V�����̃|�C���^</param>
		/// <param name="_context">�X�N���[�����W�n�ւ̕ϊ��ɗ��p����E�B���h�E</param>
		void RectContains(const RectF& _rect, const std::string& _name, std::vector<ScreenCoordContainsInfo>* _info, WindowContext _context);
		
		/// <summary>
		/// <para> �Q�[���I�u�W�F�N�g���E�B���h�E�̃X�N���[�����W�n�֕ϊ��A�����œn���ꂽ��`�Ɋ܂܂�Ă��邩���� </para>
		/// <para>  �܂܂�Ă���ꍇ�̓R���N�V�����Ɋi�[����� </para>
		/// <para>  �E�B���h�E�̌��Ɉʒu����ꍇ�ł��i�[�����̂ŁA�E�B���h�E�ɉf���Ă��邩���肷��ꍇ��z��0����1���ǂ����m�F���邱�� </para> 
		/// </summary>
		/// <param name="_rect">�܂܂�Ă��邩�̔���Ɏg�����`</param>
		/// <param name="_tag">���肳���Q�[���I�u�W�F�N�g�̃^�O</param>
		/// <param name="_info">���肳�ꂽ�Q�[���I�u�W�F�N�g�̏����i�[����R���N�V�����̃|�C���^</param>
		/// <param name="_context">�X�N���[�����W�n�ւ̕ϊ��ɗ��p����E�B���h�E</param>
		void RectContains(const RectF& _rect, GameObjectTag _tag, std::vector<ScreenCoordContainsInfo>* _info, WindowContext _context);

	private:
		void RectContainsImpl(const RectF& _rect,const std::vector<GameObject*>& _objs, std::vector<ScreenCoordContainsInfo>* _info, WindowContext _context);
	};
}
