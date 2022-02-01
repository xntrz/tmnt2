#include "ResultSequence.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/AreaInfo.hpp"
#include "Game/Component/GameMain/NexusInfo.hpp"
#include "Game/System/2d/GameText.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/2d/Animation2D.hpp"
#include "Game/System/2d/MenuController.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Misc/RenderStateManager.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "System/Common/Sprite.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/File/AfsFileID.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/TextData.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/System2D.hpp"


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
    RwTexture* m_apTexture[25];
    CSprite m_sprite;    
    bool m_bTextureSettingFlag;
    int32 m_aOldCry[GAMETYPES::CRYSTALTYPE_NUM - 1];
    int32 m_aAddCry[GAMETYPES::CRYSTALTYPE_NUM - 1];
    int32 m_aLvlupCry[GAMETYPES::CRYSTALTYPE_NUM - 1];
    int32 m_aCryTable[(GAMETYPES::CRYSTALTYPE_NUM - 1) * 3];
    int32 m_nLvlupCryIndex;
    ANIMTYPE m_animtype;
    float m_afRotate[2];
    uint32 m_auAnimCnt[4];
    bool m_bAnimFlag;
    int32 m_nPrizeNo;
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
    float fDuration = CScreen::Framerate() * 0.5f;
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
        m_sprite.Move(
            float(i) * (dt * 64.0f) + (float(nNumAddCry) * -32.0f) + 32.0f,
            -50.0f
        );
        m_sprite.Draw();
    };
};


void CResultWorkPool::CrystalLvlUpDraw(void)
{
    int32 nCryData = (GetCrystalData(m_nLvlupCryIndex) / 10) - 1;

	float fDuration = Math::Floor(CScreen::Framerate() * 0.16f);
    if (m_auAnimCnt[0] < uint32(fDuration))
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
        float fDurationSub0 = CScreen::Framerate() * 0.4f;

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
            float fDurationSub1 = CScreen::Framerate() * 0.4f;

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
                float fDurationSub2 = CScreen::Framerate() * 0.4f;

                if (m_auAnimCnt[3] < uint32(fDurationSub1))
                    ++m_auAnimCnt[3];

                uAlphaBasis = uint8(Math::LinearTween(0.0f, 255.0f, float(m_auAnimCnt[3]), fDurationSub2));

                const wchar* pwszText = CGameText::GetText(GAMETEXT::VALUE(790));
                float x = -(CGameFont::GetStringWidth(pwszText) * 0.5f);
                float y = Math::LinearTween(-100.0f, 10.0f, float(m_auAnimCnt[3]), fDurationSub2);
                
                CGameFont::m_fHeight = CGameFont::GetScreenSize() / 180.0f;                
                CGameFont::SetRGBA(255, 255, 255, uAlphaBasis);
                CGameFont::Show(pwszText, x, y);
                
                pwszText = CGameText::GetText(GAMETEXT::VALUE(m_nLvlupCryIndex + 791));
                x = -(CGameFont::GetStringWidth(pwszText) * 0.5f);
                y = Math::LinearTween(-70.0f, 10.0f, float(m_auAnimCnt[3]), fDurationSub2);

                CGameFont::Show(pwszText, x, y);
            };
        };
    };
};


void CResultWorkPool::PersonalResultDraw(void)
{
    float fDuration = Math::Floor(CScreen::Framerate() * 0.26f);
    if (m_auAnimCnt[0] < uint32(fDuration))
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

        float fDurationSub = CScreen::Framerate() * 0.4f;

        if (m_auAnimCnt[1] >= uint32(fDurationSub))
        {
            int32 nMvp = CGameData::PlayResult().GetMVP();

            RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
            RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
            RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDONE);

            float fRotSpeed = 0.05f;
            m_afRotate[0] = Math::InvClamp(m_afRotate[0] + fRotSpeed, 0.0f, 360.0f);
            m_afRotate[1] = Math::InvClamp(m_afRotate[1] - fRotSpeed, 0.0f, 360.0f);

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

        float w = Math::LinearTween(320.0f, -192.0f, float(m_auAnimCnt[1]), fDurationSub);
        float h = Math::LinearTween(160.0f, -96.0f, float(m_auAnimCnt[1]), fDurationSub);

        m_sprite.SetOffset(0.5f, 0.5f);
        m_sprite.Resize(w, h);
        m_sprite.SetRGBA(255, 255, 255, 255);

        for (int32 i = 0; i < COLUMNS; ++i)
        {
            GAMETYPES::CLEARRANK personalrank = CGameData::PlayResult().GetPersonalRank(i);
            ASSERT(personalrank >= GAMETYPES::CLEARRANK_E && personalrank < GAMETYPES::CLEARRANK_NUM);
            
            m_sprite.SetTexture(m_apTexture[personalrank]);
            m_sprite.Move(float(i) * 106.0f - 92.0f, 141.0f);
            m_sprite.Draw();
        };
    };
};


