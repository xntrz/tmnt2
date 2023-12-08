#include "AntiqueShopSequence.hpp"

#include "Game/Component/Menu/Dialog.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/AntiqueID.hpp"
#include "Game/Component/GameMain/AreaInfo.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/Text/GameText.hpp"
#include "Game/System/2d/MenuController.hpp"
#include "Game/System/2d/Animation2D.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/ProcessList.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/Sprite.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/File/FileID.hpp"
#include "System/Common/SystemTime.hpp"
#include "System/Common/TextData.hpp"


static int32 s_aAntiqueMap[7][10] =
{
    { ANTIQUEID::ID_AT01, ANTIQUEID::ID_AT02, ANTIQUEID::ID_AT47, ANTIQUEID::ID_AT48, ANTIQUEID::ID_AT49, ANTIQUEID::ID_AT50, ANTIQUEID::ID_AT51, ANTIQUEID::ID_AT52, ANTIQUEID::ID_AT53, ANTIQUEID::ID_AT54 },
    { ANTIQUEID::ID_AT03, ANTIQUEID::ID_AT04, ANTIQUEID::ID_AT05, ANTIQUEID::ID_AT09, ANTIQUEID::ID_AT10, ANTIQUEID::ID_AT11, ANTIQUEID::ID_AT28, ANTIQUEID::ID_AT55, -1, -1 },
    { ANTIQUEID::ID_AT06, ANTIQUEID::ID_AT07, ANTIQUEID::ID_AT08, ANTIQUEID::ID_AT35, ANTIQUEID::ID_AT36, -1, -1, -1, -1, -1 },
    { ANTIQUEID::ID_AT12, ANTIQUEID::ID_AT13, ANTIQUEID::ID_AT14, ANTIQUEID::ID_AT15, ANTIQUEID::ID_AT16, ANTIQUEID::ID_AT17, ANTIQUEID::ID_AT18, ANTIQUEID::ID_AT19, ANTIQUEID::ID_AT20, ANTIQUEID::ID_AT21 },
    { ANTIQUEID::ID_AT22, ANTIQUEID::ID_AT23, ANTIQUEID::ID_AT24, ANTIQUEID::ID_AT25, ANTIQUEID::ID_AT26, ANTIQUEID::ID_AT27, -1, -1, -1, -1 },
    { ANTIQUEID::ID_AT29, ANTIQUEID::ID_AT30, ANTIQUEID::ID_AT31, ANTIQUEID::ID_AT32, ANTIQUEID::ID_AT33, ANTIQUEID::ID_AT34, -1, -1, -1, -1 },
    { ANTIQUEID::ID_AT37, ANTIQUEID::ID_AT38, ANTIQUEID::ID_AT39, ANTIQUEID::ID_AT40, ANTIQUEID::ID_AT41, ANTIQUEID::ID_AT42, ANTIQUEID::ID_AT43, ANTIQUEID::ID_AT44, ANTIQUEID::ID_AT45, ANTIQUEID::ID_AT46 },
};


static int32 s_aAntiquePrice[ANTIQUEID::ID_MAX] =
{
    0,  // ANTIQUEID::ID_NONE
    3,  // ANTIQUEID::ID_AT01
    3,  // ANTIQUEID::ID_AT02
    2,  // ANTIQUEID::ID_AT03
    3,  // ANTIQUEID::ID_AT04
    3,  // ANTIQUEID::ID_AT05
    2,  // ANTIQUEID::ID_AT06
    2,  // ANTIQUEID::ID_AT07
    2,  // ANTIQUEID::ID_AT08
    3,  // ANTIQUEID::ID_AT09
    3,  // ANTIQUEID::ID_AT10
    2,  // ANTIQUEID::ID_AT11
    2,  // ANTIQUEID::ID_AT12
    3,  // ANTIQUEID::ID_AT13
    2,  // ANTIQUEID::ID_AT14
    2,  // ANTIQUEID::ID_AT15
    3,  // ANTIQUEID::ID_AT16
    3,  // ANTIQUEID::ID_AT17
    3,  // ANTIQUEID::ID_AT18
    3,  // ANTIQUEID::ID_AT19
    2,  // ANTIQUEID::ID_AT20
    3,  // ANTIQUEID::ID_AT21
    2,  // ANTIQUEID::ID_AT22
    3,  // ANTIQUEID::ID_AT23
    1,  // ANTIQUEID::ID_AT24
    2,  // ANTIQUEID::ID_AT25
    1,  // ANTIQUEID::ID_AT26
    2,  // ANTIQUEID::ID_AT27
    3,  // ANTIQUEID::ID_AT28
    4,  // ANTIQUEID::ID_AT29
    4,  // ANTIQUEID::ID_AT30
    4,  // ANTIQUEID::ID_AT31
    1,  // ANTIQUEID::ID_AT32
    2,  // ANTIQUEID::ID_AT33
    4,  // ANTIQUEID::ID_AT34
    1,  // ANTIQUEID::ID_AT35
    3,  // ANTIQUEID::ID_AT36
    2,  // ANTIQUEID::ID_AT37
    2,  // ANTIQUEID::ID_AT38
    3,  // ANTIQUEID::ID_AT39
    2,  // ANTIQUEID::ID_AT40
    5,  // ANTIQUEID::ID_AT41
    5,  // ANTIQUEID::ID_AT42
    2,  // ANTIQUEID::ID_AT43
    3,  // ANTIQUEID::ID_AT44
    3,  // ANTIQUEID::ID_AT45
    3,  // ANTIQUEID::ID_AT46
    4,  // ANTIQUEID::ID_AT47
    1,  // ANTIQUEID::ID_AT48
    2,  // ANTIQUEID::ID_AT49
    3,  // ANTIQUEID::ID_AT50
    2,  // ANTIQUEID::ID_AT51
    1,  // ANTIQUEID::ID_AT52
    5,  // ANTIQUEID::ID_AT53
    4,  // ANTIQUEID::ID_AT54
    5,  // ANTIQUEID::ID_AT55
};


