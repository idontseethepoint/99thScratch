#include "MainWindow.h"
#include "CollapsibleSection.h"
#include "LJ_Simulation.h"
#include "SceneGraphModel.h"
#include "SceneTree.h"
#include "SceneWidget.h"
#include "SimulationControls.h"
#include "SimulationDisplay.h"
#include "VecDisplay.h"

#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QSplitter>
#include <QTimer>
#include <QToolButton>

int cnt = 0;

MainWindow* MainWindow::_instance = nullptr;

MainWindow* MainWindow::Instance()
{
	if (!_instance)
		_instance = new MainWindow();
	return _instance;
}

MainWindow::MainWindow() : _leftExpanded(true)
{
	auto mainWidget = new QWidget(this);
	setCentralWidget(mainWidget);
	auto layout = new QVBoxLayout(mainWidget);
	mainWidget->setLayout(layout);

	//auto gw = new GLWindow();
	//auto holder = QWidget::createWindowContainer(gw);
	//holder->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	//layout->addWidget(holder);

	auto sim = std::make_shared<LJ_Simulation>();

	_splitter = new QSplitter(mainWidget);
	layout->addWidget(_splitter);
	{
		auto controlPane = new QWidget(_splitter);
		_splitter->addWidget(controlPane);
		{
			auto controlPaneLayout = new QVBoxLayout(_splitter);
			controlPane->setLayout(controlPaneLayout);
			controlPaneLayout->setAlignment(Qt::AlignRight);
			_expButton = new QToolButton(controlPane);
			QObject::connect(_expButton, &QToolButton::pressed, this,
				&MainWindow::toggleLeftExpanded);
			controlPaneLayout->addWidget(_expButton);
			_sceneTree = new SceneTree(_splitter);
			auto sc = CollapsibleSection::Wrap("Scene Tree",
				_sceneTree, _splitter);
			controlPaneLayout->addWidget(sc);

			auto simControls = new SimulationControls(sim, _splitter);
			auto simControlsSecion = CollapsibleSection::Wrap(
				"Sim Controls", simControls, _splitter);
			controlPaneLayout->addWidget(simControlsSecion);

			controlPaneLayout->addStretch();

			_leftWidgets.push_back(sc);
			_leftWidgets.push_back(simControlsSecion);
		}
		_sw = new SceneWidget(_splitter);
		_sw->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		_splitter->addWidget(_sw);
		auto m = new SceneGraphModel(SceneNode::Root(), _sceneTree);
		_sceneTree->setModel(m);
	}
	_splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	auto bottom = new QWidget(mainWidget);
	layout->addWidget(bottom);
	{
		auto bottomLayout = new QHBoxLayout(bottom);
		bottom->setLayout(bottomLayout);
		auto sd = new SimulationDisplay(sim, bottom);
		bottomLayout->addWidget(sd);
	}

	SceneNode::Root()->addSimulationNode(sim, "LJ_Sim");
}

void MainWindow::resizeEvent(QResizeEvent* e)
{
	setLeftPane();
}

void MainWindow::onAtomScaleChanged(float newScale)
{
}

void MainWindow::onViewUpdate()
{
	//_eyeLabel->updateVector(SceneWidget::GetCamPos());
	//_upLabel->updateVector(SceneWidget::GetUp());
}

void MainWindow::toggleLeftExpanded()
{
	_leftExpanded = !_leftExpanded;
	setLeftPane();
}

void MainWindow::setLeftPane()
{
	static constexpr int COLLAPSED_SIZE = 20;
	static constexpr int EXPANDED_SIZE = 180;
	if (_leftExpanded)
	{
		for (auto w : _leftWidgets)
		{
			w->show();
		}
		_splitter->setSizes({ EXPANDED_SIZE, _splitter->width() - EXPANDED_SIZE });
		_expButton->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
	}
	else
	{
		for (auto w : _leftWidgets)
		{
			w->hide();
		}
		_splitter->setSizes({ COLLAPSED_SIZE, _splitter->width() - COLLAPSED_SIZE });
		_expButton->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
	}
}

void MainWindow::onBtnPress()
{
}
