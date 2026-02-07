void offsetMesh(
    const std::vector<Vec3s>& inVerts,
    const std::vector<Vec3I>& inTris,
    float offset,
    std::vector<Vec3s>& outVerts,
    std::vector<Vec3I>& outTris)
{
    openvdb::initialize();

    float voxelSize = std::abs(offset) / 3.0f;
    float halfWidth = 3.0f;

    auto transform =
        openvdb::math::Transform::createLinearTransform(voxelSize);

    auto sdf =
        openvdb::tools::meshToSignedDistanceField<openvdb::FloatGrid>(
            *transform, inVerts, inTris, {}, halfWidth, halfWidth);

    for (auto it = sdf->beginValueOn(); it; ++it)
        *it -= offset;

    openvdb::tools::volumeToMesh(*sdf, outVerts, outTris, 0.0f);
}