void CResultWorkPool::PrizeCursorDraw(void)
{
    if (m_nPrizeNo == -1)
        return;

    float fDuration = CScreen::Framerate() * 0.5f;
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
    RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, nullptr);

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
    float fDuration = CScreen::Framerate() * 0.4f;
    if (m_auAnimCnt[0] < uint32(fDuration))
        ++m_auAnimCnt[0];

    CSystem2D::PushRenderState();

	uint8 uAlphaBasis = uint8(Math::LinearTween(0.0f, 255.0f, float(m_auAnimCnt[0]), fDuration));
    float fOfsY = Math::LinearTween(0.0f, 10.0f, float(m_auAnimCnt[0]), fDuration);

    CGameFont::m_fHeight = CGameFont::GetScreenSize() / 223.0f;
    CGameFont::SetRGBA(255, 255, 255, uAlphaBasis);

    const wchar* pwszText = CGameText::GetText(GAMETEXT::VALUE(m_nLvlupCryIndex + 786));
    float x = -(CGameFont::GetStringWidth(pwszText) * 0.5f);
    float y = fOfsY - 120.0f;
    CGameFont::Show(pwszText, x, y);

    int32 aCryText[4] =
    {
        768,
        771,
        777,
        774,
    };

    Rt2dBBox bbox = { 0 };
    bbox.x = -250.0f;
    bbox.y = fOfsY - 130.0f;
    bbox.w = 500.0f;
    bbox.h = 200.0f;

    int32 nCryData = GetCrystalData(m_nLvlupCryIndex);
    int32 nCryLevel = (nCryData / 10) - 1;

    pwszText = CGameText::GetText(GAMETEXT::VALUE( nCryLevel + aCryText[m_nLvlupCryIndex] ));
    CGameFont::m_pFont->Flow(pwszText, CGameFont::m_fHeight, &bbox);

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
            0, 0, 2, -1, -1,    // AAC
            0, 0, 1,  1, -1,    // AABB
            0, 0, 1,  2, -1,    // AABC
            0, 0, 1,  1,  1,    // AABBB
            0, 0, 1,  1,  2,    // AABBC
        };

        float wh = Math::LinearTween(384.0f, -256.0f, float(m_auAnimCnt[0]), fDuration);
        float ofs_x = 0.0f;
        
        m_sprite.SetOffset(0.5f, 0.5f);
        m_sprite.Resize(wh, wh);
		m_sprite.SetRGBA(255, 255, 255, uAlphaBasis);

        ASSERT(nComboIndex >= 0 && nComboIndex < COUNT_OF(aComboTexIndex));

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
    float fDuration = CScreen::Framerate() * 0.4f;
    if (m_auAnimCnt[0] < uint32(fDuration))
        ++m_auAnimCnt[0];

    CSystem2D::PushRenderState();

	uint8 uAlphaBasis = uint8(Math::LinearTween(0.0f, 255.0f, float(m_auAnimCnt[0]), fDuration));

    CGameFont::m_fHeight = CGameFont::GetScreenSize() * 180.0f;
    CGameFont::SetRGBA(255, 255, 255, uAlphaBasis);

    const wchar* pwszText = CGameText::GetText(GAMETEXT::VALUE(812));
    float fOfsX = -(CGameFont::GetStringWidth(pwszText) * 0.5f);
    CGameFont::Show(pwszText, fOfsX, 70.0f);

    AREAID::VALUE idArea = CGameData::PlayParam().GetArea();
    switch (idArea)
    {
    case AREAID::ID_AREA60_A:
        pwszText = CGameText::GetText(GAMETEXT::VALUE(85));
        break;

    case AREAID::ID_AREA60_B:
        pwszText = CGameText::GetText(GAMETEXT::VALUE(86));
        break;

    case AREAID::ID_AREA60_C:
        pwszText = CGameText::GetText(GAMETEXT::VALUE(87));
        break;

    case AREAID::ID_AREA60_D:
        pwszText = CGameText::GetText(GAMETEXT::VALUE(88));
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
    ASSERT(index >= 0 && index < COUNT_OF(m_aOldCry));
    ASSERT(index >= 0 && index < COUNT_OF(m_aAddCry));
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
    return (CGameData::PlayResult().GetAreaResult() == CGamePlayResult::AREARESULT_CLEAR);
};


