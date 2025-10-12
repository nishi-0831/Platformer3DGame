#pragma once
#include "TypeRegistry.h"
#include <refl.hpp>
#include <iostream>
#include "../ImGui/imgui.h"
#include "WindowContext.h"
#include "MTAssert.h"
#include "MTStringUtility.h"
#include "DefaultShow.h"
#include "ShowAttributes.h"





// TypeRegistry�̃e���v���[�g����
template<typename T>
void TypeRegistry::RegisterType()
{
	
	using Type = std::remove_pointer_t<std::remove_cvref_t<T>>;
	showFunctions_[typeid(Type)] = [this](std::any ptr, const char* name)
		{
			
			if constexpr (refl::is_reflectable<Type>())
			{
				Type* registerInstance = nullptr;
				if (ptr.type() == typeid(T*))
				{
					registerInstance = std::any_cast<T*>(ptr);
				}
				else if (ptr.type() == typeid(const T*))
				{
					registerInstance = const_cast<T*>(std::any_cast<const T*>(ptr));
				}
				massert(registerInstance != nullptr
					&& "instance��any_cast�Ɏ��s:ptr��nullptr�ł� @TypeRegistry::RegisterType");
				constexpr auto type = refl::reflect<Type>();

				// Check if ShowFunc is present and execute it
				bool showFuncExecuted = false;
				//type.attributes�̊e�����������_���̈����ɓn���Ĉ������
				std::apply([&](auto&&... attrs)
					{
						(
							(
								[&]
								{
									//�����̃C���X�^���X�̌^���擾
									//�{���̌^������m�肽���̂�decay_t�ŏ����Ȓl�^�ɕϊ�
									using AttrType = std::decay_t<decltype(attrs)>;
									//ShowFunc�^�̃C���X�^���X���ۂ�
									if constexpr (refl::trait::is_instance_of_v<ShowFunc, AttrType>)
									{
										attrs(registerInstance, name);
										showFuncExecuted = true;
									}
								}()
									),
							...);
					}, type.attributes);

				if (!showFuncExecuted)
				{
					
						ImGui::PushID(registerInstance);
						//�����o���Ƃɑ���
						refl::util::for_each(type.members, [&](auto&& member)
							{
								// �����o�̎��ۂ̌^���擾�i�|�C���^���ǂ������܂ށj
								using MemberValueType = std::remove_cvref_t<decltype(member(*registerInstance))>;

								if constexpr (std::is_pointer_v<MemberValueType>)
								{
									// �|�C���^�^�̏ꍇ�F���̂܂ܓn��
									auto memberValue = member(*registerInstance);

									// �����o�̌^�����t���N�V��������Ă��邩�`�F�b�N
									if (!this->ShowMemberWithReflection(memberValue, member.name.c_str()))
									{
										// �������`�F�b�N���ēK�؂ȕ\�����@��I��
										bool hasCustomAttribute = false;

										// �����o�[�̑������擾
										auto memberAttributes = refl::descriptor::get_attributes(member);



										hasCustomAttribute = this->CheckCustomAttrs(memberAttributes, memberValue, member.name.c_str());

										// �J�X�^���������Ȃ��ꍇ�̓f�t�H���g�\��
										if (!hasCustomAttribute) {
											mtgb::DefaultShow(memberValue, member.name.c_str());
										}
									}
								}
								else
								{
									// �l�^�̏ꍇ�F�A�h���X���擾���ēn��
									auto memberPtr = &(member(*registerInstance));

									// �����o�̌^�����t���N�V��������Ă��邩�`�F�b�N
									if (!this->ShowMemberWithReflection(memberPtr, member.name.c_str()))
									{
										// �������`�F�b�N���ēK�؂ȕ\�����@��I��
										bool hasCustomAttribute = false;

										// �����o�[�̑������擾
										auto memberAttributes = refl::descriptor::get_attributes(member);

										hasCustomAttribute = this->CheckCustomAttrs(memberAttributes, memberPtr, member.name.c_str());

										// �J�X�^���������Ȃ��ꍇ�̓f�t�H���g�\��
										if (!hasCustomAttribute) {
											mtgb::DefaultShow(memberPtr, member.name.c_str());
										}
									}
								}
							});

						ImGui::PopID();
					
				}
			}
			else
			{
				//���t���N�V��������Ă��Ȃ�
				ImGui::Text("%s,NotReflectable", name);
			}
		};
}

