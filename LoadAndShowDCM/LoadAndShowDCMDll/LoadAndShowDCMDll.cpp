// 这是主 DLL 文件。

#include "stdafx.h"
#include "LoadAndShowDCMDll.h"

int LoadAndShowDCM(const std::string& sPath)
{
    WisdomTechLoadAndShowDCM *instance = new WisdomTechLoadAndShowDCM();
    int i = instance->LoadAndShowByPathAndDim(sPath);
    delete instance;
    return i;
}


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

