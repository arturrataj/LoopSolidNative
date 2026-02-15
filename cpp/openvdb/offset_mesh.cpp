#include <openvdb/openvdb.h>
#include <openvdb/tools/MeshToVolume.h>
#include <openvdb/tools/VolumeToMesh.h>
#include <openvdb/tools/LevelSetRebuild.h>
#include <openvdb/tools/LevelSetFilter.h>

#include <cmath>
#include <vector>
#include <cstdio>

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
    std::vector<openvdb::Vec3I>& outTris) {
    
    openvdb::initialize();
    if(voxelSize <= 0.0f) {
        voxelSize = std::abs(offset) / 3.0f;
    }
    

    float smoothWorldBefore =
        std::sqrt((float)gaussianItBefore) *
        gaussianRBefore *
        voxelSize;
    float smoothWorldAfter =
        std::sqrt((float)gaussianItAfter) *
        gaussianRAfter *
        voxelSize;
    float halfWidth =
        std::max(5.0f,
            std::max(3.0f * std::abs(offset),
                     2.0f * std::max(smoothWorldBefore, smoothWorldAfter)));
    
    std::printf("[offsetMesh] voxelSize=%f before(gaussianIt=%i gaussianR=%f) offset=%f after(gaussianIt=%i gaussianR=%f) halfWidth=%f\n",
                voxelSize,
                gaussianItBefore, gaussianRBefore,
                offset,
                gaussianItAfter, gaussianRAfter,
                halfWidth);
    
    auto transform = 
        openvdb::math::Transform::createLinearTransform(voxelSize);
    
    //
    // Modifying operations
    //
    // Convert to SDF
    openvdb::FloatGrid::Ptr sdf =
        openvdb::tools::meshToSignedDistanceField<openvdb::FloatGrid>(
            *transform, inVerts, inTris, {}, halfWidth, halfWidth);
    // --- Pre-conditioning (technical)
    {
        openvdb::tools::LevelSetFilter<openvdb::FloatGrid> filter(*sdf);
        for(int i = 0; i < gaussianItBefore; ++i)
            filter.gaussian(gaussianRBefore);
    }
    // --- Optional rebuild before offset (usually not necessary unless heavy smoothing)
    if(gaussianItBefore*gaussianRBefore*gaussianRBefore > 15) {
        sdf = openvdb::tools::levelSetRebuild(*sdf, 0.0f, halfWidth);
    }
    // --- Offset
    if(offset != 0.0f) {
        openvdb::tools::LevelSetFilter<openvdb::FloatGrid> filter(*sdf);
        filter.offset(-offset);
    }
    // --- Rebuild after offset (important)
    sdf = openvdb::tools::levelSetRebuild(*sdf, 0.0f, halfWidth);
    // --- Post smoothing (actual aesthetic smoothing)
    if(gaussianItAfter != 0) {
        openvdb::tools::LevelSetFilter<openvdb::FloatGrid> filter(*sdf);
        for(int i = 0; i < gaussianItAfter; ++i)
            filter.gaussian(gaussianRAfter);
        
        // Final rebuild only if smoothing was strong
        if(gaussianItAfter*gaussianRAfter*gaussianRAfter > 15)
            sdf = openvdb::tools::levelSetRebuild(*sdf, 0.0f, halfWidth);
    }
    
    // Convert back to mesh
    std::vector<openvdb::Vec4I> outQuads;
    openvdb::tools::volumeToMesh(*sdf, outVerts, outTris, outQuads, 
                                 0.0, 0.0, true);
    
    if(!outQuads.empty()) {
        outTris.reserve(outTris.size() + outQuads.size() * 2);
        for(const auto& q : outQuads) {
            outTris.emplace_back(q[0], q[1], q[2]);
            outTris.emplace_back(q[0], q[2], q[3]);
        }
    }
}

} // namespace loopsolid
