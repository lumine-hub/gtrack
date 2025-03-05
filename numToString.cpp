#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char* floatToFilledString(float f, const unsigned fillDigit)
{
    int radixPointPos, sign, decimalPartLen = 5;
    char* fStr = fcvt(f, decimalPartLen, &radixPointPos, &sign);

    //计算需要总字符串位数
    char* fStrFilled = NULL;
    int fStrLen = (radixPointPos <= 0 ? 1 : radixPointPos) + decimalPartLen + 1 + sign;
    if (fillDigit >= fStrLen)    //数字总位数>指定填充字符串位数
    {
        //初始化fStrFilled
        fStrFilled = (char*)malloc(sizeof(char) * (fillDigit + 1));
        memset(fStrFilled, '0', fillDigit);
        fStrFilled[fillDigit] = '\0';
        char* fStrFilledCpy = fStrFilled;
        //填充负号
        if (sign)
        {
            *fStrFilledCpy++ = '-';
        }
        //小于1的数填充小数点前0
        unsigned index;
        if (radixPointPos <= 0)
        {
            *fStrFilledCpy++ = '0';
            *fStrFilledCpy++ = '.';
            for (index = 0; index < -radixPointPos; index++)
            {
                *fStrFilledCpy++ = '0';
            }
        }

        //填充数值
        for (index = 0; index < strlen(fStr); index++)
        {
            *fStrFilledCpy++ = fStr[index];
            if (index == radixPointPos - 1)
            {
                *fStrFilledCpy++ = '.';

            }
        }
    }
    return fStrFilled;
}

char* intToFilledString(int i, const unsigned fillDigit)
{
    //计算整数位数
    int iCpy = i;
    unsigned count = 0;
    while (iCpy != 0)
    {
        iCpy /= 10;
        count++;
    }

    char* iStr = (char*)malloc(sizeof(char) * (count + 1));
    sprintf(iStr, "%d", (i >= 0 ? i : -i));

    //计算需要总字符串位数
    char* fStrFilled = NULL;
    int sign = i < 0 ? 1 : 0;
    int fStrLen = count + sign;
    if (fillDigit >= fStrLen)
    {
        //初始化fStrFilled
        fStrFilled = (char*)malloc(sizeof(char) * (fillDigit + 1));
        memset(fStrFilled, '0', fillDigit);
        fStrFilled[fillDigit] = '\0';
        char* fStrFilledCpy = fStrFilled;
        //填充负号
        if (sign)
        {
            *fStrFilledCpy = '-';
        }
        //填充数值
        fStrFilledCpy = fStrFilled + fillDigit - 1;
        unsigned index;
        for (index = strlen(iStr); index >= 1; index--)
        {
            *fStrFilledCpy-- = iStr[index - 1];
        }
    }
    return fStrFilled;
}


void strSplicing(char** str1, char* str2)
{

    char* str1AndStr2 = (char*)malloc(sizeof(char) * (strlen(*str1) + strlen(str2) + 1));
    strcpy(str1AndStr2, *str1);
    strcat(str1AndStr2, str2);
    free(*str1);
    *str1 = str1AndStr2;
}
