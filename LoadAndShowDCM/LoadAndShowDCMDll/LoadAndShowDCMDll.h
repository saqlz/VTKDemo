#ifndef WISDOM_TECH_LOAD_SHOW_DCM_H
#define WISDOM_TECH_LOAD_SHOW_DCM_H

#ifndef WISDOMTECHDLLDEFINE
#define WISDOMTECHDLLDEFINE __declspec(dllexport)
#else
#define WISDOMTECHDLLDEFINE __declspec(dllimport)
#endif

extern "C"
{
    WISDOMTECHDLLDEFINE int LoadAndShowDCM(const std::string& sPath, const int& iWidth, const int& iHeight);
};


class WISDOMTECHDLLDEFINE WisdomTechLoadAndShowDCM
{
public:
    WisdomTechLoadAndShowDCM();
    virtual ~WisdomTechLoadAndShowDCM();
public:
    int LoadAndShowByPathAndDim(const std::string& sPath, const int& iWidth, const int& iHeight);
};

#endif
