#pragma once

#include "SceneNode.h"

#include <QAbstractItemModel>

class SceneGraphModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	SceneGraphModel(SceneNode::Ptr root, QObject* parent);
	SceneNode::Ptr nodeFromIndex(const QModelIndex& index) const;
	virtual QModelIndex index(int row, int column, 
		QModelIndex const& parent = QModelIndex()) const override;
	virtual QModelIndex parent(const QModelIndex& child) const override;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;
	void nodeAboutToBeAdded(SceneNode* parentNode, int position);
	void nodeAdded();
	void nodeAboutToBeRemoved(SceneNode* parentNode, int position);
	void nodeRemoved();
private:
	QModelIndex indexForNode(SceneNode::Ptr node) const;
	SceneNode::Ptr _root;
};