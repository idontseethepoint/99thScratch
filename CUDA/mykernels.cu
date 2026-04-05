#include <windows.h>
#include <GL/gl.h>
#include <cuda_gl_interop.h>
#include "device_launch_parameters.h"
#include "cuda_runtime.h"
#include <cuda.h>
#include "cuda_gl_interop.h"

#include "mykernels.cuh"

#include <cmath>

__global__ void deformKernel(float* posNormals, float t, int N)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i >= N)
	{
		return;
	}
	if (posNormals[6*i] > 0)
		posNormals[6 * i] = 1 + cos(t);
}

void runDeform(cudaGraphicsResource* cuda_vbo, float t)
{
	const int nVert = 36;
	cudaGraphicsMapResources(1, &cuda_vbo, 0);
	float* dev_ptr;
	size_t num_bytes;
	cudaGraphicsResourceGetMappedPointer((void**)&dev_ptr, &num_bytes, cuda_vbo);

	constexpr int threadsPerBlock = 256;
	const int blocksPerGrid = (nVert + threadsPerBlock - 1) / threadsPerBlock;
	deformKernel << <blocksPerGrid, threadsPerBlock >> > (dev_ptr, t, nVert);

	cudaGraphicsUnmapResources(1, &cuda_vbo, 0);
}