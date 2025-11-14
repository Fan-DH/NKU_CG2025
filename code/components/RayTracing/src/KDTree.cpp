#include "KDTree.hpp"

namespace RayTracer {

    using std::vector;

    static inline Vec3 centroidOf(const PrimitiveRef& pr, const Scene& scene) {
        if (pr.type == PrimitiveRef::Type::TRIANGLE) {
            const auto& t = scene.triangleBuffer[pr.index];
            return (t.v1 + t.v2 + t.v3) / 3.0f;
        } else {
            const auto& s = scene.sphereBuffer[pr.index];
            return s.position;
        }
    }

    void KDTree::rebuild(const Scene& scene) {
        prims_.clear();
        nodes_.clear();
        root_ = -1;

        // Collect primitives: triangles and spheres
        prims_.reserve(scene.triangleBuffer.size() + scene.sphereBuffer.size());
        for (unsigned int i = 0; i < scene.triangleBuffer.size(); ++i) {
            const auto& t = scene.triangleBuffer[i];
            PrimitiveRef pr;
            pr.type = PrimitiveRef::Type::TRIANGLE;
            pr.index = i;
            pr.bbox = AABB::fromTriangle(t);
            pr.centroid = (t.v1 + t.v2 + t.v3) / 3.0f;
            prims_.push_back(pr);
        }
        for (unsigned int i = 0; i < scene.sphereBuffer.size(); ++i) {
            const auto& s = scene.sphereBuffer[i];
            PrimitiveRef pr;
            pr.type = PrimitiveRef::Type::SPHERE;
            pr.index = i;
            pr.bbox = AABB::fromSphere(s);
            pr.centroid = s.position;
            prims_.push_back(pr);
        }

        // Build indices list
        vector<unsigned int> indices(prims_.size());
        for (unsigned int i = 0; i < indices.size(); ++i) indices[i] = i;
        if (!indices.empty()) {
            root_ = buildNode(indices, 0);
        }
    }

    int KDTree::buildNode(vector<unsigned int>& indices, int depth) {
        KDNode node;

        // Compute bbox of this set
        AABB bbox;
        for (auto idx : indices) {
            bbox.expand(prims_[idx].bbox);
        }
        node.bbox = bbox;

        // Leaf condition
        if (indices.size() <= (size_t)leafThreshold_ || depth >= 32) {
            node.leaf = true;
            node.primIndices = indices;
            nodes_.push_back(node);
            return (int)nodes_.size() - 1;
        }

        // Choose split axis by largest extent
        Vec3 ext = bbox.max - bbox.min;
        int axis = 0;
        if (ext.y > ext.x && ext.y >= ext.z) axis = 1;
        else if (ext.z > ext.x && ext.z >= ext.y) axis = 2;
        node.axis = axis;

        // Partition by median of centroid along axis
        vector<unsigned int> sorted = indices;
        size_t mid = sorted.size() / 2;
        std::nth_element(sorted.begin(), sorted.begin() + mid, sorted.end(), [&](unsigned int a, unsigned int b){
            return prims_[a].centroid[axis] < prims_[b].centroid[axis];
        });
        float splitPos = prims_[sorted[mid]].centroid[axis];

        vector<unsigned int> leftIdx;
        vector<unsigned int> rightIdx;
        leftIdx.reserve(indices.size());
        rightIdx.reserve(indices.size());
        for (auto idx : indices) {
            if (prims_[idx].centroid[axis] < splitPos) leftIdx.push_back(idx);
            else rightIdx.push_back(idx);
        }

        // Degenerate split: fallback to leaf
        if (leftIdx.empty() || rightIdx.empty()) {
            node.leaf = true;
            node.primIndices = indices;
            nodes_.push_back(node);
            return (int)nodes_.size() - 1;
        }

        node.leaf = false;
        nodes_.push_back(node);
        int nodeIdx = (int)nodes_.size() - 1;

        int l = buildNode(leftIdx, depth + 1);
        int r = buildNode(rightIdx, depth + 1);
        nodes_[nodeIdx].left = l;
        nodes_[nodeIdx].right = r;
        return nodeIdx;
    }

    void KDTree::traverse(int nodeIdx, const Ray& r, const Scene& scene, float& closest, HitRecord& closestHit) const {
        const KDNode& node = nodes_[nodeIdx];
        if (!node.bbox.intersect(r, 0.000001f, closest)) return;
        if (node.leaf) {
            for (auto pIdx : node.primIndices) {
                const PrimitiveRef& pr = prims_[pIdx];
                HitRecord hit;
                if (pr.type == PrimitiveRef::Type::TRIANGLE) {
                    hit = Intersection::xTriangle(r, scene.triangleBuffer[pr.index], 0.000001f, closest);
                } else {
                    hit = Intersection::xSphere(r, scene.sphereBuffer[pr.index], 0.000001f, closest);
                }
                if (hit && hit->t < closest) {
                    closest = hit->t;
                    closestHit = hit;
                }
            }
            return;
        }
        if (node.left != -1) traverse(node.left, r, scene, closest, closestHit);
        if (node.right != -1) traverse(node.right, r, scene, closest, closestHit);
    }

    HitRecord KDTree::intersect(const Ray& r, const Scene& scene, float tMax) const {
        if (root_ == -1) return getMissRecord();
        float closest = tMax;
        HitRecord closestHit = getMissRecord();
        traverse(root_, r, scene, closest, closestHit);
        return closestHit;
    }
}