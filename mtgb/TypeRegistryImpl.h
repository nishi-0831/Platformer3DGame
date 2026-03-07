#pragma once
#include "TypeRegistry.h"
#include <refl-cpp/refl.hpp>
#include <iostream>
#include "ImGui/imgui.h"
#include "WindowContext.h"
#include "MTAssert.h"
#include "MTStringUtility.h"
#include "DefaultShow.h"
#include "ShowAttributes.h"

// TypeRegistryのテンプレート実装
template <typename T> void PropertyDisplayRegistry::RegisterType()
{
	using Type					 = std::remove_pointer_t<std::remove_cvref_t<T>>;
	showFunctions_[typeid(Type)] = [this](std::any _ptr, const char* _name) -> Command*
	{
		if constexpr (refl::is_reflectable<Type>())
		{
			Type* registerInstance = nullptr;
			if (_ptr.type() == typeid(T*))
			{
				registerInstance = std::any_cast<T*>(_ptr);
			}
			else if (_ptr.type() == typeid(const T*))
			{
				registerInstance = const_cast<T*>(std::any_cast<const T*>(_ptr));
			}
			massert(
				registerInstance != nullptr &&
				"instanceのany_castに失敗:ptrがnullptrです @PropertyDisplayRegistry::RegisterType"
			);
			constexpr auto type = refl::reflect<Type>();

			bool showFuncExecuted = false;
			Command* result		  = nullptr;

			// type.attributesの各属性をラムダ式の引数に渡して一つずつ処理
			std::apply(
				[&](auto&&... _attrs)
				{
					((
						 [&]
						 {
							 // 属性のインスタンスの型を取得
							 // 本来の型特性を知りたいのでdecay_tで純粋な値型に変換
							 using AttrType = std::decay_t<decltype(_attrs)>;
							 // ShowFunc型のインスタンスか否か
							 if constexpr (refl::trait::is_instance_of_v<ShowFunc, AttrType>)
							 {
								 result			  = _attrs(registerInstance, _name);
								 showFuncExecuted = true;
							 }
						 }()
					 ),
					 ...);
				},
				type.attributes
			);

			if (!showFuncExecuted)
			{
				ImGui::PushID(registerInstance);
				// メンバごとに走査
				refl::util::for_each(
					type.members,
					[&](auto&& _member) -> Command*
					{
						// メンバの実際の型を取得（ポインタかどうかを含む）
						using MemberValueType = std::remove_cvref_t<decltype(_member(*registerInstance))>;

						if constexpr (std::is_pointer_v<MemberValueType>)
						{
							// ポインタ型の場合：そのまま渡す
							auto memberValue = _member(*registerInstance);

							// メンバの型がリフレクションされているかチェック
							if (!this->ShowMemberWithReflection(memberValue, _member.name.c_str()))
							{
								// 属性をチェックして適切な表示方法を選択
								bool hasCustomAttribute = false;
								// メンバーの属性を取得
								auto memberAttributes = refl::descriptor::get_attributes(_member);
								hasCustomAttribute =
									this->CheckCustomAttrs(memberAttributes, memberValue, _member.name.c_str());

								// カスタム属性がない場合はデフォルト表示
								if (!hasCustomAttribute)
								{
									result = mtgb::DefaultShow(memberValue, _member.name.c_str());
								}
							}
						}
						else
						{
							// 値型の場合：アドレスを取得して渡す
							auto memberPtr = &(_member(*registerInstance));

							// メンバの型がリフレクションされているかチェック
							if (this->ShowMemberWithReflection(memberPtr, _member.name.c_str(), result) == false)
							{
								// 属性をチェックして適切な表示方法を選択
								bool hasCustomAttribute = false;
								// メンバーの属性を取得
								auto memberAttributes = refl::descriptor::get_attributes(_member);

								result = this->CheckCustomAttrs(memberAttributes, memberPtr, _member.name.c_str());
								// カスタム属性がない場合はデフォルト表示
								if (result == nullptr)
								{
									result = mtgb::DefaultShow(memberPtr, _member.name.c_str());
								}
							}
						}
						return result;
					}
				);
				ImGui::PopID();
				return result;
			}
			return result;
		}
		else
		{
			// リフレクションされていない
			ImGui::Text("%s,NotReflectable", _name);
		}
		return nullptr;
	};
}

// メンバの型がリフレクションされているかチェックし、ShowFunc属性があればそれを使用
template <typename T>
bool PropertyDisplayRegistry::ShowMemberWithReflection(T _memberValue, const char* _name, Command* _command)
{
	using MemberType = std::remove_pointer_t<std::remove_cvref_t<T>>;
	if constexpr (refl::is_reflectable<MemberType>())
	{
		constexpr auto memberType = refl::reflect<MemberType>();

		// メンバの型にShowFunc属性があるかチェック
		bool showFuncExecuted = false;

		std::apply(
			[&](auto&&... _attrs)
			{
				((
					 [&]
					 {
						 using AttrType = std::decay_t<decltype(_attrs)>;
						 if constexpr (refl::trait::is_instance_of_v<ShowFunc, AttrType>)
						 {
							 // ポインタの場合は値を渡し、値型の場合はそのまま渡す
							 if constexpr (std::is_pointer_v<T>)
							 {
								 _command = _attrs(_memberValue, _name);
							 }
							 else
							 {
								 _command = _attrs(&_memberValue, _name);
							 }
							 showFuncExecuted = true;
						 }
					 }()
				 ),
				 ...);
			},
			memberType.attributes
		);

		if (showFuncExecuted)
		{
			return true;
		}

		// ShowFunc属性がない場合は、TypeRegistryに登録された表示関数を使用
		std::type_index memberTypeIdx(typeid(MemberType));

		// TypeRegistryに登録されているかチェック
		if (this->IsRegisteredType(memberTypeIdx))
		{
			if constexpr (std::is_pointer_v<T>)
			{
				this->ShowProperty(memberTypeIdx, std::any(_memberValue), _name);
			}
			else
			{
				this->ShowProperty(memberTypeIdx, std::any(&_memberValue), _name);
			}
			return true;
		}
	}

	return false; // リフレクションされていない、または表示関数が見つからない
}

template <typename... Args, typename T>
Command* PropertyDisplayRegistry::CheckCustomAttrs(std::tuple<Args...>& _attrs, T _valPtr, const char* _name)
{
	Command* command = nullptr;
	std::apply(
		[&](auto&&... _attr)
		{
			((
				 [&]
				 {
					 using AttrType = std::decay_t<decltype(_attr)>;
					 if constexpr (std::is_base_of_v<refl::attr::usage::member, AttrType>)
					 {
						 command = _attr(_valPtr, _name);
					 }
				 }()
			 ),
			 ...);
		},
		_attrs
	);
	return command;
}
