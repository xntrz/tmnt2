#include "Animation2D.hpp"
#include "GameFont.hpp"

#include "Game/System/Texture/TextureManager.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/Sprite.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/TextData.hpp"


/*static*/ CAnimation2DLoader::LoadAnimationData CAnimation2DLoader::m_aList[FILEMAX] =
{
    { nullptr, "" },
    { nullptr, "" },
    { nullptr, "" },
};


/*static*/ int32 CAnimation2DLoader::m_iFileNum = 0;


/*static*/ void CAnimation2DLoader::Open(const char* pszName, void* pBuffer, uint32 uBufferSize)
{
    ASSERT(strlen(pszName) < NAMEMAX);
    ASSERT(m_iFileNum < FILEMAX);

    Rt2dFontSetPath("Common/Fonts/");
    
    CAnimation2D* pAnimation2D = CAnimation2D::Open(pBuffer, uBufferSize);
    ASSERT(pAnimation2D);

    for (int32 i = 0; i < COUNT_OF(m_aList); ++i)
    {
        if (!m_aList[i].m_pAnimation2D)
        {
            m_aList[i].m_pAnimation2D = pAnimation2D;
            std::strcpy(m_aList[i].m_szName, pszName);

            ++m_iFileNum;

            return;
        };
    };
    
    ASSERT(false);
};


/*static*/ void CAnimation2DLoader::Close(const char* pszName)
{    
    ASSERT(m_iFileNum > 0);
    ASSERT(m_iFileNum < FILEMAX);

    for (int32 i = 0; i < COUNT_OF(m_aList); ++i)
    {
        if (!std::strcmp(m_aList[i].m_szName, pszName))
        {
            ASSERT(m_aList[i].m_pAnimation2D);
            
            delete m_aList[i].m_pAnimation2D;
            m_aList[i].m_pAnimation2D = nullptr;
            m_aList[i].m_szName[0] = '\0';

            --m_iFileNum;

            return;
        };
    };

    ASSERT(false);
};


/*static*/ CAnimation2D* CAnimation2DLoader::Get(const char* pszName)
{
    ASSERT(m_iFileNum > 0);
    ASSERT(m_iFileNum < FILEMAX);

    for (int32 i = 0; i < m_iFileNum; ++i)
    {
        ASSERT(m_aList[i].m_pAnimation2D);
        
        if (!std::strcmp(m_aList[i].m_szName, pszName))
            return m_aList[i].m_pAnimation2D;
    };

    return nullptr;
};


/*static*/ CAnimation2D* CAnimation2DLoader::Get(int32 index)
{
    ASSERT(index >= 0);
    ASSERT(index < FILEMAX);

    ASSERT(index >= 0);
    ASSERT(index < m_iFileNum);

    return m_aList[index].m_pAnimation2D;
};


//
// *********************************************************************************
//


/*static*/ Rt2dObject*
CAnimation2D::CCenterStringObject::SetCenterAllStringObjectCallback(Rt2dObject* object,
                                                                    Rt2dObject* parent,
                                                                    void* data)
{
    ASSERT(data);
    
    if (Rt2dObjectIsScene(object))
    {
        RwV3d vPos = Rt2dObjectGetMTM(object)->pos;
        vPos.x += static_cast<RwV3d*>(data)->x;
        vPos.y += static_cast<RwV3d*>(data)->y;
        vPos.z += static_cast<RwV3d*>(data)->z;

        Rt2dSceneForAllChildren(object, SetCenterAllStringObjectCallback, &vPos);
    }
    else if (Rt2dObjectIsObjectString(object))
    {
        float fHeight = Rt2dObjectStringGetHeight(object);
        char* pszText = Rt2dObjectStringGetText(object);
        float fWidth = Rt2dFontGetStringWidth(CGameFont::GetFontObj(), pszText, fHeight);

        RwMatrix* pMatrix = Rt2dObjectGetMTM(object);
        RwV3d vTranslation = pMatrix->pos;
        
        vTranslation.x = fWidth * -0.5f - static_cast<RwV3d*>(data)->x;
        
        RwMatrixTranslate(pMatrix, &vTranslation, rwCOMBINEREPLACE);
    };

    return object;
};


