#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdint.h>
#include <winsock2.h>
#include <string.h>
#include "./gtrack/gtrack.h"
#include "./numToString.h"
#define PI 3.1415926535898
#pragma comment(lib,"ws2_32.lib")

int sendAll(SOCKET socket, const char *buffer, size_t length)
{
    char *ptr = (char*) buffer;
    while (length > 0)
    {
        int i = send(socket, ptr, length, 0);
        if (i < 1) return 0;
        ptr += i;
        length -= i;
    }
    return 1;
}

int recvAll(SOCKET socket, char *buf, size_t length)
{
    int i;

    while (length > 0)
    {
        i = recv(socket, buf, length, 0);
        if (i <= 0)
            return 0;
        buf += i;
        length -= i;
    }
    return 1;
}

char* parseSendData(GTRACK_targetDesc *targetDescr, uint16_t tNum, int frameId, uint8_t *bestIndex, uint16_t mNum, uint8_t *isUniqueIndex)
{
    /*
    frame format:
    ^^^    //start tag
    frameId(10)tNum(10)mNum(10)    //header
    uid(10)tid(10)posX(10)posY(10)posZ(10)velX(10)velY(10)velZ(10)accX(10)accY(10)accZ(10)    //targetData
    destIndex(3)*mNum    //pointsData
    ###
    */
    const size_t dataLength = 39 + 111*tNum + 3*mNum;
    char* trackedObjs = (char *)gtrack_alloc(dataLength + 1, sizeof(char));
    trackedObjs[dataLength] = '\0';
    char* trackedObjsCursor = trackedObjs;

    //帧开始标记
    strncpy(trackedObjsCursor, "^^^\n", 4);
    trackedObjsCursor += 4;

    //头部
    char* frameIdStr = intToFilledString(frameId, 10);
    strncpy(trackedObjsCursor, frameIdStr, 10);
    trackedObjsCursor += 10;
    free(frameIdStr);
    char* tNumStr = intToFilledString(tNum, 10);
    strncpy(trackedObjsCursor, tNumStr, 10);
    trackedObjsCursor += 10;
    free(tNumStr);
    char* mNumStr = intToFilledString(mNum, 10);
    strncpy(trackedObjsCursor, mNumStr, 10);
    trackedObjsCursor += 10;
    free(mNumStr);

    *trackedObjsCursor++ = '\n';
    //目标数据
    uint16_t n;
    for (n = 0; n < tNum; n++)
    {
        //tid 10位
        char* tIdStr = intToFilledString(targetDescr[n].tid, 10);
        strncpy(trackedObjsCursor, tIdStr, 10);
        trackedObjsCursor += 10;
        free(tIdStr);
        //uid 10位
        char* uIdStr = intToFilledString(targetDescr[n].uid, 10);
        strncpy(trackedObjsCursor, uIdStr, 10);
        trackedObjsCursor += 10;
        free(uIdStr);

        //posXYZ/velXYZ/accXYZ
        unsigned i;
#ifdef GTRACK_3D
        for (i = 0; i < 9; i++)
        {
            char* sStr = floatToFilledString(targetDescr[n].S[i], 10);
            strncpy(trackedObjsCursor, sStr, 10);
            trackedObjsCursor += 10;
            free(sStr);
        }
#endif // GTRACK_3D
#ifdef GTRACK_2D
        for (i = 0; i < 6; i++)
        {
            char* sStr = floatToFilledString(targetDescr[n].S[i], 10);
            strncpy(trackedObjsCursor, sStr, 10);
            trackedObjsCursor += 10;
            free(sStr);
            if (i == 1 || i == 3 || i == 5)
            {
                char* sZStr = floatToFilledString(0, 10);
                strncpy(trackedObjsCursor, sZStr, 10);
                trackedObjsCursor += 10;
                free(sZStr);
            }
        }
#endif // GTRACK_2D
        
        *trackedObjsCursor++ = '\n';
    }
    //点数据
    for (n = 0; n < mNum; n++)
    {
        char* pIdStr = intToFilledString(bestIndex[n], 3);
        strncpy(trackedObjsCursor, pIdStr, 3);
        trackedObjsCursor += 3;
        free(pIdStr);
    }
    //帧结束标记
    strncpy(trackedObjsCursor, "\n###", 4);

    return trackedObjs;
}


int sendData(SOCKET socket, const char* data)
{
    const size_t dataLength = strlen(data);
    int flag;
    flag = send(socket, data, dataLength, 0);
    if (flag <= 0)
    {
        printf("sending data error\n");
        return 0;
    }
    return 1;
}


int recvData(SOCKET socket, int* frameId, uint16_t* mNum, char** data)
{
    /*
    frame format:
    ^^^    //start tag
    frameId(10)mNum(10)    //header
    range(10)azimuth(10)elev(10)doppler(10)snr(10)    //data
    ###
    */
    //接收数据
    int flag;
    char* dataAll = (char *)gtrack_alloc(1, sizeof(char));
    dataAll[0] = '\0';
    while (1)
    {
        char buf[256];
        flag = recv(socket, buf, 255, 0);
        printf(buf);
        if (flag <= 0)
        {
            printf("Receiving data error\n");
            free(dataAll);
            return 0;
        }
        buf[flag] = '\0';
        strSplicing(&dataAll, buf);
        //如果出现结束符
        if (strstr(dataAll, "###"))
        {
            break;
        }
    }

    //判断帧完整
    if (!strstr(dataAll, "^^^"))
    {
        sendData(socket, "Server:Receiving data format error\n");
        free(dataAll);
        return 0;
    }
    
    //解析头部
    char frameIdString[11], mNumString[11];
    strncpy(frameIdString, dataAll + 4, 10);
    frameIdString[10] = '\0';
    *frameId = atoi(frameIdString);

    strncpy(mNumString, dataAll + 14, 10);
    mNumString[10] = '\0';
    *mNum = atoi(mNumString);

    //解析数据
    const size_t dataLength = sizeof(char) * 51 * (*mNum);
    *data = (char*)gtrack_alloc(dataLength + 1, sizeof(char));
    strncpy(*data, dataAll + 25, dataLength);
    (*data)[dataLength] = '\0';

    return 1;

}


