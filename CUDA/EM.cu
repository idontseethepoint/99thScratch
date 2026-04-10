#include "EM.cuh"
#include "device_launch_parameters.h"
#include "cuda_runtime.h"


#include "vec3D.h"

#include <cmath>
#include <iostream>

struct Index2D
{
	unsigned int x, y;
};

struct Index3D
{
	unsigned int x, y, z;
};

struct Size3D
{
	unsigned int NX, NY, NZ;
};

struct LJParams
{
	double epsilon;
	double sigma;
};

#define MAX_ATOM_TYPES 20
__constant__ LJParams d_ljTable[MAX_ATOM_TYPES][MAX_ATOM_TYPES];

__constant__ double d_boxLow[3];
__constant__ double d_boxHigh[3];

__device__ Index2D getIndex2D()
{
	return Index2D{ blockIdx.x * blockDim.x + threadIdx.x,
	blockIdx.y * blockDim.y + threadIdx.y};
}


__device__ vec3Dd force(vec3Dd const& p1, vec3Dd const& p2,
	const int ty1, const int ty2)
{
	auto v = p1 - p2;
	auto r2Inv = 1.0 / (v * v);
	auto eps = d_ljTable[ty1][ty2].epsilon;
	auto sigma = d_ljTable[ty1][ty2].sigma;
	auto s2inv = (sigma * sigma) * r2Inv;
	auto f6 = s2inv * s2inv * s2inv;
	auto f12 = f6 * f6;

	return 24 * eps * (2 * f12 - f6) * r2Inv * v;
}

__global__ void SetForces(const AtomState* prev, const int* atomTypes, vec3Dd* Fij, const int nAtom)
{
	auto idx = getIndex2D();
	if (idx.x >= nAtom || idx.y >= nAtom)
		return;
	if (idx.x == idx.y)
		Fij[idx.x * nAtom + idx.y] = vec3Dd::null();
	else
		Fij[idx.x * nAtom + idx.y] = force(prev[idx.x].Pos, 
			prev[idx.y].Pos, atomTypes[idx.x], atomTypes[idx.y]);
}

__global__ void LJ_Update(const AtomState* prev, AtomState* next, 
	const vec3Dd* Fij, vec3Dd* netForces, const double dt, const int nAtom)
{
	extern __shared__ vec3Dd sdata[];

	auto col = blockIdx.x;
	auto tid = threadIdx.x;

	vec3Dd sum = vec3Dd::null();
	for (int row = tid; row < nAtom; row += blockDim.x)
	{
		sum += Fij[col * nAtom + row];
	}
	sdata[tid] = sum;
	__syncthreads();
	
	//Reduction in shared memory
	for (int s = blockDim.x / 2; s > 0; s /= 2)
	{
		if (tid < s)
		{
			sdata[tid] += sdata[tid + s];
		}
		__syncthreads();
	}

	if (tid == 0)
	{
		netForces[col] = sdata[0];
		next[col].Vel = prev[col].Vel + dt * netForces[col];
		next[col].Pos = prev[col].Pos + dt * next[col].Vel;

		for (int iDim = 0; iDim < 3; ++iDim)
		{
			if (next[col].Pos[iDim] < d_boxLow[iDim])
			{
				next[col].Pos[iDim] = d_boxLow[iDim];
				next[col].Vel[iDim] *= -1;
			} else if (next[col].Pos[iDim] > d_boxHigh[iDim])
			{
				next[col].Pos[iDim] = d_boxHigh[iDim];
				next[col].Vel[iDim] *= -1;
			}
		}
	}
}

__global__ void LJ_HeatCool(AtomState* state, double factor, int nAtom)
{
	auto idx = blockIdx.x * blockDim.x + threadIdx.x;
	if (idx >= nAtom) return;
	state[idx].Vel *= factor;
}

template<typename T>
struct CrossData3D
{
	T Data[6];
	//Represents neigboring values in a 3D grid
	//px is the value at x-1, nx is at x + 1, ...
	__host__ __device__ T& px() { return Data[0]; }
	__host__ __device__ T& py() { return Data[1]; }
	__host__ __device__ T& pz() { return Data[2]; }
	__host__ __device__ T& nx() { return Data[3]; }
	__host__ __device__ T& ny() { return Data[4]; }
	__host__ __device__ T& nz() { return Data[5]; }

	__host__ __device__ T const& px() const { return Data[0]; }
	__host__ __device__ T const& py() const { return Data[1]; }
	__host__ __device__ T const& pz() const { return Data[2]; }
	__host__ __device__ T const& nx() const { return Data[3]; }
	__host__ __device__ T const& ny() const { return Data[4]; }
	__host__ __device__ T const& nz() const { return Data[5]; }

	__host__ __device__ T* Low() { return Data; }
	__host__ __device__ T* High() { return Data + 3; }

	__host__ __device__ const T* Low() const { return Data; }
	__host__ __device__ const T* High() const { return Data + 3; }
};

struct Jacobian
{
	double Data[9];

