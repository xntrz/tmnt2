#include "ResultSequence.hpp"

#include "Game/Component/GameMain/AreaInfo.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/NexusInfo.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/2d/Animation2D.hpp"
#include "Game/System/2d/MenuController.hpp"
#include "Game/System/DataLoader/DataLoader.hpp"
#include "Game/System/Misc/RenderStateManager.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Text/GameText.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/Configure.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/Sprite.hpp"
#include "System/Common/TextData.hpp"


class CResultWorkPool
{
public:
    enum ANIMTYPE
    {
        ANIMTYPE_NONE = 0,
        ANIMTYPE_CRYSTAL_PIECE,
        ANIMTYPE_CRYSTAL_LVLUP,
        ANIMTYPE_PERSONAL_RESULT,
        ANIMTYPE_PRIZE_CURSOR,
        ANIMTYPE_CRYSTAL_GROWING,
        ANIMTYPE_BATTLENEXUS_WINNER,
    };

public:
    CResultWorkPool(void);
    ~CResultWorkPool(void);
    void Draw(void);
    void TextureSetting(void);
    void ResultAnimAnit(ANIMTYPE animtype);
    void CrystalPieceDraw(void);
    void CrystalLvlUpDraw(void);
    void PersonalResultDraw(void);
    void PrizeCursorDraw(void);
    void CrystalGrowingDraw(void);
    void BattleNexusWinnerDraw(void);
    void ResultDataSet(void);
    int32 GetCrystalData(int32 index) const;
    int32 GetLvlUpCrystal(void);
    bool CheckLvlUpCrystal(void) const;
    bool CheckGetCrystal(void) const;
    int32 GetBattleNexusBattleNo(void) const;
    bool IsBattleNexusCleared(void) const;
    void SetPrizeNo(int32 nPrizeNo);
    
private:
    RwTexture*  m_apTexture[25];
    CSprite     m_sprite;    
    bool        m_bTextureSettingFlag;
    int32       m_aOldCry[GAMETYPES::CRYSTALTYPE_NUM - 1];
    int32       m_aAddCry[GAMETYPES::CRYSTALTYPE_NUM - 1];
    int32       m_aLvlupCry[GAMETYPES::CRYSTALTYPE_NUM - 1];
    int32       m_aCryTable[(GAMETYPES::CRYSTALTYPE_NUM - 1) * 3];
    int32       m_nLvlupCryIndex;
    ANIMTYPE    m_animtype;
    float       m_afRotate[2];
    uint32      m_auAnimCnt[4];
    bool        m_bAnimFlag;
    int32       m_nPrizeNo;
};


CResultWorkPool::CResultWorkPool(void)
: m_bTextureSettingFlag(false)
, m_nLvlupCryIndex(0)
, m_animtype(ANIMTYPE_NONE)
, m_bAnimFlag(false)
, m_nPrizeNo(-1)
{
    std::memset(m_apTexture, 0x00, sizeof(m_apTexture));
    std::memset(m_aOldCry, 0x00, sizeof(m_aOldCry));
    std::memset(m_aAddCry, 0x00, sizeof(m_aAddCry));
    std::memset(m_aLvlupCry, 0x00, sizeof(m_aLvlupCry));
    std::memset(m_aCryTable, 0x00, sizeof(m_aCryTable));
    std::memset(m_auAnimCnt, 0x00, sizeof(m_auAnimCnt));

    m_afRotate[0] = 0.0f;
    m_afRotate[1] = 0.0f;
};


CResultWorkPool::~CResultWorkPool(void)
{
    ;
};


void CResultWorkPool::Draw(void)
{
    if (!m_bTextureSettingFlag)
        return;

    RENDERSTATE_PUSH(rwRENDERSTATECULLMODE, rwCULLMODECULLNONE);

    switch (m_animtype)
    {
    case ANIMTYPE_NONE:
        break;
        
    case ANIMTYPE_CRYSTAL_PIECE:
        CrystalPieceDraw();
        break;
        
    case ANIMTYPE_CRYSTAL_LVLUP:
        CrystalLvlUpDraw();
        break;
        
    case ANIMTYPE_PERSONAL_RESULT:
        PersonalResultDraw();
        break;
        
    case ANIMTYPE_PRIZE_CURSOR:
        PrizeCursorDraw();
        break;
        
    case ANIMTYPE_CRYSTAL_GROWING:
        CrystalGrowingDraw();
        break;
        
    case ANIMTYPE_BATTLENEXUS_WINNER:
        BattleNexusWinnerDraw();
        break;
        
    default:
        ASSERT(false);
        break;
    };
};


void CResultWorkPool::TextureSetting(void)
{
    if (m_bTextureSettingFlag)
        return;

    m_bTextureSettingFlag = true;

    m_apTexture[0] = CTextureManager::GetRwTexture("res_rank_e");
    m_apTexture[1] = CTextureManager::GetRwTexture("res_rank_d");
    m_apTexture[2] = CTextureManager::GetRwTexture("res_rank_c");
    m_apTexture[3] = CTextureManager::GetRwTexture("res_rank_b");
    m_apTexture[4] = CTextureManager::GetRwTexture("res_rank_a");
    m_apTexture[5] = CTextureManager::GetRwTexture("res_rank_s");
    m_apTexture[6] = CTextureManager::GetRwTexture("res_rank_ss");
    m_apTexture[7] = CTextureManager::GetRwTexture("res_flash");
    m_apTexture[8] = CTextureManager::GetRwTexture("res_status_none");
    m_apTexture[9] = CTextureManager::GetRwTexture("res_status");
    m_apTexture[10] = CTextureManager::GetRwTexture("res_status_flash");
    m_apTexture[11] = CTextureManager::GetRwTexture("res_crystal_s_attack");
    m_apTexture[12] = CTextureManager::GetRwTexture("res_crystal_s_deffence");
    m_apTexture[13] = CTextureManager::GetRwTexture("res_crystal_s_charge");
    m_apTexture[14] = CTextureManager::GetRwTexture("res_crystal_s_shuriken");
    m_apTexture[15] = CTextureManager::GetRwTexture("res_person_e");
    m_apTexture[16] = CTextureManager::GetRwTexture("res_person_d");
    m_apTexture[17] = CTextureManager::GetRwTexture("res_person_c");
    m_apTexture[18] = CTextureManager::GetRwTexture("res_person_b");
    m_apTexture[19] = CTextureManager::GetRwTexture("res_person_a");
    m_apTexture[20] = CTextureManager::GetRwTexture("res_person_s");
    m_apTexture[21] = CTextureManager::GetRwTexture("res_person_ss");
    m_apTexture[22] = CTextureManager::GetRwTexture("res_combo_a");
    m_apTexture[23] = CTextureManager::GetRwTexture("res_combo_b");
    m_apTexture[24] = CTextureManager::GetRwTexture("res_combo_c");    
};


void CResultWorkPool::ResultAnimAnit(ANIMTYPE animtype)
{
    m_animtype = animtype;
    m_bAnimFlag = false;
    std::memset(m_auAnimCnt, 0x00, sizeof(m_auAnimCnt));
};


void CResultWorkPool::CrystalPieceDraw(void)
{
    float fDuration = ANIM_DURATION_FRAMES(30);
    if (m_auAnimCnt[0] < uint32(fDuration))
        ++m_auAnimCnt[0];

    uint8 uAlphaBasis = uint8(Math::LinearTween(0.0f, 255.0f, float(m_auAnimCnt[0]), fDuration));
    float wh = Math::LinearTween(128.0f, -64.0f, float(m_auAnimCnt[0]), fDuration);

    m_sprite.SetOffset(0.5f, 0.5f);
    m_sprite.SetRGBA(255, 255, 255, uAlphaBasis);
    m_sprite.Resize(wh, wh);

    int32 nNumAddCry = m_aAddCry[0] + m_aAddCry[1] + m_aAddCry[2] + m_aAddCry[3];

    for (int32 i = 0; i < nNumAddCry; ++i)
    {
        float dt = (float(m_auAnimCnt[0]) / fDuration);
        
        m_sprite.SetTexture(m_apTexture[m_aCryTable[i] + 11]);
        m_sprite.Move(float(i) * (dt * 64.0f) + (float(nNumAddCry) * -32.0f) + 32.0f, -50.0f);
        m_sprite.Draw();
    };
};


