#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char* floatToFilledString(float f, const unsigned fillDigit)
{
    int radixPointPos, sign, decimalPartLen = 5;
    char* fStr = fcvt(f, decimalPartLen, &radixPointPos, &sign);

    //������Ҫ���ַ���λ��
    char* fStrFilled = NULL;
    int fStrLen = (radixPointPos <= 0 ? 1 : radixPointPos) + decimalPartLen + 1 + sign;
    if (fillDigit >= fStrLen)    //������λ��>ָ������ַ���λ��
    {
        //��ʼ��fStrFilled
        fStrFilled = (char*)malloc(sizeof(char) * (fillDigit + 1));
        memset(fStrFilled, '0', fillDigit);
        fStrFilled[fillDigit] = '\0';
        char* fStrFilledCpy = fStrFilled;
        //��为��
        if (sign)
        {
            *fStrFilledCpy++ = '-';
        }
        //С��1�������С����ǰ0
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

        //�����ֵ
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
    //��������λ��
    int iCpy = i;
    unsigned count = 0;
    while (iCpy != 0)
    {
        iCpy /= 10;
        count++;
    }

    char* iStr = (char*)malloc(sizeof(char) * (count + 1));
    sprintf(iStr, "%d", (i >= 0 ? i : -i));

    //������Ҫ���ַ���λ��
    char* fStrFilled = NULL;
    int sign = i < 0 ? 1 : 0;
    int fStrLen = count + sign;
    if (fillDigit >= fStrLen)
    {
        //��ʼ��fStrFilled
        fStrFilled = (char*)malloc(sizeof(char) * (fillDigit + 1));
        memset(fStrFilled, '0', fillDigit);
        fStrFilled[fillDigit] = '\0';
        char* fStrFilledCpy = fStrFilled;
        //��为��
        if (sign)
        {
            *fStrFilledCpy = '-';
        }
        //�����ֵ
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
