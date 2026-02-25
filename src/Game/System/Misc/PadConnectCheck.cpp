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


/*virtual*/ void CPadConnectCheckMsgWindow::DrawInWindow(const Rt2dBBox& bbox) const /*override*/
{
    if (m_textTitle.Text())
    {
        Rt2dBBox bboxTitle;
        bboxTitle.x = bbox.x - bbox.w * 0.44f;
        bboxTitle.y = (bbox.h * 0.2f) - bbox.y;
        bboxTitle.w = bbox.w * 0.88f;
        bboxTitle.h = bbox.h * 0.2f;

        Rt2dBBox bboxMsg;
        bboxMsg.x = bbox.x - bbox.w * 0.44f;
        bboxMsg.y = bbox.h * -0.4f - bbox.y;
        bboxMsg.w = bbox.w * 0.88f;
        bboxMsg.h = bbox.h * 0.6f;

        m_textTitle.Draw(bboxTitle);
        m_textMsg.Draw(bboxMsg);
    }
    else if (m_textMsg.Text())
    {
        Rt2dBBox bboxMsg;
        bboxMsg.x = bbox.x - bbox.w * 0.44f;
        bboxMsg.y = bbox.h * -0.4f - bbox.y;
        bboxMsg.w = bbox.w * 0.88f;
        bboxMsg.h = bbox.h * 0.8f;

        m_textMsg.Draw(bboxMsg);
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
            const MESSAGE* pMessage = reinterpret_cast<const MESSAGE*>(mail.m_param);
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

    RwCamera* camera = CCamera::CameraCurrent();

    if (camera != NULL)
    {
        float x = static_cast<float>(CScreen::Width());
        float y = static_cast<float>(CScreen::Height());
        float z = RwIm2DGetNearScreenZ();
        float rhw = 1.0f / RwCameraGetNearClipPlane(camera);
        RwRGBA color = { 0x00, 0x00, 0x00, 0xB0 };

        RwIm2DVertex aVertices[4];

        RwIm2DVertexSetScreenX(&aVertices[0], 0.0f);
        RwIm2DVertexSetScreenY(&aVertices[0], y);
        RwIm2DVertexSetScreenZ(&aVertices[0], z);
        RwIm2DVertexSetIntRGBA(&aVertices[0], color.red, color.green, color.blue, color.alpha);
        RwIm2DVertexSetU(&aVertices[0], 0.0f, rhw);
        RwIm2DVertexSetV(&aVertices[0], 0.0f, rhw);
        RwIm2DVertexSetRecipCameraZ(&aVertices[0], rhw);

        RwIm2DVertexSetScreenX(&aVertices[1], 0.0f);
        RwIm2DVertexSetScreenY(&aVertices[1], 0.0f);
        RwIm2DVertexSetScreenZ(&aVertices[1], z);
        RwIm2DVertexSetIntRGBA(&aVertices[1], color.red, color.green, color.blue, color.alpha);
        RwIm2DVertexSetU(&aVertices[1], 0.0f, rhw);
        RwIm2DVertexSetV(&aVertices[1], 0.0f, rhw);
        RwIm2DVertexSetRecipCameraZ(&aVertices[1], rhw);

        RwIm2DVertexSetScreenX(&aVertices[2], x);
        RwIm2DVertexSetScreenY(&aVertices[2], y);
        RwIm2DVertexSetScreenZ(&aVertices[2], z);
        RwIm2DVertexSetIntRGBA(&aVertices[2], color.red, color.green, color.blue, color.alpha);
        RwIm2DVertexSetU(&aVertices[2], 0.0f, rhw);
        RwIm2DVertexSetV(&aVertices[2], 0.0f, rhw);
        RwIm2DVertexSetRecipCameraZ(&aVertices[2], rhw);

        RwIm2DVertexSetScreenX(&aVertices[3], x);
        RwIm2DVertexSetScreenY(&aVertices[3], 0.0f);
        RwIm2DVertexSetScreenZ(&aVertices[3], z);
        RwIm2DVertexSetIntRGBA(&aVertices[3], color.red, color.green, color.blue, color.alpha);
        RwIm2DVertexSetU(&aVertices[3], 0.0f, rhw);
        RwIm2DVertexSetV(&aVertices[3], 0.0f, rhw);
        RwIm2DVertexSetRecipCameraZ(&aVertices[3], rhw);

        RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, aVertices, COUNT_OF(aVertices));
    };

    RENDERSTATE_POP(rwRENDERSTATECULLMODE);
    RENDERSTATE_POP(rwRENDERSTATEFOGCOLOR);
    RENDERSTATE_POP(rwRENDERSTATEFOGENABLE);
    RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
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