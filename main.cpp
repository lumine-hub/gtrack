#define _CRT_SECURE_NO_WARNINGS
#include "./gtrack/gtrack.h"
#include "./gtrack/gtrack_int.h"
#include "./gtrackInterface.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>

void printTargetDesc(GTRACK_targetDesc* t, const uint16_t tNum)
{
    GTRACK_targetDesc* tmpP = t;
    uint16_t n;
    printf("Current target description:\n");
    for (n = 0; n < tNum; n++)
    {
        printf("%d %u %f %f %f\n", (tmpP+n)->uid, (tmpP + n)->tid, (tmpP + n)->uCenter[0], (tmpP + n)->uCenter[1], (tmpP + n)->uCenter[2]);
    }
}

int main()
{
    //创建TCP连接
    SOCKET sListen, sClient;
    if (!(socketConnect(&sListen, &sClient)))
    {
        return 0;
    }

    //为GTRACK_moduleConfig分配内存空间
    GTRACK_moduleConfig *cfg = mallocCfg();

    //通过配置文件初始化cfg
    const char *cfgFilePath = "./gtrack.cfg";
    if (setCfg(cfg, cfgFilePath))
    {
        sendData(sClient, "Server:Configuration initialization succeeded\n");
        //输出配置实例
        printCfg(*cfg);
    }
    else
    {
        sendData(sClient, "Server:Configuration initialization failed\n");
        return 0;
    }

    //创建模块实例
    int32_t errCode;
    GtrackModuleInstance *h = (GtrackModuleInstance*) gtrack_create(cfg, &errCode);
    if (h)
    {
        sendData(sClient, "Server:Module instance creation succeeded\n");
        sendData(sClient, "Server:Start");
    }
    else
    {
        sendData(sClient, "Server:Module instance creation failed\n");
        goto exit;
    }
    
    //进行追踪
    GTRACK_targetDesc targetDescr[GTRACK_NUM_TRACKS_MAX];
    uint16_t tNum;
    uint8_t presence;
    while (1)
    {
        //接收数据
        int frameId;
        uint16_t mNum;
        char* data;
        if (recvData(sClient, &frameId, &mNum, &data))
        {
            GTRACK_measurementPoint* points;
            points = parseRecvData(data, mNum);
            //printMeasurementPoints(points, mNum);

            //gtrack_step
            gtrack_step(h, points, 0, mNum, targetDescr, &tNum, 0, 0, &presence, 0);

            printf("===%d\n", frameId);
            //printTargetDesc(targetDescr, tNum);

            //发送数据
            char* trackedObjs = parseSendData(targetDescr, tNum, frameId, h->bestIndex, mNum, h->isUniqueIndex);
            sendData(sClient, trackedObjs);
        }
        else
        {
            sendData(sClient, "Server:Received data error\n");
            goto exit;
        }

    }
exit:
    sendData(sClient, "Server:Close");
    socketClose(sListen, sClient);
    return 0;
}