	__host__ __device__ double const& xx() const { return Data[0]; }
	__host__ __device__ double const& xy() const { return Data[1]; }
	__host__ __device__ double const& xz() const { return Data[2]; }
	__host__ __device__ double const& yx() const { return Data[3]; }
	__host__ __device__ double const& yy() const { return Data[4]; }
	__host__ __device__ double const& yz() const { return Data[5]; }
	__host__ __device__ double const& zx() const { return Data[6]; }
	__host__ __device__ double const& zy() const { return Data[7]; }
	__host__ __device__ double const& zz() const { return Data[8]; }

	__host__ __device__ double& xx() { return Data[0]; }
	__host__ __device__ double& xy() { return Data[1]; }
	__host__ __device__ double& xz() { return Data[2]; }
	__host__ __device__ double& yx() { return Data[3]; }
	__host__ __device__ double& yy() { return Data[4]; }
	__host__ __device__ double& yz() { return Data[5]; }
	__host__ __device__ double& zx() { return Data[6]; }
	__host__ __device__ double& zy() { return Data[7]; }
	__host__ __device__ double& zz() { return Data[8]; }

	__host__ __device__ double* operator[](int idx) { return Data + 3 * idx; }
	__host__ __device__ double const* operator[](int idx) const { return Data + 3 * idx; }
};

__device__ double PartialDeriv(CrossData3D<vec3Dd> const& crossData,
	unsigned int numerDim, unsigned int denomDim)
{
	return crossData.High()[numerDim][denomDim] - crossData.Low()[numerDim][denomDim];
}

__device__ Jacobian Derivs(CrossData3D<vec3Dd> const& crossData)
{
	Jacobian ret;
#pragma unroll
	for (int iDim = 0; iDim < 3; ++iDim)
	{
#pragma unroll
		for (int jDim = 0; jDim < 3; ++jDim)
		{
			ret[iDim][jDim] = PartialDeriv(crossData, iDim, jDim);
		}
	}
	return ret;
}

__device__ Index3D getIndex3D()
{
	return Index3D{ blockIdx.x * blockDim.x + threadIdx.x,
	blockIdx.y * blockDim.y + threadIdx.y,
	blockIdx.z * blockDim.z + threadIdx.z };
}

__device__ bool isIndexOutOfBounds(Index3D const& idx, Size3D const& size)
{
	return idx.x >= size.NX || idx.y >= size.NY || idx.z >= size.NZ;
}

template<typename T>
__device__ T ValueAt(T* data, Index3D const& idx, Size3D const& size)
{
	return data[idx.x + size.NX * idx.y + size.NX * size.NY * idx.z];
}


template<typename T>
__device__ CrossData3D<T> GetCrossData(T* data, Index3D const& idx, Size3D const& size)
{
	auto px = idx.x ? idx.x - 1 : 0;
	auto py = idx.y ? idx.y - 1 : 0;
	auto pz = idx.z ? idx.z - 1 : 0;
	auto nx = idx.x ? idx.x + 1 : 0;
	auto ny = idx.y ? idx.y + 1 : 0;
	auto nz = idx.z ? idx.z + 1 : 0;
	return CrossData3D{
		ValueAt(data, {px, idx.y, idx.z}, size),
		ValueAt(data, {idx.x, py, idx.z}, size),
		ValueAt(data, {idx.x, idx.y, pz}, size),
		ValueAt(data, {nx, idx.y, idx.z}, size),
		ValueAt(data, {idx.x, ny, idx.z}, size),
		ValueAt(data, {idx.x, idx.y, nz}, size),
	};
}

__device__ double curl(const vec3Dd* F, int x, int y, int z, int NX, int NY, int NZ)
{
	double Fx = 0;
	return Fx;
}

__global__ void x(const vec3Dd* E, const vec3Dd* B, 
	vec3Dd* Enext, vec3Dd* Bnext, Size3D size)
{
	auto idx = getIndex3D();
	if (isIndexOutOfBounds(idx, size)) return;


}

__global__ void f()
{
	return;
}

LJ_CUDA_Evolver::LJ_CUDA_Evolver(LJ_Parameters const& params,
	std::vector<int> atomTypes,
	std::vector<AtomState> const &initialState) : 
	_nAtom(initialState.size()), _latestState(initialState)
{
	if (atomTypes.size() != _nAtom)
	{
		throw std::runtime_error("atomTypes size must match initialState size");
	}
	setParamTable(params);
	setBox(params);
	cudaMalloc(&_d_Fij, sizeof(vec3Dd) * _nAtom * _nAtom);
	cudaMalloc(&_d_netForces, sizeof(vec3Dd) * _nAtom);
	cudaMalloc(&_d_stateCur, sizeof(AtomState) * _nAtom);
	cudaMalloc(&_d_stateNext, sizeof(AtomState) * _nAtom);
	cudaMalloc(&_d_atomTypes, sizeof(int) * _nAtom);

	//Pinned hist host memory for copying data back from device
	cudaMallocHost(&_h_stateBuffer, sizeof(AtomState) * _nAtom);

	cudaStreamCreate(&_stream);

	for (int i = 0; i < _nAtom; ++i)
	{
		_h_stateBuffer[i] = initialState[i];
	}
	cudaMemcpy(_d_stateCur, _h_stateBuffer, 
		sizeof(AtomState) * _nAtom, cudaMemcpyHostToDevice);
	cudaMemcpy(_d_atomTypes, atomTypes.data(),
		sizeof(int) * _nAtom, cudaMemcpyHostToDevice);
}

