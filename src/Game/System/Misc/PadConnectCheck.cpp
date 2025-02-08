#include "PadConnectCheck.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/Menu/MessageWindow.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/ProcessList.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/Process/Sequence.hpp"
#include "System/Common/Process/ProcessMail.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/Sprite.hpp"
#include "System/Common/Camera.hpp"


class CPadConnectCheckMsgWindow : public CMessageWindow
{
public:
    inline CPadConnectCheckMsgWindow(void) : CMessageWindow(CMessageWindow::COLOR_ERROR) {};
    virtual ~CPadConnectCheckMsgWindow(void) {};
    virtual void DrawInWindow(const Rt2dBBox& bbox) const override;
};


void CPadConnectCheckMsgWindow::DrawInWindow(const Rt2dBBox& bbox) const
{
    if (m_Title.Text())
    {
        Rt2dBBox bboxTitle;
        Rt2dBBox bboxText;

        bboxTitle.x = bbox.x - bbox.w * 0.44f;
        bboxTitle.y = (bbox.h * 0.2f) - bbox.y;
        bboxTitle.w = bbox.w * 0.88f;
        bboxTitle.h = bbox.h * 0.2f;

        bboxText.x = bbox.x - bbox.w * 0.44f;
        bboxText.y = bbox.h * -0.4f - bbox.y;
        bboxText.w = bbox.w * 0.88f;
        bboxText.h = bbox.h * 0.6f;

        m_Title.Draw(bboxTitle);
        m_Text.Draw(bboxText);
    }
    else if (m_Text.Text())
    {
        Rt2dBBox bboxText;

        bboxText.x = bbox.x - bbox.w * 0.44f;
        bboxText.y = bbox.h * -0.4f - bbox.y;
        bboxText.w = bbox.w * 0.88f;
        bboxText.h = bbox.h * 0.8f;

        m_Text.Draw(bboxText);
    };
};


/*static*/ CProcess* CPadConnectCheckProcess::Instance(void)
{
    return new CPadConnectCheckProcess;
};


/*static*/ void CPadConnectCheckProcess::Initialize(CProcess* pSender)
{
    ASSERT(pSender);
    
    pSender->Mail().Send(PROCLABEL_PADCONNECTCHECK, PROCESSTYPES::MAIL::TYPE_ATTACH);
};


/*static*/ void CPadConnectCheckProcess::Terminate(CProcess* pSender)
{
    ASSERT(pSender);
    
    pSender->Mail().Send(PROCLABEL_PADCONNECTCHECK, PROCESSTYPES::MAIL::TYPE_DETACH);
};


/*static*/ void CPadConnectCheckProcess::Reset(CProcess* pSender)
{
    static MESSAGE s_message;
    
    s_message.m_type = MESSAGE::TYPE_RESET;

    pSender->Mail().Send(PROCLABEL_PADCONNECTCHECK, PROCESSTYPES::MAIL::TYPE_MSG, &s_message);
};


CPadConnectCheckProcess::CPadConnectCheckProcess(void)
: m_bSleepFlag(false)
, m_iSeqLabel(PROCESSTYPES::LABEL_EOL)
, m_pWindow(nullptr)
, m_iWarningMsgTextID(-1)
{
    ;
};


CPadConnectCheckProcess::~CPadConnectCheckProcess(void)
{
    ;
};


bool CPadConnectCheckProcess::Attach(void)
{
    m_bSleepFlag = false;
    m_iSeqLabel = PROCESSTYPES::LABEL_EOL;

    m_pWindow = new CPadConnectCheckMsgWindow;
    ASSERT(m_pWindow);
    m_pWindow->Set(0.0f, 0.0f, CSprite::m_fVirtualScreenW, 160.0f);
    m_pWindow->SetOpenAction(false);
    m_pWindow->SetPriority(CMessageWindow::PRIORITY_TOP);

    return true;
};


void CPadConnectCheckProcess::Detach(void)
{
    if(m_bSleepFlag)
    {
        SeqWakeup(m_iSeqLabel);
        m_bSleepFlag = false;
        m_iSeqLabel = PROCESSTYPES::LABEL_EOL;
    };

    if (m_pWindow)
    {
        delete m_pWindow;
        m_pWindow = nullptr;
    };
};


void CPadConnectCheckProcess::Move(void)
{
    MessageProc();

    if (WarningIsShowing())
    {
        if (CGameData::Attribute().IsInteractive())
        {
            bool PadOkFlag = true;
            int32 LostPort = 7;

            int32 PadCnt = CController::Max();
            for (int32 i = 0; i < PadCnt; ++i)
            {
                if (!CController::IsLocked(i))
                    continue;

                if (CController::GetState(i) == CController::STATE_UNCONNECT)
                {
                    int32 Port = CController::GetPhysicalPort(i);
                    if (Port < LostPort)
                        LostPort = Port;

                    PadOkFlag = false;
                };
            };

            if (PadOkFlag)
            {
                if (CController::GetDigitalTrigger(CController::CONTROLLER_LOCKED_ON_VIRTUAL, CController::DIGITAL_START))
                    WarningEnd();
            };
        }
        else
        {
            WarningEnd();
        };
    }
    else
    {
        if (CGameData::Attribute().IsInteractive())
        {
            int32 PadCnt = CController::Max();
            for (int32 i = 0; i < PadCnt; ++i)
            {
                if (!CController::IsLocked(i))
                    continue;

                if (CController::GetState(i) == CController::STATE_UNCONNECT)
                {
                    WarningBegin(i);
                    break;
                };
            };
        };
    };
};


