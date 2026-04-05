#pragma once

#include <QVector3D>

#include <array>

template <typename Scalar>
struct vec3Dt
{
	vec3Dt(Scalar x, Scalar y, Scalar z);
	vec3Dt(std::array<Scalar, 3> const& arr);
	vec3Dt(vec3Dt<double> const& vD);
	QVector3D Qvec() const;
	Scalar DistanceSquaredTo(vec3Dt<Scalar> const& other);
	Scalar DistanceTo(vec3Dt<Scalar> const& other);
	static vec3Dt<Scalar> null();
	vec3Dt<Scalar> operator+(vec3Dt<Scalar> const& other) const;
	void operator+=(vec3Dt<Scalar> const& other);
	vec3Dt<Scalar> operator-(vec3Dt<Scalar> const& other) const;
	void operator-=(vec3Dt<Scalar> const& other);
	Scalar operator*(vec3Dt<Scalar> const& other) const;
	vec3Dt<Scalar> operator*(Scalar s) const;
	void operator*=(Scalar s);
	vec3Dt<Scalar> operator^(vec3Dt<Scalar> const& other) const; //cross product
	vec3Dt<Scalar> cross(vec3Dt<Scalar> const& other) const;
	Scalar& x();
	Scalar& y();
	Scalar& z();
	Scalar& operator[](int iDim);
	Scalar const& operator[](int iDim) const;
	Scalar* data();
	const Scalar* constData() const;
	std::array<Scalar, 3> Arr;
};

extern template class vec3Dt<float>;
extern template class vec3Dt<double>;

using vec3D = vec3Dt<float>;
using vec3Dd = vec3Dt<double>;

template <typename Scalar>
vec3Dt<Scalar> operator*(Scalar s, vec3Dt<Scalar> const& v);

extern template vec3Dt<float> operator*(float s, vec3Dt<float> const& v);
extern template vec3Dt<double> operator*(double s, vec3Dt<double> const& v);