static const char* s_apszAprilFace[] =
{
    "shop_aprilCPS",
    "shop_aprilEPS",
    "shop_aprilDPS",
    "shop_aprilCPS",
    "shop_aprilBPS",
    "shop_aprilAPS",
};


static const char* s_apszAntiqueTexOrg[] =
{
    "Antique_017",
    "Antique_019",
    "Antique_021",
    "Antique_023",
    "Antique_058",
    "Antique_060",
    "Antique_062",
    "Antique_064",
    "Antique_066",
    "Antique_068",
    "Antique_093",
    "Antique_095",
    "Antique_097",
    "Antique_099",
    "Antique_101",
    "Antique_103",
    "Antique_070",
    "Antique_072",
    "Antique_074",
    "Antique_076",
    "Antique_078",
    "Antique_080",
    "Antique_114",
    "Antique_116",
    "Antique_118",
    "Antique_120",
    "Antique_122",
    "Antique_124",
    "Antique_126",
    "Antique_128",
    "Antique_130",
    "Antique_132",
    "Antique_142",
    "Antique_144",
    "Antique_146",
    "Antique_148",
    "Antique_150",
    "Antique_152",
    "Antique_134",
    "Antique_136",
    "Antique_138",
    "Antique_140",
    "Antique_155",
    "Antique_157",
    "Antique_159",
    "Antique_161",
    "Antique_163",
    "Antique_165",
    "Antique_167",
    "Antique_169",
    "Antique_171",
    "Antique_173",
    "Antique_175",
    "Antique_177",
    "Antique_082",
    "Antique_084",
    "Antique_180",
    "Antique_182",
    "Antique_184",
    "Antique_186",
    "Antique_188",
    "Antique_190",
    "Antique_192",
    "Antique_194",
    "Antique_196",
    "Antique_198",
    "Antique_200",
    "Antique_202",
    "Antique_105",
    "Antique_107",
    "Antique_109",
    "Antique_111",
    "Antique_205",
    "Antique_207",
    "Antique_209",
    "Antique_211",
    "Antique_213",
    "Antique_215",
    "Antique_217",
    "Antique_219",
    "Antique_221",
    "Antique_223",
    "Antique_225",
    "Antique_227",
    "Antique_229",
    "Antique_231",
    "Antique_233",
    "Antique_235",
    "Antique_237",
    "Antique_239",
    "Antique_241",
    "Antique_243",
    "Antique_025",
    "Antique_027",
    "Antique_029",
    "Antique_031",
    "Antique_033",
    "Antique_035",
    "Antique_037",
    "Antique_039",
    "Antique_041",
    "Antique_043",
    "Antique_045",
    "Antique_047",
    "Antique_049",
    "Antique_051",
    "Antique_053",
    "Antique_055",
    "Antique_086",
    "Antique_088",
};


static const char* s_apszAntiqueTex[] =
{
    nullptr,
    "shop_A01BPS",
    "shop_A02BPS",
    "shop_A03BPS",
    "shop_A04BPS",
    "shop_A05BPS",
    "shop_A06BPS",
    "shop_A07BPS",
    "shop_A08BPS",
    "shop_A09BPS",
    "shop_A10BPS",
    "shop_A11BPS",
    "shop_A12BPS",
    "shop_A13BPS",
    "shop_A14BPS",
    "shop_A15BPS",
    "shop_A16BPS",
    "shop_A17BPS",
    "shop_A18BPS",
    "shop_A19BPS",
    "shop_A20BPS",
    "shop_A21BPS",
    "shop_A22BPS",
    "shop_A23BPS",
    "shop_A24BPS",
    "shop_A25BPS",
    "shop_A26BPS",
    "shop_A27BPS",
    "shop_A28BPS",
    "shop_A29BPS",
    "shop_A30BPS",
    "shop_A31BPS",
    "shop_A32BPS",
    "shop_A33BPS",
    "shop_A34BPS",
    "shop_A35BPS",
    "shop_A36BPS",
    "shop_A37BPS",
    "shop_A38BPS",
    "shop_A39BPS",
    "shop_A40BPS",
    "shop_A41BPS",
    "shop_A42BPS",
    "shop_A43BPS",
    "shop_A44BPS",
    "shop_A45BPS",
    "shop_A46BPS",
    "shop_A47BPS",
    "shop_A48BPS",
    "shop_A49BPS",
    "shop_A50BPS",
    "shop_A51BPS",
    "shop_A52BPS",
    "shop_A53BPS",
    "shop_A54BPS",
    "shop_A55BPS",
};


