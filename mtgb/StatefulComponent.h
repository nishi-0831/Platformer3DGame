#pragma once
#include "IComponent.h"
#include "ComponentConcept.h"

#include "nlohmann/json.hpp"
#include "JsonConverter.h"
#define UseConcept 0
namespace mtgb
{
	template<typename TDerived,typename TComponentPool,typename TData,typename TMemento>
	
	class StatefulComponent : public IComponent<TComponentPool, TDerived>
	{
	public:	
		using IComponent<TComponentPool, TDerived>::IComponent;
		
		virtual ~StatefulComponent() {};
		virtual void Initialize() override {};

		TMemento* SaveToMemento() const
		{
			// concept�Ō��؍ς݂Ȃ̂�static_cast
			return new TMemento(this->GetEntityId(), static_cast<const TDerived&>(*this));
		}

		void RestoreFromMemento(const TMemento& _memento)
		{
			// concept�Ō��؍ς݂Ȃ̂�static_cast
			auto& derivedRef = static_cast<TDerived&>(*this);
			const TData& data = _memento.GetData();

			static_cast<TData&>(derivedRef) = data;
			OnPostRestore();
		}

		nlohmann::json Serialize() override;
		

	protected:
		/// <summary>
		/// <para> Memento���畜�����s��ꂽ�ۂɌĂ΂��t�b�N </para>
		/// </summary>
		virtual void OnPostRestore()
		{
			// �f�t�H���g�ł͉������Ȃ�
		}
	private:
	};
	
	
	
	template<typename TDerived, typename TComponentPool, typename TData, typename TMemento>
	nlohmann::json StatefulComponent<TDerived, TComponentPool, TData, TMemento>::Serialize()
	{
		auto& derivedRef = static_cast<TDerived&>(*this);
		TData& data = static_cast<TData&>(derivedRef);
		//return nlohmann::json{};
		return JsonConverter::Serialize<TData>(data);
	}

	

}

//#include "StatefulComponent.inl"