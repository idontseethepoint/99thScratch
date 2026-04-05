#pragma once

#include "CollapsibleSection.h"

#include <QWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include <QFrame>

CollapsibleSection::CollapsibleSection(const QString& title, 
	QWidget* parent) : QWidget(parent)
{
	toggleButton_ = new QToolButton(this);
	toggleButton_->setText(title);
	toggleButton_->setCheckable(true);
	toggleButton_->setChecked(false);
	toggleButton_->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	toggleButton_->setArrowType(Qt::RightArrow);
	toggleButton_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	headerLine_ = new QFrame(this);
	headerLine_->setFrameShape(QFrame::HLine);
	headerLine_->setFrameShadow(QFrame::Sunken);

	contentWidget_ = new QWidget(this);
	contentWidget_->setVisible(false);

	contentLayout_ = new QVBoxLayout(contentWidget_);
	contentLayout_->setContentsMargins(0, 0, 0, 0);
	contentLayout_->setSpacing(6);

	auto* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(4);

	auto* headerLayout = new QHBoxLayout();
	headerLayout->setContentsMargins(0, 0, 0, 0);
	headerLayout->addWidget(toggleButton_);
	headerLayout->addWidget(headerLine_);

	mainLayout->addLayout(headerLayout);
	mainLayout->addWidget(contentWidget_);

	connect(toggleButton_, &QToolButton::toggled, this, [this](bool checked) {
		toggleButton_->setArrowType(checked ? Qt::DownArrow : Qt::RightArrow);
		contentWidget_->setVisible(checked);
		});
}

void CollapsibleSection::setContentLayout(QLayout* layout)
{
	delete contentLayout_;
	contentLayout_ = nullptr;

	contentWidget_->setLayout(layout);
}

QVBoxLayout* CollapsibleSection::contentLayout() const
{
	return contentLayout_;
}

CollapsibleSection* CollapsibleSection::Wrap(QString const& title, QWidget* content, QWidget* parent)
{
	auto ret = new CollapsibleSection(title, parent);
	ret->contentLayout()->addWidget(content);
	return ret;
}

CollapsibleSection* CollapsibleSection::Wrap(QString const& title, std::vector<QWidget*> contents, QWidget* parent)
{
	auto ret = new CollapsibleSection(title, parent);
	for (auto c : contents)
	{
		ret->contentLayout()->addWidget(c);
	}
	return ret;
}

