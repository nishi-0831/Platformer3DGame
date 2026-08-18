#include "PropertyChangedEvent.h"
#include "Game.h"
mtgb::PropertyChangeCommand::PropertyChangeCommand(IComponentMemento* _pPrevMemento, IComponentMemento* _pMemento)
	: pPrevMemento_ { _pPrevMemento }
	, pMemento_ { _pMemento }
{
}

void mtgb::PropertyChangeCommand::Execute()
{
	Game::GetComponentFactory().AddComponentFromMemento(*pMemento_);
}

void mtgb::PropertyChangeCommand::Undo()
{
	Game::GetComponentFactory().AddComponentFromMemento(*pPrevMemento_);
}

std::string mtgb::PropertyChangeCommand::Name() const
{
	return "PropertyChanged";
}