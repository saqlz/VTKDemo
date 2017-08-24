// 这是主 DLL 文件。

#include "stdafx.h"
#include "LoadAndShowDCMDll.h"

WisdomTechLoadAndShowDCM::WisdomTechLoadAndShowDCM()
{
}

WisdomTechLoadAndShowDCM::~WisdomTechLoadAndShowDCM()
{
}

int WisdomTechLoadAndShowDCM::LoadAndShowByPathAndDim(const std::string & sPath)
{
    if (sPath.find("Image") != std::string::npos)
    {
        return 1;
    }
    return 0;
}