void CResultWorkPool::CrystalLvlUpDraw(void)
{
    int32 nCryData = (GetCrystalData(m_nLvlupCryIndex) / 10) - 1;

    float fDuration = ANIM_DURATION_FRAMES(10);

    if (m_auAnimCnt[0] < static_cast<uint32>(fDuration))
        ++m_auAnimCnt[0];

    uint8 uAlphaBasis = uint8(Math::LinearTween(0.0f, 255.0f, float(m_auAnimCnt[0]), fDuration));
    float wh = Math::LinearTween(256.0f, -192.0f, float(m_auAnimCnt[0]), fDuration);

    m_sprite.Resize(wh, wh);
    m_sprite.SetRGBA(255, 255, 255, uAlphaBasis);

    for (int32 i = 0; i < 3; ++i)
    {
        if (i >= nCryData)
            m_sprite.SetTexture(m_apTexture[8]);
        else
            m_sprite.SetTexture(m_apTexture[9]);

        m_sprite.SetOffset(1.0f - float(i) * 0.5f, 0.5f);
        m_sprite.Move(float(i) * 32.0f - 32.0f, -11.0f);
        m_sprite.Draw();
    };

    if (m_auAnimCnt[0] >= uint32(fDuration))
    {
        float fDurationSub0 = ANIM_DURATION_FRAMES(24);

        if (m_auAnimCnt[1] < uint32(fDurationSub0))
            ++m_auAnimCnt[1];

        uAlphaBasis = uint8(Math::LinearTween(0.0f, 255.0f, float(m_auAnimCnt[1]), fDurationSub0));
        wh = Math::LinearTween(256.0f, -192.0f, float(m_auAnimCnt[1]), fDurationSub0);

        m_sprite.Resize(wh, wh);
        m_sprite.SetRGBA(255, 255, 255, uAlphaBasis);
        m_sprite.SetTexture(m_apTexture[9]);
        m_sprite.SetOffset(1.0f - float(nCryData) * 0.5f, 0.5f);
        m_sprite.Move(float(nCryData) * 32.0f - 32.0f, -11.0f);
        m_sprite.Draw();

        if (m_auAnimCnt[1] >= uint32(fDurationSub0))
        {
            float fDurationSub1 = ANIM_DURATION_FRAMES(24);

            if (m_auAnimCnt[2] < uint32(fDurationSub1))
                ++m_auAnimCnt[2];

            uAlphaBasis = uint8(Math::LinearTween(255.0f, -255.0f, float(m_auAnimCnt[2]), fDurationSub1));

            m_sprite.Resize(64.0f, 64.0f);
            m_sprite.SetRGBA(255, 255, 255, uAlphaBasis);
            m_sprite.SetTexture(m_apTexture[10]);
            m_sprite.SetOffset(1.0f - float(nCryData) * 0.5f, 0.5f);
            m_sprite.Move(float(nCryData) * 32.0f - 32.0f, -11.0f);
            m_sprite.Draw();

            if (m_auAnimCnt[2] >= uint32(fDurationSub1))
            {
                float fDurationSub2 = ANIM_DURATION_FRAMES(24);

                if (m_auAnimCnt[3] < uint32(fDurationSub1))
                    ++m_auAnimCnt[3];

                uAlphaBasis = uint8(Math::LinearTween(0.0f, 255.0f, float(m_auAnimCnt[3]), fDurationSub2));

                const wchar* pwszText = CGameText::GetText(GAMETEXT_RES_CRY_LVLUP);
                float x = -(CGameFont::GetStringWidth(pwszText) * 0.5f);
                float y = Math::LinearTween(-100.0f, 10.0f, float(m_auAnimCnt[3]), fDurationSub2);
                
                CGameFont::SetHeightScaled(2.5f);
                CGameFont::SetRGBA(255, 255, 255, uAlphaBasis);
                CGameFont::Show(pwszText, x, y);
                
                pwszText = CGameText::GetText(GAMETEXT(GAMETEXT_RES_CRY_LVLUP_R + m_nLvlupCryIndex));
                x = -(CGameFont::GetStringWidth(pwszText) * 0.5f);
                y = Math::LinearTween(-70.0f, 10.0f, float(m_auAnimCnt[3]), fDurationSub2);

                CGameFont::Show(pwszText, x, y);
            };
        };
    };
};


void CResultWorkPool::PersonalResultDraw(void)
{
    float fDuration = ANIM_DURATION_FRAMES(16);

    if (m_auAnimCnt[0] < static_cast<uint32>(fDuration))
        ++m_auAnimCnt[0];

    float w = Math::LinearTween(0.0f, 32.0f, float(m_auAnimCnt[0]), fDuration);

    m_sprite.SetOffset(0.5f, 0.5f);
    m_sprite.Resize(w, 32.0f);
    m_sprite.SetRGBA(255, 255, 255, 255);

    static const int32 COLUMNS = 4;
    static const int32 ROWS = 3;

    for (int32 i = 0; i < COLUMNS; ++i)
    {
        for (int32 j = 0; j < ROWS; ++j)
        {
            int32 nEvalResult = -1;

            GAMETYPES::RESULTTYPE resulttype = CGameData::PlayResult().GetResultType();
            switch (resulttype)
            {
            case GAMETYPES::RESULTTYPE_NORMAL:
                {
                    switch (j)
                    {
                    case 0:
                        nEvalResult = CGameData::PlayResult().GetOffenseTechRank(i);
                        break;

                    case 1:
                        nEvalResult = CGameData::PlayResult().GetDefenceTechRank(i);
                        break;

                    case 2:
                        nEvalResult = CGameData::PlayResult().GetAerialTechRank(i);
                        break;

                    default:
                        break;
                    };
                }
                break;

            case GAMETYPES::RESULTTYPE_RIDE:
                {
                    switch (j)
                    {
                    case 0:
                        nEvalResult = CGameData::PlayResult().GetCoinRank(i);
                        break;

                    case 1:
                        nEvalResult = CGameData::PlayResult().GetControlRank(i);
                        break;

                    case 2:
                        if (CGameData::PlayParam().IsSpaceRide())
                            nEvalResult = CGameData::PlayResult().GetShotRank(i);
                        else
                            nEvalResult = CGameData::PlayResult().GetTrickRank(i);
                        break;
                        
                    default:
                        break;
                    };
                }
                break;

            default:
                ASSERT(false);
                break;
            };

            ASSERT(nEvalResult != -1);

            m_sprite.SetTexture(m_apTexture[nEvalResult + 14]);
            m_sprite.Move(float(i) * 106.0f - 92.0f, float(j) * 80.0f - 78.0f);
            m_sprite.Draw();
        };
    };

    if (m_auAnimCnt[0] >= uint32(fDuration))
    {
        if (!m_auAnimCnt[1])
            CGameSound::PlaySE(SDCODE_SE(4111));

        float fDurationSub = ANIM_DURATION_FRAMES(24);

        if (m_auAnimCnt[1] >= uint32(fDurationSub))
        {
            int32 nMvp = CGameData::PlayResult().GetMVP();

            RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
            RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
            RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDONE);

            float fRotSpeed = 0.05f;
            m_afRotate[0] = InvClamp(m_afRotate[0] + fRotSpeed, 0.0f, 360.0f);
            m_afRotate[1] = InvClamp(m_afRotate[1] - fRotSpeed, 0.0f, 360.0f);

            uint8 uAlphaBasis = uint8(Math::LinearTween(0.0f, 255.0f, float(m_auAnimCnt[1]), fDurationSub));

            m_sprite.SetOffset(0.5f, 0.5f);
            m_sprite.SetTexture(m_apTexture[7]);
            m_sprite.Move(float(nMvp) * 106.0f -92.0f, 141.0f);
            m_sprite.SetRGBA(255, 255, 255, uAlphaBasis);

            for (int32 i = 0; i < COUNT_OF(m_afRotate); ++i)
            {
                if (i)
                    m_sprite.Resize(128.0f, 128.0f);
                else
                    m_sprite.Resize(192.0f, 192.0f);

                m_sprite.SetRotate(m_afRotate[i]);
                m_sprite.DrawRotate();
            };
            
            RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
            RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
            RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
        }
        else
        {
            ++m_auAnimCnt[1];
        };

        float spr_w = Math::LinearTween(320.0f, -192.0f, float(m_auAnimCnt[1]), fDurationSub);
        float spr_h = Math::LinearTween(160.0f, -96.0f, float(m_auAnimCnt[1]), fDurationSub);

        m_sprite.SetOffset(0.5f, 0.5f);
        m_sprite.Resize(spr_w, spr_h);
        m_sprite.SetRGBA(255, 255, 255, 255);

        for (int32 i = 0; i < COLUMNS; ++i)
        {
            GAMETYPES::CLEARRANK personalrank = CGameData::PlayResult().GetPersonalRank(i);
            
            ASSERT(personalrank >= GAMETYPES::CLEARRANK_E);
            ASSERT(personalrank < GAMETYPES::CLEARRANK_NUM);
            
            m_sprite.SetTexture(m_apTexture[personalrank - 1]);
            m_sprite.Move(float(i) * 106.0f - 92.0f, 141.0f);
            m_sprite.Draw();
        };
    };
};


