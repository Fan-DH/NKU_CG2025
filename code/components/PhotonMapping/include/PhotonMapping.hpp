#pragma once
#ifndef __PHOTON_MAPPING_HPP__
#define __PHOTON_MAPPING_HPP__

#include <vector>
#include <tuple>
#include <memory>
#include <algorithm>
#include <queue>

#include "scene/Scene.hpp"
#include "Ray.hpp"
#include "Camera.hpp"
#include "intersections/HitRecord.hpp"
#include "intersections/intersections.hpp"
#include "VertexTransformer.hpp"

namespace PhotonMapping
{
    using namespace NRenderer;
    using namespace std;

    struct Photon
    {
        Vec3 position;
        Vec3 incident;
        RGB power;
    };

    class PhotonKDTree
    {
    private:
        struct Node { int index; int axis; int left; int right; };
        vector<Photon>* photonsRef = nullptr;
        vector<Node> nodes;
        int root = -1;
    public:
        void build(vector<Photon>& photons);
        vector<pair<float,int>> kNearest(const Vec3& p, int k) const;
    private:
        int buildRec(vector<int>& indices, int depth, vector<Photon>& photons);
        void searchRec(int nodeIdx, const Vec3& p, int k, priority_queue<pair<float,int>>& heap) const;
    };

    class PhotonMapperRenderer
    {
    private:
        SharedScene spScene;
        Scene& scene;
        unsigned int width;
        unsigned int height;
        Camera camera;
        vector<Photon> photons;
        unique_ptr<PhotonKDTree> photonTree;
        int gatherK = 50;
        int maxBounces = 8;
        int photonCount = 50000;
        RGB totalEmitted = {0,0,0};
        RGB totalRecorded = {0,0,0};
        RGB totalEmittedRaw = {0,0,0};
        RGB totalRecordedRaw = {0,0,0};
        int emittedCount = 0;
        int absorbedCount = 0;
        int escapedCount = 0;
        RGB emittedEnergy = {0,0,0};
        RGB absorbedEnergy = {0,0,0};
        RGB escapedEnergy = {0,0,0};
    public:
        PhotonMapperRenderer(SharedScene spScene)
            : spScene               (spScene)
            , scene                 (*spScene)
            , camera                (spScene->camera)
        {
            width = scene.renderOption.width;
            height = scene.renderOption.height;
            gatherK = (int)scene.renderOption.photonGatherK;
            photonCount = (int)scene.renderOption.photonCount;
        }
        using RenderResult = tuple<RGBA*, unsigned int, unsigned int>;
        RenderResult render();
        void release(const RenderResult& r);
    private:
        void buildPhotonMap();
        HitRecord closestHitObject(const Ray& r);
        std::tuple<float, Vec3> closestHitLight(const Ray& r) {
            Vec3 v = {};
            auto closest = getHitRecord(FLOAT_INF, {}, {}, {});
            for (auto& a : scene.areaLightBuffer) {
                auto hitRecord = Intersection::xAreaLight(r, a, 0.000001f, closest->t);
                if (hitRecord && closest->t > hitRecord->t) {
                    closest = hitRecord;
                    v = a.radiance;
                }
            }
            return { closest->t, v };
        }
        RGB gamma(const RGB& rgb);
        RGB trace(const Ray& r, int currDepth);
        RGB getAlbedo(Material& mat);
        void savePhase1PNG(const std::string& dir);
    };
}

#endif
