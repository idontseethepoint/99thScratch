#pragma once

#include "GeometryRendererAbstract.h"
#include "vec3D.h"

#include <QAbstractItemModel>
#include <QMatrix4x4>
#include <QtGui/qfilesystemmodel.h>

#include <memory>
#include <vector>

class LJ_Simulation;

class SceneNode : public std::enable_shared_from_this<SceneNode>
{
public:
	using Ptr = std::shared_ptr<SceneNode>;
	using WkPtr = std::weak_ptr<SceneNode>;
	static SceneNode::Ptr Root();
	SceneNode(QString const& name, SceneNode::WkPtr parent);
	inline QString const& Name() const { return _name; }
	virtual void onFrame();
	void setRenderer(GeometryRendererAbstract::Ptr renderer);
	SceneNode::Ptr addNode(QString const& name);
	SceneNode::Ptr addSimulationNode(std::shared_ptr<LJ_Simulation> sim,
		QString const& name);
	SceneNode::Ptr addBoxNode(vec3Dd const& low, vec3Dd high, 
		QString const& name = "Box");
	SceneNode::Ptr addConeNode(vec3D const& baseCenter, vec3D axis,
		float r, QString const& name = "Cone");
	void setTransform(QMatrix4x4 const& trans);
	void Remove();
	void RemoveChild(SceneNode* child);
	void Clear();
	inline size_t nChild() const { return _children.size(); }
	SceneNode::Ptr childAt(int idx);
	inline WkPtr const getParent() const { return _parent; }
	int indexOf(SceneNode* child);
	int indexInParent();
	QMatrix4x4 const& Transform() const { return _baseTrans; }
protected:
	void updateTrans(QMatrix4x4 const& newParentTrans);
	WkPtr _parent;
	GeometryRendererAbstract::Ptr _renderer;
	QMatrix4x4 _baseTrans;
	QMatrix4x4 _trans;
	QString _name;
	std::vector<Ptr> _children;
	static SceneNode::Ptr _root;
};
