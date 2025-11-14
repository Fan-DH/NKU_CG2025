#pragma once
#ifndef __NR_RAYTRACING_KDTREE_HPP__
#define __NR_RAYTRACING_KDTREE_HPP__

#include <vector>
#include <memory>
#include <algorithm>

#include "geometry/vec.hpp"
#include "scene/Scene.hpp"
#include "Ray.hpp"
#include "intersections/HitRecord.hpp"
#include "intersections/intersections.hpp"

namespace RayTracer
{
    using namespace NRenderer;

    struct AABB {
        Vec3 min;
        Vec3 max;
        AABB() : min{ FLOAT_INF, FLOAT_INF, FLOAT_INF }, max{ -FLOAT_INF, -FLOAT_INF, -FLOAT_INF } {}
        AABB(const Vec3& mn, const Vec3& mx) : min(mn), max(mx) {}
        inline void expand(const AABB& b) {
            min = glm::min(min, b.min);
            max = glm::max(max, b.max);
        }
        inline void expand(const Vec3& p) {
            min = glm::min(min, p);
            max = glm::max(max, p);
        }
        static inline AABB fromTriangle(const Triangle& t) {
            AABB b;
            b.expand(t.v1);
            b.expand(t.v2);
            b.expand(t.v3);
            return b;
        }
        static inline AABB fromSphere(const Sphere& s) {
            Vec3 r = Vec3(s.radius);
            return AABB{s.position - r, s.position + r};
        }
        inline bool intersect(const Ray& ray, float tmin = 0.0f, float tmax = FLOAT_INF) const {
            // Slab method
            for (int i = 0; i < 3; i++) {
                float invD = 1.0f / ray.direction[i];
                float t0 = (min[i] - ray.origin[i]) * invD;
                float t1 = (max[i] - ray.origin[i]) * invD;
                if (invD < 0.0f) std::swap(t0, t1);
                tmin = t0 > tmin ? t0 : tmin;
                tmax = t1 < tmax ? t1 : tmax;
                if (tmax <= tmin) return false;
            }
            return true;
        }
    };

    struct PrimitiveRef {
        enum class Type { SPHERE, TRIANGLE };
        Type type;
        unsigned int index;
        AABB bbox;
        Vec3 centroid;
    };

    struct KDNode {
        AABB bbox;
        int left = -1;
        int right = -1;
        std::vector<unsigned int> primIndices; // indices into KDTree::prims
        int axis = -1; // split axis
        bool leaf = true;
    };

    class KDTree {
    public:
        KDTree() = default;
        KDTree(const Scene& scene, int leafThreshold = 8) {
            leafThreshold_ = leafThreshold;
            rebuild(scene);
        }
        void rebuild(const Scene& scene);
        HitRecord intersect(const Ray& r, const Scene& scene, float tMax = FLOAT_INF) const;
    private:
        std::vector<PrimitiveRef> prims_;
        std::vector<KDNode> nodes_;
        int root_ = -1;
        int leafThreshold_ = 8;

        int buildNode(std::vector<unsigned int>& indices, int depth);
        void traverse(int nodeIdx, const Ray& r, const Scene& scene, float& closest, HitRecord& closestHit) const;
    };
}

#endif