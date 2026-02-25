#include "CameraDataMan.hpp"


class CCameraData
{
private:
    struct CMDHEADER
    {
        char    szIdentity[4];
        int32   type;
        uint32  ofsPathDataList;
        uint32  ofsSetCamDataList;
    };

    CHECK_SIZE(CMDHEADER, 0x10);

    struct PATHKEY
    {
        float x;
        float y;
        float z;
        float ofsX;
        float ofsY;
        float ofsZ;
        float value;
    };

    CHECK_SIZE(PATHKEY, 0x1C);

    struct PATHDATA
    {
        char    szName[32];
        int32   type;
        int32   numPathKey;
        PATHKEY aPathKey[1];
    };

    CHECK_SIZE(PATHDATA, 0x44);

    struct SETCAMDATA
    {
        char  szName[32];
        RwV3d posEye;
        RwV3d posLookat;
        float fov;
        int32 type;
    };

    CHECK_SIZE(SETCAMDATA, 0x40);

    struct PATHDATALIST
    {
        int32    numPathData;
        PATHDATA aPathData[1];
    };

    CHECK_SIZE(PATHDATALIST, 0x48);

    struct SETCAMDATALIST
    {
        int32      numSetCamData;
        SETCAMDATA aSetCamData[1];
    };

    CHECK_SIZE(SETCAMDATALIST, 0x44);

public:
    CCameraData(void);
    ~CCameraData(void);
    void Read(void* pBuffer, uint32 uBufferSize);
    void PostRead(void);
    const CMDHEADER* GetCmdHeader(void) const;
    const PATHDATALIST* GetPathDataList(void) const;
    const PATHDATA* GetPathData(const PATHDATALIST* pPathDataList, int32 nPathID) const;
    const PATHKEY* GetPathKey(const PATHDATA* pPathData, int32 nIndex) const;
    bool GetSpline(RwV3d* pPos, const PATHDATA* pPathData, float fTime) const;
    bool GetSplinePos(RwV3d* pPos, int32 nPathID, float fTime) const;
    float FindNearestPosValue(RwV3d* pPos, int32 nPathID, int32 nNumDivPath) const;
    float FindNearestPosValueLight(RwV3d* pPos, int32 nPathID, float fNowTime) const;
    int32 GetPathIDFromName(const char* pszName) const;
    int32 GetPathType(int32 nPathID) const;
    const SETCAMDATALIST* GetSetCamDataList(void) const;
    const SETCAMDATA* GetSetCamData(int32 id) const;
    int32 GetSetCamIDNearestPos(RwV3d* pPos, const char* pszName = nullptr) const;
    int32 GetSetCamIDFromName(const char* pszName) const;
    float GetSetCamFOV(int32 id) const;
    const RwV3d* GetSetCamPosLookat(int32 id) const;
    const RwV3d* GetSetCamPosEye(int32 id) const;

private:
    char* m_pCmdBuffer;
};


CCameraData::CCameraData(void)
: m_pCmdBuffer(nullptr)
{
    ;
};


CCameraData::~CCameraData(void)
{
    if (m_pCmdBuffer)
    {
        delete[] m_pCmdBuffer;
        m_pCmdBuffer = nullptr;
    };
};


void CCameraData::Read(void* pBuffer, uint32 uBufferSize)
{
    ASSERT(pBuffer);
    ASSERT(uBufferSize > 0);
    ASSERT(m_pCmdBuffer == nullptr);
    
    m_pCmdBuffer = new char[uBufferSize];
    std::memcpy(m_pCmdBuffer, pBuffer, uBufferSize);

    PostRead();
};


void CCameraData::PostRead(void)
{
    ;
};


const CCameraData::CMDHEADER* CCameraData::GetCmdHeader(void) const
{
    ASSERT(m_pCmdBuffer);

    return reinterpret_cast<const CMDHEADER*>(m_pCmdBuffer);
};


const CCameraData::PATHDATALIST* CCameraData::GetPathDataList(void) const
{
    const CCameraData::CMDHEADER* pCmdHeader = GetCmdHeader();

    return reinterpret_cast<const PATHDATALIST*>(m_pCmdBuffer + pCmdHeader->ofsPathDataList);
};


