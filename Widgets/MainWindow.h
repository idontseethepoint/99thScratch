#pragma once

#include "LJ_Simulation.h"

#include <QMainWindow>

#include <vector>

class SceneTree;
class SceneWidget;
class VecDisplay;

class QLabel;
class QSplitter;
class QToolButton;

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	static MainWindow* Instance();
protected:
	MainWindow();
	void resizeEvent(QResizeEvent* e) override;
public slots:
	void onAtomScaleChanged(float newScale);
private slots:
	void onBtnPress();
	void onViewUpdate();
	void toggleLeftExpanded();
private:
	static MainWindow* _instance;
	void setLeftPane();
	bool _leftExpanded;
	QToolButton* _expButton;
	SceneTree* _sceneTree;
	QSplitter* _splitter;
	SceneWidget* _sw;
	std::vector<QWidget*> _leftWidgets;
};