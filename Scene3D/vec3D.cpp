#include "vec3D.h"

#include <assert.h>

template <typename Scalar>
vec3Dt<Scalar>::vec3Dt(Scalar x, Scalar y, Scalar z) : Arr({x, y, z})
{}

template <typename Scalar>
vec3Dt<Scalar>::vec3Dt(std::array<Scalar, 3> const& arr) : Arr(arr)
{}

template <typename Scalar>
vec3Dt<Scalar>::vec3Dt(vec3Dt<double> const& vD) : Arr(
	{ static_cast<Scalar>(vD[0]), static_cast<Scalar>(vD[1]), static_cast<Scalar>(vD[2])})
{}

template <typename Scalar>
QVector3D vec3Dt<Scalar>::Qvec() const
{
	return QVector3D(Arr[0], Arr[1], Arr[2]);
}

template<typename Scalar>
Scalar vec3Dt<Scalar>::DistanceSquaredTo(vec3Dt<Scalar> const& other)
{
	auto v = *(this) - other;
	return v * v;
}

template<typename Scalar>
Scalar vec3Dt<Scalar>::DistanceTo(vec3Dt<Scalar> const& other)
{
	return std::sqrt(DistanceSquaredTo(other));
}

template <typename Scalar>
vec3Dt<Scalar> vec3Dt<Scalar>::null()
{
	return vec3Dt<Scalar>(0, 0, 0);
}

template <typename Scalar>
vec3Dt<Scalar> vec3Dt<Scalar>::operator+(vec3Dt<Scalar> const& other) const
{
	return vec3Dt<Scalar>(Arr[0] + other[0],
		Arr[1] + other[1], Arr[2] + other[2]);
}

template <typename Scalar>
void vec3Dt<Scalar>::operator+=(vec3Dt<Scalar> const& other)
{
	for (int iDim = 0; iDim < 3; ++iDim)
	{
		Arr[iDim] += other[iDim];
	}
}

template <typename Scalar>
vec3Dt<Scalar> vec3Dt<Scalar>::operator-(vec3Dt<Scalar> const& other) const
{
	return vec3Dt<Scalar>(Arr[0] - other[0],
		Arr[1] - other[1], Arr[2] - other[2]);
}

template <typename Scalar>
void vec3Dt<Scalar>::operator-=(vec3Dt<Scalar> const& other)
{
	for (int iDim = 0; iDim < 3; ++iDim)
	{
		Arr[iDim] -= other[iDim];
	}
}

template <typename Scalar>
Scalar vec3Dt<Scalar>::operator*(vec3Dt<Scalar> const& other) const
{
	return Arr[0] * other[0] + Arr[1] * other[1]
		+ Arr[2] * other[2];
}

template <typename Scalar>
vec3Dt<Scalar> vec3Dt<Scalar>::operator*(Scalar s) const
{
	return { s * Arr[0], s * Arr[1], s * Arr[2] };
}

template <typename Scalar>
void vec3Dt<Scalar>::operator*=(Scalar s)
{
	Arr[0] *= s;
	Arr[1] *= s;
	Arr[2] *= s;
}

template <typename Scalar>
vec3Dt<Scalar> vec3Dt<Scalar>::operator^(vec3Dt<Scalar> const& other) const
{
	return vec3Dt<Scalar>(Arr[1] * other[2] - Arr[2] * other[1],
		Arr[2] * other[0] - Arr[0] * other[2],
		Arr[0] * other[1] - Arr[1] * other[0]);
}

template <typename Scalar>
vec3Dt<Scalar> vec3Dt<Scalar>::cross(vec3Dt<Scalar> const& other) const
{
	return (*this) ^ other;
}

template <typename Scalar>
Scalar& vec3Dt<Scalar>::x()
{
	return Arr[0];
}

template <typename Scalar>
Scalar& vec3Dt<Scalar>::y()
{
	return Arr[1];
}

template <typename Scalar>
Scalar& vec3Dt<Scalar>::z()
{
	return Arr[2];
}

template <typename Scalar>
Scalar& vec3Dt<Scalar>::operator[](int iDim)
{
	assert(iDim >= 0 && iDim < 3);
	return Arr[iDim];
}

template <typename Scalar>
Scalar const& vec3Dt<Scalar>::operator[](int iDim) const
{
	assert(iDim >= 0 && iDim < 3);
	return Arr[iDim];
}

template <typename Scalar>
Scalar* vec3Dt<Scalar>::data()
{
	return Arr.data();
}

template <typename Scalar>
const Scalar* vec3Dt<Scalar>::constData() const
{
	return Arr.data();
}

template <typename Scalar>
vec3Dt<Scalar> operator*(Scalar s, vec3Dt<Scalar> const& v)
{
	return { s * v.Arr[0], s * v.Arr[1], s * v.Arr[2] };
}
template vec3Dt<float> operator*(float s, vec3Dt<float> const& v);
template vec3Dt<double> operator*(double s, vec3Dt<double> const& v);

template class vec3Dt<float>;
template class vec3Dt<double>;