// �����o�̌^�����t���N�V��������Ă��邩�`�F�b�N���AShowFunc����������΂�����g�p
template<typename T>
bool TypeRegistry::ShowMemberWithReflection(T memberValue, const char* name)
{
	using MemberType = std::remove_pointer_t<std::remove_cvref_t<T>>;
	
	if constexpr (refl::is_reflectable<MemberType>())
	{
		constexpr auto memberType = refl::reflect<MemberType>();
		
		// �����o�̌^��ShowFunc���������邩�`�F�b�N
		bool showFuncExecuted = false;
		std::apply([&](auto&&... attrs)
			{
				(
					(
						[&]
						{
							using AttrType = std::decay_t<decltype(attrs)>;
							if constexpr (refl::trait::is_instance_of_v<ShowFunc, AttrType>)
							{
								// �|�C���^�̏ꍇ�͒l��n���A�l�^�̏ꍇ�͂��̂܂ܓn��
								if constexpr (std::is_pointer_v<T>)
								{
									attrs(memberValue, name);
								}
								else
								{
									attrs(&memberValue, name);
								}
								showFuncExecuted = true;
							}
						}()
							),
					...);
			}, memberType.attributes);
		
		if (showFuncExecuted)
		{
			return true;
		}
		
		// ShowFunc�������Ȃ��ꍇ�́ATypeRegistry�ɓo�^���ꂽ�\���֐����g�p
		std::type_index memberTypeIdx(typeid(MemberType));
		
		//TypeRegistry�ɓo�^����Ă��邩�`�F�b�N
		if (this->IsRegisteredType(memberTypeIdx))
		{
			if constexpr (std::is_pointer_v<T>)
			{
				this->CallFunc(memberTypeIdx, std::any(memberValue), name);
			}
			else
			{
				this->CallFunc(memberTypeIdx, std::any(&memberValue), name);
			}
			return true;
		}
	}
	
	return false; // ���t���N�V��������Ă��Ȃ��A�܂��͕\���֐���������Ȃ�
}

template<typename ...Args, typename T>
bool TypeRegistry::CheckCustomAttrs(std::tuple<Args...>& attrs, T valPtr, const char* name)
{
	bool ret = false;
	std::apply([&](auto&&... attr)
		{
			(
				(
					[&] {
						using AttrType = std::decay_t<decltype(attr)>;
						if constexpr (std::is_base_of_v<refl::attr::usage::member, AttrType>)
						{
							attr(valPtr, name);
							ret = true;
						}
					}()
				), 
			...);
		}, attrs);
	return ret;
}

template<typename T>
void TypeRegistry::CheckProxyAttrs()
{
	using ProxyType = std::remove_pointer_t<std::remove_cvref_t<T>>;

	if constexpr (refl::is_reflectable<T>())
	{
		constexpr auto typeDesc = refl::reflect<ProxyType>();
		std::apply([&](auto&&... attr)
			{
				(
					(
						[&] {
							using AttrType = std::decay_t<decltype(attr)>;
							if constexpr (refl::trait::is_instance_of_v<ProxyFor, AttrType>)
							{
								/*RegisterFunc<typename AttrType::TargetType>([this](std::any instance, const char* name) {
									using Proxy = T;
									using Target = typename AttrType::TargetType;
									Target* targetPtr = std::any_cast<Target*>(instance);
									Proxy proxy(targetPtr);
									showFunctions_[typeid(Proxy)](std::any(&proxy), name);
									});*/

							}
						}()
							),
					...);
			}, typeDesc.attributes);
	}
}



