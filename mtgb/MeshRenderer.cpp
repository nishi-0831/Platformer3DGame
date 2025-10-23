#include "MeshRenderer.h"
#include "MeshRendererMemento.h"
mtgb::MeshRenderer::MeshRenderer()
{
	
}

void mtgb::MeshRenderer::RestoreFromMemento(const MeshRendererMemento& _memento)
{
	this->meshHandle_ = _memento.hMesh_;
	this->layer_ = _memento.layer_;
	this->shaderType_ = _memento.shaderType_;
}