void CResultWorkPool::PrizeCursorDraw(void)
{
    if (m_nPrizeNo == -1)
        return;

    float fDuration = ANIM_DURATION_FRAMES(30);
    if (m_auAnimCnt[0] < uint32(fDuration))
    {
        ++m_auAnimCnt[0];
    }
    else
    {
        m_bAnimFlag = !m_bAnimFlag;
        m_auAnimCnt[0] = 0;
    };

    uint8 uAlphaBasis = 0;
    if (m_bAnimFlag)
        uAlphaBasis = uint8(Math::LinearTween(0.0f, 255.0f, float(m_auAnimCnt[0]), fDuration));
    else
        uAlphaBasis = uint8(Math::LinearTween(255.0f, -255.0f, float(m_auAnimCnt[0]), fDuration));

    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDONE);
    RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, 0);

    m_sprite.SetOffset(0.5f, 0.5f);
    m_sprite.Resize(float(CScreen::Width()), 32.0f);
    m_sprite.SetRGBA(0, 200, 20, uAlphaBasis);
    m_sprite.Move(0.0f, m_nPrizeNo * 42.0f - 36.0f);
    m_sprite.Draw();
    m_sprite.SetRGBA(255, 255, 255, 255);

    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
};


void CResultWorkPool::CrystalGrowingDraw(void)
{
    float fDuration = ANIM_DURATION_FRAMES(24);
    if (m_auAnimCnt[0] < uint32(fDuration))
        ++m_auAnimCnt[0];

    CSystem2D::PushRenderState();

    uint8 uAlphaBasis = uint8(Math::LinearTween(0.0f, 255.0f, float(m_auAnimCnt[0]), fDuration));
    float fOfsY = Math::LinearTween(0.0f, 10.0f, float(m_auAnimCnt[0]), fDuration);

    CGameFont::SetHeightScaled(2.0f);
    CGameFont::SetRGBA(255, 255, 255, uAlphaBasis);

    const wchar* pwszText = CGameText::GetText(GAMETEXT(GAMETEXT_GG_ATTACK + m_nLvlupCryIndex));
    float x = -(CGameFont::GetStringWidth(pwszText) * 0.5f);
    float y = fOfsY - 120.0f;
    CGameFont::Show(pwszText, x, y);

    GAMETEXT aCryText[4] =
    {
        GAMETEXT_RES_GROW_RED_1,
        GAMETEXT_RES_GROW_GREEN_1,
        GAMETEXT_RES_GROW_ORANGE_1,
        GAMETEXT_RES_GROW_WHITE_1,
    };

    Rt2dBBox bbox = {};
    bbox.x = -250.0f;
    bbox.y = fOfsY - 130.0f;
    bbox.w = 500.0f;
    bbox.h = 200.0f;

    int32 nCryData = GetCrystalData(m_nLvlupCryIndex);
    int32 nCryLevel = (nCryData / 10) - 1;

    pwszText = CGameText::GetText(GAMETEXT(nCryLevel + aCryText[m_nLvlupCryIndex]));
    CGameFont::Flow(pwszText, &bbox);

    CSystem2D::PopRenderState();

    int32 nComboIndex = -1;

    if (m_nLvlupCryIndex && nCryData)
    {
        if (m_nLvlupCryIndex || nCryData != 1)
        {
            if (m_nLvlupCryIndex == 1 && nCryData == 1)
            {
                nComboIndex = 2;
            }
            else if (m_nLvlupCryIndex == 2 && nCryData == 2)
            {
                nComboIndex = 4;
            }
            else if (m_nLvlupCryIndex == 3 && nCryData == 2)
            {
                nComboIndex = 3;
            };
        }
        else
        {
            nComboIndex = 1;
        };
    }
    else
    {
        nComboIndex = 0;
    };
    
    if (nComboIndex != -1)
    {
        int32 aComboTexIndex[5][5] =
        {
            { 0, 0, 2, -1, -1, }, // AAC
            { 0, 0, 1,  1, -1, }, // AABB
            { 0, 0, 1,  2, -1, }, // AABC
            { 0, 0, 1,  1,  1, }, // AABBB
            { 0, 0, 1,  1,  2, }, // AABBC
        };

        float wh = Math::LinearTween(384.0f, -256.0f, float(m_auAnimCnt[0]), fDuration);
        float ofs_x = 0.0f;
        
        m_sprite.SetOffset(0.5f, 0.5f);
        m_sprite.Resize(wh, wh);
        m_sprite.SetRGBA(255, 255, 255, uAlphaBasis);

        ASSERT(nComboIndex >= 0);
        ASSERT(nComboIndex < COUNT_OF(aComboTexIndex));

        for (int32 i = 0; i < COUNT_OF(aComboTexIndex[0]); ++i)
        {
            int32 nTexIndex = aComboTexIndex[nComboIndex][i];

            if (nTexIndex == -1)
                continue;
            
            m_sprite.SetTexture(m_apTexture[nTexIndex + 22]);
            m_sprite.Move(ofs_x - 220.0f, 50.0f);
            m_sprite.Draw();

            ofs_x += 105.0f;
        };
    };
};


void CResultWorkPool::BattleNexusWinnerDraw(void)
{
    float fDuration = ANIM_DURATION_FRAMES(24);
    if (m_auAnimCnt[0] < uint32(fDuration))
        ++m_auAnimCnt[0];

    CSystem2D::PushRenderState();

    uint8 uAlphaBasis = uint8(Math::LinearTween(0.0f, 255.0f, float(m_auAnimCnt[0]), fDuration));

    CGameFont::SetHeightScaled(2.5f);
    CGameFont::SetRGBA(255, 255, 255, uAlphaBasis);

    const wchar* pwszText = CGameText::GetText(GAMETEXT_RES_NEXUS_WON);
    float fOfsX = -(CGameFont::GetStringWidth(pwszText) * 0.5f);
    CGameFont::Show(pwszText, fOfsX, 70.0f);

    AREAID::VALUE idArea = CGameData::PlayParam().GetArea();
    switch (idArea)
    {
    case AREAID::ID_AREA60_A:
        pwszText = CGameText::GetText(GAMETEXT_AREA60A_NAME);
        break;

    case AREAID::ID_AREA60_B:
        pwszText = CGameText::GetText(GAMETEXT_AREA60B_NAME);
        break;

    case AREAID::ID_AREA60_C:
        pwszText = CGameText::GetText(GAMETEXT_AREA60C_NAME);
        break;

    case AREAID::ID_AREA60_D:
        pwszText = CGameText::GetText(GAMETEXT_AREA60D_NAME);
        break;

    default:
        ASSERT(false);
        break;
    };

    fOfsX = -(CGameFont::GetStringWidth(pwszText) * 0.5f);
    CGameFont::Show(pwszText, fOfsX, 100.0f);

    CSystem2D::PopRenderState();
};


void CResultWorkPool::ResultDataSet(void)
{
    m_aOldCry[0] = CGameData::Record().Item().GetCrystalNum(GAMETYPES::CRYSTALTYPE_RED);
    m_aOldCry[1] = CGameData::Record().Item().GetCrystalNum(GAMETYPES::CRYSTALTYPE_GREEN);
    m_aOldCry[2] = CGameData::Record().Item().GetCrystalNum(GAMETYPES::CRYSTALTYPE_ORANGE);
    m_aOldCry[3] = CGameData::Record().Item().GetCrystalNum(GAMETYPES::CRYSTALTYPE_WHITE);

    m_aAddCry[0] = CGameData::PlayResult().GetTakenCrystalNum(GAMETYPES::CRYSTALTYPE_RED);
    m_aAddCry[1] = CGameData::PlayResult().GetTakenCrystalNum(GAMETYPES::CRYSTALTYPE_GREEN);
    m_aAddCry[2] = CGameData::PlayResult().GetTakenCrystalNum(GAMETYPES::CRYSTALTYPE_ORANGE);
    m_aAddCry[3] = CGameData::PlayResult().GetTakenCrystalNum(GAMETYPES::CRYSTALTYPE_WHITE);

    for (int32 i = 0, j = 0; i < COUNT_OF(m_aAddCry); ++i)
    {
        if (m_aAddCry[i] > 0)
        {
            std::memset(&m_aCryTable[j], i, m_aAddCry[i]);
            j += m_aAddCry[i];
        };
    };

    std::memset(m_aLvlupCry, 0x00, sizeof(m_aLvlupCry));    

    for (int32 i = 0; i < COUNT_OF(m_aLvlupCry); ++i)
    {
        if ((m_aOldCry[i] / 10) < (GetCrystalData(i) / 10))
            ++m_aLvlupCry[i];
    };
};