void CAnimation2D::CCenterStringObject::SetCenterAllStringObject(Rt2dMaestro* pMaestro)
{
    ASSERT(pMaestro);
    
    Rt2dObject* pScene = Rt2dMaestroGetScene(pMaestro);
    ASSERT(pScene);
    
    RwV3d vPos = Rt2dObjectGetMTM(pScene)->pos;    
    Rt2dSceneForAllChildren(pScene, SetCenterAllStringObjectCallback, &vPos);
};


//
// *********************************************************************************
//


/*static*/ CAnimation2D* CAnimation2D::m_pAnimation2D = nullptr;
/*static*/ uint32 CAnimation2D::m_uAnimation2DRef = 0;


/*static*/ bool CAnimation2D::Initialize(void)
{
    if (!m_uAnimation2DRef++)
    {
        if (!CSystem2D::Initialize())
            return false;

        if (!CMenuController::Initialize())
            return false;

        Rt2dAnimOpen();        
    };

    return (m_uAnimation2DRef > 0);
};


/*static*/ void CAnimation2D::Terminate(void)
{
    ASSERT(m_uAnimation2DRef > 0);
    if (!--m_uAnimation2DRef)
    {
        Rt2dAnimClose();
        CMenuController::Terminate();
        CSystem2D::Terminate();
    };    
};


/*static*/ CAnimation2D* CAnimation2D::Open(void* pBuffer, uint32 uBufferSize)
{
    ASSERT(pBuffer);
    ASSERT(uBufferSize);

    CAnimation2D* pAnimation2D = new CAnimation2D;
    pAnimation2D->ReadBuffer(pBuffer, uBufferSize);

    return pAnimation2D;
};


/*static*/ void CAnimation2D::FlashKeyLock(uint32 uKey)
{
    CMenuController::KeyLock(uKey);
};


/*static*/ void CAnimation2D::FlashKeyUnlock(uint32 uKey)
{
    CMenuController::KeyUnlock(uKey);
};


/*static*/ void CAnimation2D::ConvertButtonSymbol(Rt2dObject* pString)
{
    ASSERT(pString);
	
    char* pszName = Rt2dObjectStringGetFont(pString);
    ASSERT(pszName);

    Rt2dFont* pFont = Rt2dFontGetFont(pszName);
    ASSERT(pFont);
};


CAnimation2D::CAnimation2D(void)
: m_iMessageIndex(-1)
, m_pMaestro(nullptr)
{
	;
};


CAnimation2D::~CAnimation2D(void)
{
	;
};


void CAnimation2D::ReadBuffer(void* pBuffer, uint32 uBufferSize)
{
    RwMemory memStream;
    memStream.start  = static_cast<RwUInt8*>(pBuffer);
    memStream.length = uBufferSize;

    RwStream* pRwStream = RwStreamOpen(rwSTREAMMEMORY, rwSTREAMREAD, &memStream);
    ASSERT(pRwStream);

    if (pRwStream)
    {
        if (RwStreamFindChunk(pRwStream, rwID_2DMAESTRO, nullptr, nullptr))
        {
            m_pMaestro = Rt2dMaestroStreamRead(nullptr, pRwStream);
            ASSERT(m_pMaestro);
        };

        RwStreamClose(pRwStream, &memStream);
    };
};


