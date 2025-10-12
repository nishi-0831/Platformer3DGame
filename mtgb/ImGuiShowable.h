#pragma once

#include <string>

#include "ISystem.h"
#include <any>
#include <unordered_map>
#include <typeindex>
#include "WindowContext.h"
#include "ShowType.h"
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
		ImGuiShowable();
		ImGuiShowable(ShowType _showType);
		ImGuiShowable(const std::string& _name,ShowType _showType);
		virtual ~ImGuiShowable();

		/// <summary>
		/// �w�肵���\���L���[�ɐς܂��
		/// </summary>
		virtual void ShowImGui();
	protected:
		std::string displayName_;//�\�������ۂ̖��O
		ShowType show_;//�\��������ImGuiWindow
	};
	
}