static GAMETEXT s_aAntiqueText[] =
{
    GAMETEXT(0x0),
    GAMETEXT(0x3B9),
    GAMETEXT(0x3BA),
    GAMETEXT(0x3BB),
    GAMETEXT(0x3BC),
    GAMETEXT(0x3BD),
    GAMETEXT(0x3BE),
    GAMETEXT(0x3BF),
    GAMETEXT(0x3C0),
    GAMETEXT(0x3C1),
    GAMETEXT(0x3C2),
    GAMETEXT(0x3C3),
    GAMETEXT(0x3C4),
    GAMETEXT(0x3C5),
    GAMETEXT(0x3C6),
    GAMETEXT(0x3C7),
    GAMETEXT(0x3C8),
    GAMETEXT(0x3C9),
    GAMETEXT(0x3CA),
    GAMETEXT(0x3CB),
    GAMETEXT(0x3CC),
    GAMETEXT(0x3CD),
    GAMETEXT(0x3CE),
    GAMETEXT(0x3CF),
    GAMETEXT(0x3D0),
    GAMETEXT(0x3D1),
    GAMETEXT(0x3D2),
    GAMETEXT(0x3D3),
    GAMETEXT(0x3D4),
    GAMETEXT(0x3D5),
    GAMETEXT(0x3D6),
    GAMETEXT(0x3D7),
    GAMETEXT(0x3D8),
    GAMETEXT(0x3D9),
    GAMETEXT(0x3DA),
    GAMETEXT(0x3DB),
    GAMETEXT(0x3DC),
    GAMETEXT(0x3DD),
    GAMETEXT(0x3DE),
    GAMETEXT(0x3DF),
    GAMETEXT(0x3E0),
    GAMETEXT(0x3E1),
    GAMETEXT(0x3E2),
    GAMETEXT(0x3E3),
    GAMETEXT(0x3E4),
    GAMETEXT(0x3E5),
    GAMETEXT(0x3E6),
    GAMETEXT(0x3E7),
    GAMETEXT(0x3E8),
    GAMETEXT(0x3E9),
    GAMETEXT(0x3EA),
    GAMETEXT(0x3EB),
    GAMETEXT(0x3EC),
    GAMETEXT(0x3ED),
    GAMETEXT(0x3EE),
    GAMETEXT(0x3EF),
};


static GAMETEXT s_aAntiqueComment[] =
{
    GAMETEXT(0x0),
    GAMETEXT(0x427),
    GAMETEXT(0x428),
    GAMETEXT(0x429),
    GAMETEXT(0x42A),
    GAMETEXT(0x42B),
    GAMETEXT(0x42C),
    GAMETEXT(0x42D),
    GAMETEXT(0x42E),
    GAMETEXT(0x42F),
    GAMETEXT(0x430),
    GAMETEXT(0x431),
    GAMETEXT(0x432),
    GAMETEXT(0x433),
    GAMETEXT(0x434),
    GAMETEXT(0x435),
    GAMETEXT(0x436),
    GAMETEXT(0x437),
    GAMETEXT(0x438),
    GAMETEXT(0x439),
    GAMETEXT(0x43A),
    GAMETEXT(0x43B),
    GAMETEXT(0x43C),
    GAMETEXT(0x43D),
    GAMETEXT(0x43E),
    GAMETEXT(0x43F),
    GAMETEXT(0x440),
    GAMETEXT(0x441),
    GAMETEXT(0x442),
    GAMETEXT(0x443),
    GAMETEXT(0x444),
    GAMETEXT(0x445),
    GAMETEXT(0x446),
    GAMETEXT(0x447),
    GAMETEXT(0x448),
    GAMETEXT(0x449),
    GAMETEXT(0x44A),
    GAMETEXT(0x44B),
    GAMETEXT(0x44C),
    GAMETEXT(0x44D),
    GAMETEXT(0x44E),
    GAMETEXT(0x44F),
    GAMETEXT(0x450),
    GAMETEXT(0x451),
    GAMETEXT(0x452),
    GAMETEXT(0x453),
    GAMETEXT(0x454),
    GAMETEXT(0x455),
    GAMETEXT(0x456),
    GAMETEXT(0x457),
    GAMETEXT(0x458),
    GAMETEXT(0x459),
    GAMETEXT(0x45A),
    GAMETEXT(0x45B),
    GAMETEXT(0x45C),
    GAMETEXT(0x45D),
    GAMETEXT(0x3B3),
    GAMETEXT(0x3B5),
    GAMETEXT(0x3B6),
};


static RwV2d s_vMarker = { 173.0f, -4.0f };
static RwV2d s_vMarkerInc = { 10.0f, 11.0f };
static RwV2d s_vMarkerSize = { 8.0f, 8.0f };
static RwV2d s_vAboutPos = { 38.0f, 0.0f };
static RwV2d s_vAboutItemPos = { 50.0f, -58.0f };
static RwV2d s_vAboutStarPos = { -45.0f, 18.0f };
static RwV2d s_avAboutTextPos[] =
{
    { -226.0f, -136.0f },
    { -226.0f, -68.0f },
    { -226.0f, 0.0f },
};
static Rt2dBBox s_aAboutBox[] =
{
    { -226.0f, 70.0f, 158.0f, 64.0f },
    { -226.0f, -137.0f, 380.0f, 100.0f },
    { -226.0f, 2.0f, 158.0f, 64.0f },
};
static RwV2d s_vGetDatePos = { -226.0f, 16.0f };
static RwV2d s_vAboutArrowUPos = { 170.0f, 48.0f };
static RwV2d s_vAboutArrowDPos = { 170.0f, 140.0f };


class CAntiqueShop
{
private:
    static const uint32 ABOUT_ANIMDUR_FRAMES;
    static const float ABOUT_MOVE_INC;
    
    class CAntiqueSprite final : public CSprite
    {
    public:
        CAntiqueSprite(void);
        virtual ~CAntiqueSprite(void);
        void SetDisplay(bool bState);
        bool IsDisplaying(void) const;

    private:
        bool m_bDispState;
    };