void CAnimation2D::Start(void)
{
	ASSERT(m_pMaestro);

	RwV2d xstep = Math::VECTOR2_ZERO;
	RwV2d ystep = Math::VECTOR2_ZERO;
	RwV2d origin = Math::VECTOR2_ZERO;
	Rt2dDeviceGetStep(&xstep, &ystep, &origin);
    
    Rt2dBBox* pBBox = Rt2dMaestroGetBBox(m_pMaestro);
    ASSERT(pBBox);

    RwV2d scale = Math::VECTOR2_ZERO;
    scale.x = ((xstep.y + xstep.x) * static_cast<float>(CScreen::Width()))  / pBBox->w;
    scale.y = ((ystep.y + ystep.x) * static_cast<float>(CScreen::Height())) / pBBox->h;

    RwV2d translation = Math::VECTOR2_ZERO;
	translation.x =  ((CSprite::m_fVirtualScreenW - pBBox->w) * 0.5f) + origin.x;
	translation.y = (((CSprite::m_fVirtualScreenH - pBBox->h) * 0.5f) + pBBox->h) + origin.y;
	
    Rt2dObject* pScene = Rt2dMaestroGetScene(m_pMaestro);
    ASSERT(pScene);

    Rt2dObjectMTMScale(pScene, scale.x, scale.y);
	Rt2dObjectMTMTranslate(pScene, translation.x, translation.y);

	SetHandler();
	SetInterpolateAll(true);
	m_menuController.CheckButtonLabelList(m_pMaestro);
	m_menuSound.CheckSoundLabelList(m_pMaestro);
	CheckStringObject();

    Rt2dSceneUpdateLTM(pScene);
	Rt2dCTMSetIdentity();

	m_iMessageIndex = -1;

	Rt2dMaestroAddDeltaTime(m_pMaestro, 0.0f);
	Rt2dMaestroUpdateAnimations(m_pMaestro);
	Rt2dMaestroProcessMessages(m_pMaestro);
};


void CAnimation2D::Stop(void)
{
    ASSERT(m_pMaestro);

    Rt2dMaestro* pMaestro = Rt2dMaestroSetMessageHandler(m_pMaestro, Rt2dMessageHandlerDefaultCallBack);
    ASSERT(pMaestro);

    Rt2dMaestroDestroy(m_pMaestro);
    m_pMaestro = nullptr;
};


void CAnimation2D::Update(void)
{
    ASSERT(m_pMaestro);
    
    m_pAnimation2D = this;
    m_iMessageIndex = -1;

	Rt2dMaestroAddDeltaTime(m_pMaestro, CScreen::TimerStride());
    Rt2dMaestroUpdateAnimations(m_pMaestro);
    Rt2dMaestroProcessMessages(m_pMaestro);

    m_pAnimation2D = nullptr;
};


void CAnimation2D::Draw(void) const
{
    ASSERT(m_pMaestro);

    Rt2dMaestroRender(m_pMaestro);
};


void CAnimation2D::Input(void)
{
    ASSERT(m_pMaestro);
    
    m_menuController.Trigger(m_pMaestro);
};


void CAnimation2D::SetHandler(void)
{
    ASSERT(m_pMaestro);
    
    Rt2dMaestroSetMessageHandler(m_pMaestro, &CAnimation2D::MessageHandlerCallback);
};


void CAnimation2D::ResetHandler(void)
{
    ASSERT(m_pMaestro);

    Rt2dMaestro* pMaestro = Rt2dMaestroSetMessageHandler(m_pMaestro, Rt2dMessageHandlerDefaultCallBack);
    ASSERT(pMaestro);
    ASSERT(pMaestro == m_pMaestro);
};


void CAnimation2D::CheckStringObject(void)
{
    ASSERT(m_pMaestro);

    Rt2dObject* pScene = Rt2dMaestroGetScene(m_pMaestro);
    ASSERT(pScene);

    int32 iCounter = 0;
    Rt2dSceneForAllChildren(pScene, &CAnimation2D::AllStringObjectCallback, &iCounter);
};


bool CAnimation2D::CheckMessageGetURL(const char* pszName) const
{
    ASSERT(pszName);
    ASSERT(m_pMaestro);

    if (m_iMessageIndex == -1)
        return false;

    int32 index = -1;
    if (!Rt2dMaestroFindStringLabel(m_pMaestro, rt2dANIMLABELTYPEURL, pszName, &index))
        return false;

    return (index == m_iMessageIndex);
};


