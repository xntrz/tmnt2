#pragma once

#include "MenuController.hpp"
#include "MenuSound.hpp"


class CAnimation2D;


class CAnimation2DLoader
{
public:
    static const int32 NAMEMAX = 16;
    static const int32 FILEMAX = 3;

    struct LoadAnimationData
    {
        CAnimation2D* m_pAnimation2D;
        char m_szName[NAMEMAX];
    };
    
public:
    static void Open(const char* pszName, void* pBuffer, uint32 uBufferSize);
    static void Close(const char* pszName);
    static CAnimation2D* Get(const char* pszName);
    static CAnimation2D* Get(int32 index);

private:
    static LoadAnimationData m_aList[FILEMAX];
    static int32 m_iFileNum;
};


class CAnimation2D
{
private:
    class CCenterStringObject
    {
    public:
        static Rt2dObject* SetCenterAllStringObjectCallback(Rt2dObject* object, Rt2dObject* parent, void* data);
        void SetCenterAllStringObject(Rt2dMaestro* pMaestro);
    };
    
public:
    static bool Initialize(void);
    static void Terminate(void);
    static CAnimation2D* Open(void* pBuffer, uint32 uBufferSize);
    static void FlashKeyLock(uint32 uKey);
    static void FlashKeyUnlock(uint32 uKey);
    static void ConvertButtonSymbol(Rt2dObject* pString);
    
    CAnimation2D(void);
    virtual ~CAnimation2D(void);
    void ReadBuffer(void* pBuffer, uint32 uBufferSize);
    void Start(void);
    void Stop(void);
    void Update(void);
    void Draw(void) const;
    void Input(void);
    void SetHandler(void);
    void ResetHandler(void);
    void CheckStringObject(void);
    bool CheckMessageGetURL(const char* pszName) const;
    bool SetText(const char* pszOrgString, const char* pszNewString);
    bool SetStringObjectText(Rt2dObject* pScene, const char* pszOrgString, const char* pszNewString);
    RwTexture* GetTextureAddr(const char* pszFilename);
    void SetTexture(const char* pszOrgTexture, const char* pszNewTexture);
    void SubstituteTexture(Rt2dObject* pScene, RwTexture* pOrgTexture, RwTexture* pNewTexture);
    void SetInterpolateAll(bool bSet);
    void FlashKeyPress(uint32 uKey);
    void FlashUnlockKeyEnable(bool bEnable);
    void KeyInfoChange(void);
    void KeyInfoChange_Sub(Rt2dObject* pScene);
    void SetCenterAllStrings(void);
    
private:
    static Rt2dMessage* MessageHandlerCallback(Rt2dMaestro* maestro, Rt2dMessage* message);
    static Rt2dObject* AllStringObjectCallback(Rt2dObject* object, Rt2dObject* parent, void* data);
    static Rt2dMaestro* AllAnimSetInterpolateCallback(Rt2dMaestro* maestro, Rt2dAnim* anim, Rt2dAnimProps* props, void* pData);

private:
    static CAnimation2D* m_pAnimation2D;
    static uint32 m_uAnimation2DRef;
    int32 m_iMessageIndex;
    Rt2dMaestro* m_pMaestro;
    CMenuSound m_MenuSound;
    CMenuController m_MenuController;
    CCenterStringObject m_CenterStringObj;
};