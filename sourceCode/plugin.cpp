#include <stdexcept>
#include <vector>
#include <cmath>
#include <cstdlib>

#include <sys/types.h>
#include <sys/stat.h>

#include <Eigen/Dense>

#include <ldrawloader.h>

#include <simPlusPlus/Plugin.h>
#include "plugin.h"
#include "stubs.h"
#include "config.h"

using namespace std;

class Plugin : public sim::Plugin
{
public:
    void onInit()
    {
        if(!registerScriptStuff())
            throw sim::exception("failed to register script stuff");

        setExtVersion("LDraw");
        setBuildDate(BUILD_DATE);

        auto ldrawDir = sim::getNamedStringParam("ldraw.dir");
        if(!ldrawDir)
        {
            const char* home = std::getenv("HOME");
            int plat = sim::getInt32Param(sim_intparam_platform);
            if(plat == 0)
            {
                // Windows
                home = std::getenv("UserProfile");
                if(home)
                    sim::setNamedStringParam("ldraw.dir", std::string(home) + "/Documents/LDraw");
            }
            else if(plat == 1)
            {
                // Mac
                if(home)
                    sim::setNamedStringParam("ldraw.dir", std::string(home) + "/Documents/ldraw");
            }
            else if(plat == 2)
            {
                // Linux
                if(home)
                    sim::setNamedStringParam("ldraw.dir", std::string(home) + "/ldraw");
            }
        }
    }

    int dirExists(const char *path)
    {
        struct stat info;
        if(stat(path, &info) != 0)
            return 0;
        else if(info.st_mode & S_IFDIR)
            return 1;
        else
            return 0;
    }

    std::string ldrResultToString(LdrResult r)
    {
        switch(r)
        {
        case LDR_WARNING_IN_USE:
            return "LDR_WARNING_IN_USE";
        case LDR_WARNING_PART_NOT_FOUND:
            return "LDR_WARNING_PART_NOT_FOUND";
        case LDR_SUCCESS:
            return "LDR_SUCCESS";
        case LDR_ERROR_FILE_NOT_FOUND:
            return "LDR_ERROR_FILE_NOT_FOUND";
        case LDR_ERROR_PARSER:
            return "LDR_ERROR_PARSER";
        case LDR_ERROR_INVALID_OPERATION:
            return "LDR_ERROR_INVALID_OPERATION";
        case LDR_ERROR_RESERVED_MEMORY:
            return "LDR_ERROR_RESERVED_MEMORY";
        case LDR_ERROR_INITIALIZATION:
            return "LDR_ERROR_INITIALIZATION";
        case LDR_ERROR_DEPENDENT_OPERATION:
            return "LDR_ERROR_DEPENDENT_OPERATION";
        case LDR_ERROR_OTHER:
            return "LDR_ERROR_OTHER";
        }
        return std::to_string(r);
    }