int32 CResultWorkPool::GetCrystalData(int32 index) const
{
    ASSERT(index >= 0);
    ASSERT(index < COUNT_OF(m_aOldCry));

    ASSERT(index >= 0);
    ASSERT(index < COUNT_OF(m_aAddCry));

    return (m_aOldCry[index] + m_aAddCry[index]);
};


int32 CResultWorkPool::GetLvlUpCrystal(void)
{
    for (int32 i = 0; i < COUNT_OF(m_aLvlupCry); ++i)
    {
        if (m_aLvlupCry[i])
        {
            --m_aLvlupCry[i];
            m_nLvlupCryIndex = i;
            return true;
        };
    };

    m_nLvlupCryIndex = 0;
    return false;
};


bool CResultWorkPool::CheckLvlUpCrystal(void) const
{
    for (int32 i = 0; i < COUNT_OF(m_aLvlupCry); ++i)
    {
        if (m_aLvlupCry[i])
            return true;
    };

    return false;
};


bool CResultWorkPool::CheckGetCrystal(void) const
{
    for (int32 i = 0; i < COUNT_OF(m_aAddCry); ++i)
    {
        if (m_aAddCry[i])
            return true;
    };

    return false;
};


int32 CResultWorkPool::GetBattleNexusBattleNo(void) const
{
    STAGEID::VALUE idStage = CGameData::PlayParam().GetStage();
    AREAID::VALUE idArea = CGameData::PlayParam().GetArea();
    int32 nStageIndex = CAreaInfo::IndexOfStage(idArea, idStage) + 1;
    
    if (!IsBattleNexusCleared())
        --nStageIndex;
    
    return nStageIndex;
};


bool CResultWorkPool::IsBattleNexusCleared(void) const
{
    return (CGameData::PlayResult().GetAreaResult() == CGamePlayResult::AREARESULT_GAMECLEAR);
};


void CResultWorkPool::SetPrizeNo(int32 nPrizeNo)
{
    m_nPrizeNo = nPrizeNo;
};


//
// *********************************************************************************
//


/*static*/ CProcess* CResultSequence::Instance(void)
{
    return new CResultSequence;
};


CResultSequence::CResultSequence(void)
: m_pResultWorkPool(nullptr)
, m_resultType(GAMETYPES::RESULTTYPE_NORMAL)
, m_pszLvlUpCrystal(nullptr)
{
    ;
};


CResultSequence::~CResultSequence(void)
{
    ;
};


bool CResultSequence::OnAttach(const void* pParam)
{    
    m_resultType = CGameData::PlayResult().GetResultType();

    m_pResultWorkPool = new CResultWorkPool;
    m_pResultWorkPool->ResultDataSet();

    const char* pszAnimName = nullptr;
    FNAME fileID = FNAME_INVALID;

    switch (m_resultType)
    {
    case GAMETYPES::RESULTTYPE_NORMAL:
        pszAnimName = "result";
        fileID = FPATH("Common/Result/Result.lpac");
        break;

    case GAMETYPES::RESULTTYPE_RIDE:
        pszAnimName = "result_r";
        fileID = FPATH("Common/Result/Result_R.lpac");
        break;

    case GAMETYPES::RESULTTYPE_NEXUS:        
        if (CGameData::PlayResult().GetAreaResult() == CGamePlayResult::AREARESULT_GAMECLEAR)
        {
            pszAnimName = "result_n";
            fileID = FPATH("Common/Result/Result_N.lpac");
        }
        else
        {
            pszAnimName = "result_n_lose";
            fileID = FPATH("Common/Result/Result_N_Lose.lpac");
        };
        break;

    case GAMETYPES::RESULTTYPE_ENDING:
        pszAnimName = "result_e";
        fileID = FPATH("Common/Result/Result_E.lpac");
        break;

    default:
        ASSERT(false);
        break;
    };

#ifdef TMNT2_BUILD_EU
    SetAnimationName("result");
    CDataLoader::Regist(fileID);
    return CAnim2DSequence::OnAttach(FPATH_LANG("Language/English/Res/Res.lpac"));
#else /* TMNT2_BUILD_EU */
    SetAnimationName(pszAnimName);
    return CAnim2DSequence::OnAttach(fileID);
#endif /* TMNT2_BUILD_EU */
};


void CResultSequence::OnDetach(void)
{
    if (m_pResultWorkPool)
    {
        delete m_pResultWorkPool;
        m_pResultWorkPool = nullptr;
    };
    
    CGameSound::FadeOut(CGameSound::FADESPEED_NORMAL);
    CAnim2DSequence::OnDetach();
};


void CResultSequence::OnMove(bool bRet, const void* pReturnValue)
{
    CAnim2DSequence::OnMove(bRet, pReturnValue);

    switch (AnimStep())
    {
    case ANIMSTEP_FADEIN:
        {
            CMenuController::KeyLock(CController::DIGITAL_LUP);
            CMenuController::KeyLock(CController::DIGITAL_LDOWN);
        }
        break;
        
    case ANIMSTEP_DRAW:
        {
            MessageProc();
        }        
        break;

    case ANIMSTEP_END:
        {
            CMenuController::KeyUnlock(CController::DIGITAL_LUP);
            CMenuController::KeyUnlock(CController::DIGITAL_LDOWN);

            Ret();
        }
        break;

    default:
        break;
    };
};


void CResultSequence::OnDraw(void) const
{
    CAnim2DSequence::OnDraw();

    if (IsDrawing())
    {
        CRenderStateManager::SetDefault();
        m_pResultWorkPool->TextureSetting();
        m_pResultWorkPool->Draw();
        CRenderStateManager::SetDefault();
    };

#ifdef TMNT2_BUILD_EU
    CSystem2D::PushRenderState();

    CGameFont::SetHeightScaled(2.0f);
    CGameFont::SetRGBA(255, 255, 255, 255);

    const wchar* pwszText = CGameText::GetText(GAMETEXT_EU_HELP_9);

    if (CGameFont::GetStringWidth(pwszText) >= 500.0f)
    {
        Rt2dBBox bbox;
        bbox.x = -272.0f;
        bbox.y = -230.0f;
        bbox.w =  500.0f;
        bbox.h =   60.0f;

        CGameFont::Flow(pwszText, &bbox);
    }
    else
    {
        CGameFont::Show(pwszText, -272.0f, 190.0f);
    };

    CSystem2D::PopRenderState();
#endif /* TMNT2_BUILD_EU */
};