const CCameraData::PATHDATA* CCameraData::GetPathData(const PATHDATALIST* pPathDataList, int32 nPathID) const
{
    ASSERT(pPathDataList);
    ASSERT(nPathID >= 0);
    ASSERT(nPathID < pPathDataList->numPathData);

    const PATHDATA* pResult = pPathDataList->aPathData;

    for (int32 i = 0; i < nPathID; ++i)
        pResult = reinterpret_cast<const PATHDATA*>(pResult->aPathKey + pResult->numPathKey);

    return pResult;
};


const CCameraData::PATHKEY* CCameraData::GetPathKey(const PATHDATA* pPathData, int32 nIndex) const
{
    ASSERT(pPathData);
    ASSERT(nIndex >= 0);
    ASSERT(nIndex < pPathData->numPathKey);

    return &pPathData->aPathKey[nIndex];
};


bool CCameraData::GetSpline(RwV3d* pPos, const PATHDATA* pPathData, float fTime) const
{
    ASSERT(fTime >= 0.0f);
    ASSERT(fTime <= 1.0f);
    ASSERT(pPos);
    ASSERT(pPathData);

    fTime = Clamp(fTime, 0.0f, 1.0f);
    
    int32 i = 0;
    int32 j = pPathData->numPathKey - 1;

    while (i < j)
    {
        int32 d = i + j;
        int32 k = d / 2;

        if (d < 0)
            k = (d + 1) / 2;

        const PATHKEY key = *GetPathKey(pPathData, k);

        if (key.value >= fTime)
            j = k;
        else
            i = k + 1;        
    };

    if (i > 0)
        --i;

    const PATHKEY key0 = *GetPathKey(pPathData, i);
    const PATHKEY key1 = *GetPathKey(pPathData, i + 1);

    float d = key1.value - key0.value;
    float h = fTime - key0.value;

    pPos->x = ((key1.x - key0.x) * (1.0f / d)
        - (key0.ofsX + key0.ofsX + key1.ofsX) * d
        + ((key1.ofsX - key0.ofsX) * (1.0f / d) * h + key0.ofsX * 3.0f) * h)
        * h
        + key0.x;
    
    pPos->y = ((key1.y - key0.y) * (1.0f / d)
        - (key0.ofsY + key0.ofsY + key1.ofsY) * d
        + ((key1.ofsY - key0.ofsY) * (1.0f / d) * h + key0.ofsY * 3.0f) * h)
        * h
        + key0.y;
    
    pPos->z = ((key1.z - key0.z) * (1.0f / d)
        - (key0.ofsZ + key0.ofsZ + key1.ofsZ) * d
        + ((key1.ofsZ - key0.ofsZ) * (1.0f / d) * h + key0.ofsZ * 3.0f) * h)
        * h
        + key0.z;

    return true;
};


bool CCameraData::GetSplinePos(RwV3d* pPos, int32 nPathID, float fTime) const
{
    const PATHDATALIST* pPathDataList = GetPathDataList();
    const PATHDATA* pPathData = GetPathData(pPathDataList, nPathID);

    return GetSpline(pPos, pPathData, fTime);
};


float CCameraData::FindNearestPosValue(RwV3d* pPos, int32 nPathID, int32 nNumDivPath) const
{
    const PATHDATALIST* pPathDataList = GetPathDataList();
    const PATHDATA* pPathData = GetPathData(pPathDataList, nPathID);

    float fMinDist = 1000.0f;
    float fNearestTime = 0.0f;

    for (int32 i = 0; i < nNumDivPath; ++i)
    {
        float fDeltaDivPath = (1.0f / nNumDivPath);
        float fPathT = (static_cast<float>(i) * fDeltaDivPath);
        
        RwV3d vPathPos = Math::VECTOR3_ZERO;
        GetSpline(&vPathPos, pPathData, fPathT);

        RwV3d vDltPos = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vDltPos, &vPathPos, pPos);
        
        float fDist = Math::Vec3_Length(&vDltPos);
        if (fDist < fMinDist)
        {
            fMinDist = fDist;
            fNearestTime = fPathT;
        };
    };

    return fNearestTime;
};