    void import_(import__in *in, import__out *out)
    {
        auto ldrawDir = sim::getNamedStringParam("ldraw.dir");
        if(!ldrawDir)
            throw sim::exception("the LDraw directory (ldraw.dir) is not set");
        if(!dirExists(ldrawDir->c_str()))
            throw sim::exception("the LDraw directory (ldraw.dir=\"%s\") is not a valid directory", *ldrawDir);

        sim::addLog(sim_verbosity_scriptinfos, "LDraw loader v%d.%d.%d", LDR_LOADER_VERSION_MAJOR, LDR_LOADER_VERSION_MINOR, LDR_LOADER_VERSION_CACHE);
        sim::addLog(sim_verbosity_scriptinfos, "LDraw directory is \"%s\"", *ldrawDir);

        LdrResult result;
        LdrLoaderHDL loader;
        LdrLoaderCreateInfo cfg;
        cfg.basePath = ldrawDir->c_str();
        cfg.cacheFile = nullptr;
        cfg.partFixMode = LDR_PART_FIX_NONE;
        cfg.renderpartBuildMode = LDR_RENDERPART_BUILD_ONLOAD;
        cfg.partFixTjunctions = LDR_TRUE;
        cfg.partHiResPrimitives = LDR_FALSE;
        cfg.renderpartChamfer = 0.35f;
        result = ldrCreateLoader(&cfg, &loader);
        if(result < LDR_SUCCESS)
            throw sim::exception("failed to create LDR loader: %s", ldrResultToString(result));

        LdrModelHDL model;
        result = ldrCreateModel(loader, in->filePath.c_str(), LDR_TRUE, &model);
        if(result > 0)
            sim::addLog(sim_verbosity_warnings, "loader returned %s", ldrResultToString(result));
        if(result < LDR_SUCCESS)
            throw sim::exception("failed to load model: %s", ldrResultToString(result));

        LdrRenderModelHDL renderModel;
        result = ldrCreateRenderModel(loader, model, LDR_TRUE, &renderModel);
        if(result < LDR_SUCCESS)
            throw sim::exception("failed to create render model: %s", ldrResultToString(result));

        const double ldr2m = 0.0004;
        const Eigen::Matrix4d T = (Eigen::Matrix4d() <<
            1, 0, 0, 0,
            0, 0, 1, 0,
            0, -1, 0, 0,
            0, 0, 0, 1
        ).finished();

        for(uint32_t i = 0; i < model->num_instances; i++)
        {
            sim::addLog(sim_verbosity_scriptinfos, "instance #%d", i);
            const LdrInstance& instance = model->instances[i];
            const LdrMatrix& transform = instance.transform;
            const LdrMaterial *material = ldrGetMaterial(loader, instance.material);
            const LdrPart *part = ldrGetPart(loader, instance.part);
            const LdrRenderPart *rpart = ldrGetRenderPart(loader, instance.part);
            if(!rpart->num_triangles || !rpart->num_vertices) continue;
            int verticesLen = 3 * rpart->num_vertices;
            double *vertices = new double[verticesLen];
            for(int i = 0; i < rpart->num_vertices; i++)
            {
                vertices[3*i+0] = ldr2m * rpart->vertices[i].position.x;
                vertices[3*i+1] = ldr2m * rpart->vertices[i].position.y;
                vertices[3*i+2] = ldr2m * rpart->vertices[i].position.z;
            }
            int trianglesLen = 3 * rpart->num_triangles;
            int *triangles = new int[trianglesLen];
            for(int i = 0; i < 3 * rpart->num_triangles; i++)
            {
                triangles[i] = rpart->triangles[i];
            }
            int handle = sim::createMeshShape(0, 0, vertices, verticesLen, triangles, trianglesLen);
            std::string n(part->name);
            if(n.rfind(*ldrawDir, 0) == 0)
                n = n.substr(ldrawDir->length() + 1);
            sim::setObjectAlias(handle, n);
            Eigen::Matrix4d t;
            t = (Eigen::Matrix4d() <<
                transform.values[0], transform.values[4], transform.values[8],  ldr2m * transform.values[12],
                transform.values[1], transform.values[5], transform.values[9],  ldr2m * transform.values[13],
                transform.values[2], transform.values[6], transform.values[10], ldr2m * transform.values[14],
                0, 0, 0, 1
            ).finished();
            t = T * t;
            double m[12] = {
                t(0, 0), t(0, 1), t(0, 2), t(0, 3),
                t(1, 0), t(1, 1), t(1, 2), t(1, 3),
                t(2, 0), t(2, 1), t(2, 2), t(2, 3)
            };
            sim::setObjectMatrix(handle, -1, m);
            const float colorAD[3] = {
                float(material->baseColor[0] / 255.0),
                float(material->baseColor[1] / 255.0),
                float(material->baseColor[2] / 255.0)
            };
            sim::setShapeColor(handle, {}, sim_colorcomponent_ambient_diffuse, &colorAD[0]);
            const float colorA[3] = {
                float(1 - material->alpha / 255.0), 0, 0
            };
            sim::setShapeColor(handle, {}, sim_colorcomponent_transparency, &colorA[0]);
            out->handles.push_back(handle);
            sim::setObjectSel(out->handles);
            delete[] vertices;
            delete[] triangles;
        }

        ldrDestroyRenderModel(loader, renderModel);
        ldrDestroyModel(loader, model);
        ldrDestroyLoader(loader);
    }
};

SIM_PLUGIN(Plugin)
#include "stubsPlusPlus.cpp"