void CResultSequence::BeginFadeIn(void)
{
#ifdef TMNT2_BUILD_EU
    if (m_resultType != GAMETYPES::RESULTTYPE_ENDING)
    {
        Animation2D().SetText("RB_S_00", CGameText::GetText(GAMETEXT_RES_ANTIQ_GET));
        Animation2D().SetText("RC_S_00", CGameText::GetText(GAMETEXT_RES_CRY_GET));
        Animation2D().SetText("RC_S_01", CGameText::GetText(GAMETEXT_GG_ATTACK));
        Animation2D().SetText("RC_S_02", CGameText::GetText(GAMETEXT_GG_DEFENCE));
        Animation2D().SetText("RC_S_03", CGameText::GetText(GAMETEXT_GG_CHARGE));
        Animation2D().SetText("RC_S_04", CGameText::GetText(GAMETEXT_GG_AERIAL));
    };
#endif /* TMNT2_BUILD_EU */

    char szOrgStr[64];
    szOrgStr[0] = '\0';

    char szNewStr[64];
    szNewStr[0] = '\0';

    const char* pszBgTexName = nullptr;

    switch (m_resultType)
    {
    case GAMETYPES::RESULTTYPE_NORMAL:
        {
            pszBgTexName = "result_018";

#ifdef TMNT2_BUILD_EU
            m_pszLvlUpCrystal = "result_070";
#else /* TMNT2_BUILD_EU */
            m_pszLvlUpCrystal = "result_071";
#endif /* TMNT2_BUILD_EU */

#ifdef TMNT2_BUILD_EU            
            Animation2D().SetText("RA_T_00", CGameText::GetText(GAMETEXT_EU_RES_RANK_CLEAR));            
            Animation2D().SetText("RA_S_00", CGameText::GetText(GAMETEXT_RES_CLR_TIME));            
            Animation2D().SetText("RA_S_01", CGameText::GetText(GAMETEXT_RES_CLR_LIFE));
            Animation2D().SetText("RA_S_02", CGameText::GetText(GAMETEXT_RES_CLR_ITEM));
#endif /* TMNT2_BUILD_EU */

            const CGameTime& clearTime = CGameData::PlayResult().GetCleartimeTotal();
            std::sprintf(szNewStr,
                         "%d:%02d:%02d",
                         clearTime.GetHour(),
                         clearTime.GetMinute(),
                         clearTime.GetSecond());
            Animation2D().SetText("RA_N_00", szNewStr);

            float fRemainHPRatio = CGameData::PlayResult().GetRemainedHPRatio();
            std::sprintf(szNewStr, "%.1f%%", fRemainHPRatio);
            Animation2D().SetText("RA_N_01", szNewStr);

            float fItemTakeRatio = CGameData::PlayResult().GetItemTakeRatio();
            std::sprintf(szNewStr, "%.1f%%", fItemTakeRatio);
            Animation2D().SetText("RA_N_02", szNewStr);

#ifdef TMNT2_BUILD_EU   
            Animation2D().SetText("RD_T_00", CGameText::GetText(GAMETEXT_RES_RANK_TIT));
            Animation2D().SetText("RD_S_00", CGameText::GetText(GAMETEXT_RES_RANK_TECH));            
            Animation2D().SetText("RD_S_01", CGameText::GetText(GAMETEXT_RES_RANK_OFFENSE));            
            Animation2D().SetText("RD_S_02", CGameText::GetText(GAMETEXT_RES_RANK_DEFENCE));
            Animation2D().SetText("RD_S_03", CGameText::GetText(GAMETEXT_RES_RANK_AERIAL));
#endif /* TMNT2_BUILD_EU */
            
            for (int32 i = 0; i < GAMETYPES::CHARACTER_MAX; ++i)
            {
                const char* pszTextureName = nullptr;

                PLAYERID::VALUE idPlayer = CGameData::PlayResult().GetPlayerCharacter(i);
                switch (idPlayer)
                {
                case PLAYERID::ID_LEO: pszTextureName = "res_per_face_leo"; break;
                case PLAYERID::ID_RAP: pszTextureName = "res_per_face_rap"; break;
                case PLAYERID::ID_MIC: pszTextureName = "res_per_face_mic"; break;
                case PLAYERID::ID_DON: pszTextureName = "res_per_face_don"; break;
                case PLAYERID::ID_SLA: pszTextureName = "res_per_face_sla"; break;
                case PLAYERID::ID_CAS: pszTextureName = "res_per_face_cay"; break;
                case PLAYERID::ID_KAR: pszTextureName = "res_per_face_kar"; break;
                case PLAYERID::ID_SPL: pszTextureName = "res_per_face_spl"; break;
                default: ASSERT(false); break;
                };


                static const char* s_apszOrgTexName[] =
                {
#ifdef TMNT2_BUILD_EU
                    "result_079",
                    "result_084",
                    "result_087",
                    "result_090",
#else /* TMNT2_BUILD_EU */
                    "result_080",
                    "result_085",
                    "result_088",
                    "result_091",
#endif /* TMNT2_BUILD_EU */
                };

                static_assert(COUNT_OF(s_apszOrgTexName) == GAMETYPES::CHARACTER_MAX, "table incorrect");

                Animation2D().SetTexture(s_apszOrgTexName[i], pszTextureName);
            };
        }
        break; /* case GAMETYPES::RESULTTYPE_NORMAL */
        
    case GAMETYPES::RESULTTYPE_RIDE:
        {
            pszBgTexName = "result_r_019";

#ifdef TMNT2_BUILD_EU
            m_pszLvlUpCrystal = "result_r_081";
#else /* TMNT2_BUILD_EU */
            m_pszLvlUpCrystal = "result_r_082";
#endif /* TMNT2_BUILD_EU */

#ifdef TMNT2_BUILD_EU
            Animation2D().SetText("RRA_T_00", CGameText::GetText(GAMETEXT_EU_RES_RANK_CLEAR));
            Animation2D().SetText("RRA_S_00", CGameText::GetText(GAMETEXT_RES_RIDE_GOLD));
            Animation2D().SetText("RRA_S_01", CGameText::GetText(GAMETEXT_RES_RIDE_SILV));
            Animation2D().SetText("RRA_S_02", CGameText::GetText(GAMETEXT_RES_RIDE_POINT));
#endif /* TMNT2_BUILD_EU */

            int32 nNumGoldCoin = CGameData::PlayResult().GetRideGoldCoin();
            std::sprintf(szNewStr, "%d", nNumGoldCoin);
            Animation2D().SetText("RRA_N_00", szNewStr);

            int32 nNumSilverCoin = CGameData::PlayResult().GetRideSilverCoin();
            std::sprintf(szNewStr, "%d", nNumSilverCoin);
            Animation2D().SetText("RRA_N_01", szNewStr);

            int32 nNumPoint = CGameData::PlayResult().GetRideCoinPoint();
            std::sprintf(szNewStr, "%d", nNumPoint);
            Animation2D().SetText("RRA_N_02", szNewStr);

#ifdef TMNT2_BUILD_EU
            Animation2D().SetText("RRB_T_00", CGameText::GetText(GAMETEXT_RES_RIDE_PRIZE));
#endif /* TMNT2_BUILD_EU */

            m_pResultWorkPool->SetPrizeNo(-1);

            int32 nNumPrize = CGameData::PlayParam().GetPrizeInfoNum();
            int32 prizeNo = 0;
            for (int32 i = 0; i < nNumPrize; ++i)
            {
                bool bIsPrizeTaken = CGameData::PlayParam().PrizeInfo(i).m_bTaken;
                bool bIsPrizeComeback = (CGameData::PlayParam().PrizeInfo(i).m_PrizeType == GAMETYPES::PRIZE_COMEBACK);

                if (bIsPrizeTaken && (!bIsPrizeComeback))
                    continue;
                
                const wchar* pwszPrizeName = nullptr;

                switch (CGameData::PlayParam().PrizeInfo(i).m_PrizeType)
                {
                case GAMETYPES::PRIZE_CRYSTAL:  pwszPrizeName = CGameText::GetText(GAMETEXT_PRIZE_CRY);    break;
                case GAMETYPES::PRIZE_ANTIQUE:  pwszPrizeName = CGameText::GetText(GAMETEXT_PRIZE_ANTIQ);  break;
                case GAMETYPES::PRIZE_COMEBACK: pwszPrizeName = CGameText::GetText(GAMETEXT_PRIZE_DEKU);   break;
                case GAMETYPES::PRIZE_BONUS:    pwszPrizeName = CGameText::GetText(GAMETEXT_PRIZE_DB);     break;
                default: ASSERT(false); break;
                };

                std::sprintf(szOrgStr, "RRB_S_%02d", prizeNo);
                Animation2D().SetText(szOrgStr, pwszPrizeName);
                
                if ((CGameData::PlayParam().PrizeInfo(i).m_PrizeType == GAMETYPES::PRIZE_COMEBACK) &&
                    CGameData::PlayParam().PrizeInfo(i).m_bTaken)
                {
                    std::sprintf(szOrgStr, "RRB_N_%02d", prizeNo);
                    Animation2D().SetText(szOrgStr, "-----");
                }
                else
                {
                    std::sprintf(szOrgStr, "RRB_N_%02d", prizeNo);
                    std::sprintf(szNewStr, "%d", CGameData::PlayParam().PrizeInfo(i).m_iPointsNum);
                    Animation2D().SetText(szOrgStr, szNewStr);
                };

                if (CGameData::PlayResult().GetRideTakenPrizeNo() == i)
                    m_pResultWorkPool->SetPrizeNo(prizeNo);

                ++prizeNo;
            };

            for (int32 i = prizeNo; i < (GAMETYPES::PRIZE_NUM - 1); ++i)
            {
                std::sprintf(szOrgStr, "RRB_S_%02d", i);
                Animation2D().SetText(szOrgStr, " ");

                std::sprintf(szOrgStr, "RRB_N_%02d", i);
                Animation2D().SetText(szOrgStr, " ");
            };

#ifdef TMNT2_BUILD_EU            
            Animation2D().SetText("RRD_T_00", CGameText::GetText(GAMETEXT_RES_RANK_TIT));            
            Animation2D().SetText("RRD_S_00", CGameText::GetText(GAMETEXT_RES_RANK_TECH));            
            Animation2D().SetText("RRD_S_01", CGameText::GetText(GAMETEXT_RES_RIDE_COIN));
            Animation2D().SetText("RRD_S_02", CGameText::GetText(GAMETEXT_RES_RIDE_CTRL));
#endif /* TMNT2_BUILD_EU */

            if ((CGameData::PlayParam().GetArea() == AREAID::ID_AREA22) ||
                (CGameData::PlayParam().GetArea() == AREAID::ID_AREA32))
#ifdef TMNT2_BUILD_EU
                Animation2D().SetText("RRD_S_03", CGameText::GetText(GAMETEXT_RES_RIDE_SHOT));
            else
                Animation2D().SetText("RRD_S_03", CGameText::GetText(GAMETEXT_RES_RIDE_TRICK));
#else /* TMNT2_BUILD_EU */
                Animation2D().SetText("RIDE_PERSONAL3", CGameText::GetText(GAMETEXT_RES_RIDE_SHOT));
            else
                Animation2D().SetText("RIDE_PERSONAL3", CGameText::GetText(GAMETEXT_RES_RIDE_TRICK));
#endif /* TMNT2_BUILD_EU */

            for (int32 i = 0; i < GAMETYPES::CHARACTER_MAX; ++i)
            {
                const char* pszTextureName = nullptr;

                PLAYERID::VALUE idPlayer = CGameData::PlayResult().GetPlayerCharacter(i);
                switch (idPlayer)
                {
                case PLAYERID::ID_LEO:  pszTextureName = "res_per_face_leo";    break;
                case PLAYERID::ID_RAP:  pszTextureName = "res_per_face_rap";    break;
                case PLAYERID::ID_MIC:  pszTextureName = "res_per_face_mic";    break;
                case PLAYERID::ID_DON:  pszTextureName = "res_per_face_don";    break;
                case PLAYERID::ID_SLA:  pszTextureName = "res_per_face_sla";    break;
                case PLAYERID::ID_CAS:  pszTextureName = "res_per_face_cay";    break;
                case PLAYERID::ID_KAR:  pszTextureName = "res_per_face_kar";    break;
                case PLAYERID::ID_SPL:  pszTextureName = "res_per_face_spl";    break;
                default: ASSERT(false); break;
                };

                static const char* s_apszOrgTexName[] =
                {
#ifdef TMNT2_BUILD_EU
                    "result_r_093",
                    "result_r_098",
                    "result_r_101",
                    "result_r_104",
#else /* TMNT2_BUILD_EU */
                    "result_r_094",
                    "result_r_099",
                    "result_r_102",
                    "result_r_105",
#endif /* TMNT2_BUILD_EU */
                };

                static_assert(COUNT_OF(s_apszOrgTexName) == GAMETYPES::CHARACTER_MAX, "table incorrect");

                Animation2D().SetTexture(s_apszOrgTexName[i], pszTextureName);
            };
        }
        break; /* case GAMETYPES::RESULTTYPE_RIDE */
        
    case GAMETYPES::RESULTTYPE_NEXUS:
        {
            pszBgTexName = nullptr;

#ifdef TMNT2_BUILD_EU
            m_pszLvlUpCrystal = "result_n_070";
#else /* TMNT2_BUILD_EU */
            m_pszLvlUpCrystal = "result_n_071";
#endif /* TMNT2_BUILD_EU */

#ifdef TMNT2_BUILD_EU
            Animation2D().SetText("NRB_T_00", CGameText::GetText(GAMETEXT_RES_RIDE_PRIZE));
            Animation2D().SetText("NRD_T_00", CGameText::GetText(GAMETEXT_EU_RES_RANK_CLEAR));
            Animation2D().SetText("NRD_T_01", CGameText::GetText(GAMETEXT_RES_TIT));
            Animation2D().SetText("NRD_I_00", CGameText::GetText(GAMETEXT_EU_RES_RANK_TIME));
            Animation2D().SetText("NRD_I_01", CGameText::GetText(GAMETEXT_RES_RANK_MVP));

            switch (CConfigure::GetLanguage())
            {
            case TYPEDEF::CONFIG_LANG_ENGLISH: Animation2D().SetTexture("result_n_028", "res_nex_congrasd");   break;
            case TYPEDEF::CONFIG_LANG_GERMAN:  Animation2D().SetTexture("result_n_028", "res_nex_congrasd_g"); break;
            case TYPEDEF::CONFIG_LANG_FRENCH:  Animation2D().SetTexture("result_n_028", "res_nex_congrasd_f"); break;
            case TYPEDEF::CONFIG_LANG_SPANISH: Animation2D().SetTexture("result_n_028", "res_nex_congrasd_s"); break;
            case TYPEDEF::CONFIG_LANG_ITALIAN: Animation2D().SetTexture("result_n_028", "res_nex_congrasd_i"); break;
            default: ASSERT(false); break;
            };
#endif /* TMNT2_BUILD_EU */

            AREAID::VALUE idArea = CGameData::PlayParam().GetArea();
            switch (idArea)
            {
            case AREAID::ID_AREA60_A:
                {
                    if (CGameData::Record().Nexus().GetTournamentState(GAMETYPES::NEXUSID_KITTY_OPEN) == CNexusRecord::STATE_CLEAR)
                    {
                        Animation2D().SetText("NRB_S_00", CGameText::GetText(GAMETEXT_PRIZE_DEKU));
                        Animation2D().SetText("NRB_S_01", CGameText::GetText(GAMETEXT_EMPTY));
                    }
                    else
                    {
                        Animation2D().SetText("NRB_S_00", CGameText::GetText(GAMETEXT_PRIZE_CRY));
                        Animation2D().SetText("NRB_S_01", CGameText::GetText(GAMETEXT_AT_SEARCH));
                    };

#ifdef TMNT2_BUILD_EU
                    Animation2D().SetTexture("result_n_026", "res_nex_trophy2");
                    Animation2D().SetTexture("result_n_031", "res_nex_trophy2");
#else /* TMNT2_BUILD_EU */
                    Animation2D().SetTexture("result_n_027", "res_nex_trophy2");
                    Animation2D().SetTexture("result_n_032", "res_nex_trophy2");
#endif /* TMNT2_BUILD_EU */
                }
                break;

            case AREAID::ID_AREA60_B:
                {
                    if (CGameData::Record().Nexus().GetTournamentState(GAMETYPES::NEXUSID_MONSTER_OPEN) == CNexusRecord::STATE_CLEAR)
                        Animation2D().SetText("NRB_S_00", CGameText::GetText(GAMETEXT_PRIZE_DEKU));
                    else
                        Animation2D().SetText("NRB_S_00", CGameText::GetText(GAMETEXT_PRIZE_CRY));

                    Animation2D().SetText("NRB_S_01", CGameText::GetText(GAMETEXT_EMPTY));

#ifdef TMNT2_BUILD_EU
                    Animation2D().SetTexture("result_n_026", "res_nex_trophy2");
                    Animation2D().SetTexture("result_n_031", "res_nex_trophy2");
#else /* TMNT2_BUILD_EU */
                    Animation2D().SetTexture("result_n_027", "res_nex_trophy2");
                    Animation2D().SetTexture("result_n_032", "res_nex_trophy2");
#endif /* TMNT2_BUILD_EU */
                }
                break;

            case AREAID::ID_AREA60_C:
                {
                    if (CGameData::Record().Nexus().GetTournamentState(GAMETYPES::NEXUSID_FOOT_COMBAT) == CNexusRecord::STATE_CLEAR)
                    {
                        Animation2D().SetText("NRB_S_00", CGameText::GetText(GAMETEXT_PRIZE_DEKU));
                        Animation2D().SetText("NRB_S_01", CGameText::GetText(GAMETEXT_EMPTY));
                    }
                    else
                    {
                        Animation2D().SetText("NRB_S_00", CGameText::GetText(GAMETEXT_PRIZE_CRY));
                        Animation2D().SetText("NRB_S_01", CGameText::GetText(GAMETEXT_PRIZE_ANTIQ));
                    };

#ifdef TMNT2_BUILD_EU
                    Animation2D().SetTexture("result_n_026", "res_foot_trophy");
                    Animation2D().SetTexture("result_n_031", "res_nex_trophy_hikari2");
#else /* TMNT2_BUILD_EU */
                    Animation2D().SetTexture("result_n_027", "res_foot_trophy");
                    Animation2D().SetTexture("result_n_032", "res_nex_trophy_hikari2");
#endif /* TMNT2_BUILD_EU */
                }
                break;

            case AREAID::ID_AREA60_D:
                {
                    if (CGameData::Record().Nexus().GetTournamentState(GAMETYPES::NEXUSID_BATTLE_NEXUS) == CNexusRecord::STATE_CLEAR)
                    {
                        Animation2D().SetText("NRB_S_00", CGameText::GetText(GAMETEXT_PRIZE_DEKU));
                        Animation2D().SetText("NRB_S_01", CGameText::GetText(GAMETEXT_EMPTY));
                    }
                    else
                    {
                        Animation2D().SetText("NRB_S_00", CGameText::GetText(GAMETEXT_PRIZE_CRY));
                        Animation2D().SetText("NRB_S_01", CGameText::GetText(GAMETEXT_PRIZE_ANTIQ));
                    };
                }
                break;

            default:
                ASSERT(false);
                break;
            };

            int32 nStageNum = CAreaInfo::GetStageNum(idArea);
            
            for (int32 i = 0; i < nStageNum; ++i)
            {
                if (i >= m_pResultWorkPool->GetBattleNexusBattleNo())
                {
                    std::sprintf(szOrgStr, "NRD_S_%02d", i + 1);
                    Animation2D().SetText(szOrgStr, "-----");
                    
                    std::sprintf(szOrgStr, "NRD_N_%02d", i + 1);
                    Animation2D().SetText(szOrgStr, "-----");
                    
                    std::sprintf(szOrgStr, "NRD_S2_%02d", i + 1);
                    Animation2D().SetText(szOrgStr, "-----");
                }
                else
                {
                    char szBuff[256];
                    szBuff[0] = '\0';

                    GAMETYPES::NEXUSID idNexus = CAreaInfo::GetNexusID(idArea);
                    const wchar* pwszText = CNexusInfo::GetTeamName(idNexus, i);
                    CTextData::ToMultibyte(szBuff, sizeof(szBuff), pwszText);

                    std::sprintf(szOrgStr, "NRD_S_%02d", i + 1);
                    std::sprintf(szNewStr, "%s", szBuff);
                    Animation2D().SetText(szOrgStr, szNewStr);

                    CGameTime clearTime = CGameData::PlayResult().GetStageCleartime(i);
                    std::sprintf(szOrgStr, "NRD_N_%02d", i + 1);
                    std::sprintf(szNewStr,
                                 "%d:%02d:%02d",
                                 clearTime.GetHour(),
                                 clearTime.GetMinute(),
                                 clearTime.GetSecond());
                    Animation2D().SetText(szOrgStr, szNewStr);
                
                    const wchar* pwszMvpName = nullptr;

                    int32 mvpChrNo = CGameData::PlayResult().GetStageMVP(i);
                    PLAYERID::VALUE mvpPlayerId = CGameData::PlayResult().GetPlayerCharacter(mvpChrNo);
                    switch (mvpPlayerId)
                    {
                    case PLAYERID::ID_LEO:  pwszMvpName = CGameText::GetText(GAMETEXT_DB_CHR_LEO); break;
                    case PLAYERID::ID_RAP:  pwszMvpName = CGameText::GetText(GAMETEXT_DB_CHR_RAP); break;
                    case PLAYERID::ID_MIC:  pwszMvpName = CGameText::GetText(GAMETEXT_DB_CHR_MIC); break;
                    case PLAYERID::ID_DON:  pwszMvpName = CGameText::GetText(GAMETEXT_DB_CHR_DON); break;
                    case PLAYERID::ID_SLA:  pwszMvpName = CGameText::GetText(GAMETEXT_DB_CHR_SLA); break;
                    case PLAYERID::ID_CAS:  pwszMvpName = CGameText::GetText(GAMETEXT_DB_CHR_CAS); break;
                    case PLAYERID::ID_KAR:  pwszMvpName = CGameText::GetText(GAMETEXT_DB_CHR_KAR); break;
                    case PLAYERID::ID_SPL:  pwszMvpName = CGameText::GetText(GAMETEXT_DB_CHR_SPL); break;
                    default: ASSERT(false); break;
                    };

                    std::sprintf(szOrgStr, "NRD_S2_%02d", i + 1);
                    Animation2D().SetText(szOrgStr, pwszMvpName);
                };
            };

#ifdef TMNT2_BUILD_EU
            Animation2D().SetText("NRE_S_00", CGameText::GetText(GAMETEXT_RES_CLR_LIFE));

            float fRemainHPRatio = CGameData::PlayResult().GetRemainedHPRatio();
            std::sprintf(szNewStr, "%.1f%%", fRemainHPRatio);
            Animation2D().SetText("NRE_N_00", szNewStr);

            Animation2D().SetText("NRE_S_01", CGameText::GetText(GAMETEXT_RES_CLR_TIME));

            CGameTime clearTimeTotal = CGameData::PlayResult().GetCleartimeTotal();
            std::sprintf(szNewStr,
                         "%d:%02d:%02d",
                         clearTimeTotal.GetHour(),
                         clearTimeTotal.GetMinute(),
                         clearTimeTotal.GetSecond());
            Animation2D().SetText("NRE_N_01", szNewStr);
#else /* TMNT2_BUILD_EU */
            float fRemainHPRatio = CGameData::PlayResult().GetRemainedHPRatio();
            std::sprintf(szNewStr, "%.1f%%", fRemainHPRatio);
            Animation2D().SetText("NRE_N_01", szNewStr);

            CGameTime clearTimeTotal = CGameData::PlayResult().GetCleartimeTotal();
            std::sprintf(szNewStr,
                         "%d:%02d:%02d",
                         clearTimeTotal.GetHour(),
                         clearTimeTotal.GetMinute(),
                         clearTimeTotal.GetSecond());
            Animation2D().SetText("NRE_S_02", szNewStr);
#endif /* TMNT2_BUILD_EU */
        }
        break; /* case GAMETYPES::RESULTTYPE_NEXUS */
        
    case GAMETYPES::RESULTTYPE_ENDING:
        {
#ifdef TMNT2_BUILD_EU            
            Animation2D().SetText("ERA_T_00", CGameText::GetText(GAMETEXT_EU_RES_RANK_PERF));
            Animation2D().SetText("ERB_T_00", CGameText::GetText(GAMETEXT_RES_CLR_TIME));
            Animation2D().SetText("ERC_T_00", CGameText::GetText(GAMETEXT_RES_END_TIT));
            Animation2D().SetText("ERE_T_00", CGameText::GetText(GAMETEXT_RES_END_ACHIEVE_RATIO));
            Animation2D().SetText("ERA_S_00", CGameText::GetText(GAMETEXT_EU_RES_RANK_SS));
            Animation2D().SetText("ERA_S_01", CGameText::GetText(GAMETEXT_EU_RES_RANK_S));
            Animation2D().SetText("ERA_S_02", CGameText::GetText(GAMETEXT_EU_RES_RANK_A));
            Animation2D().SetText("ERA_S_03", CGameText::GetText(GAMETEXT_EU_RES_RANK_B));
            Animation2D().SetText("ERA_S_04", CGameText::GetText(GAMETEXT_EU_RES_RANK_C));
            Animation2D().SetText("ERA_S_05", CGameText::GetText(GAMETEXT_EU_RES_RANK_D));
            Animation2D().SetText("ERA_S_06", CGameText::GetText(GAMETEXT_EU_RES_RANK_E));
#endif /* TMNT2_BUILD_EU */

            for (int32 i = 0; i < GAMETYPES::CLEARRANK_NUM; ++i)
            {
                int32 numRankedArea = CGameData::Record().Area().CountRankedArea(GAMETYPES::CLEARRANK(i));

                std::sprintf(szOrgStr, "ERA_N_%02d", GAMETYPES::CLEARRANK_NUM - i);
                std::sprintf(szNewStr, "%d", numRankedArea);
                Animation2D().SetText(szOrgStr, szNewStr);
            };

            CGameTime clearTime;
            CGameData::Record().Area().CalcTotalClearTime(clearTime);

            std::sprintf(szNewStr,
                         "%d:%02d:%02d",
                         clearTime.GetHour(),
                         clearTime.GetMinute(),
                         clearTime.GetSecond());
            Animation2D().SetText("ERB_N_00", szNewStr);

            GAMETYPES::CLEARRANK aClearRank[2] = { GAMETYPES::CLEARRANK_NONE };
            aClearRank[0] = CGameData::Record().Area().CalcTotalClearTimeRank();
            aClearRank[1] = CGameData::Record().Area().CalcTotalClearRank();
            
            const char* apszClearRankTexName[2] = { nullptr };

            for (int32 i = 0; i < COUNT_OF(aClearRank); ++i)
            {
                switch (aClearRank[i])
                {
                case GAMETYPES::CLEARRANK_E:    apszClearRankTexName[i] = "res_rank_e";     break;                    
                case GAMETYPES::CLEARRANK_D:    apszClearRankTexName[i] = "res_rank_d";     break;                    
                case GAMETYPES::CLEARRANK_C:    apszClearRankTexName[i] = "res_rank_c";     break;                    
                case GAMETYPES::CLEARRANK_B:    apszClearRankTexName[i] = "res_rank_b";     break;                    
                case GAMETYPES::CLEARRANK_A:    apszClearRankTexName[i] = "res_rank_a";     break;                    
                case GAMETYPES::CLEARRANK_S:    apszClearRankTexName[i] = "res_rank_s";     break;                    
                case GAMETYPES::CLEARRANK_SS:   apszClearRankTexName[i] = "res_rank_ss";    break;                    
                default: ASSERT(false); break;
                };
            };

#ifdef TMNT2_BUILD_EU
            Animation2D().SetTexture("result_e_041", apszClearRankTexName[0]);
            Animation2D().SetTexture("result_e_044", apszClearRankTexName[1]);
#else /* TMNT2_BUILD_EU */
            Animation2D().SetTexture("result_e_042", apszClearRankTexName[0]);
            Animation2D().SetTexture("result_e_045", apszClearRankTexName[1]);
#endif /* TMNT2_BUILD_EU */

            int32 points = CGameData::Record().GetAchievedPoint();
            std::sprintf(szNewStr, "%d%%", points);

#ifdef TMNT2_BUILD_EU
            Animation2D().SetText("ERE_N_00", szNewStr);
#else /* TMNT2_BUILD_EU */
            Animation2D().SetText("100%", szNewStr);
#endif /* TMNT2_BUILD_EU */
        }
        break; /* case GAMETYPES::RESULTTYPE_ENDING */

    default:
        ASSERT(false);
        break;
    };

    if ((m_resultType == GAMETYPES::RESULTTYPE_NORMAL) ||
        (m_resultType == GAMETYPES::RESULTTYPE_RIDE))
    {
        ASSERT(pszBgTexName);
        
        AREAID::VALUE idArea = CGameData::PlayParam().GetArea();
        WORLDID::VALUE idWorld = CAreaInfo::GetWorldNo(idArea);
        switch (idWorld)
        {
        case WORLDID::ID_MNY: Animation2D().SetTexture(pszBgTexName, "res_bg_ny");          break;
        case WORLDID::ID_DHO: Animation2D().SetTexture(pszBgTexName, "res_bg_dhoonib");     break;
        case WORLDID::ID_TRI: Animation2D().SetTexture(pszBgTexName, "res_bg_tri");         break;
        case WORLDID::ID_JPN: Animation2D().SetTexture(pszBgTexName, "res_bg_japan");       break;
        case WORLDID::ID_FNY: Animation2D().SetTexture(pszBgTexName, "res_bg_fny");         break;
        case WORLDID::ID_KUR: Animation2D().SetTexture(pszBgTexName, "res_bg_kuraiyama");   break;
        default: ASSERT(false); break;
        };
    };

    for (int32 i = 0; i < 4; ++i)
    {
        std::sprintf(szOrgStr, "RC_N_%02d", i + 1);
        std::sprintf(szNewStr, "%02d", m_pResultWorkPool->GetCrystalData(GAMETYPES::CRYSTALTYPE(i)));
        Animation2D().SetText(szOrgStr, szNewStr);
    };

    CGameSound::PlayBGM(SDCODE_BGM(12322));
    
    CAnim2DSequence::BeginFadeIn();
};


