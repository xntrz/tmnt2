#include "CameraDataMan.hpp"


class CCameraData
{
private:
    struct CMDHEADER
    {
        char m_szIdentity[4];
        int32 m_nType;
        uint32 m_uOffsetPathDataList;
        uint32 m_uOffsetSetCamDataList;
    };

    struct PATHKEY
    {
        float m_fX;
        float m_fY;
        float m_fZ;
        float m_fOffsetX;
        float m_fOffsetY;
        float m_fOffsetZ;
        float m_fValue;
    };

    struct PATHDATA
    {
        char m_szName[32];
        int32 m_nType;
        int32 m_nNumKey;
        //PATHKEY m_aKey[];
    };

    struct SETCAMDATA
    {
        char m_szName[32];
        RwV3d m_vPosEye;
        RwV3d m_vPosLookat;
        float m_fFOV;
        int32 m_nType;
    };

    struct PATHDATALIST
    {
        int32 m_nNumPathData;
        PATHDATA m_aPathData[];
    };

    struct SETCAMDATALIST
    {
        int32 m_nNumSetCamData;
        SETCAMDATA m_aSetCamData[];
    };

public:
    CCameraData(void);
    ~CCameraData(void);
    void Read(const void* pBuffer, uint32 uBufferSize);
    void PostRead(void);
    const CMDHEADER* GetCmdHeader(void) const;
    const PATHDATALIST* GetPathDataList(void) const;
    const PATHDATA* GetPathData(const PATHDATALIST* pPathDataList, int32 nPathID) const;
    const PATHKEY* GetPathKey(const PATHDATA* pPathData, int32 nIndex) const;
    bool GetSpline(RwV3d* pPos, const PATHDATA* pPathData, float fTime);
    bool GetSplinePos(RwV3d* pPos, int32 nPathID, float fTime);
    float FindNearestPosValue(RwV3d* pPos, int32 nPathID, int32 nNumDivPath);
    float FindNearestPosValueLight(RwV3d* pPos, int32 nPathID, float fNowTime);
    int32 GetPathIDFromName(const char* pszName);
    int32 GetPathType(int32 nPathID);    
    const SETCAMDATALIST* GetSetCamDataList(void) const;
    const SETCAMDATA* GetSetCamData(int32 id) const;
    int32 GetSetCamIDNearestPos(RwV3d* pPos, const char* pszName = nullptr);
    int32 GetSetCamIDFromName(const char* pszName);
    float GetSetCamFOV(int32 id) const;
    const RwV3d* GetSetCamPosLookat(int32 id) const;
    const RwV3d* GetSetCamPosEye(int32 id) const;

private:
    void* m_pCmdBuffer;
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


void CCameraData::Read(const void* pBuffer, uint32 uBufferSize)
{
    ASSERT(pBuffer);
    ASSERT(uBufferSize > 0);
    ASSERT(!m_pCmdBuffer);
    
    m_pCmdBuffer = new char[uBufferSize];
    ASSERT(m_pCmdBuffer);
    
    std::memcpy(m_pCmdBuffer, pBuffer, uBufferSize);
};


void CCameraData::PostRead(void)
{
    ;
};


const CCameraData::CMDHEADER* CCameraData::GetCmdHeader(void) const
{
    ASSERT(m_pCmdBuffer);
    
    return ((CMDHEADER*)m_pCmdBuffer);
};


const CCameraData::PATHDATALIST* CCameraData::GetPathDataList(void) const
{
    return ((PATHDATALIST*)((uint8*)m_pCmdBuffer + GetCmdHeader()->m_uOffsetPathDataList));
};


const CCameraData::PATHDATA* CCameraData::GetPathData(const PATHDATALIST* pPathDataList, int32 nPathID) const
{
    ASSERT(pPathDataList);
    ASSERT(nPathID >= 0 && nPathID < pPathDataList->m_nNumPathData);
    
    const PATHDATA* pResult = pPathDataList->m_aPathData;

    for (int32 i = 0; i < nPathID; ++i)
        pResult = (const PATHDATA*)((uint8*)pResult + (sizeof(PATHDATA) + pResult->m_nNumKey * sizeof(PATHKEY)));

    return pResult;
};


const CCameraData::PATHKEY* CCameraData::GetPathKey(const PATHDATA* pPathData, int32 nIndex) const
{
    ASSERT(pPathData);
    ASSERT(nIndex >= 0 && nIndex < pPathData->m_nNumKey);
    
    return (const PATHKEY*)( ((uint8*)pPathData + sizeof(PATHDATA)) + (nIndex * sizeof(PATHKEY)) );
};


bool CCameraData::GetSpline(RwV3d* pPos, const PATHDATA* pPathData, float fTime)
{
    ASSERT(fTime >= 0.0f && fTime <= 1.0f);
    ASSERT(pPos);
    ASSERT(pPathData);

    fTime = Math::Clamp(fTime, 0.0f, 1.0f);

    int32 nKeyIndex = 0;
    int32 nNumKey = pPathData->m_nNumKey - 1;
    
    for (int32 i = 0; i < nNumKey;)
    {
        const PATHKEY* pKey = GetPathKey(pPathData, (i + nNumKey) / 2);

        if (pKey->m_fValue >= fTime)
        {
            nNumKey = (i + nNumKey) / 2;
        }
        else
        {
            i = ((i + nNumKey) / 2) + 1;
            nKeyIndex = i;
        };
    };

    if (nKeyIndex > 0)
        --nKeyIndex;

    const PATHKEY* pKey0 = GetPathKey(pPathData, nKeyIndex);
    const PATHKEY* pKey1 = GetPathKey(pPathData, nKeyIndex + 1);

    float duration = pKey1->m_fValue - pKey0->m_fValue;
    float progress = fTime - pKey0->m_fValue;

    pPos->x = ((pKey1->m_fX - pKey0->m_fX) * (1.0f / duration)
        - (pKey0->m_fOffsetX + pKey0->m_fOffsetX + pKey1->m_fOffsetX) * duration
        + ((pKey1->m_fOffsetX - pKey0->m_fOffsetX) * (1.0f / duration) * progress + pKey0->m_fOffsetX * 3.0f)
        * progress)
        * progress
        + pKey0->m_fX;

    pPos->y = ((pKey1->m_fY - pKey0->m_fY) * (1.0f / duration)
        - (pKey0->m_fOffsetY + pKey0->m_fOffsetY + pKey1->m_fOffsetY) * duration
        + ((pKey1->m_fOffsetY - pKey0->m_fOffsetY) * (1.0f / duration) * progress + pKey0->m_fOffsetY * 3.0f)
        * progress)
        * progress
        + pKey0->m_fY;

    pPos->z = ((pKey1->m_fZ - pKey0->m_fZ) * (1.0f / duration)
        - (pKey0->m_fOffsetZ + pKey0->m_fOffsetZ + pKey1->m_fOffsetZ) * duration
        + ((pKey1->m_fOffsetZ - pKey0->m_fOffsetZ) * (1.0f / duration) * progress + pKey0->m_fOffsetZ * 3.0f)
        * progress)
        * progress
        + pKey0->m_fZ;

    return true;
};


bool CCameraData::GetSplinePos(RwV3d* pPos, int32 nPathID, float fTime)
{
    return GetSpline(
        pPos, 
        GetPathData(GetPathDataList(), nPathID),
        fTime
    );
};


float CCameraData::FindNearestPosValue(RwV3d* pPos, int32 nPathID, int32 nNumDivPath)
{
    const PATHDATALIST* pPathDataList = GetPathDataList();
    const PATHDATA* pPathData = GetPathData(pPathDataList, nPathID);
    float fMinDist = 1000.0f;
    float fNearestTime = 0.0f;

    for (int32 i = 0; i < nNumDivPath; ++i)
    {
        float fDeltaDivPath = 1.0f / nNumDivPath;
        float fValue = float(i) * fDeltaDivPath;
        RwV3d vPathPos = Math::VECTOR3_ZERO;
        RwV3d vTemp = Math::VECTOR3_ZERO;
        
        GetSpline(&vPathPos, pPathData, fValue);
        Math::Vec3_Sub(&vTemp, &vPathPos, pPos);
        
        float fDist = Math::Vec3_Length(&vTemp);
        if (fDist < fMinDist)
        {
            fMinDist = fDist;
            fNearestTime = fValue;
        };
    };

    return fNearestTime;
};


float CCameraData::FindNearestPosValueLight(RwV3d* pPos, int32 nPathID, float fNowTime)
{
    const PATHDATALIST* pPathDataList = GetPathDataList();
    const PATHDATA* pPathData = GetPathData(pPathDataList, nPathID);
    const int32 nDivPath = 200;
    const float fDeltaDivPath = (1.0f / float(nDivPath)) / 100.0f;
    float fMinDist = 1000.0f;
    float fNearestTime = 0.0f;

    if (fNowTime < 1.0f)
    {
        for (int32 i = 0; i < nDivPath; ++i)
        {
            float fValue = (i * fDeltaDivPath) + fNowTime;
            RwV3d vPathPos = Math::VECTOR3_ZERO;
            RwV3d vTemp = Math::VECTOR3_ZERO;

            if (fValue > 1.0f)
                fValue = 1.0f;

            GetSpline(&vPathPos, pPathData, fValue);
            Math::Vec3_Sub(&vTemp, &vPathPos, pPos);

            float fDist = Math::Vec3_Length(&vTemp);
            if (fDist < fMinDist)
            {
                fMinDist = fDist;
                fNearestTime = fValue;
            };
        };
    };    

    if (Math::FEqual(fNowTime, fNearestTime) ||
        Math::FEqual(fNowTime, 1.0f))
    {
        for (int32 i = 0; i < nDivPath; ++i)
        {
            float fValue = (i * -fDeltaDivPath) + fNowTime;
            RwV3d vPathPos = Math::VECTOR3_ZERO;
            RwV3d vTemp = Math::VECTOR3_ZERO;

            if (fValue < 0.0f)
                fValue = 0.0f;

            GetSpline(&vPathPos, pPathData, fValue);
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


int32 CCameraData::GetPathIDFromName(const char* pszName)
{
    const PATHDATALIST* pPathDataList = GetPathDataList();
    
    for (int32 i = 0; i < pPathDataList->m_nNumPathData; ++i)
    {
        if (!std::strcmp(GetPathData(pPathDataList, i)->m_szName, pszName))
            return i;
    };

    return -1;
};


int32 CCameraData::GetPathType(int32 nPathID)
{
    return GetPathData(GetPathDataList(), nPathID)->m_nType;
};


const CCameraData::SETCAMDATALIST* CCameraData::GetSetCamDataList(void) const
{
    ASSERT(m_pCmdBuffer);
    
    return ((SETCAMDATALIST*)((uint8*)m_pCmdBuffer + GetCmdHeader()->m_uOffsetSetCamDataList));
};


const CCameraData::SETCAMDATA* CCameraData::GetSetCamData(int32 id) const
{
    ASSERT(id >= 0 && id < GetSetCamDataList()->m_nNumSetCamData);
    
    return &GetSetCamDataList()->m_aSetCamData[id];
};


int32 CCameraData::GetSetCamIDNearestPos(RwV3d* pPos, const char* pszName)
{
    const SETCAMDATALIST* pSetCamDataList = GetSetCamDataList();
    float fMinDist = 1000.0f;
    int32 nMinSetCamID = -1;

    for (int32 i = 0; i < pSetCamDataList->m_nNumSetCamData; ++i)
    {
        const SETCAMDATA* pSetCamData = GetSetCamData(i);

        if (!pszName || !std::strcmp(pSetCamData->m_szName, pszName))
        {
            RwV3d vTemp = Math::VECTOR3_ZERO;
            Math::Vec3_Sub(&vTemp, pPos, &pSetCamData->m_vPosLookat);
            
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


int32 CCameraData::GetSetCamIDFromName(const char* pszName)
{
    const SETCAMDATALIST* pSetCamDataList = GetSetCamDataList();

    for (int32 i = 0; i < pSetCamDataList->m_nNumSetCamData; ++i)
    {
        if (!std::strcmp(GetSetCamData(i)->m_szName, pszName))
            return i;
    };

    return -1;
};


float CCameraData::GetSetCamFOV(int32 id) const
{
    return GetSetCamData(id)->m_fFOV;
};


const RwV3d* CCameraData::GetSetCamPosLookat(int32 id) const
{
    return &GetSetCamData(id)->m_vPosLookat;
};


const RwV3d* CCameraData::GetSetCamPosEye(int32 id) const
{
    return &GetSetCamData(id)->m_vPosEye;
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
    {
        s_pCameraData = new CCameraData;
    };
};


/*static*/ void CCameraDataManager::Terminate(void)
{
    if (s_pCameraData)
    {
        delete s_pCameraData;
        s_pCameraData = nullptr;
    };
};


/*static*/ void CCameraDataManager::Read(const void* pBuffer, uint32 uBufferSize)
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