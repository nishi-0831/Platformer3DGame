#pragma once

#include <string>

#include "ISystem.h"
#include <any>
#include <unordered_map>
#include <typeindex>
#include "WindowContext.h"
#include "ShowType.h"
#include "Entity.h"
#include <string_view>
namespace mtgb
{
	/// <summary>
	/// �p������ShowImGui���I�[�o���C�h���Ďg��
	/// ������g��Ȃ��Ă�ImGui�\���͂ł���̂ŁA�ʂ̉ӏ��ɕ����������Ƃ��Ȃǂɂǂ���
	/// </summary>
	class ImGuiShowable
	{
		friend class MTImGui;
	public:
		ImGuiShowable(EntityId _entityId = INVALID_ENTITY);
		ImGuiShowable(ShowType _showType, EntityId _entityId = INVALID_ENTITY);
		ImGuiShowable(const std::string& _name,ShowType _showType, EntityId _entityId = INVALID_ENTITY);
		virtual ~ImGuiShowable();

		/// <summary>
		/// �w�肵���\���L���[�ɐς܂��
		/// </summary>
		virtual void ShowImGui();
	protected:
		EntityId entityId_; // Entity�̏ꍇ��Id��n������
		std::string displayName_; // �\�������ۂ̖��O
		ShowType show_; // �\��������ImGuiWindow
	};
	
}