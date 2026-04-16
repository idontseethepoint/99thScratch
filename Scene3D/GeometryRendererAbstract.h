#pragma once

#include <QColor>
#include <QMatrix4x4>

#include <memory>

class GeometryRendererAbstract
{
public:
	using Ptr = std::shared_ptr<GeometryRendererAbstract>;
	virtual void Render(QMatrix4x4 const& trans) = 0;
	virtual void SetColor(QColor const& color)=0;
};