#include "SceneNode.h"
#include "ConeMesh.h"
#include "MeshRenderer.h"
#include "SimulationSceneNode.h"

SceneNode::Ptr SceneNode::_root(new SceneNode("Root", SceneNode::WkPtr()));

SceneNode::Ptr SceneNode::Root()
{
	return _root;
}

SceneNode::SceneNode(QString const& name, WkPtr parent) :
	_name(name), _parent(parent)
{}

void SceneNode::onFrame()
{
	if (_renderer)
		_renderer->Render(_trans);
	for (auto c : _children)
	{
		c->onFrame();
	}
}

void SceneNode::setRenderer(GeometryRendererAbstract::Ptr renderer)
{
	_renderer = renderer;
}

SceneNode::Ptr SceneNode::addNode(QString const& name)
{
	auto nn = std::make_shared<SceneNode>(name, shared_from_this());
	_children.push_back(nn);
	return nn;
}

SceneNode::Ptr SceneNode::addSimulationNode(std::shared_ptr<LJ_Simulation> sim, QString const& name)
{
	auto nn = std::make_shared<SimulationSceneNode>(sim, name, shared_from_this());
	_children.push_back(nn);
	return nn;
}

SceneNode::Ptr SceneNode::addBoxNode(vec3Dd const& low, vec3Dd high, QString const& name)
{
	auto n = addNode(name);
	auto cr = MeshRenderer::Cuboid(low, high);
	n->setRenderer(cr);
	return n;
}

SceneNode::Ptr SceneNode::addConeNode(vec3D const& baseCenter, vec3D axis,
	float r, QString const& name)
{
	auto n = addNode(name);
	auto cr = MeshRenderer::Cone(baseCenter, axis, r);
	n->setRenderer(cr);
	return n;
}

void SceneNode::setTransform(QMatrix4x4 const& trans)
{
	_baseTrans = trans;
	if (auto p = _parent.lock())
	{
		_trans = p->_trans * trans;
	}
	else
	{
		_trans = trans;
	}
	for (auto c : _children)
	{
		c->updateTrans(_trans);
	}
}

void SceneNode::Remove()
{
	if (auto p = _parent.lock())
		p->RemoveChild(this);
}

void SceneNode::RemoveChild(SceneNode* child)
{
	_children.erase(std::remove_if(_children.begin(), _children.end(),
		[child](SceneNode::Ptr c) {
			return c.get() == child;
		}), _children.end());
}

SceneNode::Ptr SceneNode::childAt(int idx)
{
	if (idx < 0 || idx >= _children.size())
		return Ptr();
	return _children[idx];
}

int SceneNode::indexOf(SceneNode* child)
{
	auto cLoc = std::find_if(_children.begin(), _children.end(),
		[child](Ptr n)
		{return n.get() == child;});
	return cLoc == _children.end() ? -1 : cLoc - _children.begin();
}

int SceneNode::indexInParent()
{
	if (auto p = _parent.lock())
		return p->indexOf(this);
	return -1;
}

void SceneNode::updateTrans(QMatrix4x4 const& newParentTrans)
{
	_trans = newParentTrans * _baseTrans;
	for (auto c : _children)
	{
		c->updateTrans(_trans);
	}
}