float CCameraData::FindNearestPosValueLight(RwV3d* pPos, int32 nPathID, float fNowTime) const
{
    const PATHDATALIST* pPathDataList = GetPathDataList();
    const PATHDATA* pPathData = GetPathData(pPathDataList, nPathID);

    const int32 nDivPath = 200;
    const float fDeltaDivPath = (1.0f / static_cast<float>(nDivPath)) / 100.0f;

    float fMinDist = 1000.0f;
    float fNearestTime = 0.0f;

    if (fNowTime < 1.0f)
    {
        for (int32 i = 0; i < nDivPath; ++i)
        {
            float fValue = (static_cast<float>(i) * fDeltaDivPath) + fNowTime;

            if (fValue > 1.0f)
                fValue = 1.0f;

            RwV3d vPathPos = Math::VECTOR3_ZERO;
            GetSpline(&vPathPos, pPathData, fValue);

            RwV3d vTemp = Math::VECTOR3_ZERO;
            Math::Vec3_Sub(&vTemp, &vPathPos, pPos);

            float fDist = Math::Vec3_Length(&vTemp);
            if (fDist < fMinDist)
            {
                fMinDist = fDist;
                fNearestTime = fValue;
            };
        };
    };    

    if ((fNowTime == fNearestTime) || (fNowTime == 1.0f))
    {
        for (int32 i = 0; i < nDivPath; ++i)
        {
            float fValue = (static_cast<float>(i) * -fDeltaDivPath) + fNowTime;

            if (fValue < 0.0f)
                fValue = 0.0f;

            RwV3d vPathPos = Math::VECTOR3_ZERO;
            GetSpline(&vPathPos, pPathData, fValue);

            RwV3d vTemp = Math::VECTOR3_ZERO;
            Math::Vec3_Sub(&vTemp, &vPathPos, pPos);

            float fDist = Math::Vec3_Length(&vTemp);
            if (fDist < fMinDist)
            {
                fMinDist = fDist;
                fNearestTime = fValue;
            };
        };
    };
    
    return fNearestTime;
};


int32 CCameraData::GetPathIDFromName(const char* pszName) const
{
    const PATHDATALIST* pPathDataList = GetPathDataList();

    for (int32 i = 0; i < pPathDataList->numPathData; ++i)
    {
        const PATHDATA* pPathData = GetPathData(pPathDataList, i);

        if (!std::strcmp(pPathData->szName, pszName))
            return i;
    };

    return -1;
};


int32 CCameraData::GetPathType(int32 nPathID) const
{
    const PATHDATALIST* pPathDataList = GetPathDataList();
    const PATHDATA* pPathData = GetPathData(pPathDataList, nPathID);

    return pPathData->type;
};


const CCameraData::SETCAMDATALIST* CCameraData::GetSetCamDataList(void) const
{
    const CMDHEADER* pCmdHeader = GetCmdHeader();

    return reinterpret_cast<const SETCAMDATALIST*>(m_pCmdBuffer + pCmdHeader->ofsSetCamDataList);
};


const CCameraData::SETCAMDATA* CCameraData::GetSetCamData(int32 id) const
{
    const SETCAMDATALIST* pSetCamDataList = GetSetCamDataList();

    ASSERT(id >= 0);
    ASSERT(id < pSetCamDataList->numSetCamData);

    return &pSetCamDataList->aSetCamData[id];
};


int32 CCameraData::GetSetCamIDNearestPos(RwV3d* pPos, const char* pszName /*= nullptr*/) const
{
    const SETCAMDATALIST* pSetCamDataList = GetSetCamDataList();

    float fMinDist = 1000.0f;
    int32 nMinSetCamID = -1;

    size_t nameLen = std::strlen(pszName);

    for (int32 i = 0; i < pSetCamDataList->numSetCamData; ++i)
    {
        const SETCAMDATA* pSetCamData = GetSetCamData(i);

        if (!pszName || !std::strncmp(pSetCamData->szName, pszName, nameLen))
        {
            RwV3d vTemp = Math::VECTOR3_ZERO;
            Math::Vec3_Sub(&vTemp, pPos, &pSetCamData->posLookat);
            
            float fDist = Math::Vec3_Length(&vTemp);
            if (fDist < fMinDist)
            {
                fMinDist = fDist;
                nMinSetCamID = i;
            };
        };
    };

    return nMinSetCamID;
};


