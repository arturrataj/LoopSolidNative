#include "loopsolid_native.h"
#include "loopsolid_native_wrapper.h"

#include <openvdb/openvdb.h>

#include <cstring>
#include <exception>
#include <string>
#include <vector>

namespace loopsolid {
void offsetMesh(
    const std::vector<openvdb::Vec3s>& inVerts,
    const std::vector<openvdb::Vec3I>& inTris,
    float voxelSize,
    int gaussianItBefore,
    float gaussianRBefore,
    float offset,
    int gaussianItAfter,
    float gaussianRAfter,
    std::vector<openvdb::Vec3s>& outVerts,
    std::vector<openvdb::Vec3I>& outTris);
} // namespace loopsolid

namespace {

// Store result data across create/fetch calls.
struct MeshResult {
    std::vector<openvdb::Vec3s> verts;
    std::vector<openvdb::Vec3I> tris;
};

static void setError(char* errBuf, int errBufLen, const std::string& msg)
{
    if (!errBuf || errBufLen <= 0) {
        return;
    }
    int n = static_cast<int>(msg.size());
    if (n >= errBufLen) {
        n = errBufLen - 1;
    }
    if (n > 0) {
        std::memcpy(errBuf, msg.data(), static_cast<size_t>(n));
    }
    errBuf[n] = '\0';
}

template <typename ParamT>
static int validateInput(const LS_Mesh* inMesh, const ParamT* params)
{
    if (!inMesh || !params) {
        return 1;
    }
    if (!inMesh->verts || !inMesh->tris) {
        return 2;
    }
    if (inMesh->vertCount < 0 || inMesh->triCount < 0) {
        return 3;
    }
    return 0;
}

template <typename ParamT, typename MeshOp>
static int meshOpCreate(
    const LS_Mesh* inMesh,
    const ParamT* params,
    void** outHandle,
    int* outVertCount,
    int* outTriCount,
    char* errBuf,
    int errBufLen,
    MeshOp&& op)
{
    try {
        if (int v = validateInput(inMesh, params)) {
            setError(errBuf, errBufLen, "Invalid input.");
            return v;
        }
        if (!outHandle || !outVertCount || !outTriCount) {
            setError(errBuf, errBufLen, "Output pointers are null.");
            return 4;
        }

        std::vector<openvdb::Vec3s> inVerts;
        std::vector<openvdb::Vec3I> inTris;
        inVerts.reserve(static_cast<size_t>(inMesh->vertCount));
        inTris.reserve(static_cast<size_t>(inMesh->triCount));

        const float* v = inMesh->verts;
        for (int i = 0; i < inMesh->vertCount; ++i) {
            inVerts.emplace_back(v[i * 3 + 0], v[i * 3 + 1], v[i * 3 + 2]);
        }

        const int* t = inMesh->tris;
        for (int i = 0; i < inMesh->triCount; ++i) {
            inTris.emplace_back(t[i * 3 + 0], t[i * 3 + 1], t[i * 3 + 2]);
        }

        MeshResult* result = new MeshResult();
        op(inVerts, inTris,
           static_cast<float>(params->voxelSize),
           static_cast<int>(params->gaussianItBefore),
           static_cast<float>(params->gaussianRBefore),
           static_cast<float>(params->offset),
           static_cast<int>(params->gaussianItAfter),
           static_cast<float>(params->gaussianRAfter),
           result->verts, result->tris);

        *outVertCount = static_cast<int>(result->verts.size());
        *outTriCount = static_cast<int>(result->tris.size());
        *outHandle = result;
        return 0;
    } catch (const std::exception& ex) {
        setError(errBuf, errBufLen, ex.what());
        return 100;
    } catch (...) {
        setError(errBuf, errBufLen, "Unknown error.");
        return 101;
    }
}

static int meshOpFetch(
    void* handle,
    float* outVerts,
    int outVertCapacity,
    int* outTris,
    int outTriCapacity,
    int* outVertCount,
    int* outTriCount,
    char* errBuf,
    int errBufLen)
{
    try {
        if (!handle) {
            setError(errBuf, errBufLen, "Handle is null.");
            return 7;
        }
        if (!outVerts || !outTris || !outVertCount || !outTriCount) {
            setError(errBuf, errBufLen, "Output pointers are null.");
            return 4;
        }
        if (outVertCapacity < 0 || outTriCapacity < 0) {
            setError(errBuf, errBufLen, "Output capacity is negative.");
            return 5;
        }

        MeshResult* result = static_cast<MeshResult*>(handle);
        if (static_cast<int>(result->verts.size()) > outVertCapacity ||
            static_cast<int>(result->tris.size()) > outTriCapacity) {
            setError(errBuf, errBufLen, "Output capacity too small.");
            return 6;
        }

        for (size_t i = 0; i < result->verts.size(); ++i) {
            const openvdb::Vec3s& p = result->verts[i];
            outVerts[i * 3 + 0] = p.x();
            outVerts[i * 3 + 1] = p.y();
            outVerts[i * 3 + 2] = p.z();
        }

        for (size_t i = 0; i < result->tris.size(); ++i) {
            const openvdb::Vec3I& t3 = result->tris[i];
            outTris[i * 3 + 0] = t3.x();
            outTris[i * 3 + 1] = t3.y();
            outTris[i * 3 + 2] = t3.z();
        }

        *outVertCount = static_cast<int>(result->verts.size());
        *outTriCount = static_cast<int>(result->tris.size());
        return 0;
    } catch (const std::exception& ex) {
        setError(errBuf, errBufLen, ex.what());
        return 100;
    } catch (...) {
        setError(errBuf, errBufLen, "Unknown error.");
        return 101;
    }
}

static void meshOpFree(void* handle)
{
    MeshResult* result = static_cast<MeshResult*>(handle);
    delete result;
}

} // namespace

LS_DEFINE_MESH_OP_HANDLE_IMPL(offset_mesh, LS_OffsetParams, loopsolid::offsetMesh)