void CResultWorkPool::SetPrizeNo(int32 nPrizeNo)
{
    m_nPrizeNo = nPrizeNo;
};


/*static*/ CProcess* CResultSequence::Instance(void)
{
    return new CResultSequence;
};


CResultSequence::CResultSequence(void)
: m_pResultWorkPool(nullptr)
, m_resulttype(GAMETYPES::RESULTTYPE_NORMAL)
, m_pszLvlUpCrystal(nullptr)
{
    ;
};


CResultSequence::~CResultSequence(void)
{
    ;
};


bool CResultSequence::OnAttach(const void* param)
{    
    m_resulttype = CGameData::PlayResult().GetResultType();
    m_pResultWorkPool = new CResultWorkPool;
    ASSERT(m_pResultWorkPool);
    m_pResultWorkPool->ResultDataSet();

    bool bResult = false;
    
    switch (m_resulttype)
    {
    case GAMETYPES::RESULTTYPE_NORMAL:
        SetAnimationName("result");
        bResult = CAnim2DSequence::OnAttach(AFSFILEID::ID_RESULT);
        break;

    case GAMETYPES::RESULTTYPE_RIDE:
        SetAnimationName("result_r");
        bResult = CAnim2DSequence::OnAttach(AFSFILEID::ID_RESULT_R);
        break;

    case GAMETYPES::RESULTTYPE_NEXUS:        
        if (CGameData::PlayResult().GetAreaResult() == CGamePlayResult::AREARESULT_CLEAR)
        {
            bResult = CAnim2DSequence::OnAttach(AFSFILEID::ID_RESULT_N);
            SetAnimationName("result_n");
        }
        else
        {
            bResult = CAnim2DSequence::OnAttach(AFSFILEID::ID_RESULT_N_LOSE);
            SetAnimationName("result_n_lose");
        };
        break;

    case GAMETYPES::RESULTTYPE_ENDING:
        SetAnimationName("result_e");
        bResult = CAnim2DSequence::OnAttach(AFSFILEID::ID_RESULT_E);
        break;

    default:
        ASSERT(false);
        break;
    };

    ASSERT(bResult);

    return bResult;
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


void CResultSequence::OnMove(bool bResume, const void* param)
{
    CAnim2DSequence::OnMove(bResume, param);
    switch (m_step)
    {
    case STEP_FADE_OUT:
        CMenuController::KeyLock(CController::DIGITAL_UP);
        CMenuController::KeyLock(CController::DIGITAL_DOWN);
        break;
        
    case STEP_DRAW:
        ResultItemProc();
        break;

    case STEP_END:
        CMenuController::KeyUnlock(CController::DIGITAL_UP);
        CMenuController::KeyUnlock(CController::DIGITAL_DOWN);        
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
};


void CResultSequence::BeginFadeOut(void)
{
    const char* pszBgTexName = nullptr;
    char szOrg[32];
    char szNew[32];

    szOrg[0] = '\0';
    szNew[0] = '\0';

    switch (m_resulttype)
    {
    case GAMETYPES::RESULTTYPE_NORMAL:
        {
            m_pszLvlUpCrystal = "result_071";
            pszBgTexName = "result_018";

            const CGameTime& Cleartime = CGameData::PlayResult().GetCleartimeTotal();
            std::sprintf(szNew, "%d:%02d:%02d", Cleartime.GetHour(), Cleartime.GetMinute(), Cleartime.GetSecond());
            m_pAnimation2D->SetText("RA_N_00", szNew);

            float fRemainHPRatio = CGameData::PlayResult().GetRemainedHPRatio();
            std::sprintf(szNew, "%.1f%%", fRemainHPRatio);
            m_pAnimation2D->SetText("RA_N_01", szNew);

            float fItemTakeRatio = CGameData::PlayResult().GetItemTakeRatio();
            std::sprintf(szNew, "%.1f%%", fItemTakeRatio);
            m_pAnimation2D->SetText("RA_N_02", szNew);

            for (int32 i = 0; i < GAMETYPES::CHARACTER_MAX; ++i)
            {
                const char* pszTextureName = nullptr;

                PLAYERID::VALUE idPlayer = CGameData::PlayResult().GetPlayerCharacter(i);
                switch (idPlayer)
                {
                case PLAYERID::ID_LEO:
                    pszTextureName = "res_per_face_leo";
                    break;

                case PLAYERID::ID_RAPH:
                    pszTextureName = "res_per_face_rap";
                    break;

                case PLAYERID::ID_MIC:
                    pszTextureName = "res_per_face_mic";
                    break;

                case PLAYERID::ID_DON:
                    pszTextureName = "res_per_face_don";
                    break;

                case PLAYERID::ID_SLASHUUR:
                    pszTextureName = "res_per_face_sla";
                    break;

                case PLAYERID::ID_CASEY:
                    pszTextureName = "res_per_face_cay";
                    break;

                case PLAYERID::ID_KARAI:
                    pszTextureName = "res_per_face_kar";
                    break;

                case PLAYERID::ID_SPLINTER:
                    pszTextureName = "res_per_face_spl";
                    break;

                default:
                    ASSERT(false);
                    break;
                };

                switch (i)
                {
                case 0:
                    m_pAnimation2D->SetTexture("result_080", pszTextureName);
                    break;

                case 1:
                    m_pAnimation2D->SetTexture("result_085", pszTextureName);
                    break;

                case 2:
                    m_pAnimation2D->SetTexture("result_088", pszTextureName);
                    break;
                    
                case 3:
                    m_pAnimation2D->SetTexture("result_091", pszTextureName);
                    break;
                };
            };
        }
        break;
        
    case GAMETYPES::RESULTTYPE_RIDE:
        {
            m_pszLvlUpCrystal = "result_r_082";
            pszBgTexName = "result_r_019";
            
            int32 nNumGoldCoin = CGameData::PlayResult().GetRideGoldCoin();
            std::sprintf(szNew, "%d", nNumGoldCoin);
            m_pAnimation2D->SetText("RRA_N_00", szNew);

            int32 nNumSilverCoin = CGameData::PlayResult().GetRideSilverCoin();
            std::sprintf(szNew, "%d", nNumSilverCoin);
            m_pAnimation2D->SetText("RRA_N_01", szNew);

            int32 nNumPoint = CGameData::PlayResult().GetRideCoinPoint();
            std::sprintf(szNew, "%d", nNumPoint);
            m_pAnimation2D->SetText("RRA_N_02", szNew);

            m_pResultWorkPool->SetPrizeNo(-1);

            int32 nNumPrize = CGameData::PlayParam().GetPrizeInfoNum();
            int32 nIndex = 0;
            for (int32 i = 0; i < nNumPrize; ++i)
            {
                if (CGameData::PlayParam().PrizeInfo(i).m_bTaken && CGameData::PlayParam().PrizeInfo(i).m_PrizeType != GAMETYPES::PRIZE_COMEBACK)
                    continue;
            
                const char* pszPrizeName = nullptr;

                switch (CGameData::PlayParam().PrizeInfo(i).m_PrizeType)
                {
                case GAMETYPES::PRIZE_CRYSTAL:
                    pszPrizeName = "Piece of Crystal";
                    break;

                case GAMETYPES::PRIZE_ANTIQUE:
                    pszPrizeName = "Antique";
                    break;

                case GAMETYPES::PRIZE_COMEBACK:
                    pszPrizeName = "Doppelganger";
                    break;

                case GAMETYPES::PRIZE_BONUS:
                    pszPrizeName = "Bonus Material";
                    break;

                default:
                    ASSERT(false);
                    break;
                };
                
                std::sprintf(szOrg, "RRB_S_%02d", nIndex);
                m_pAnimation2D->SetText(szOrg, pszPrizeName);

                if (CGameData::PlayParam().PrizeInfo(i).m_PrizeType == GAMETYPES::PRIZE_COMEBACK &&
                    CGameData::PlayParam().PrizeInfo(i).m_bTaken)
                {
                    std::sprintf(szOrg, "RRB_N_%02d", nIndex);
                    m_pAnimation2D->SetText(szOrg, "---------");
                }
                else
                {
                    std::sprintf(szOrg, "RRB_N_%02d", nIndex);
                    std::sprintf(szNew, "%d", CGameData::PlayParam().PrizeInfo(i).m_iPointsNum);
                    m_pAnimation2D->SetText(szOrg, szNew);
                };

                if (CGameData::PlayResult().GetRideTakenPrizeNo() == i)
                    m_pResultWorkPool->SetPrizeNo(nIndex);

                ++nIndex;
            };

            for (int32 i = nIndex; i < GAMETYPES::PRIZE_NUM - 1; ++i)
            {
                std::sprintf(szOrg, "RRB_S_%02d", i);
                m_pAnimation2D->SetText(szOrg, " ");

                std::sprintf(szOrg, "RRB_N_%02d", i);
                m_pAnimation2D->SetText(szOrg, " ");
            };

            for (int32 i = 0; i < GAMETYPES::CHARACTER_MAX; ++i)
            {
                const char* pszTextureName = nullptr;

                PLAYERID::VALUE idPlayer = CGameData::PlayResult().GetPlayerCharacter(i);
                switch (idPlayer)
                {
                case PLAYERID::ID_LEO:
                    pszTextureName = "res_per_face_leo";
                    break;

                case PLAYERID::ID_RAPH:
                    pszTextureName = "res_per_face_rap";
                    break;

                case PLAYERID::ID_MIC:
                    pszTextureName = "res_per_face_mic";
                    break;

                case PLAYERID::ID_DON:
                    pszTextureName = "res_per_face_don";
                    break;

                case PLAYERID::ID_SLASHUUR:
                    pszTextureName = "res_per_face_sla";
                    break;

                case PLAYERID::ID_CASEY:
                    pszTextureName = "res_per_face_cay";
                    break;

                case PLAYERID::ID_KARAI:
                    pszTextureName = "res_per_face_kar";
                    break;

                case PLAYERID::ID_SPLINTER:
                    pszTextureName = "res_per_face_spl";
                    break;

                default:
                    ASSERT(false);
                    break;
                };
                
                switch (i)
                {
                case 0:
                    m_pAnimation2D->SetTexture("result_r_094", pszTextureName);
                    break;

                case 1:
                    m_pAnimation2D->SetTexture("result_r_099", pszTextureName);
                    break;

                case 2:
                    m_pAnimation2D->SetTexture("result_r_102", pszTextureName);
                    break;

                case 3:
                    m_pAnimation2D->SetTexture("result_r_105", pszTextureName);
                    break;
                };
            };
        }
        break;
        
    case GAMETYPES::RESULTTYPE_NEXUS:
        {
            m_pszLvlUpCrystal = "result_n_071";
            pszBgTexName = nullptr;

            AREAID::VALUE idArea = CGameData::PlayParam().GetArea();
            switch (idArea)
            {
            case AREAID::ID_AREA60_A:
                {
                    if (CGameData::Record().Nexus().GetTournamentState(GAMETYPES::NEXUSID_KITTY_OPEN) == CNexusRecord::STATE_CLEAR)
                    {
                        m_pAnimation2D->SetText("NRB_S_00", "Doppelganger");
						m_pAnimation2D->SetText("NRB_S_01", "");
                    }
                    else
                    {
                        m_pAnimation2D->SetText("NRB_S_00", "Piece of Crystal");
                        m_pAnimation2D->SetText("NRB_S_01", "Antique");
                    };

                    m_pAnimation2D->SetTexture("result_n_027", "res_nex_trophy2");
                    m_pAnimation2D->SetTexture("result_n_032", "res_nex_trophy2");
                }
                break;

            case AREAID::ID_AREA60_B:
                {
                    if (CGameData::Record().Nexus().GetTournamentState(GAMETYPES::NEXUSID_MONSTER_OPEN) == CNexusRecord::STATE_CLEAR)
                    {
                        m_pAnimation2D->SetText("NRB_S_00", "Doppelganger");
                    }
                    else
                    {
                        m_pAnimation2D->SetText("NRB_S_00", "Piece of Crystal");
                    };

                    m_pAnimation2D->SetText("NRB_S_01", "");

                    m_pAnimation2D->SetTexture("result_n_027", "res_nex_trophy2");
                    m_pAnimation2D->SetTexture("result_n_032", "res_nex_trophy2");
                }
                break;

            case AREAID::ID_AREA60_C:
                {
                    if (CGameData::Record().Nexus().GetTournamentState(GAMETYPES::NEXUSID_FOOT_COMBAT) == CNexusRecord::STATE_CLEAR)
                    {
                        m_pAnimation2D->SetText("NRB_S_00", "Doppelganger");
                        m_pAnimation2D->SetText("NRB_S_01", "");
                    }
                    else
                    {
                        m_pAnimation2D->SetText("NRB_S_00", "Piece of Crystal");
                        m_pAnimation2D->SetText("NRB_S_01", "Antique");
                    };

                    m_pAnimation2D->SetTexture("result_n_027", "res_foot_trophy");
                    m_pAnimation2D->SetTexture("result_n_032", "res_nex_trophy_hikari2");
                }
                break;

            case AREAID::ID_AREA60_D:
                {
                    if (CGameData::Record().Nexus().GetTournamentState(GAMETYPES::NEXUSID_BATTLE_NEXUS) == CNexusRecord::STATE_CLEAR)
                    {
                        m_pAnimation2D->SetText("NRB_S_00", "Doppelganger");
                        m_pAnimation2D->SetText("NRB_S_01", "");
                    }
                    else
                    {
                        m_pAnimation2D->SetText("NRB_S_00", "Piece of Crystal");
                        m_pAnimation2D->SetText("NRB_S_01", "Antique");
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
                    std::sprintf(szOrg, "NRD_S_%02d", i + 1);
                    m_pAnimation2D->SetText(szOrg, "---------");
                    
                    std::sprintf(szOrg, "NRD_N_%02d", i + 1);
                    m_pAnimation2D->SetText(szOrg, "---------");
                    
                    std::sprintf(szOrg, "NRD_S2_%02d", i + 1);
                    m_pAnimation2D->SetText(szOrg, "---------");
                }
                else
                {
                    char szBuff[256];
                    szBuff[0] = '\0';

                    GAMETYPES::NEXUSID idNexus = CAreaInfo::GetNexusID(idArea);
                    const wchar* pwszText = CNexusInfo::GetTeamName(idNexus, i);
                    CTextData::ToMultibyte(szBuff, sizeof(szBuff), pwszText);

                    std::sprintf(szOrg, "NRD_S_%02d", i + 1);
                    std::sprintf(szNew, "%s", szBuff);
                    m_pAnimation2D->SetText(szOrg, szNew);

                    std::sprintf(szOrg, "NRD_N_%02d", i + 1);
                    std::sprintf(szNew, "%d:%02d:%02d",
                        CGameData::PlayResult().GetStageCleartime(i).GetHour(),
                        CGameData::PlayResult().GetStageCleartime(i).GetMinute(),
                        CGameData::PlayResult().GetStageCleartime(i).GetSecond()
                    );
                    m_pAnimation2D->SetText(szOrg, szNew);
                
                    const char* pszMvpName = nullptr;
                    int32 nMvpNo = CGameData::PlayResult().GetStageMVP(i);
                    PLAYERID::VALUE idMvp = CGameData::PlayResult().GetPlayerCharacter(nMvpNo);
                    switch (idMvp)
                    {
                    case PLAYERID::ID_LEO:
                        pszMvpName = "LEONARDO";
                        break;

                    case PLAYERID::ID_RAPH:
                        pszMvpName = "RAPHAEL";
                        break;

                    case PLAYERID::ID_MIC:
                        pszMvpName = "MICHELANGELO";
                        break;

                    case PLAYERID::ID_DON:
                        pszMvpName = "DONATELLO";
                        break;

                    case PLAYERID::ID_SLASHUUR:
                        pszMvpName = "SLASHUUR";
                        break;

                    case PLAYERID::ID_CASEY:
                        pszMvpName = "CASEY";
                        break;

                    case PLAYERID::ID_KARAI:
                        pszMvpName = "KARAI";
                        break;

                    case PLAYERID::ID_SPLINTER:
                        pszMvpName = "SPLINTER";
                        break;

                    default:
                        ASSERT(false);
                        break;
                    };

                    std::sprintf(szOrg, "NRD_S2_%02d", i + 1);
                    std::sprintf(szNew, "%s", pszMvpName);
                    m_pAnimation2D->SetText(szOrg, szNew);                    
                };
            };

            float fRemainHPRatio = CGameData::PlayResult().GetRemainedHPRatio();
            std::sprintf(szNew, "%.1f%%", fRemainHPRatio);
            m_pAnimation2D->SetText("NRE_N_01", szNew);

            std::sprintf(szNew, "%d:%02d:%02d",
                CGameData::PlayResult().GetCleartimeTotal().GetHour(),
                CGameData::PlayResult().GetCleartimeTotal().GetMinute(),
                CGameData::PlayResult().GetCleartimeTotal().GetSecond()
            );
            m_pAnimation2D->SetText("NRE_S_02", szNew);            
        }
        break;
        
    case GAMETYPES::RESULTTYPE_ENDING:
        {
            for (int32 i = 0; i < GAMETYPES::CLEARRANK_NUM; ++i)
            {
                int32 nNum = CGameData::Record().Area().CountRankedArea(GAMETYPES::CLEARRANK(i));

                std::sprintf(szOrg, "ERA_N_%02d", GAMETYPES::CLEARRANK_NUM - i);
                std::sprintf(szNew, "%d", nNum);
                m_pAnimation2D->SetText(szOrg, szNew);
            };

            CGameTime ClearTime;
            CGameData::Record().Area().CalcTotalClearTime(ClearTime);

            std::sprintf(szNew, "%d:%02d:%02d", ClearTime.GetHour(), ClearTime.GetMinute(), ClearTime.GetSecond());
            m_pAnimation2D->SetText("ERB_N_00", szNew);

            GAMETYPES::CLEARRANK ClearRank[2] = { GAMETYPES::CLEARRANK_NONE };
            ClearRank[0] = CGameData::Record().Area().CalcTotalClearRank();
            ClearRank[1] = CGameData::Record().Area().CalcTotalClearTimeRank();
            
            const char* apszClearRankTexName[2] = { nullptr };

            for (int32 i = 0; i < COUNT_OF(ClearRank); ++i)
            {
                switch (ClearRank[i])
                {
                case GAMETYPES::CLEARRANK_E:
                    apszClearRankTexName[i] = "res_rank_e";
                    break;
                    
                case GAMETYPES::CLEARRANK_D:
                    apszClearRankTexName[i] = "res_rank_d";
                    break;
                    
                case GAMETYPES::CLEARRANK_C:
                    apszClearRankTexName[i] = "res_rank_c";
                    break;
                    
                case GAMETYPES::CLEARRANK_B:
                    apszClearRankTexName[i] = "res_rank_b";
                    break;
                    
                case GAMETYPES::CLEARRANK_A:
                    apszClearRankTexName[i] = "res_rank_a";
                    break;
                    
                case GAMETYPES::CLEARRANK_S:
                    apszClearRankTexName[i] = "res_rank_s";
                    break;
                    
                case GAMETYPES::CLEARRANK_SS:
                    apszClearRankTexName[i] = "res_rank_ss";
                    break;
                    
                default:
                    ASSERT(false);
                    break;
                };
            };

            m_pAnimation2D->SetTexture("result_e_042", apszClearRankTexName[1]);
            m_pAnimation2D->SetTexture("result_e_045", apszClearRankTexName[0]);

            uint32 uPoints = CGameData::Record().GetAchievedPoint();
            std::sprintf(szNew, "%u%%", uPoints);
            m_pAnimation2D->SetText("100%", szNew);
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    if (m_resulttype == GAMETYPES::RESULTTYPE_NORMAL ||
        m_resulttype == GAMETYPES::RESULTTYPE_RIDE)
    {
        ASSERT(pszBgTexName);
        
        AREAID::VALUE idArea = CGameData::PlayParam().GetArea();
        WORLDID::VALUE idWorld = CAreaInfo::GetWorldNo(idArea);
        switch (idWorld)
        {
        case WORLDID::ID_MNY:
            m_pAnimation2D->SetTexture(pszBgTexName, "res_bg_ny");
            break;

        case WORLDID::ID_DHO:
            m_pAnimation2D->SetTexture(pszBgTexName, "res_bg_dhoonib");
            break;

        case WORLDID::ID_TRI:
            m_pAnimation2D->SetTexture(pszBgTexName, "res_bg_tri");
            break;

        case WORLDID::ID_JPN:
            m_pAnimation2D->SetTexture(pszBgTexName, "res_bg_japan");
            break;

        case WORLDID::ID_FNY:
            m_pAnimation2D->SetTexture(pszBgTexName, "res_bg_fny");
            break;

        case WORLDID::ID_KUR:
            m_pAnimation2D->SetTexture(pszBgTexName, "res_bg_kuraiyama");
            break;

        default:
            ASSERT(false);
            break;
        };
    };

    for (int32 i = 0; i < 4; ++i)
    {
        std::sprintf(szOrg, "RC_N_%02d", i + 1);
        std::sprintf(szNew, "%02d", m_pResultWorkPool->GetCrystalData(GAMETYPES::CRYSTALTYPE(i)));
        m_pAnimation2D->SetText(szOrg, szNew);
    };

    CAnim2DSequence::BeginFadeOut();
    CGameSound::PlayBGM(SDCODE_BGM(12322));
};


void CResultSequence::ResultItemProc(void)
{
    if (m_pAnimation2D->CheckMessageGetURL("CheckAntique"))
    {
        if (CGameData::PlayResult().IsAntiqueTaken())
        {
            m_pAnimation2D->FlashKeyPress(CController::DIGITAL_UP);
            m_pResultWorkPool->ResultAnimAnit(CResultWorkPool::ANIMTYPE_NONE);
        }
        else
        {
            m_pAnimation2D->FlashKeyPress(CController::DIGITAL_DOWN);
        };
    }
    else if (m_pAnimation2D->CheckMessageGetURL("CheckCrystal"))
    {
        if (m_pResultWorkPool->CheckGetCrystal())
        {
            m_pAnimation2D->FlashKeyPress(CController::DIGITAL_UP);
            m_pResultWorkPool->ResultAnimAnit(CResultWorkPool::ANIMTYPE_NONE);
        }
        else
        {
            m_pAnimation2D->FlashKeyPress(CController::DIGITAL_DOWN);
        };
    }
    else if (m_pAnimation2D->CheckMessageGetURL("CheckCrystal10") ||
        m_pAnimation2D->CheckMessageGetURL("CheckCrystal10R"))
    {
        m_pResultWorkPool->ResultAnimAnit(CResultWorkPool::ANIMTYPE_NONE);
        if (m_pResultWorkPool->CheckLvlUpCrystal())
        {
            m_pAnimation2D->FlashKeyPress(CController::DIGITAL_UP);
        }
        else
        {
            m_pAnimation2D->FlashKeyPress(CController::DIGITAL_DOWN);
        };
    }
    else if (m_pAnimation2D->CheckMessageGetURL("CrystalPiece"))
    {
        m_pResultWorkPool->ResultAnimAnit(CResultWorkPool::ANIMTYPE_CRYSTAL_PIECE);
    }
    else if (m_pAnimation2D->CheckMessageGetURL("CrystalLevelUp"))
    {
        m_pResultWorkPool->ResultAnimAnit(CResultWorkPool::ANIMTYPE_CRYSTAL_LVLUP);
    }
    else if (m_pAnimation2D->CheckMessageGetURL("CrystalLevelUpEnd"))
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

        ASSERT(pszCryTexName);
        m_pAnimation2D->SetTexture(m_pszLvlUpCrystal, pszCryTexName);
        m_pszLvlUpCrystal = pszCryTexName;
    }
    else if (m_pAnimation2D->CheckMessageGetURL("PersonalRank"))
    {
        m_pResultWorkPool->ResultAnimAnit(CResultWorkPool::ANIMTYPE_PERSONAL_RESULT);
    }
    else if (m_pAnimation2D->CheckMessageGetURL("PrizeCursor"))
    {
        m_pResultWorkPool->ResultAnimAnit(CResultWorkPool::ANIMTYPE_PRIZE_CURSOR);
    }
    else if (m_pAnimation2D->CheckMessageGetURL("PrizeCursorEnd") ||
             m_pAnimation2D->CheckMessageGetURL("GROWIN_START"))
    {
        m_pResultWorkPool->ResultAnimAnit(CResultWorkPool::ANIMTYPE_NONE);
    }
    else if (m_pAnimation2D->CheckMessageGetURL("GROWIN"))
    {
        m_pResultWorkPool->ResultAnimAnit(CResultWorkPool::ANIMTYPE_CRYSTAL_GROWING);
    }
    else if (m_pAnimation2D->CheckMessageGetURL("GROWIN_END"))
    {
        m_pResultWorkPool->ResultAnimAnit(CResultWorkPool::ANIMTYPE_NONE);
    }
    else if (m_pAnimation2D->CheckMessageGetURL("CUpNameStart"))
    {
        m_pResultWorkPool->ResultAnimAnit(CResultWorkPool::ANIMTYPE_BATTLENEXUS_WINNER);
    }
    else if (m_pAnimation2D->CheckMessageGetURL("CupNameEnd"))
    {
        m_pResultWorkPool->ResultAnimAnit(CResultWorkPool::ANIMTYPE_NONE);
    }
    else if (m_pAnimation2D->CheckMessageGetURL("SoundWindow"))
    {
        CGameSound::PlaySE(SDCODE_SE(4113));
    }
    else if (m_pAnimation2D->CheckMessageGetURL("SoundCrystal"))
    {
        CGameSound::PlaySE(SDCODE_SE(4119));
    }
    else if (m_pAnimation2D->CheckMessageGetURL("SoundAntique"))
    {
        CGameSound::PlaySE(SDCODE_SE(4114));
    }
    else if (m_pAnimation2D->CheckMessageGetURL("SoundCrystalLvUp"))
    {
        CGameSound::PlaySE(SDCODE_SE(4108));
    }
    else if (m_pAnimation2D->CheckMessageGetURL("ResultEnd"))
    {
        Ret();
    };
};