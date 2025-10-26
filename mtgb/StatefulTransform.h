#pragma once
#include "ComponentConcept.h"
#include "StatefulComponent.h"
#include "TransformCP.h"
#include "TransformData.h"
#include "IComponentMemento.h"
#include "TransformMemento.h"
namespace mtgb
{
	//using TransformMemento = ComponentMemento<StatefulTransform, TransformData>;
	
	class StatefulTransform : public TransformData,public StatefulComponent<StatefulTransform, TransformCP, TransformData, ComponentMemento<StatefulTransform, TransformData>>
	{
	public:
		StatefulTransform()
		{
			
		} 

		/*void Compute(){...}*/
	};

	using TransformMemento = ComponentMemento<StatefulTransform, TransformData>;
}