bool CAnimation2D::SetText(const char* pszOrgString, const char* pszNewString)
{
    ASSERT(m_pMaestro);

    Rt2dObject* pScene = Rt2dMaestroGetScene(m_pMaestro);
    ASSERT(pScene);

#ifdef TMNT2_BUILD_EU
    wchar wszOrgStr[512];
    wszOrgStr[0] = UTEXT('\0');
    CGameFont::ConvertToUnicode(wszOrgStr, pszOrgString);

    wchar wszNewStr[512];
    wszNewStr[0] = UTEXT('\0');
    CGameFont::ConvertToUnicode(wszNewStr, pszNewString);

    return SetStringObjectText(pScene, wszOrgStr, wszNewStr);
#else /* TMNT2_BUILD_EU */
    return SetStringObjectText(pScene, pszOrgString, pszNewString);
#endif /* TMNT2_BUILD_EU */
};


bool CAnimation2D::SetText(const char* pszOrgString, const wchar* pwszNewString)
{
    ASSERT(m_pMaestro);

    Rt2dObject* pScene = Rt2dMaestroGetScene(m_pMaestro);
    ASSERT(pScene);

#if !defined(TMNT2_BUILD_EU)
    char szNewString[512];
    szNewString[0] = '\0';
    CGameFont::ConvertToMultibyte(szNewString, pwszNewString);

    return SetStringObjectText(pScene, pszOrgString, szNewString);
#else /* !defined(TMNT2_BUILD_EU) */    
    wchar wszOrgStr[512];
    wszOrgStr[0] = UTEXT('\0');
    CGameFont::ConvertToUnicode(wszOrgStr, pszOrgString);

    return SetStringObjectText(pScene, wszOrgStr, pwszNewString);
#endif /* !defined(TMNT2_BUILD_EU) */
};


bool CAnimation2D::SetStringObjectText(Rt2dObject* pScene, const char* pszOrgString, const char* pszNewString)
{
    ASSERT(pScene);
    ASSERT(pszOrgString);
    ASSERT(pszNewString);

    bool bResult = false;

    int32 childCount = Rt2dSceneGetChildCount(pScene);
    for (int32 i = 0; i < childCount; ++i)
    {
        Rt2dObject* pObject = Rt2dSceneGetChildByIndex(pScene, i);
        ASSERT(pObject);

        int32 type = Rt2dObjectGetObjectType(pObject);
        switch (type)
        {
        case rt2DOBJECTTYPEOBJECTSTRING:
            {
                const char* pszString = Rt2dObjectStringGetText(pObject);
                ASSERT(pszString);

                if (!std::strcmp(pszString, pszOrgString))
                {
                    Rt2dObjectStringSetText(pObject, pszNewString);
                    bResult = true;
                };
            }            
            break;
            
        case rt2DOBJECTTYPESCENE:
            SetStringObjectText(pObject, pszOrgString, pszNewString);
            break;

        default:
            break;
        };
    };

    return bResult;
};


bool CAnimation2D::SetStringObjectText(Rt2dObject* pScene, const wchar* pwszOrgString, const wchar* pwszNewString)
{
    ASSERT(pScene);
    ASSERT(pwszOrgString);
    ASSERT(pwszNewString);

    bool bResult = false;

    int32 childCount = Rt2dSceneGetChildCount(pScene);
    for (int32 i = 0; i < childCount; ++i)
    {
        Rt2dObject* pObject = Rt2dSceneGetChildByIndex(pScene, i);
        ASSERT(pObject);

        int32 type = Rt2dObjectGetObjectType(pObject);
        switch (type)
        {
        case rt2DOBJECTTYPEOBJECTSTRING:
            {
                const char* pszString = Rt2dObjectStringGetText(pObject);
                ASSERT(pszString);

                wchar wszStringW[512];
                wszStringW[0] = UTEXT('\0');
                CGameFont::ConvertToUnicode(wszStringW, pszString);

                if (!CTextData::StrCmp(wszStringW, pwszOrgString))
                {
                    Rt2dObjectStringSetText(pObject, reinterpret_cast<const RwChar*>(pwszNewString));
                    bResult = true;
                };
            }
            break;

        case rt2DOBJECTTYPESCENE:
            SetStringObjectText(pObject, pwszOrgString, pwszNewString);
            break;

        default:
            break;
        };
    };

    return bResult;
};


