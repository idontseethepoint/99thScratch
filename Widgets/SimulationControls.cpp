#include "SimulationControls.h"
#include "Application.h"
#include "LJ_AtomsOptions.h"
#include "MainWindow.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QScrollArea>
#include <QSlider>
#include <QSpinBox>
#include <QStyle>
#include <QToolButton>

SimulationControls::SimulationControls(std::shared_ptr<LJ_Simulation> sim, 
	QWidget* parent) : QWidget(parent), _sim(sim)
{
	auto layout = new QVBoxLayout(this);

	auto scaleSlider = new QSlider(this);
	scaleSlider->setRange(0, 200);
	scaleSlider->setValue(100);
	scaleSlider->setOrientation(Qt::Horizontal);
	layout->addWidget(scaleSlider);
	QObject::connect(scaleSlider, &QSlider::valueChanged,
		[this](int value) 
		{
			_sim->onAtomScaleChange(static_cast<double>(value) / 100);
		});
	scaleSlider->setToolTip("Atom Scale");

	_cudaCB = new QCheckBox("CUDA", this);
	_cudaCB->setChecked(true);
	layout->addWidget(_cudaCB);

	auto dtWidget = new QWidget(this);
	{
		auto dtLayout = new QHBoxLayout(dtWidget);
		dtWidget->setLayout(dtLayout);
		auto dtLabel = new QLabel("dt=10^", dtWidget);
		dtLayout->addWidget(dtLabel);
		_dtSpin = new QDoubleSpinBox(dtWidget);
		dtLayout->addWidget(_dtSpin);
		_dtSpin->setRange(-10, 2);
		_dtSpin->setValue(-4);
	}
	layout->addWidget(dtWidget);

	auto offDiagWidget = new QWidget(this);
	{
		auto offLayout = new QHBoxLayout(offDiagWidget);
		auto offLabel = new QLabel("Off Diagonal Eps Factor:");
		offLayout->addWidget(offLabel);
		_offDiagSpin = new QDoubleSpinBox(offDiagWidget);
		offLayout->addWidget(_offDiagSpin);
		_offDiagSpin->setRange(0, 100);
		_offDiagSpin->setValue(1.0);
		_offDiagSpin->setSingleStep(0.1);
	}
	layout->addWidget(offDiagWidget);

	auto boxWidget = new QWidget(this);
	{
		auto boxLayout = new QGridLayout(boxWidget);
		boxWidget->setLayout(boxLayout);
		auto lowLabel = new QLabel("Low", boxWidget);
		boxLayout->addWidget(lowLabel, 0, 0);
		_lxSpinner = new QDoubleSpinBox(boxWidget);
		boxLayout->addWidget(_lxSpinner, 0, 1);
		_lySpinner = new QDoubleSpinBox(boxWidget);
		boxLayout->addWidget(_lySpinner, 0, 2);
		_lzSpinner = new QDoubleSpinBox(boxWidget);
		boxLayout->addWidget(_lzSpinner, 0, 3);

		auto highLabel = new QLabel("High", boxWidget);
		boxLayout->addWidget(highLabel, 1, 0);
		_hxSpinner = new QDoubleSpinBox(boxWidget);
		boxLayout->addWidget(_hxSpinner, 1, 1);
		_hySpinner = new QDoubleSpinBox(boxWidget);
		boxLayout->addWidget(_hySpinner, 1, 2);
		_hzSpinner = new QDoubleSpinBox(boxWidget);
		boxLayout->addWidget(_hzSpinner, 1, 3);
		std::vector< QDoubleSpinBox*> lows{
			_lxSpinner, _lySpinner, _lzSpinner
		};
		std::vector< QDoubleSpinBox*> highs{
			_hxSpinner, _hySpinner, _hzSpinner
		};
		for (auto w : lows)
		{
			w->setRange(-1'000, 1'000);
			w->setValue(-5);
		}
		for (auto w : highs)
		{
			w->setRange(-1'000, 1'000);
			w->setValue(5);
		}
	}
	layout->addWidget(boxWidget);

	auto cntSpin = new QSpinBox(this);
	layout->addWidget(cntSpin);
	auto atomsWidget = new QWidget(this);
	_atomsLayout = new QVBoxLayout(atomsWidget);
	auto atomsArea = new QScrollArea(this);
	atomsArea->setWidget(atomsWidget);
	atomsArea->setWidgetResizable(true);
	layout->addWidget(atomsArea);
	QObject::connect(cntSpin, &QSpinBox::valueChanged,
		this, &SimulationControls::onCountChange);
	cntSpin->setValue(2);

	auto hcWidget = new QWidget(this);
	{
		auto hcLayout = new QHBoxLayout(hcWidget);
		auto heatButton = new QToolButton(hcWidget);
		hcLayout->addWidget(heatButton);
		auto coolButton = new QToolButton(hcWidget);
		hcLayout->addWidget(coolButton);
		heatButton->setText("🡅");
		coolButton->setText("🡇");
		heatButton->setStyleSheet("color: #f00");
		coolButton->setStyleSheet("color: #00f");
		QObject::connect(heatButton, &QToolButton::pressed,
			_sim.get(), &LJ_Simulation::onHeatPress);
		QObject::connect(coolButton, &QToolButton::pressed,
			_sim.get(), &LJ_Simulation::onCoolPress);
	}
	layout->addWidget(hcWidget);

	_startPauseButton = new QToolButton(this);
	_startPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
	layout->addWidget(_startPauseButton);
	QObject::connect(_startPauseButton, &QToolButton::pressed,
		this, &SimulationControls::onStartPausePress);
}

LJ_Parameters SimulationControls::GetParameters()
{
	std::vector<LJ_SpeciesParams> atOpts;
	for (auto w : _atomControls)
	{
		atOpts.push_back(w->GetAtomParams());
	}
	return LJ_Parameters({ 
		std::pow(10, _dtSpin->value()),
		_offDiagSpin->value(),
		{
			{_lxSpinner->value(),_lySpinner->value(), _lxSpinner->value()},
			{_hxSpinner->value(),_hySpinner->value(), _hxSpinner->value()}
		}, 
		atOpts,
		_cudaCB->isChecked()});
}

void SimulationControls::onStartPausePress()
{
	if (_playing)
	{
		_playing = false;
		emit Application::App()->StopSimulation();
		_startPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
	}
	else
	{
		_playing = true;
		emit Application::App()->StartSimulation(GetParameters());
		_startPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
	}
}

void SimulationControls::onCountChange(int newCount)
{
	for (int i = _atomControls.size(); i < newCount; ++i)
	{
		auto f = new QFrame(this);
		f->setFrameStyle(QFrame::Box | QFrame::Plain);
		auto fLayout = new QVBoxLayout(f);
		auto w = new LJ_AtomsOptions(i, f);
		fLayout->addWidget(w);
		_atomControls.push_back(w);
		_atomsLayout->addWidget(f);
	}

	for (int i = newCount; i < _atomControls.size(); ++i)
	{
		auto w = _atomControls.back();
		_atomControls.pop_back();
		delete w->parent();
	}
}
