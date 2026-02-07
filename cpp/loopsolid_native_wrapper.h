#ifndef LOOPSOLID_NATIVE_WRAPPER_H
#define LOOPSOLID_NATIVE_WRAPPER_H

// Macro for generating C ABI mesh-in/mesh-out wrappers.
// Expected converter signature:
//   void converter(
//       const std::vector<openvdb::Vec3s>& inVerts,
//       const std::vector<openvdb::Vec3I>& inTris,
//       float param,
//       float voxelSize,
//       std::vector<openvdb::Vec3s>& outVerts,
//       std::vector<openvdb::Vec3I>& outTris);
//
// Usage (declare in a .cpp):
//   LS_DEFINE_MESH_OP_HANDLE(offset_mesh, LS_OffsetParams, loopsolid::offsetMesh)

#define LS_DEFINE_MESH_OP_HANDLE(NAME, PARAM_T, OP_FUNC)                         \
    int ls_##NAME##_create(                                                     \
        const LS_Mesh* inMesh,                                                   \
        const PARAM_T* params,                                                   \
        void** outHandle,                                                        \
        int* outVertCount,                                                       \
        int* outTriCount,                                                        \
        char* errBuf,                                                            \
        int errBufLen);                                                          \
                                                                                 \
    int ls_##NAME##_fetch(                                                      \
        void* handle,                                                            \
        float* outVerts,                                                         \
        int outVertCapacity,                                                     \
        int* outTris,                                                            \
        int outTriCapacity,                                                      \
        int* outVertCount,                                                       \
        int* outTriCount,                                                        \
        char* errBuf,                                                            \
        int errBufLen);                                                          \
                                                                                 \
    void ls_##NAME##_free(void* handle);

#define LS_DEFINE_MESH_OP_HANDLE_IMPL(NAME, PARAM_T, OP_FUNC)                    \
    /* Use in a .cpp after meshOpCreate/meshOpFetch/meshOpFree are visible. */   \
    int ls_##NAME##_create(                                                     \
        const LS_Mesh* inMesh,                                                   \
        const PARAM_T* params,                                                   \
        void** outHandle,                                                        \
        int* outVertCount,                                                       \
        int* outTriCount,                                                        \
        char* errBuf,                                                            \
        int errBufLen)                                                           \
    {                                                                            \
        return meshOpCreate<PARAM_T>(                                            \
            inMesh,                                                              \
            params,                                                              \
            outHandle,                                                           \
            outVertCount,                                                        \
            outTriCount,                                                         \
            errBuf,                                                              \
            errBufLen,                                                           \
            OP_FUNC);                                                            \
    }                                                                            \
                                                                                 \
    int ls_##NAME##_fetch(                                                      \
        void* handle,                                                            \
        float* outVerts,                                                         \
        int outVertCapacity,                                                     \
        int* outTris,                                                            \
        int outTriCapacity,                                                      \
        int* outVertCount,                                                       \
        int* outTriCount,                                                        \
        char* errBuf,                                                            \
        int errBufLen)                                                           \
    {                                                                            \
        return meshOpFetch(                                                      \
            handle,                                                              \
            outVerts,                                                            \
            outVertCapacity,                                                     \
            outTris,                                                             \
            outTriCapacity,                                                      \
            outVertCount,                                                        \
            outTriCount,                                                         \
            errBuf,                                                              \
            errBufLen);                                                          \
    }                                                                            \
                                                                                 \
    void ls_##NAME##_free(void* handle)                                         \
    {                                                                            \
        meshOpFree(handle);                                                      \
    }

#endif /* LOOPSOLID_NATIVE_WRAPPER_H */