int32 CCameraData::GetSetCamIDFromName(const char* pszName) const
{
    const SETCAMDATALIST* pSetCamDataList = GetSetCamDataList();

    for (int32 i = 0; i < pSetCamDataList->numSetCamData; ++i)
    {
        const SETCAMDATA* pSetCamData = GetSetCamData(i);

        if (!std::strcmp(pSetCamData->szName, pszName))
            return i;
    };

    return -1;
};


float CCameraData::GetSetCamFOV(int32 id) const
{
    const SETCAMDATA* pSetCamData = GetSetCamData(id);

    return pSetCamData->fov;
};


const RwV3d* CCameraData::GetSetCamPosLookat(int32 id) const
{
    const SETCAMDATA* pSetCamData = GetSetCamData(id);

    return &pSetCamData->posLookat;
};


const RwV3d* CCameraData::GetSetCamPosEye(int32 id) const
{
    const SETCAMDATA* pSetCamData = GetSetCamData(id);

    return &pSetCamData->posEye;
};


static CCameraData* s_pCameraData = nullptr;


static inline CCameraData& CameraData(void)
{
    ASSERT(s_pCameraData);
    return *s_pCameraData;
};


/*static*/ void CCameraDataManager::Initialize(void)
{
    if (!s_pCameraData)
        s_pCameraData = new CCameraData;
};


/*static*/ void CCameraDataManager::Terminate(void)
{
    if (s_pCameraData)
    {
        delete s_pCameraData;
        s_pCameraData = nullptr;
    };
};


/*static*/ void CCameraDataManager::Read(void* pBuffer, uint32 uBufferSize)
{
    CameraData().Read(pBuffer, uBufferSize);
};


/*static*/ bool CCameraDataManager::GetSplinePos(RwV3d* pPos, int32 nPathID, float fTime)
{
    return CameraData().GetSplinePos(pPos, nPathID, fTime);
};


/*static*/ int32 CCameraDataManager::GetPathIDFromName(const char* pszName)
{
    return CameraData().GetPathIDFromName(pszName);
};


/*static*/ int32 CCameraDataManager::GetPathType(int32 nPathID)
{
    return CameraData().GetPathType(nPathID);
};


/*static*/ float CCameraDataManager::FindNearestPosValue(RwV3d* pPos, int32 nPathID, int32 nNumDivPath)
{
    return CameraData().FindNearestPosValue(pPos, nPathID, nNumDivPath);
};


/*static*/ float CCameraDataManager::FindNearestPosValueLight(RwV3d* pPos, int32 nPathID, float fNowTime)
{
    return CameraData().FindNearestPosValueLight(pPos, nPathID, fNowTime);
};


/*static*/ int32 CCameraDataManager::GetSetCamIDFromName(const char* pszName)
{
    return CameraData().GetSetCamIDFromName(pszName);
};


/*static*/ int32 CCameraDataManager::GetSetCamIDNearestPos(RwV3d* pPos)
{
    return CameraData().GetSetCamIDNearestPos(pPos);
};


/*static*/ int32 CCameraDataManager::GetSetCamIDNearestPosFromName(RwV3d* pPos, const char* pszName)
{
    return CameraData().GetSetCamIDNearestPos(pPos, pszName);
};


/*static*/ const RwV3d* CCameraDataManager::GetSetCamPosEye(int32 id)
{
    return CameraData().GetSetCamPosEye(id);
};


/*static*/ const RwV3d* CCameraDataManager::GetSetCamPosLookat(int32 id)
{
    return CameraData().GetSetCamPosLookat(id);
};


/*static*/ float CCameraDataManager::GetSetCamFov(int32 id)
{
    return CameraData().GetSetCamFOV(id);
};