GTRACK_measurementPoint* parseRecvData(char *data, const uint16_t mNum)
{
    GTRACK_measurementPoint *points = (GTRACK_measurementPoint *)gtrack_alloc(mNum, sizeof(GTRACK_measurementPoint));
    GTRACK_measurementPoint* pointsCursor = points;
    uint16_t n;
    for(n=0; n<mNum; n++)
    {
        char rangeString[11], azimuthString[11], elevationString[11], dopplerString[11], snrString[11];
        memcpy(rangeString, data + 51*n, 10);
        memcpy(azimuthString, data + 51*n + 10, 10);
        memcpy(elevationString, data + 51*n + 20, 10);
        memcpy(dopplerString, data + 51*n + 30, 10);
        memcpy(snrString, data + 51*n + 40, 10);
        rangeString[10] = azimuthString[10] = elevationString[10] = dopplerString[10] = snrString[10] = '\0';
        float range = atof(rangeString), azimuth = atof(azimuthString), elevation = atof(elevationString), doppler = atof(dopplerString), snr = atof(snrString);
        //角度转弧度
        azimuth = azimuth * PI / 180;
        elevation = elevation * PI / 180;

#ifdef GTRACK_2D
        pointsCursor->vector.range = range;
        pointsCursor->vector.angle = azimuth;
        pointsCursor->vector.doppler = doppler;
        pointsCursor->snr = snr;
        pointsCursor++;
#endif // GTRACK_2D
#ifdef GTRACK_3D
        pointsCursor->vector.range = range;
        pointsCursor->vector.azimuth = azimuth;
        pointsCursor->vector.elev = elevation;
        pointsCursor->vector.doppler = doppler;
        pointsCursor->snr = snr;
        pointsCursor++;
#endif // GTRACK_3D

    }

    free(data);
    return points;
}

void printMeasurementPoints(GTRACK_measurementPoint* points, const uint16_t mNum)
{
    GTRACK_measurementPoint* pointsCursor = points;
    uint16_t n;
    printf("===============measurement points===============\n");
    for (n = 0; n < mNum; n++)
    {
#ifdef GTRACK_2D
        printf("range:%f  angle:%f  doppler:%f  snr:%f\n", pointsCursor->vector.range, pointsCursor->vector.angle, pointsCursor->vector.doppler, pointsCursor->snr);
#endif // GTRACK_2D
#ifdef GTRACK_3D
        printf("range:%f  azimuth:%f  elev:%f  doppler:%f  snr:%f\n", pointsCursor->vector.range, pointsCursor->vector.azimuth, points->vector.elev, pointsCursor->vector.doppler, pointsCursor->snr);
#endif // GTRACK_3D
        pointsCursor++;
    }
    printf("===============END===============\n");
}


int socketConnect(SOCKET * sListen, SOCKET* sClient)
{
    //初始化WSA
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(sockVersion, &wsaData) != 0)
    {
        return 0;
    }

    //创建套接字
    *sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (*sListen == INVALID_SOCKET)
    {
        printf("socket error !");
        return 0;
    }

    //绑定IP和端口
    SOCKADDR_IN  sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(8888);
    sin.sin_addr.S_un.S_addr = INADDR_ANY;
    if (bind(*sListen, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
    {
        printf("bind error !");
    }

    //开始监听
    if (listen(*sListen, 5) == SOCKET_ERROR)
    {
        printf("listen error !");
        return 0;
    }

    //建立连接
    SOCKADDR_IN remoteAddr;
    int nAddrlen = sizeof(remoteAddr);

    printf("Waiting for connection...\n");
    *sClient = accept(*sListen, (SOCKADDR*)&remoteAddr, &nAddrlen);
    if (*sClient == INVALID_SOCKET)
    {
        printf("accept error !");
        return 0;
    }
    printf("Successfully connected: %s \r\n", inet_ntoa(remoteAddr.sin_addr));

    //设置读数据超时
    int recvTimeout = 3 * 1000;   //3s
    setsockopt(*sClient, SOL_SOCKET, SO_RCVTIMEO, (char*)&recvTimeout, sizeof(int));
    return 1;
}

void socketClose(SOCKET sListen, SOCKET sClient)
{
    closesocket(sClient);
    closesocket(sListen);
    WSACleanup();
}




//int main(int argc, char* argv[])
//{
//
//    SOCKET sListen, sClient;
//    if (!(socketConnect(&sListen, &sClient)))
//    {
//        return 0;
//    }
//
//    while (true)
//    {
//        //接收数据
//        int frameId;
//        uint16_t mNum;
//        char* data;
//        if (recvData(sClient, &frameId, &mNum, &data))
//        {
//            GTRACK_measurementPoint* points;
//            points = parseSendData(data, mNum);
//            //int i = 0;
//            //for (i = 0; i < mNum; i++)
//            //{
//            //    printf("###%f %f %f %f\n", points->vector.range, points->vector.angle, points->vector.doppler, points->snr);
//            //    points++;
//            //}
//        }
//        else
//        {
//            printf("close connect\n");
//            break;
//        }
//
//
//        //发送数据
//        //const char * sendData = "你好，TCP客户端！\n";
//        //send(sClient, sendData, strlen(sendData), 0);
//        
//    }
//
//    socketClose(sListen, sClient);
//
//    return 0;
//}