#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "./gtrack/gtrack.h"


GTRACK_moduleConfig* mallocCfg()
{
    GTRACK_moduleConfig* cfg;

    cfg = (GTRACK_moduleConfig*)malloc(sizeof(GTRACK_moduleConfig));
    //advParams分配内存
    cfg->advParams = (GTRACK_advancedParameters*)malloc(sizeof(GTRACK_advancedParameters));

    //allocationParams分配内存
    cfg->advParams->allocationParams = (GTRACK_allocationParams*)malloc(sizeof(GTRACK_allocationParams));

    //gatingParams分配内存
    cfg->advParams->gatingParams = (GTRACK_gatingParams*)malloc(sizeof(GTRACK_gatingParams));

    //presenceParams分配内存
    cfg->advParams->presenceParams = (GTRACK_presenceParams*)malloc(sizeof(GTRACK_presenceParams));

    //sceneryParams分配内存
    cfg->advParams->sceneryParams = (GTRACK_sceneryParams*)malloc(sizeof(GTRACK_sceneryParams));

    //stateParams分配内存
    cfg->advParams->stateParams = (GTRACK_stateParams*)malloc(sizeof(GTRACK_stateParams));

    return cfg;
}

int setCfg(GTRACK_moduleConfig* cfg, const char* cfgFilePath)
{
    FILE* fp = NULL;

    fp = fopen(cfgFilePath, "r");

    if (fp != NULL)
    {
        char buff[255];

        //设置基础配置
#ifdef GTRACK_3D
        cfg->stateVectorType = static_cast<GTRACK_STATE_VECTOR_TYPE>(3);
#else
        cfg->stateVectorType = static_cast<GTRACK_STATE_VECTOR_TYPE>(1);
#endif // GTRACK_3D
        cfg->verbose = static_cast<GTRACK_VERBOSE_TYPE>(0);
        cfg->maxNumPoints = 800;
        cfg->maxNumTracks = 30;
        cfg->initialRadialVelocity = 0;
        cfg->maxRadialVelocity = 4;
        cfg->radialVelocityResolution = 0.033;
        cfg->deltaT = 0.1;
        cfg->boresightFilteringEnable = true;

        //高级配置
        /*
        配置文件示例：
        staticBoundaryBox -3 3 0.5 7.5 0 3
        boundaryBox -4 4 0 8 0 3
        sensorPosition 2 0 0
        gatingParam 3 2 2 2 4
        stateParam 3 3 12 500 5 6000
        allocationParam 20 100 0.1 20 0.5 20
        maxAcceleration 0.1 0.1 0.1
        trackingCfg 1 2 800 30 46 96 55
        presenceBoundaryBox -3 3 0.5 7.5 0 3
        */

        //maxAcceleration
        fscanf(fp, "%s %f %f %f\n", buff, &cfg->maxAcceleration[0], &cfg->maxAcceleration[1], &cfg->maxAcceleration[2]);

        //allocationParams
        GTRACK_allocationParams* allocationParams = cfg->advParams->allocationParams;
        fscanf(fp, "%s %f %f %f %hu %f %f\n", buff, &allocationParams->snrThre, &allocationParams->snrThreObscured, &allocationParams->velocityThre, &allocationParams->pointsThre, &allocationParams->maxDistanceThre, &allocationParams->maxVelThre);

        //gatingParams
        GTRACK_gatingParams* gatingParams = cfg->advParams->gatingParams;
        fscanf(fp, "%s %f %f %f %f %f\n", buff, &gatingParams->gain, &gatingParams->limits.depth, &gatingParams->limits.width, &gatingParams->limits.height, &gatingParams->limits.vel);

        //presenceParams
        cfg->advParams->presenceParams->numOccupancyBoxes = 1;
        cfg->advParams->presenceParams->on2offThre = 10;
        cfg->advParams->presenceParams->pointsThre = 10;
        cfg->advParams->presenceParams->velocityThre = 1;
        GTRACK_boundaryBox* occupancyBox = cfg->advParams->presenceParams->occupancyBox;
        fscanf(fp, "%s %f %f %f %f %f %f\n", buff, &occupancyBox->x1, &occupancyBox->x2, &occupancyBox->y1, &occupancyBox->y2, &occupancyBox->z1, &occupancyBox->z2);

        //stateParams
        GTRACK_stateParams* stateParams = cfg->advParams->stateParams;
        fscanf(fp, "%s %hu %hu %hu %hu %hu %hu\n", buff, &stateParams->det2actThre, &stateParams->det2freeThre, &stateParams->active2freeThre, &stateParams->static2freeThre, &stateParams->exit2freeThre, &stateParams->sleep2freeThre);

        //sceneryParams
        //staticBoundaryBox
        GTRACK_boundaryBox* staticBox = cfg->advParams->sceneryParams->staticBox;
        cfg->advParams->sceneryParams->numStaticBoxes = 1;
        fscanf(fp, "%s %f %f %f %f %f %f\n", buff, &staticBox->x1, &staticBox->x2, &staticBox->y1, &staticBox->y2, &staticBox->z1, &staticBox->z2);

        //boundaryBox
        GTRACK_boundaryBox* boundaryBox = cfg->advParams->sceneryParams->boundaryBox;
        cfg->advParams->sceneryParams->numBoundaryBoxes = 1;
        fscanf(fp, "%s %f %f %f %f %f %f\n", buff, &boundaryBox->x1, &boundaryBox->x2, &boundaryBox->y1, &boundaryBox->y2, &boundaryBox->z1, &boundaryBox->z2);

        //sensorPosition
        GTRACK_sensorPosition* sensorPosition = &(cfg->advParams->sceneryParams->sensorPosition);
        fscanf(fp, "%s %f %f %f\n", buff, &sensorPosition->z, &sensorPosition->x, &sensorPosition->y);

        //sensorOrientation
        cfg->advParams->sceneryParams->sensorOrientation.azimTilt = 0;
        cfg->advParams->sceneryParams->sensorOrientation.elevTilt = 0;

        fclose(fp);
        return 1;
    }
    else
    {
        printf("file open failed!!!\n");
        return 0;
    }

}

