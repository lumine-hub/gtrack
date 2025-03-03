#ifndef GTRACK_INTERFACE_H
#define GTRACK_INTERFACE_H

#include "./gtrack/gtrack.h"

GTRACK_moduleConfig* mallocCfg();

int setCfg(GTRACK_moduleConfig* cfg, const char* cfgFilePath);

void printCfg(GTRACK_moduleConfig cfg);

int socketConnect(SOCKET* sListen, SOCKET* sClient);

void socketClose(SOCKET sListen, SOCKET sClient);

int recvData(SOCKET socket, int* frameId, uint16_t* mNum, char** data);

GTRACK_measurementPoint* parseRecvData(char* data, const uint16_t mNum);

int sendData(SOCKET socket, const char* data);

char* parseSendData();

void printMeasurementPoints(GTRACK_measurementPoint* points, const uint16_t mNum);

#endif
