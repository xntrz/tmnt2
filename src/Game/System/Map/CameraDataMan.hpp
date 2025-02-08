#pragma once


class CCameraDataManager
{
public:
    static void Initialize(void);
    static void Terminate(void);
    static void Read(void* pBuffer, uint32 uBufferSize);
    static bool GetSplinePos(RwV3d* pPos, int32 nPathID, float fTime);
    static int32 GetPathIDFromName(const char* pszName);
    static int32 GetPathType(int32 nPathID);
    static float FindNearestPosValue(RwV3d* pPos, int32 nPathID, int32 nNumDivPath);
    static float FindNearestPosValueLight(RwV3d* pPos, int32 nPathID, float fNowTime);
    static int32 GetSetCamIDFromName(const char* pszName);
    static int32 GetSetCamIDNearestPos(RwV3d* pPos);
    static int32 GetSetCamIDNearestPosFromName(RwV3d* pPos, const char* pszName);
    static const RwV3d* GetSetCamPosEye(int32 id);
    static const RwV3d* GetSetCamPosLookat(int32 id);
    static float GetSetCamFov(int32 id);
};