    struct CTRLINFO
    {
        GAMETEXT IdText;
        RwV2d Position;
    };

public:
    CAntiqueShop(void);
    ~CAntiqueShop(void);
    void Attach(void);
    void Detach(void);
    void Loaded(CAnimation2D* pAnimation2D);
    bool Move(void);
    void Draw(void);
    void DlgCreate(void);
    void DlgDestroy(void);
    void AntiqueInit(void);
    void AntiqueCheck(void);
    void AntiqueDispChange(void);
    void AntiqueDispCtrl(void);
    void AntiqueOpen(void);
    void AntiqueClose(void);
    void AntiqueItemSet(void);
    void AntiqueIdentify(int32 idAntique);
    void AprilFaceChange(bool bIdentify);
    
private:
    CAnimation2D* m_pAnim2D;
    CAntiqueRecord::STATE m_aAntiqueState[ANTIQUEID::ID_MAX];
    int32 m_iCursorV;
    int32 m_iCursorH;
    int32 m_idAntiqueSel;
    CAntiqueSprite m_aAntiqueSprite[ANTIQUEID::ID_MAX];
    CAntiqueSprite m_aAboutSprite[9];
    bool m_bAntiqueOpenFlag;
    bool m_bAnimFlag;
    bool m_bDispAboutAntiqText;
    bool m_bDispAboutCtrlText;
    uint32 m_uAnimCnt;
    float m_fWndOfsX;
    char m_szTextAntiqueTakenDate[256];
    RwV2d m_vTextAntiqueTakenDatePos;
    GAMETEXT m_idTextAntiqueName;
    Rt2dBBox m_bboxTextAntiqueName;
    AREAID::VALUE m_idAntiqueTakenArea;
    Rt2dBBox m_bboxTextAntiqueArea;
    GAMETEXT m_idTextAntiqueComment;
    Rt2dBBox m_bboxTextAntiqueComment;
    int32 m_iTextAntiqueCommentLineNum;
    int32 m_iTextAntiqueCommentLineCur;
    CTimeObj m_takendate;
    const char* m_pszAprilFace;
    CDialog* m_pDlg;
    CTRLINFO m_aCtrlInfo[3];
};


/*static*/ const uint32 CAntiqueShop::ABOUT_ANIMDUR_FRAMES = 15;
/*static*/ const float CAntiqueShop::ABOUT_MOVE_INC = 40.133335f;


CAntiqueShop::CAntiqueSprite::CAntiqueSprite(void)
: CSprite()
, m_bDispState(false)
{
    ;
};


CAntiqueShop::CAntiqueSprite::~CAntiqueSprite(void)
{
    ;
};


void CAntiqueShop::CAntiqueSprite::SetDisplay(bool bState)
{
    m_bDispState = bState;
};


bool CAntiqueShop::CAntiqueSprite::IsDisplaying(void) const
{
    return m_bDispState;
};


CAntiqueShop::CAntiqueShop(void)
: m_pAnim2D(nullptr)
, m_aAntiqueState()
, m_iCursorV(0)
, m_iCursorH(0)
, m_idAntiqueSel(ANTIQUEID::ID_NONE)
, m_aAntiqueSprite()
, m_aAboutSprite()
, m_bAntiqueOpenFlag(false)
, m_bAnimFlag(false)
, m_bDispAboutAntiqText(false)
, m_bDispAboutCtrlText(false)
, m_uAnimCnt(0)
, m_fWndOfsX(0.0f)
, m_szTextAntiqueTakenDate()
, m_vTextAntiqueTakenDatePos(Math::VECTOR2_ZERO)
, m_idTextAntiqueName(GAMETEXT(0))
, m_bboxTextAntiqueName()
, m_idAntiqueTakenArea(AREAID::ID_NONE)
, m_bboxTextAntiqueArea()
, m_idTextAntiqueComment(GAMETEXT(0))
, m_bboxTextAntiqueComment()
, m_iTextAntiqueCommentLineNum(0)
, m_iTextAntiqueCommentLineCur(0)
, m_takendate()
, m_pszAprilFace(nullptr)
, m_pDlg(nullptr)
, m_aCtrlInfo()
{
    ;
};


CAntiqueShop::~CAntiqueShop(void)
{
    DlgDestroy();
};


void CAntiqueShop::Attach(void)
{
    DlgCreate();
    AntiqueCheck();
    AntiqueInit();    
};


void CAntiqueShop::Detach(void)
{
    DlgDestroy();
};


void CAntiqueShop::Loaded(CAnimation2D* pAnimation2D)
{
    ASSERT(pAnimation2D);
    
    m_pAnim2D = pAnimation2D;
};


bool CAntiqueShop::Move(void)
{
    bool bResult = false;

    if (m_pAnim2D->CheckMessageGetURL("Home"))
        bResult = true;

    if (CController::GetDigitalTrigger(CGameData::Attribute().GetVirtualPad(), CController::DIGITAL_CANCEL))
    {
        if (!m_bAntiqueOpenFlag)
        {
            CGameSound::PlaySE(SDCODE_SE(0x1001));
            if (!m_pDlg->IsOpen())
            {
                m_pDlg->Open();
                m_pDlg->SetStatus(CDialog::STATUS_NO);
                
                CMenuController::KeyLock(CController::DIGITAL_LUP);
                CMenuController::KeyLock(CController::DIGITAL_LDOWN);
                CMenuController::KeyLock(CController::DIGITAL_LLEFT);
                CMenuController::KeyLock(CController::DIGITAL_LRIGHT);
                CMenuController::KeyLock(CController::DIGITAL_OK);
            };
        };
    };

    if (!m_pDlg->IsOpen())
    {
        if (m_pDlg->GetStatus() == CDialog::STATUS_YES)
        {
            bResult = true;
            CMenuController::KeyUnlock(CController::DIGITAL_LUP);
            CMenuController::KeyUnlock(CController::DIGITAL_LDOWN);
            CMenuController::KeyUnlock(CController::DIGITAL_LLEFT);
            CMenuController::KeyUnlock(CController::DIGITAL_LRIGHT);
            CMenuController::KeyUnlock(CController::DIGITAL_OK);
            AntiqueDispCtrl();
        }
        else
        {
            CMenuController::KeyUnlock(CController::DIGITAL_LUP);
            CMenuController::KeyUnlock(CController::DIGITAL_LDOWN);
            CMenuController::KeyUnlock(CController::DIGITAL_LLEFT);
            CMenuController::KeyUnlock(CController::DIGITAL_LRIGHT);
            CMenuController::KeyUnlock(CController::DIGITAL_OK);
            AntiqueDispCtrl();
        };
    };

    return bResult;
};


