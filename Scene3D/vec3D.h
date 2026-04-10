#pragma once

#ifdef __CUDACC__
#define HOST_DEVICE __host__ __device__
#define CPP_ONLY /##/
#else
#define HOST_DEVICE
#define CPP_ONLY 
#include <QVector3D>
#endif


#include <array>

template <typename Scalar>
struct vec3Dt
{
	HOST_DEVICE vec3Dt(Scalar x, Scalar y, Scalar z);
	CPP_ONLY vec3Dt(std::array<Scalar, 3> const& arr);
	HOST_DEVICE vec3Dt(vec3Dt<double> const& vD);
	CPP_ONLY QVector3D Qvec() const;
	HOST_DEVICE Scalar DistanceSquaredTo(vec3Dt<Scalar> const& other);
	HOST_DEVICE Scalar DistanceTo(vec3Dt<Scalar> const& other);
	HOST_DEVICE static vec3Dt<Scalar> null();
	HOST_DEVICE vec3Dt<Scalar> operator+(vec3Dt<Scalar> const& other) const;
	HOST_DEVICE void operator+=(vec3Dt<Scalar> const& other);
	HOST_DEVICE vec3Dt<Scalar> operator-(vec3Dt<Scalar> const& other) const;
	HOST_DEVICE void operator-=(vec3Dt<Scalar> const& other);
	HOST_DEVICE Scalar operator*(vec3Dt<Scalar> const& other) const;
	HOST_DEVICE vec3Dt<Scalar> operator*(Scalar s) const;
	HOST_DEVICE void operator*=(Scalar s);
	HOST_DEVICE vec3Dt<Scalar> operator/(Scalar s) const;
	HOST_DEVICE void operator/=(Scalar s);
	HOST_DEVICE vec3Dt<Scalar> operator^(vec3Dt<Scalar> const& other) const; //cross product
	HOST_DEVICE vec3Dt<Scalar> cross(vec3Dt<Scalar> const& other) const;
	HOST_DEVICE vec3Dt<Scalar> operator-() const;
	HOST_DEVICE void normalize();
	HOST_DEVICE vec3Dt normalized() const;
	HOST_DEVICE Scalar& x();
	HOST_DEVICE Scalar& y();
	HOST_DEVICE Scalar& z();
	HOST_DEVICE Scalar const& x() const;
	HOST_DEVICE Scalar const& y() const;
	HOST_DEVICE Scalar const& z() const;
	HOST_DEVICE Scalar& operator[](int iDim);
	HOST_DEVICE Scalar const& operator[](int iDim) const;
	HOST_DEVICE Scalar* data();
	HOST_DEVICE const Scalar* constData() const;
	HOST_DEVICE inline bool operator==(vec3Dt<Scalar> const& other) const {
		return Arr[0] == other.Arr[0] && Arr[1] == other.Arr[1] && Arr[2] == other.Arr[2];
	}
	HOST_DEVICE inline bool operator!=(vec3Dt<Scalar> const& other) const {
		return !(*this == other);
	}
	HOST_DEVICE vec3Dt<Scalar> GetANormal() const;
	Scalar Arr[3];
};

extern template class vec3Dt<float>;
extern template class vec3Dt<double>;

using vec3D = vec3Dt<float>;
using vec3Dd = vec3Dt<double>;

template <typename Scalar>
HOST_DEVICE vec3Dt<Scalar> operator*(Scalar s, vec3Dt<Scalar> const& v);

extern template vec3Dt<float> operator*(float s, vec3Dt<float> const& v);
extern template vec3Dt<double> operator*(double s, vec3Dt<double> const& v);


#ifdef __CUDACC__
#include "vec3D.cpp"
#endif
