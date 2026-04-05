#include "SceneGraphModel.h"

SceneGraphModel::SceneGraphModel(SceneNode::Ptr root, QObject* parent) : 
	QAbstractItemModel(parent), _root(root)
{}

SceneNode::Ptr SceneGraphModel::nodeFromIndex(const QModelIndex & index) const
{
	if (!index.isValid())
		return SceneNode::Ptr();
	auto p = static_cast<SceneNode*>(index.internalPointer());
	if (!p)
		return SceneNode::Ptr();
	return p->shared_from_this();
}

QModelIndex SceneGraphModel::index(int row, int column, QModelIndex const& parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	auto parentNode = parent.isValid() ? nodeFromIndex(parent) : _root;
	
	if (!parentNode)
		return QModelIndex();

	auto child = parentNode->childAt(row);
	if (child)
	{
		return createIndex(row, column, child.get());
	}
	else
	{
		return QModelIndex();
	}
}

QModelIndex SceneGraphModel::parent(const QModelIndex& child) const
{
	if (!child.isValid())
		return QModelIndex();

	auto childNode = nodeFromIndex(child);
	if (!childNode)
		return QModelIndex();
	auto parentNode = childNode->getParent();
	if (auto p = parentNode.lock())
	{
		return createIndex(p->indexInParent(), 0, p.get());
	}
	else
	{
		return QModelIndex();
	}
}

int SceneGraphModel::rowCount(const QModelIndex& parent) const
{
	auto node = parent.isValid() ? nodeFromIndex(parent) : _root;
	return node ? node->nChild() : 0;
}

int SceneGraphModel::columnCount(const QModelIndex& parent) const
{
	return 1;
}

QVariant SceneGraphModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return {};

	auto node = nodeFromIndex(index);
	if (!node)
		return {};

	switch (role)
	{
	case Qt::DisplayRole:
	case Qt::EditRole:
		return node->Name();
	default:
		return {};
	}
}

QVariant SceneGraphModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section == 0)
		return tr("Scene Graph");
	return {};
}

Qt::ItemFlags SceneGraphModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	// Add Qt::ItemIsEditable for inline rename
	// Add Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled for reparenting
}

void SceneGraphModel::nodeAboutToBeAdded(SceneNode* parentNode, int position)
{
}

void SceneGraphModel::nodeAdded()
{
}

void SceneGraphModel::nodeAboutToBeRemoved(SceneNode* parentNode, int position)
{
}

void SceneGraphModel::nodeRemoved()
{
}

QModelIndex SceneGraphModel::indexForNode(SceneNode::Ptr node) const
{
	if (!node || node == _root)
		return {};
	return createIndex(node->indexInParent(), 0, node.get());
}