LJ_CUDA_Evolver::~LJ_CUDA_Evolver()
{
	cudaFree(_d_Fij);
	cudaFree(_d_netForces);
	cudaFree(_d_stateCur);
	cudaFree(_d_stateNext);
	cudaFreeHost(_h_stateBuffer);
	cudaStreamDestroy(_stream);
}

void LJ_CUDA_Evolver::evolve(double dt)
{
	int blockSize = 256;
	dim3 bsF(16, 16, 1);  // 256 threads per block total
	dim3 nbF((_nAtom + 15) / 16, (_nAtom + 15) / 16, 1);
	SetForces<<<nbF, bsF, 0, _stream >>>(_d_stateCur, _d_atomTypes, _d_Fij, _nAtom);
	LJ_Update<<<_nAtom, blockSize, sizeof(vec3Dd) * blockSize, _stream>>>(_d_stateCur, _d_stateNext,
		_d_Fij, _d_netForces, dt, _nAtom);
	if (_needsHeatCool)
	{
		int nBlocks = (_nAtom + blockSize - 1) / blockSize;
		LJ_HeatCool<<<nBlocks, blockSize, 0, _stream>>>(_d_stateNext, _heatCoolFactor, _nAtom);
		_needsHeatCool = false;
	}
	std::swap(_d_stateNext, _d_stateCur);
	if (_needHostUpdate)
	{
		copyToHost(_d_stateCur);
	}
}

std::vector<AtomState> LJ_CUDA_Evolver::getAtomStates()
{
	_needHostUpdate = true;
    return _latestState;
}

void LJ_CUDA_Evolver::copyToHost(AtomState *d_prev)
{
	cudaMemcpy(_h_stateBuffer, d_prev, sizeof(AtomState) * _nAtom, cudaMemcpyDeviceToHost);


	//std::vector<vec3Dd> h_Fij(_nAtom * _nAtom, vec3Dd::null());
	//std::vector<vec3Dd> h_Fnet(_nAtom, vec3Dd::null());
	//cudaMemcpy(h_Fij.data(), _d_Fij, sizeof(vec3Dd) * _nAtom * _nAtom, cudaMemcpyDeviceToHost);
	//cudaMemcpy(h_Fnet.data(), _d_netForces, sizeof(vec3Dd) * _nAtom, cudaMemcpyDeviceToHost);
	//cudaDeviceSynchronize();
	//double m = 0;
	//for (int i = 0; i < h_Fij.size(); ++i)
	//{
	//	for (int iDim = 0; iDim < 3; ++iDim)
	//	{
	//		if (std::abs(h_Fij[i][iDim]) > m)
	//		{
	//			m = std::abs(h_Fij[i][iDim]);
	//			std::cout << i << ", " << iDim << ": " << m << "\n";
	//		}
	//	}
	//}

	for (int i = 0; i < _nAtom; ++i)
	{
		_latestState[i] = _h_stateBuffer[i];
	}
	_needHostUpdate = false;
}

void LJ_CUDA_Evolver::setParamTable(LJ_Parameters const &params)
{
	if (params.Atoms.size() > MAX_ATOM_TYPES)
	{
		throw std::runtime_error("Too many atom types for CUDA implementation");
	}
	LJParams h_ljTable[MAX_ATOM_TYPES][MAX_ATOM_TYPES];
	for (int i = 0; i < MAX_ATOM_TYPES; ++i)
	{
		for (int j = 0; j < MAX_ATOM_TYPES; ++j)
		{
			if (i >= params.Atoms.size() || j >= params.Atoms.size())
			{
				h_ljTable[i][j].epsilon = 1;
				h_ljTable[i][j].sigma = 1;
			}
			else
			{
				h_ljTable[i][j].epsilon = std::sqrt(params.Atoms[i].Eps * params.Atoms[j].Eps);
				h_ljTable[i][j].sigma = 0.5 * (params.Atoms[i].Sigma + params.Atoms[j].Sigma);
				if (i != j)
					h_ljTable[i][j].epsilon *= params.OffDiagFactor;
			}
		}
	}
	cudaMemcpyToSymbol(d_ljTable, h_ljTable, sizeof(LJParams) * MAX_ATOM_TYPES * MAX_ATOM_TYPES);
}

void LJ_CUDA_Evolver::setBox(LJ_Parameters const &params)
{
	double h_boxLow[3] = { params.Box.Low[0], params.Box.Low[1], params.Box.Low[2] };
	double h_boxHigh[3] = { params.Box.High[0], params.Box.High[1], params.Box.High[2] };
	cudaMemcpyToSymbol(d_boxLow, h_boxLow, sizeof(double) * 3);
	cudaMemcpyToSymbol(d_boxHigh, h_boxHigh, sizeof(double) * 3);
}

void LJ_CUDA_Evolver::heatCool(double s)
{
	_heatCoolFactor = s;
	_needsHeatCool = true;
}