void printCfg(GTRACK_moduleConfig cfg)
{
    printf("===============Configuration===============\n");
    //GTRACK_STATE_VECTOR_TYPE stateVectorType
    printf("GTRACK_STATE_VECTOR_TYPE stateVectorType:\n%d\n", cfg.stateVectorType);

    //GTRACK_VERBOSE_TYPE verbose
    printf("GTRACK_VERBOSE_TYPE verbose:\n%d\n", cfg.verbose);

    //uint16_t maxNumPoints
    printf("uint16_t maxNumPoints:\n%hu\n", cfg.maxNumPoints);

    //uint16_t maxNumTracks
    printf("uint16_t maxNumTracks:\n%hu\n", cfg.maxNumTracks);

    //float initialRadialVelocity
    printf("float initialRadialVelocity:\n%f\n", cfg.initialRadialVelocity);

    //float maxRadialVelocity
    printf("float maxRadialVelocity:\n%f\n", cfg.maxRadialVelocity);

    //float radialVelocityResolution
    printf("float radialVelocityResolution:\n%f\n", cfg.radialVelocityResolution);

    //float maxAcceleration[3]
    printf("float maxAcceleration[3]:\n%f %f %f\n", cfg.maxAcceleration[0], cfg.maxAcceleration[1], cfg.maxAcceleration[2]);

    //float deltaT
    printf("float deltaT:\n%f\n", cfg.deltaT);

    //uint16_t  boresightFilteringEnable
    printf("uint16_t  boresightFilteringEnable:\n%hu\n", cfg.boresightFilteringEnable);

    //GTRACK_advancedParameters *advParams
    printf("===============Advanced configuration===============\n");
    printf("GTRACK_advancedParameters *advParams:\n");

    //GTRACK_gatingParams *gatingParams
    GTRACK_gatingParams* gatingParams = cfg.advParams->gatingParams;
    printf("GTRACK_gatingParams *gatingParams:\n%f %f %f %f %f\n", gatingParams->gain, gatingParams->limitsArray[0], gatingParams->limitsArray[1], gatingParams->limitsArray[2], gatingParams->limitsArray[3]);

    //GTRACK_allocationParams *allocationParams
    GTRACK_allocationParams* allocationParams = cfg.advParams->allocationParams;
    printf("GTRACK_allocationParams *allocationParams:\n%f %f %f %hu %f %f\n", allocationParams->snrThre, allocationParams->snrThreObscured, allocationParams->velocityThre, allocationParams->pointsThre, allocationParams->maxDistanceThre, allocationParams->maxVelThre);

    //GTRACK_stateParams *stateParams
    GTRACK_stateParams* stateParams = cfg.advParams->stateParams;
    printf("GTRACK_stateParams *stateParams:\n%hu %hu %hu %hu %hu %hu\n", stateParams->det2actThre, stateParams->det2freeThre, stateParams->active2freeThre, stateParams->static2freeThre, stateParams->exit2freeThre, stateParams->sleep2freeThre);

    //GTRACK_sceneryParams *sceneryParams
    GTRACK_sceneryParams* sceneryParams = cfg.advParams->sceneryParams;
    printf("GTRACK_sceneryParams *sceneryParams:\n");
    printf("\tGTRACK_sensorPosition sensorPosition:%f %f %f\n", sceneryParams->sensorPosition.x, sceneryParams->sensorPosition.y, sceneryParams->sensorPosition.z);
    printf("\tGTRACK_sensorOrientation sensorOrientation:%f %f\n", sceneryParams->sensorOrientation.azimTilt, sceneryParams->sensorOrientation.elevTilt);
    printf("\tBoundaryBoxes:%hu %f %f %f %f %f %f\n", sceneryParams->numBoundaryBoxes, sceneryParams->boundaryBox->x1, sceneryParams->boundaryBox->x2, sceneryParams->boundaryBox->y1, sceneryParams->boundaryBox->y2, sceneryParams->boundaryBox->z1, sceneryParams->boundaryBox->z2);
    printf("\tStaticBoxes:%hu %f %f %f %f %f %f\n", sceneryParams->numStaticBoxes, sceneryParams->staticBox->x1, sceneryParams->staticBox->x2, sceneryParams->staticBox->y1, sceneryParams->staticBox->y2, sceneryParams->staticBox->z1, sceneryParams->staticBox->z2);

    //GTRACK_presenceParams *presenceParams;
    GTRACK_presenceParams* presenceParams = cfg.advParams->presenceParams;
    printf("GTRACK_presenceParams *presenceParams:\n%hu %f %hu %hu %f %f %f %f %f %f\n", presenceParams->pointsThre, presenceParams->velocityThre, presenceParams->on2offThre, presenceParams->numOccupancyBoxes,
        presenceParams->occupancyBox->x1, presenceParams->occupancyBox->x2, presenceParams->occupancyBox->y1, presenceParams->occupancyBox->y2, presenceParams->occupancyBox->z1, presenceParams->occupancyBox->z2);
    
    printf("===============END===============\n");
}