void CAntiqueShop::Draw(void)
{
    CSystem2D::PushRenderState();
    
    for (int32 idAntiq = ANTIQUEID::ID_FIRST; idAntiq < ANTIQUEID::ID_MAX; ++idAntiq)
    {
        if (m_aAntiqueSprite[idAntiq].IsDisplaying() && (m_idAntiqueSel != idAntiq))
            m_aAntiqueSprite[idAntiq].Draw();
    };

    for (int32 i = 0; i < COUNT_OF(m_aAboutSprite); ++i)
    {
        if (m_aAboutSprite[i].IsDisplaying())
            m_aAboutSprite[i].Draw();
    };
    
    CSystem2D::PopRenderState();

	CGameFont::SetHeight(15.0f);
    if (m_bDispAboutAntiqText)
    {
        CGameFont::SetRGBA(255, 255, 255, 255);
        CGameFont::Flow(
            CGameText::GetText(m_idTextAntiqueName),
            &m_bboxTextAntiqueName,
            rt2dJUSTIFYCENTER
        );
        CGameFont::Flow(
            CAreaInfo::GetDispName(m_idAntiqueTakenArea),
            &m_bboxTextAntiqueArea
        );        
        CGameFont::Show(
            m_szTextAntiqueTakenDate,
            m_vTextAntiqueTakenDatePos.x,
            m_vTextAntiqueTakenDatePos.y
        );
        CGameFont::FlowEx(
            CGameText::GetText(m_idTextAntiqueComment),
            m_iTextAntiqueCommentLineCur,
            &m_bboxTextAntiqueComment
        );
        
        CGameFont::SetRGBA(255, 180, 0, 255);        
        wchar wszBuff[256];
        wszBuff[0] = UTEXT('\0');
        CTextData::Sprintf(wszBuff, CGameText::GetText(GAMETEXT(951)), m_idAntiqueSel);
        CGameFont::Show(wszBuff, m_fWndOfsX - 55.0f, -140.0f);
    };

    if (m_bDispAboutCtrlText)
    {
        CGameFont::SetRGBA(255, 180, 0, 255);
        for (int32 i = 0; i < COUNT_OF(m_aCtrlInfo); ++i)
        {
            CGameFont::Show(
                CGameText::GetText(m_aCtrlInfo[i].IdText),
                m_aCtrlInfo[i].Position.x,
                m_aCtrlInfo[i].Position.y
            );
        };
    };
};


void CAntiqueShop::DlgCreate(void)
{
    m_pDlg = new CDialog();
    ASSERT(m_pDlg);
    if (m_pDlg)
    {
        m_pDlg->Set(0.0f, 33.0f, 640.0f, 192.0f);
		m_pDlg->SetStatus(CDialog::STATUS_NO);
        m_pDlg->SetOpenAction(true);
        m_pDlg->SetController(CGameData::Attribute().GetVirtualPad());
        m_pDlg->SetTitle(
            CGameText::GetText(GAMETEXT(946)),
			CGameFont::GetScreenHeight() * 2.0f,
            { 0xFF, 0xFF, 0xFF, 0xFF }
        );
    };
};


void CAntiqueShop::DlgDestroy(void)
{
    if (m_pDlg)
    {
        delete m_pDlg;
        m_pDlg = nullptr;
    };
};


void CAntiqueShop::AntiqueInit(void)
{
    m_iCursorV = 0;
    m_iCursorH = 0;
    m_idAntiqueSel = ANTIQUEID::ID_NONE;
    m_bAntiqueOpenFlag = false;
    m_bAnimFlag = false;
    m_uAnimCnt = 0;

    for (int32 i = 0; i < COUNT_OF(m_aAntiqueSprite); ++i)
        m_aAntiqueSprite[i].SetDisplay(false);

    for (int32 i = 0; i < COUNT_OF(m_aAboutSprite); ++i)
        m_aAboutSprite[i].SetDisplay(false);

    m_bDispAboutAntiqText = false;
    m_bDispAboutCtrlText = false;

    m_pszAprilFace = "Antique_259";
};


void CAntiqueShop::AntiqueCheck(void)
{
    for (int32 idAntique = ANTIQUEID::ID_FIRST; idAntique < ANTIQUEID::ID_MAX; ++idAntique)
    {
        if (CGameData::Record().Antique().IsAntiqueIdentified(ANTIQUEID::VALUE(idAntique)))
        {
            m_aAntiqueState[idAntique] = CAntiqueRecord::STATE_IDENTIFIED;
        }
        else if (CGameData::Record().Antique().IsAntiqueTaken(ANTIQUEID::VALUE(idAntique)))
        {
            m_aAntiqueState[idAntique] = CAntiqueRecord::STATE_UNIDENTIFIED;
        }
        else
        {
            m_aAntiqueState[idAntique] = CAntiqueRecord::STATE_NONE;
        };
    };
};


