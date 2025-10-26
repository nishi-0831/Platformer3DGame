#pragma once
#include "IComponent.h"
#include "ComponentConcept.h"

#define UseConcept 0
namespace mtgb
{
	template<typename TDerived,typename TComponentPool,typename TData,typename TMemento>
	requires 
		// TDerivedはTDataを継承している
#if UseConcept
	DerivedFromDataT<TDerived,TData> &&
#endif 

		// TMementoはIComponentMementoを継承している
		ComponentWithMementoT<TDerived,TMemento> 
	class StatefulComponent : public IComponent<TComponentPool, TDerived>
	{
	public:
		using IComponent<TComponentPool, TDerived>::IComponent;
		
		
		TMemento* SaveToMemento() const
		{
			// conceptで検証済みなのでstatic_cast
			return new TMemento(this->GetEntityId(), static_cast<const TDerived&>(*this));
		}

		void RestoreFromMemento(const TMemento& _memento)
		{
			// conceptで検証済みなのでstatic_cast
			auto& derivedRef = static_cast<TDerived&>(*this);
			const TData& data = _memento.GetData();

			static_cast<TData&>(derivedRef) = data;
			OnPostRestore();
		}


	protected:
		/// <summary>
		/// <para> Mementoから復元が行われた際に呼ばれるフック </para>
		/// </summary>
		virtual void OnPostRestore()
		{
			// デフォルトでは何もしない
		}
	private:
	};
}
