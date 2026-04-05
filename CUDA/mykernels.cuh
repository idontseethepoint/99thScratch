#pragma once

struct cudaGraphicsResource;

#ifdef __cplusplus
extern "C" {
#endif
	void runDeform(cudaGraphicsResource* cuda_vbo, float t);
#ifdef __cplusplus
}
#endif