void CAntiqueShop::AntiqueDispChange(void)
{
    const char** pszAntiqTexOrg = s_apszAntiqueTexOrg;
    const char** pszAntiqTex = &s_apszAntiqueTex[1];
    
    for (int32 i = 1; i < COUNT_OF(m_aAntiqueState); ++i)
    {
        switch (m_aAntiqueState[i])
        {
        case CAntiqueRecord::STATE_NONE:
            {
                m_pAnim2D->SetTexture(pszAntiqTexOrg[0], "shop_marubaceAPS");
                m_pAnim2D->SetTexture(pszAntiqTexOrg[1], "shop_questAPS");
            }
            break;

        case CAntiqueRecord::STATE_UNIDENTIFIED:
            {
                m_pAnim2D->SetTexture(pszAntiqTexOrg[1], *pszAntiqTex);
            }
            break;

        case CAntiqueRecord::STATE_IDENTIFIED:
            {
                m_pAnim2D->SetTexture(pszAntiqTexOrg[0], "shop_marubaceAPS");
                m_pAnim2D->SetTexture(pszAntiqTexOrg[1], *pszAntiqTex);
            }
            break;

        default:
            ASSERT(false);
            break;
        };

        pszAntiqTexOrg += 2;
        pszAntiqTex += 1;
    };

    for (int32 i = 0; i < COUNT_OF(s_aAntiqueMap); ++i)
    {
        for (int32 j = 0; j < COUNT_OF(s_aAntiqueMap[0]); ++j)
        {
            int32 idAntiq = s_aAntiqueMap[i][j];
            if (idAntiq <= 0)
                break;

            ASSERT((idAntiq >= 0) && (idAntiq < COUNT_OF(m_aAntiqueState)));
            ASSERT((idAntiq >= 0) && (idAntiq < COUNT_OF(m_aAntiqueSprite)));
            
            CAntiqueSprite& Sprite = m_aAntiqueSprite[idAntiq];
            switch (m_aAntiqueState[idAntiq])
            {
            case CAntiqueRecord::STATE_NONE:
                {
                    Sprite.SetDisplay(false);                    
                }
                break;

            case CAntiqueRecord::STATE_UNIDENTIFIED:
                {
                    Sprite.SetDisplay(true);
                    Sprite.SetTexture(CTextureManager::GetRwTexture("shop_MiconAPS"));
                    Sprite.SetRGBA(255, 255, 255, 255);
                    Sprite.Resize(s_vMarkerSize.x, s_vMarkerSize.y);
                    Sprite.Move(
                        s_vMarker.x + (s_vMarkerInc.x * float(j)),
                        s_vMarker.y - (s_vMarkerInc.y * float(i))
                    );
                }
                break;

            case CAntiqueRecord::STATE_IDENTIFIED:
                {
                    Sprite.SetDisplay(true);
                    Sprite.SetTexture(CTextureManager::GetRwTexture("shop_MiconBPS"));
                    Sprite.SetRGBA(255, 255, 255, 255);
                    Sprite.Resize(s_vMarkerSize.x, s_vMarkerSize.y);
                    Sprite.Move(
                        s_vMarker.x + (s_vMarkerInc.x * float(j)),
                        s_vMarker.y - (s_vMarkerInc.y * float(i))
                    );
                }
                break;

            default:
                ASSERT(false);
                break;
            };
        };
    };
};


void CAntiqueShop::AntiqueDispCtrl(void)
{
    if (m_pAnim2D->CheckMessageGetURL("PressUp"))
    {
        CGameSound::PlaySE(SDCODE_SE(0x1004));
        m_iCursorV =  InvClamp(m_iCursorV + 1, 0, 6);
		OUTPUT("cursor v=%d h=%d\n", m_iCursorV, m_iCursorH);
    }
    else if (m_pAnim2D->CheckMessageGetURL("PressDown"))
    {
        CGameSound::PlaySE(SDCODE_SE(0x1004));
        m_iCursorV = InvClamp(m_iCursorV - 1, 0, 6);
		OUTPUT("cursor v=%d h=%d\n", m_iCursorV, m_iCursorH);
    }
    else if (m_pAnim2D->CheckMessageGetURL("PressLeft"))
    {
        CGameSound::PlaySE(SDCODE_SE(0x1004));
        m_iCursorH = InvClamp(m_iCursorH - 1, 0, 10 - 1);
        m_iTextAntiqueCommentLineCur = 0;
		OUTPUT("cursor v=%d h=%d\n", m_iCursorV, m_iCursorH);
    }
    else if (m_pAnim2D->CheckMessageGetURL("PressRight"))
    {
        CGameSound::PlaySE(SDCODE_SE(0x1004));
        m_iCursorH = InvClamp(m_iCursorH + 1, 0, 10 - 1);
        m_iTextAntiqueCommentLineCur = 0;
		OUTPUT("cursor v=%d h=%d\n", m_iCursorV, m_iCursorH);
    };

    m_idAntiqueSel = s_aAntiqueMap[m_iCursorV][m_iCursorH];
    if ((m_idAntiqueSel > 0) && m_aAntiqueState[m_idAntiqueSel])
        AprilFaceChange(true);
    else
        AprilFaceChange(false);
    
    if (!m_bAntiqueOpenFlag)
    {
        if (m_pAnim2D->CheckMessageGetURL("OpenWindow"))
        {
            if (m_idAntiqueSel <= ANTIQUEID::ID_NONE)
            {
                CGameSound::PlaySE(SDCODE_SE(0x1000));
            }
            else
            {
                CGameSound::PlaySE(SDCODE_SE(0x1002));
                m_bAntiqueOpenFlag = true;
                m_bAnimFlag = true;
            };
        };
    }
    else
    {
        if (m_pAnim2D->CheckMessageGetURL("Exit"))
        {
            CGameSound::PlaySE(SDCODE_SE(0x1001));
            m_bAntiqueOpenFlag = false;
            m_bAnimFlag = true;
        }
        else if(m_idAntiqueSel <= ANTIQUEID::ID_NONE)
        {
            m_bAntiqueOpenFlag = false;
            m_bAnimFlag = true;
        };
    };

    if (m_bAntiqueOpenFlag)
        AntiqueOpen();
    else
        AntiqueClose();
};