RwTexture* CAnimation2D::GetTextureAddr(const char* pszFilename)
{
    return CTextureManager::GetRwTexture(pszFilename);
};


void CAnimation2D::SetTexture(const char* pszOrgTexture, const char* pszNewTexture)
{
    ASSERT(m_pMaestro);
    ASSERT(pszOrgTexture);
    ASSERT(pszNewTexture);
    
    Rt2dObject* pScene = Rt2dMaestroGetScene(m_pMaestro);
    ASSERT(pScene);

	/* NOTE: its fine that org tex may be NULL */
    RwTexture* pTextureOrg = CTextureManager::GetRwTexture(pszOrgTexture);    

    RwTexture* pTextureNew = CTextureManager::GetRwTexture(pszNewTexture);
    
    ReplaceTexture(pScene, pTextureOrg, pTextureNew);
};


void CAnimation2D::ReplaceTexture(Rt2dObject* pScene, RwTexture* pOrgTexture, RwTexture* pNewTexture)
{
    ASSERT(pScene);

    int32 childCount = Rt2dSceneGetChildCount(pScene);
    for (int32 i = 0; i < childCount; ++i)
    {
        Rt2dObject* pObject = Rt2dSceneGetChildByIndex(pScene, i);

        int32 type = Rt2dObjectGetObjectType(pObject);
        switch (type)
        {
        case rt2DOBJECTTYPESHAPE:
            {
                int32 nodeCount = Rt2dShapeGetNodeCount(pObject);
                for (int32 j = 0; j < nodeCount; ++j)
                {
                    struct _rt2dShapeNode
                    {
                        RwUInt32    flag;
                        Rt2dPath*   path;
                        Rt2dBrush*  brush;
                    };

                    RwSList* list = pObject->data.shape.rep->nodes;
                    _rt2dShapeNode* node = (_rt2dShapeNode*)_rwSListGetEntry(list, j);
                    
                    Rt2dBrush* pBrush = node->brush;
                    if (pBrush->texture == pOrgTexture)
                    {
                        Rt2dBrush* pResult = Rt2dBrushSetTexture(pBrush, pNewTexture);
                        RpMaterialSetTexture(pBrush->material, pNewTexture);
                        ASSERT(pResult);
                    };
                };
            }
            break;

        case rt2DOBJECTTYPESCENE:
            ReplaceTexture(pObject, pOrgTexture, pNewTexture);
            break;

        default:
            break;
        };
    };
};


void CAnimation2D::SetInterpolateAll(bool bSet)
{
    ASSERT(m_pMaestro);

    Rt2dMaestroForAllAnimations(m_pMaestro, &CAnimation2D::AllAnimSetInterpolateCallback, (void*)(&bSet));
};


void CAnimation2D::FlashKeyPress(uint32 uKey)
{
    ASSERT(m_pMaestro);
    
    m_menuController.KeyPress(m_pMaestro, uKey);
};


void CAnimation2D::FlashUnlockKeyEnable(bool bEnable)
{
    m_menuController.FlashUnlockKeyEnable(bEnable);
};


void CAnimation2D::KeyInfoChange(void)
{
    ASSERT(m_pMaestro);
    
    Rt2dObject* pScene = Rt2dMaestroGetScene(m_pMaestro);
    ASSERT(pScene);

    KeyInfoChange_Sub(pScene);
};


