#include "LJ_AtomsOptions.h"

#include <QDoubleSpinBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QSpinBox>

LJ_AtomsOptions::LJ_AtomsOptions(int typeId, QWidget* parent) :
	QWidget(parent), _tId(typeId)
{
	auto layout = new QVBoxLayout(this);
	setLayout(layout);
	auto epsWidget = new QWidget(this);
	{
		auto epsLayout = new QHBoxLayout(epsWidget);
		auto epsLabel = new QLabel("Eps");
		epsLayout->addWidget(epsLabel);
		_epsSpin = new QDoubleSpinBox(this);
		_epsSpin->setRange(0.0, 1'000.0);
		_epsSpin->setDecimals(3);
		_epsSpin->setSingleStep(1.0);
		_epsSpin->setValue(1.0);
		epsLayout->addWidget(_epsSpin);
	}
	layout->addWidget(epsWidget);

	auto sigWidget = new QWidget(this);
	{
		auto sigLayout = new QHBoxLayout(sigWidget);
		auto sigLabel = new QLabel("Sigma");
		sigLayout->addWidget(sigLabel);
		_sigSpin = new QDoubleSpinBox(this);
		_sigSpin->setRange(0.0, 1'000.0);
		_sigSpin->setDecimals(3);
		_sigSpin->setSingleStep(1.0);
		_sigSpin->setValue(1.0);
		sigLayout->addWidget(_sigSpin);
	}
	layout->addWidget(sigWidget);

	auto massWidget = new QWidget(this);
	{
		auto massLayout = new QHBoxLayout(massWidget);
		auto massLabel = new QLabel("Mass");
		massLayout->addWidget(massLabel);
		_massSpin = new QDoubleSpinBox(this);
		_massSpin->setRange(0.0, 1'000.0);
		_massSpin->setDecimals(3);
		_massSpin->setSingleStep(1.0);
		_massSpin->setValue(1.0);
		massLayout->addWidget(_massSpin);
	}
	layout->addWidget(massWidget);

	auto nWidget = new QWidget(this);
	{
		auto nLayout = new QHBoxLayout(nWidget);
		auto nLabel = new QLabel("N");
		nLayout->addWidget(nLabel);
		_nSpin = new QSpinBox(this);
		_nSpin->setRange(0, 1'000'000.0);
		_nSpin->setSingleStep(1);
		_nSpin->setValue(10);
		nLayout->addWidget(_nSpin);
	}
	layout->addWidget(nWidget);
}

LJ_SpeciesParams LJ_AtomsOptions::GetAtomParams() const
{
	return LJ_SpeciesParams({_epsSpin->value(),
		_sigSpin->value(), _massSpin->value(), _nSpin->value()});
}