void CAntiqueShop::AntiqueOpen(void)
{
    CMenuController::KeyLock(CController::DIGITAL_LUP);
    CMenuController::KeyLock(CController::DIGITAL_LDOWN);

    if (m_bAnimFlag)
    {
        m_fWndOfsX = float(m_uAnimCnt) * ABOUT_MOVE_INC - 640.0f;
        if (++m_uAnimCnt >= ABOUT_ANIMDUR_FRAMES)
            m_bAnimFlag = false;
    };

    AntiqueItemSet();
};


void CAntiqueShop::AntiqueClose(void)
{
    if (m_bAnimFlag)
    {
        if (--m_uAnimCnt == 0)
        {
            m_bAnimFlag = false;
            m_iTextAntiqueCommentLineCur = 0;
            CMenuController::KeyUnlock(CController::DIGITAL_LUP);
            CMenuController::KeyUnlock(CController::DIGITAL_LDOWN);
        };

        m_fWndOfsX = float(m_uAnimCnt) * ABOUT_MOVE_INC - 640.0f;

        AntiqueItemSet();
    }
    else
    {
        for (int32 i = 0; i < COUNT_OF(m_aAboutSprite); ++i)
            m_aAboutSprite[i].SetDisplay(false);

        m_bDispAboutAntiqText = false;
        m_bDispAboutCtrlText = false;
    };
};


void CAntiqueShop::AntiqueItemSet(void)
{
    m_aAboutSprite[0].SetDisplay(true);
    m_aAboutSprite[0].SetTexture(CTextureManager::GetRwTexture("shop_PageBgPS"));
    m_aAboutSprite[0].Move(m_fWndOfsX - s_vAboutPos.x, s_vAboutPos.y);
    m_aAboutSprite[0].Resize(512.0f, 512.0f);

    m_bDispAboutCtrlText = true;
    m_aCtrlInfo[0] = { GAMETEXT(947), { s_avAboutTextPos[0].x + m_fWndOfsX, s_avAboutTextPos[0].y } };
    m_aCtrlInfo[1] = { GAMETEXT(949), { s_avAboutTextPos[1].x + m_fWndOfsX, s_avAboutTextPos[1].y } };
    m_aCtrlInfo[2] = { GAMETEXT(950), { s_avAboutTextPos[2].x + m_fWndOfsX, s_avAboutTextPos[2].y } };

    for (int32 i = 1; i < COUNT_OF(m_aAboutSprite); ++i)
        m_aAboutSprite[i].SetDisplay(false);

    m_bDispAboutAntiqText = false;
    if (m_idAntiqueSel >= 0)
    {
        switch (m_aAntiqueState[m_idAntiqueSel])
        {
        case CAntiqueRecord::STATE_NONE:
            {
                m_aAboutSprite[1].SetDisplay(true);
                m_aAboutSprite[1].SetTexture(CTextureManager::GetRwTexture("shop_questAPS"));
                m_aAboutSprite[1].Move(m_fWndOfsX + s_vAboutItemPos.x, s_vAboutItemPos.y);
                m_aAboutSprite[1].Resize(256.0f, 256.0f);
            }
            break;

        case CAntiqueRecord::STATE_UNIDENTIFIED:
            {
                AntiqueIdentify(m_idAntiqueSel);
            }
            break;

        case CAntiqueRecord::STATE_IDENTIFIED:
            {
                m_aAboutSprite[1].SetDisplay(true);
                m_aAboutSprite[1].SetTexture(CTextureManager::GetRwTexture(s_apszAntiqueTex[m_idAntiqueSel]));
                m_aAboutSprite[1].Move(m_fWndOfsX + s_vAboutItemPos.x, s_vAboutItemPos.y);
                m_aAboutSprite[1].Resize(256.0f, 256.0f);

                for (int32 i = 0; i < s_aAntiquePrice[m_idAntiqueSel]; ++i)
                {                    
                    m_aAboutSprite[i + 2].SetDisplay(true);
                    m_aAboutSprite[i + 2].SetTexture(CTextureManager::GetRwTexture("shop_star"));
                    m_aAboutSprite[i + 2].Move(
                        float(i) * 39.0f + m_fWndOfsX + s_vAboutStarPos.x,
                        s_vAboutStarPos.y
                    );
                    m_aAboutSprite[i + 2].Resize(32.0f, 32.0f);
                };

                m_bDispAboutAntiqText = true;
                
                m_idTextAntiqueName = s_aAntiqueText[m_idAntiqueSel];
                m_bboxTextAntiqueName = s_aAboutBox[0];
				m_bboxTextAntiqueName.x += m_fWndOfsX;

                m_idTextAntiqueComment = s_aAntiqueComment[m_idAntiqueSel];
                m_bboxTextAntiqueComment = s_aAboutBox[1];
                m_bboxTextAntiqueComment.x += m_fWndOfsX;

                m_iTextAntiqueCommentLineNum = CGameFont::CountFlowLine(
                    CGameText::GetText(m_idTextAntiqueComment),
                    m_bboxTextAntiqueComment.w
                );

                if ((m_iTextAntiqueCommentLineNum - m_iTextAntiqueCommentLineCur) > 6)
                {
                    m_aAboutSprite[8].SetDisplay(true);
                    m_aAboutSprite[8].SetTexture(CTextureManager::GetRwTexture("shop_arrow_txt_down"));
                    m_aAboutSprite[8].Move(s_vAboutArrowDPos.x + m_fWndOfsX, s_vAboutArrowDPos.y);
                    m_aAboutSprite[8].Resize(16.0f, 16.0f);

                    if (CController::GetDigitalTrigger(CGameData::Attribute().GetVirtualPad(), CController::DIGITAL_LDOWN))
                    {
                        CGameSound::PlaySE(SDCODE_SE(0x1004));
                        m_iTextAntiqueCommentLineCur += 6;
                    };
                };
                
                if (m_iTextAntiqueCommentLineCur > 0)
                {
                    m_aAboutSprite[7].SetDisplay(true);
                    m_aAboutSprite[7].SetTexture(CTextureManager::GetRwTexture("shop_arrow_txt_up"));
                    m_aAboutSprite[7].Move(s_vAboutArrowUPos.x + m_fWndOfsX, s_vAboutArrowUPos.y);
                    m_aAboutSprite[7].Resize(16.0f, 16.0f);

                    if (CController::GetDigitalTrigger(CGameData::Attribute().GetVirtualPad(), CController::DIGITAL_LUP))
                    {
                        CGameSound::PlaySE(SDCODE_SE(0x1004));                        
                        m_iTextAntiqueCommentLineCur = Max(m_iTextAntiqueCommentLineCur - 6, 0);
                    };
                };

                CGameData::Record().Antique().GetAntiqueTakenDate(ANTIQUEID::VALUE(m_idAntiqueSel), m_takendate);
                std::sprintf(
                    m_szTextAntiqueTakenDate,
                    "%02d-%02d-%02d %02d:%02d",
                    m_takendate.GetYear() - 2000,
                    m_takendate.GetMonth(),
                    m_takendate.GetDay(),
                    m_takendate.GetHour(),
                    m_takendate.GetMinute()
                );

                m_vTextAntiqueTakenDatePos = s_vGetDatePos;
                m_vTextAntiqueTakenDatePos.x += m_fWndOfsX;
                
                m_idAntiqueTakenArea = CAreaInfo::FindAntiqueArea(ANTIQUEID::VALUE(m_idAntiqueSel));
                m_bboxTextAntiqueArea = s_aAboutBox[2];
                m_bboxTextAntiqueArea.x += m_fWndOfsX;
            }
            break;

        default:
            ASSERT(false);
            break;
        };
    };
};