void CAnimation2D::KeyInfoChange_Sub(Rt2dObject* pScene)
{
    ASSERT(pScene);

    int32 childCount = Rt2dSceneGetChildCount(pScene);
    for (int32 i = 0; i < childCount; ++i)
    {
        Rt2dObject* pObject = Rt2dSceneGetChildByIndex(pScene, i);

        int32 type = Rt2dObjectGetObjectType(pObject);
        switch (type)
        {
        case rt2DOBJECTTYPEOBJECTSTRING:
            {
#ifdef TARGET_PC
                static const char GUARD[]   = "Guard";
                static const char ATTACK[]  = "WeakAttack";
                static const char JUMP[]    = "Jump";

                bool bIsTextChanged = false;
                int32 nWritePos = 0;

                char szBuff[256];
                szBuff[0] = '\0';

                const char* pszText = Rt2dObjectStringGetText(pObject);
                while (*pszText)
                {
                    ASSERT(nWritePos < sizeof(szBuff));
                    char ch = *pszText++;

                    switch (ch)
                    {
                    case '<':
                        bIsTextChanged = true;
                        std::strcpy(&szBuff[nWritePos], GUARD);
                        nWritePos += (sizeof(GUARD) - 1);
                        break;

                    case '#':
                        bIsTextChanged = true;
                        std::strcpy(&szBuff[nWritePos], ATTACK);
                        nWritePos += (sizeof(ATTACK) - 1);
                        break;

                    case ';':
                        bIsTextChanged = true;
                        std::strcpy(&szBuff[nWritePos], JUMP);
                        nWritePos += (sizeof(JUMP) - 1);
                        break;

                    default:
                        szBuff[nWritePos++] = ch;
                        break;
                    };
                };
#else /* TARGET_PC */
#error Not implemented for current target
#endif /* TARGET_PC */

                ASSERT(nWritePos < sizeof(szBuff));
				szBuff[nWritePos] = '\0';

                if (bIsTextChanged)
                    Rt2dObjectStringSetText(pObject, szBuff);
            }
            break;

        case rt2DOBJECTTYPESCENE:
            KeyInfoChange_Sub(pObject);
            break;

        default:
            break;
        };
    };
};


void CAnimation2D::SetCenterAllStrings(void)
{
    m_centerStrObj.SetCenterAllStringObject(m_pMaestro);
};


/*static*/ Rt2dMessage* CAnimation2D::MessageHandlerCallback(Rt2dMaestro* maestro, Rt2dMessage* message)
{
    ASSERT(message->index >= 0);

    if (message->messageType == rt2dMESSAGETYPEGETURL)
    {
        ASSERT(m_pAnimation2D);
        if (!m_pAnimation2D->m_menuSound.CallSound(message->intParam1))
            m_pAnimation2D->m_iMessageIndex = message->intParam1;
    };

    return Rt2dMessageHandlerDefaultCallBack(maestro, message);
};


/*static*/ Rt2dObject* CAnimation2D::AllStringObjectCallback(Rt2dObject* object, Rt2dObject* parent, void* data)
{
    ASSERT(data);
    
    if (Rt2dObjectIsScene(object))
    {
        Rt2dSceneForAllChildren(object, &CAnimation2D::AllStringObjectCallback, data);
    }
    else if (Rt2dObjectIsObjectString(object))
    {
        const char* pszText = Rt2dObjectStringGetText(object);
        ConvertButtonSymbol(object);

#ifdef TMNT2_BUILD_EU
        Rt2dObjectStringSetText(object, pszText);      
#else /* TMNT2_BUILD_EU */
        wchar wszBuff[128];
        wszBuff[0] = UTEXT('\0');
        CGameFont::ConvertToUnicode(wszBuff, pszText);

        Rt2dObjectStringSetText(object, reinterpret_cast<const RwChar*>(wszBuff));    
#endif /* TMNT2_BUILD_EU */

        ++(*static_cast<int32*>(data));
    };

    return object;
};


/*static*/ Rt2dMaestro* CAnimation2D::AllAnimSetInterpolateCallback(Rt2dMaestro* maestro,
                                                                    Rt2dAnim* anim,
                                                                    Rt2dAnimProps* props,
                                                                    void* pData)
{
    ASSERT(pData);
    
    Rt2dAnimSetInterpolate(anim, static_cast<RwBool>(pData != nullptr));
    return maestro;
};