void CPadConnectCheckProcess::Draw(void) const
{
    if (WarningIsShowing())
        DrawFilter();
};


void CPadConnectCheckProcess::MessageProc(void)
{
    PROCESSTYPES::MAIL mail;

    while (Mail().Recv(mail))
    {
        if (mail.m_type == PROCESSTYPES::MAIL::TYPE_MSG)
        {
            MESSAGE* pMessage = (MESSAGE*)mail.m_param;
            ASSERT(pMessage);

            switch (pMessage->m_type)
            {
            case MESSAGE::TYPE_RESET:
                {
                    if (WarningIsShowing())
                        WarningEnd();
                }
                break;
                
            default:
                ASSERT(false);
                break;
            };
        };
    };
};


void CPadConnectCheckProcess::DrawFilter(void) const
{
    RENDERSTATE_PUSH(rwRENDERSTATEZTESTENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATEFOGCOLOR, 0);
    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, 0);

    if (CCamera::CameraCurrent())
    {
        float z = RwIm2DGetNearScreenZMacro();
		float rhw = 1.0f / RwCameraGetNearClipPlaneMacro(CCamera::CameraCurrent());
        float sw = float(CScreen::Width());
        float sh = float(CScreen::Height());
        RwRGBA color = { 0x00, 0x00, 0x00, 0xB0 };
        
        RwIm2DVertex aVertices[4];
        
        aVertices[0].x = 0.0f;
        aVertices[0].y = sh;
        aVertices[0].z = z;
        aVertices[0].rhw = rhw;
        aVertices[0].emissiveColor = RWRGBALONGEX(color);
        aVertices[0].u = 0.0f;
        aVertices[0].v = 0.0f;

        aVertices[1].x = 0.0f;
        aVertices[1].y = 0.0f;
        aVertices[1].z = z;
        aVertices[1].rhw = rhw;
        aVertices[1].emissiveColor = RWRGBALONGEX(color);
        aVertices[1].u = 0.0f;
        aVertices[1].v = 0.0f;

        aVertices[2].x = sw;
        aVertices[2].y = sh;
        aVertices[2].z = z;
        aVertices[2].rhw = rhw;
        aVertices[2].emissiveColor = RWRGBALONGEX(color);
        aVertices[2].u = 0.0f;
        aVertices[2].v = 0.0f;

        aVertices[3].x = sw;
        aVertices[3].y = 0.0f;
        aVertices[3].z = z;
        aVertices[3].rhw = rhw;
        aVertices[3].emissiveColor = RWRGBALONGEX(color);
        aVertices[3].u = 0.0f;
        aVertices[3].v = 0.0f;

        RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, aVertices, COUNT_OF(aVertices));
    };

    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    RENDERSTATE_POP(rwRENDERSTATEFOGCOLOR);
    RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
    RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE)
    RENDERSTATE_POP(rwRENDERSTATEZTESTENABLE);
};


bool CPadConnectCheckProcess::SeqSleep(int32 iLabel)
{
    if (!Info().IsProcessExist(iLabel))
        return false;

    if (Info().Process(iLabel).Info().State() != PROCESSTYPES::STATE_RUN)
        return false;

    if (!Mail().Send(iLabel, PROCESSTYPES::MAIL::TYPE_MOVE_DISABLE))
        return false;

    Mail().Send(PROCLABEL_SCREENFADE, PROCESSTYPES::MAIL::TYPE_MOVE_DISABLE);
    
    return true;
};


bool CPadConnectCheckProcess::SeqWakeup(int32 iLabel)
{
    if (!Info().IsProcessExist(iLabel))
        return false;

    if (Info().Process(iLabel).Info().State() != PROCESSTYPES::STATE_RUN)
        return false;

    if (!Mail().Send(iLabel, PROCESSTYPES::MAIL::TYPE_MOVE_ENABLE))
        return false;

    Mail().Send(PROCLABEL_SCREENFADE, PROCESSTYPES::MAIL::TYPE_MOVE_ENABLE);

    return true;
};


void CPadConnectCheckProcess::WarningMsgSet(int32 iControllerPort)
{
#ifdef TARGET_PS2
    ;
#endif    
};


void CPadConnectCheckProcess::WarningMsgClear(void)
{
    ASSERT(m_pWindow);
    m_pWindow->SetText(nullptr);
};


void CPadConnectCheckProcess::WarningBegin(int32 iController)
{
    ASSERT(!m_bSleepFlag);
    
    int32 iSeqCurrently = CSequence::GetCurrently();
    if (SeqSleep(iSeqCurrently))
    {
        m_bSleepFlag = true;
        m_iSeqLabel = iSeqCurrently;
        m_iWarningMsgTextID = -1;
        int32 iControllerPort = CController::GetPhysicalPort(iController);
        WarningMsgSet(iControllerPort);
        m_pWindow->Open();
        CGameSound::PlaySE(SDCODE_SE(0x1003));
    };
};


void CPadConnectCheckProcess::WarningEnd(void)
{
    ASSERT(m_bSleepFlag);
    
    if (SeqWakeup(m_iSeqLabel))
    {
        m_bSleepFlag = false;
        m_iSeqLabel = PROCESSTYPES::LABEL_EOL;
        m_pWindow->InputAction();
        CGameSound::PlaySE(SDCODE_SE(0x1003));
    };
};


bool CPadConnectCheckProcess::WarningIsShowing(void) const
{
    return m_bSleepFlag;
};