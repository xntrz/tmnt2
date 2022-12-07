#include "UnlockSequence.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/Menu/UnlockMessageWindow.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Misc/LoadingDisplay.hpp"
#include "Game/System/DataLoader/DataLoader.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/File/FileID.hpp"


/*static*/ CProcess* CUnlockSequence::Instance(void)
{
    return new CUnlockSequence;
};


CUnlockSequence::CUnlockSequence(void)
: m_phase(PHASE_LOAD)
, m_notifyphase(NOTIFYPHASE_PEEK)
, m_UnnnotifiedSecretID(SECRETID::ID_NONE)
, m_pUnlockMessage(nullptr)
{
    ;
};


CUnlockSequence::~CUnlockSequence(void)
{
    ASSERT(!m_pUnlockMessage);
};


bool CUnlockSequence::OnAttach(const void* param)
{
    m_pUnlockMessage = new CUnlockMessage;
    ASSERT(m_pUnlockMessage);

    m_notifyphase = NOTIFYPHASE_OPEN;
    m_UnnnotifiedSecretID = CGameData::Record().Secret().GetUnnotifiedSecret();

    CTextureManager::GenerationInc();

    if (m_UnnnotifiedSecretID != SECRETID::ID_NONE)
    {
        CScreen::SetClearColor({ 0x00, 0x00, 0x00, 0xFF });
        CDataLoader::Regist(FILEID::ID_UNLOCK);
        CLoadingDisplay::Start(this);
        
        m_phase = PHASE_LOAD;
    }
    else
    {
        m_phase = PHASE_EOL;
    };

    return true;
};


void CUnlockSequence::OnDetach(void)
{
    CTextureManager::GenerationDec();
    
    if (m_pUnlockMessage)
    {
        delete m_pUnlockMessage;
        m_pUnlockMessage = nullptr;
    };
};


void CUnlockSequence::OnMove(bool bRet, const void* param)
{
    switch (m_phase)
    {
    case PHASE_LOAD:
        if (CDataLoader::IsLoadEnd())
        {
            CUnlockMessage::SetTexture();
            CLoadingDisplay::Stop(this);
            m_phase = PHASE_FADEOUT;
        }
        else
        {
            CDataLoader::Period();
        };        
        break;

    case PHASE_FADEOUT:
        CGameData::Attribute().SetInteractive(true);
        CScreenFade::StartOut(1.0f);
        m_phase = PHASE_FADEOUT_WAIT;
        break;

    case PHASE_FADEOUT_WAIT:
        if (!CScreenFade::IsFading())
            m_phase = PHASE_NOTIFY;        
        break;

    case PHASE_NOTIFY:
        if (Notify())
            m_phase = PHASE_FADEIN;        
        break;

    case PHASE_FADEIN:
        CScreenFade::StartIn(1.0f);
        m_phase = PHASE_FADEIN_WAIT;
        break;

    case PHASE_FADEIN_WAIT:
        if (!CScreenFade::IsFading())
        {
            m_phase = PHASE_EOL;
            CGameData::Attribute().SetInteractive(false);
        };
        break;

    case PHASE_EOL:
        Ret();
        break;

    default:
        ASSERT(false);
        break;
    };
};


void CUnlockSequence::OnDraw(void) const
{
    ;
};


bool CUnlockSequence::Notify(void)
{
    bool bResult = false;

    switch (m_notifyphase)
    {
    case NOTIFYPHASE_PEEK:
        m_UnnnotifiedSecretID = CGameData::Record().Secret().GetUnnotifiedSecret();
        m_notifyphase = NOTIFYPHASE_OPEN;
        break;

    case NOTIFYPHASE_OPEN:
        if (m_UnnnotifiedSecretID != SECRETID::ID_NONE)
        {
            m_pUnlockMessage->SetMessage(m_UnnnotifiedSecretID);
            m_pUnlockMessage->Open();
            m_notifyphase = NOTIFYPHASE_SHOW;
        }
        else
        {
            m_notifyphase = NOTIFYPHASE_EOL;
        };        
        break;

    case NOTIFYPHASE_SHOW:
        if (!m_pUnlockMessage->IsActive())
            m_notifyphase = NOTIFYPHASE_CLOSE;        
        break;

    case NOTIFYPHASE_CLOSE:
        if (!m_pUnlockMessage->IsOpen())
            m_notifyphase = NOTIFYPHASE_PEEK;        
        break;

    case NOTIFYPHASE_EOL:
        bResult = true;
        break;

    default:
        ASSERT(false);
        break;
    };

    return bResult;
};