#ifndef LOOPSOLID_NATIVE_H
#define LOOPSOLID_NATIVE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const float* verts; /* length = 3 * vertCount */
    int vertCount;
    const int* tris; /* length = 3 * triCount */
    int triCount;
} LS_Mesh;

typedef struct {
    double offset;
    double voxelSize;
} LS_OffsetParams;

/* Convenience macro for declaring the handle-based API.
 * Lifecycle: create(...) -> fetch(...) -> free(...)
 */
#define LS_DECLARE_MESH_OP_HANDLE(NAME, PARAM_T)                                \
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

LS_DECLARE_MESH_OP_HANDLE(offset_mesh, LS_OffsetParams)

#ifdef __cplusplus
}
#endif

#endif /* LOOPSOLID_NATIVE_H */
