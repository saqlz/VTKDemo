#include "stdafx.h"
#include "wisdom_tech_bitmap_helper.h"

void BMPParser::WriteRGBImageToBMP(const char * szFileName, const char * pBits, int iWidth, int iHeight)
{
    //////////////////////////////////////////////////////////////////////////
    //文件头(14Byte)
    unsigned char pFileHead[14];
    memset(pFileHead, 0, 14);

    pFileHead[0] = 'B';
    pFileHead[1] = 'M';

    int iFileSize = iWidth*iHeight * 3 + 14 + 40;
    pFileHead[2] = (unsigned char)(iFileSize);
    pFileHead[3] = (unsigned char)(iFileSize >> 8);
    pFileHead[4] = (unsigned char)(iFileSize >> 16);
    pFileHead[5] = (unsigned char)(iFileSize >> 24);

    pFileHead[6] = 0;
    pFileHead[7] = 0;
    pFileHead[8] = 0;
    pFileHead[9] = 0;

    pFileHead[10] = 54;//位图数据位移处
    pFileHead[11] = 0;
    pFileHead[12] = 0;
    pFileHead[13] = 0;

    //////////////////////////////////////////////////////////////////////////
    //位图信息头（40Byte）
    unsigned char pInfoHead[40];
    memset(pInfoHead, 0, 40);

    pInfoHead[0] = 40;
    pInfoHead[1] = 0;
    pInfoHead[2] = 0;
    pInfoHead[3] = 0;

    pInfoHead[4] = (unsigned char)(iWidth);
    pInfoHead[5] = (unsigned char)(iWidth >> 8);
    pInfoHead[6] = (unsigned char)(iWidth >> 16);
    pInfoHead[7] = (unsigned char)(iWidth >> 24);

    pInfoHead[8] = (unsigned char)(iHeight);
    pInfoHead[9] = (unsigned char)(iHeight >> 8);
    pInfoHead[10] = (unsigned char)(iHeight >> 16);
    pInfoHead[11] = (unsigned char)(iHeight >> 24);

    pInfoHead[12] = 1;
    pInfoHead[13] = 0;

    pInfoHead[14] = 24;
    pInfoHead[15] = 0;

    pInfoHead[16] = 0;
    pInfoHead[17] = 0;
    pInfoHead[18] = 0;
    pInfoHead[19] = 0;

    int iBitmapSize = iWidth*iHeight * 3;
    pInfoHead[20] = (unsigned char)(iBitmapSize);
    pInfoHead[21] = (unsigned char)(iBitmapSize >> 8);
    pInfoHead[22] = (unsigned char)(iBitmapSize >> 16);
    pInfoHead[23] = (unsigned char)(iBitmapSize >> 24);

    pInfoHead[24] = 0;
    pInfoHead[25] = 0;
    pInfoHead[26] = 0;
    pInfoHead[27] = 0;

    pInfoHead[28] = 0;
    pInfoHead[29] = 0;
    pInfoHead[30] = 0;
    pInfoHead[31] = 0;

    pInfoHead[32] = 0;
    pInfoHead[33] = 0;
    pInfoHead[34] = 0;
    pInfoHead[35] = 0;

    pInfoHead[36] = 0;
    pInfoHead[37] = 0;
    pInfoHead[38] = 0;
    pInfoHead[39] = 0;

    //////////////////////////////////////////////////////////////////////////
    //Wirte to file
    std::ofstream out;
    out.open(szFileName, std::ios::out | std::ios::binary);
    if (!out.is_open())
    {
        throw std::exception((std::string("Open file") + std::string(szFileName) + std::string("failed!")).c_str());
    }
    //Write info head
    out.write((char*)pFileHead, 14);
    out.write((char*)pInfoHead, 40);
    out.write((char*)pBits, iWidth*iHeight * 3);

    out.close();
}