void CAntiqueShop::AntiqueIdentify(int32 idAntique)
{
    CGameData::Record().Antique().SetAntiqueIdentified(ANTIQUEID::VALUE(idAntique));
    AntiqueCheck();
    AntiqueDispChange();
};


void CAntiqueShop::AprilFaceChange(bool bIdentify)
{
    if (bIdentify)
    {
        int32 AntiquePrice = s_aAntiquePrice[m_idAntiqueSel];
        
        ASSERT((AntiquePrice >= 0) && (AntiquePrice < COUNT_OF(s_apszAprilFace)));
        m_pAnim2D->SetTexture(m_pszAprilFace, s_apszAprilFace[AntiquePrice]);
        
        m_pszAprilFace = s_apszAprilFace[AntiquePrice];
    }
    else
    {
        m_pAnim2D->SetTexture(m_pszAprilFace, s_apszAprilFace[0]);
        m_pszAprilFace = s_apszAprilFace[0];
    };
};


static CAntiqueShop* s_pAntiqueShop;


/*static*/ CProcess* CAntiqueShopSequence::Instance(void)
{
    return new CAntiqueShopSequence;
};


CAntiqueShopSequence::CAntiqueShopSequence(void)
: CAnim2DSequence("antique")
, m_bUnlockCheckState(false)
{
    ;
};


CAntiqueShopSequence::~CAntiqueShopSequence(void)
{
    ;
};


bool CAntiqueShopSequence::OnAttach(const void* pParam)
{
    ASSERT(!s_pAntiqueShop);
    
    s_pAntiqueShop = new CAntiqueShop;
    ASSERT(s_pAntiqueShop);
    if (s_pAntiqueShop)
    {
        s_pAntiqueShop->Attach();
        m_bUnlockCheckState = false;
        return CAnim2DSequence::OnAttach(FILEID::ID_ANTIQUE_PS2);
    };
    
    return false;
};


void CAntiqueShopSequence::OnDetach(void)
{
    if (s_pAntiqueShop)
    {
        s_pAntiqueShop->Detach();
        delete s_pAntiqueShop;
        s_pAntiqueShop = nullptr;
    };

    CGameSound::FadeOut(CGameSound::FADESPEED_SLOW);
    CAnim2DSequence::OnDetach();
};


void CAntiqueShopSequence::OnMove(bool bRet, const void* pReturnValue)
{
    switch (m_animstep)
    {
    case ANIMSTEP_FADEIN:
        {
            if (!CScreenFade::IsFading())
            {               
                if (CGameData::Record().Antique().IsNewAntiqueExisted())
                {
                    // 26 voice group
                    int32 VoxCode [] = { SDCODE_VOICE(0x6000), SDCODE_VOICE(0x6001), SDCODE_VOICE(0x6002) };
                    int32 RndSel3 = Math::Rand() % 3;
                    CGameSound::PlayVoice(VoxCode[RndSel3]);
                };
            };
        }
        break;
        
    case ANIMSTEP_DRAW:
        {
            if (s_pAntiqueShop->Move())
                BeginFadeout();
        }
        break;

    case ANIMSTEP_END:
        {
            if (!m_bUnlockCheckState)
            {
                m_bUnlockCheckState = true;
                CGameData::OnEndAntiqueShop();
                Call(PROCLABEL_SEQ_UNLOCK);
            }
            else
            {
                Ret();
            };
        }
        break;
    };

    CAnim2DSequence::OnMove(bRet, pReturnValue);
};


void CAntiqueShopSequence::OnDraw(void) const
{
    CAnim2DSequence::OnDraw();
    s_pAntiqueShop->Draw();
};


void CAntiqueShopSequence::BeginFadein(void)
{
	s_pAntiqueShop->Loaded(m_pAnimation2D);
    s_pAntiqueShop->AntiqueDispChange();
    CGameSound::PlayBGM(SDCODE_BGM(0x3023));
    CAnim2DSequence::BeginFadein();
};