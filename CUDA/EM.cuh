#pragma once
#include <cuda_runtime.h>

#include "AtomState.h"
#include "LJ_Parameters.h"
#include "vec3D.h"

#include <vector>

struct cudaGraphicsResource;

#ifdef __cplusplus
extern "C" {
#endif
    class LJ_CUDA_Evolver
    {
    public:
        LJ_CUDA_Evolver(LJ_Parameters const& params,
            std::vector<int> atomTypes,
            std::vector<AtomState> const& initialState);
        ~LJ_CUDA_Evolver();

        void evolve(double dt);
        std::vector<AtomState> getAtomStates();
    private:
        void copyToHost(AtomState* d_prev);
        void setParamTable(LJ_Parameters const& params);
        void setBox(LJ_Parameters const& params);
        vec3Dd* _d_Fij;
        vec3Dd* _d_netForces;
        AtomState* _d_stateCur;
        AtomState* _d_stateNext;
        int* _d_atomTypes;
        AtomState* _h_stateBuffer;
        cudaStream_t _stream;
        int _nAtom;
        std::vector<AtomState> _latestState;
        bool _needHostUpdate = false;
    };
#ifdef __cplusplus
}
#endif
