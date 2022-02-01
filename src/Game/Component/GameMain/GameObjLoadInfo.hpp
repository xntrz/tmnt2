#pragma once


class CGameObjLoadInfo
{
public:
    CGameObjLoadInfo(void);
    virtual ~CGameObjLoadInfo(void);
    virtual int32 GetFileID(void) const;
    virtual bool MakeFileName(char* pszFilenameBuff) const = 0;
    int32 GetDependObjInfoNum(void) const;
    CGameObjLoadInfo* GetDependObjInfo(int32 nIndex);
    const char* GetFileName(void) const;
    
private:
    void initDependObjInfo(void);
    void freeDependObjInfo(void);
    
private:
    int32 m_nDependObjLoadNum;
    CGameObjLoadInfo* m_apDependObjInfo[8];
};