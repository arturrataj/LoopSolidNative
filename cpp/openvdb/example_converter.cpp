#include <openvdb/openvdb.h>

#include <vector>

namespace loopsolid {

// Example converter template:
// - adjust to your algorithm
// - keep the signature so it fits LS_DEFINE_MESH_OP
void exampleMeshOp(
    const std::vector<openvdb::Vec3s>& inVerts,
    const std::vector<openvdb::Vec3I>& inTris,
    float param,
    float voxelSize,
    std::vector<openvdb::Vec3s>& outVerts,
    std::vector<openvdb::Vec3I>& outTris)
{
    (void)param;
    (void)voxelSize;

    // Example: pass-through (not a useful operation, just a template).
    outVerts = inVerts;
    outTris = inTris;
}

} // namespace loopsolid