void CResultSequence::MessageProc(void)
{
    if (Animation2D().CheckMessageGetURL("CheckAntique"))
    {
        if (CGameData::PlayResult().IsAntiqueTaken())
        {
            Animation2D().FlashKeyPress(CController::DIGITAL_LUP);
            m_pResultWorkPool->ResultAnimAnit(CResultWorkPool::ANIMTYPE_NONE);
        }
        else
        {
            Animation2D().FlashKeyPress(CController::DIGITAL_LDOWN);
        };
    }
    else if (Animation2D().CheckMessageGetURL("CheckCrystal"))
    {
        if (m_pResultWorkPool->CheckGetCrystal())
        {
            Animation2D().FlashKeyPress(CController::DIGITAL_LUP);
            m_pResultWorkPool->ResultAnimAnit(CResultWorkPool::ANIMTYPE_NONE);
        }
        else
        {
            Animation2D().FlashKeyPress(CController::DIGITAL_LDOWN);
        };
    }
    else if (Animation2D().CheckMessageGetURL("CheckCrystal10") ||
             Animation2D().CheckMessageGetURL("CheckCrystal10R"))
    {
        m_pResultWorkPool->ResultAnimAnit(CResultWorkPool::ANIMTYPE_NONE);
        if (m_pResultWorkPool->CheckLvlUpCrystal())
        {
            Animation2D().FlashKeyPress(CController::DIGITAL_LUP);
        }
        else
        {
            Animation2D().FlashKeyPress(CController::DIGITAL_LDOWN);
        };
    }
    else if (Animation2D().CheckMessageGetURL("CrystalPiece"))
    {
        m_pResultWorkPool->ResultAnimAnit(CResultWorkPool::ANIMTYPE_CRYSTAL_PIECE);
    }
    else if (Animation2D().CheckMessageGetURL("CrystalLevelUp"))
    {
        m_pResultWorkPool->ResultAnimAnit(CResultWorkPool::ANIMTYPE_CRYSTAL_LVLUP);
    }
    else if (Animation2D().CheckMessageGetURL("CrystalLevelUpEnd"))
    {
        const char* pszCryTexName = nullptr;
        
        int32 nCryNo = m_pResultWorkPool->GetLvlUpCrystal();
        switch (nCryNo + 1)
        {
        case GAMETYPES::CRYSTALTYPE_RED:
            pszCryTexName = "res_p_crystal_attack";
            break;

        case GAMETYPES::CRYSTALTYPE_GREEN:
            pszCryTexName = "res_p_crystal_deffence";
            break;

        case GAMETYPES::CRYSTALTYPE_ORANGE:
            pszCryTexName = "res_p_crystal_charge";
            break;

        case GAMETYPES::CRYSTALTYPE_WHITE:
            pszCryTexName = "res_p_crystal_shuriken";
            break;

        default:
            ASSERT(false);
            break;
        };

        Animation2D().SetTexture(m_pszLvlUpCrystal, pszCryTexName);

        m_pszLvlUpCrystal = pszCryTexName;
    }
    else if (Animation2D().CheckMessageGetURL("PersonalRank"))
    {
        m_pResultWorkPool->ResultAnimAnit(CResultWorkPool::ANIMTYPE_PERSONAL_RESULT);
    }
    else if (Animation2D().CheckMessageGetURL("PrizeCursor"))
    {
        m_pResultWorkPool->ResultAnimAnit(CResultWorkPool::ANIMTYPE_PRIZE_CURSOR);
    }
    else if (Animation2D().CheckMessageGetURL("PrizeCursorEnd") ||
             Animation2D().CheckMessageGetURL("GROWIN_START"))
    {
        m_pResultWorkPool->ResultAnimAnit(CResultWorkPool::ANIMTYPE_NONE);
    }
    else if (Animation2D().CheckMessageGetURL("GROWIN"))
    {
        m_pResultWorkPool->ResultAnimAnit(CResultWorkPool::ANIMTYPE_CRYSTAL_GROWING);
    }
    else if (Animation2D().CheckMessageGetURL("GROWIN_END"))
    {
        m_pResultWorkPool->ResultAnimAnit(CResultWorkPool::ANIMTYPE_NONE);
    }
    else if (Animation2D().CheckMessageGetURL("CUpNameStart"))
    {
        m_pResultWorkPool->ResultAnimAnit(CResultWorkPool::ANIMTYPE_BATTLENEXUS_WINNER);
    }
    else if (Animation2D().CheckMessageGetURL("CupNameEnd"))
    {
        m_pResultWorkPool->ResultAnimAnit(CResultWorkPool::ANIMTYPE_NONE);
    }
    else if (Animation2D().CheckMessageGetURL("SoundWindow"))
    {
        CGameSound::PlaySE(SDCODE_SE(4113));
    }
    else if (Animation2D().CheckMessageGetURL("SoundCrystal"))
    {
        CGameSound::PlaySE(SDCODE_SE(4119));
    }
    else if (Animation2D().CheckMessageGetURL("SoundAntique"))
    {
        CGameSound::PlaySE(SDCODE_SE(4114));
    }
    else if (Animation2D().CheckMessageGetURL("SoundCrystalLvUp"))
    {
        CGameSound::PlaySE(SDCODE_SE(4108));
    }
    else if (Animation2D().CheckMessageGetURL("ResultEnd"))
    {
        BeginFadeOut();
    };
};