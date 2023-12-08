#include "CharacterSelectSequence.hpp"

#include "Game/Component/GameData/GameData.hpp"
#include "Game/Component/GameMain/GameTypes.hpp"
#include "Game/Component/Player/PlayerID.hpp"
#include "Game/System/2d/GameFont.hpp"
#include "Game/System/Misc/ControllerMisc.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/System/Misc/RenderStateManager.hpp"
#include "Game/System/Misc/Timeout.hpp"
#include "Game/System/Sound/GameSound.hpp"
#include "Game/System/Text/GameText.hpp"
#include "Game/System/Texture/TextureManager.hpp"
#include "Game/ProcessList.hpp"
#include "System/Common/Controller.hpp"
#include "System/Common/Sprite.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/File/FileID.hpp"
#include "System/Common/RenderState.hpp"
#include "System/Common/TextData.hpp"


class CPlayerSelectWorkPool
{
private:
    enum STATUS
    {
        STATUS_NONE = 0,
        STATUS_RUNNING,
        STATUS_DECIDE,
        STATUS_CONFIRM,
        STATUS_RETURN,
    };
    
    struct PLAYERINFO
    {
        enum STATE
        {
            STATE_INACTIVE = 0,
            STATE_ACTIVE,
        };

        enum ANIMDIR
        {
            ANIMDIR_NONE = 0,
            ANIMDIR_LEFT,
            ANIMDIR_RIGHT,
        };

        STATE m_state;
        int32 m_iCursor;
        ANIMDIR m_CursorAnimDir;
        int32 m_iCursorDisplay;
        int32 m_iCharacterMax;
        int32 m_iCharacterNum;
        int32 m_iPlayerNo;
        int32 m_iController;
        PLAYERID::VALUE m_aStackPlayerID[GAMETYPES::PLAYERS_MAX];
        GAMETYPES::COSTUME m_aStackCostume[GAMETYPES::PLAYERS_MAX];
        uint32 m_uAnimPressStartFrame;
        uint32 m_uAnimPressStartDuration;
        uint32 m_uAnimCharChangeFrame;
        uint32 m_uAnimCharChangeDuration;
        bool m_bSecret;
    };

    struct CHARACTERINFO
    {
        int32 m_iLockPlayer;
        bool m_bLocked;
        bool m_bEnable;
    };

public:
    CPlayerSelectWorkPool(void);
    virtual ~CPlayerSelectWorkPool(void);
    void Attach(void);
    void Detach(void);
    void Move(void);    
    void Draw(void);
    void DrawBg(int32 iPlayerNo, bool bCheckLock);
    void DrawChr(int32 iPlayerNo);
    void DrawWaku(int32 iPlayerNo);
    void DrawIcon(int32 iPlayerNo);
    void DrawCursor(int32 iPlayerNo);
    void DrawNames(int32 iPlayerNo);
    void DrawWakuSelect(int32 iPlayerNo);
    void DrawIconEx(int32 iPlayerNo);
    void DrawPressStart(int32 iPlayerNo);
    void DrawPlayerLabels(int32 iPlayerNo);
    void DrawConfirmCheck(void);
    void TextureLoad(void);
    bool PushStackCharacter(int32 iPlayerNo);
    bool PopStackCharacter(int32 iPlayerNo);
    void UnselectCharacterSearch(int32 iPlayerNo);
    void Confirm(void);
    void LimitCheck(void);
    void StatusCheck(void);
    void PadCheck(void);
    PLAYERID::VALUE GetCharacterID(int32 iCursor) const;
    int32 GetCharacterIndex(PLAYERID::VALUE PlayerID) const;
    PLAYERINFO* GetPlayerInfo(int32 iPlayerNo);
    int32 GetRealCursor(int32 iPlayerNo) const;
    int32 GetTotalCharNum(void) const;
    bool IsCostumeTaken(int32 i, GAMETYPES::COSTUME Costume) const;
    bool IsEnded(void) const;
    bool IsConfirmed(void) const;
    bool IsReturned(void) const;

private:
    float m_fTimer;
    STATUS m_status;
    CSprite m_sprite;
    PLAYERINFO m_aPlayerInfo[GAMETYPES::PLAYERS_MAX];
    CHARACTERINFO m_aCharacterInfo[PLAYERID::ID_MAX];
    GAMETYPES::COSTUME m_aCostumeSelect[GAMETYPES::PLAYERS_MAX];    
    RwTexture* m_pTextureWaku;
    RwTexture* m_pTextureBg[5];
    RwTexture* m_pTextureChr[8];
    RwTexture* m_pTextureIconSelect;
    RwTexture* m_pTextureIcon[4];
    RwTexture* m_pTextureTurnNone;
    RwTexture* m_pTextureTurn[4];    
    RwTexture* m_pTextureName[8];    
    RwTexture* m_pTexturePressStart;
    RwTexture* m_pTextureStartP[4];    
    RwTexture* m_pTextureP[4];
    RwTexture* m_pTextureWindow;
    RwTexture* m_pTextureCursorLeft;
    RwTexture* m_pTextureCursorRight;
    RwTexture* m_pTextureCursorUpDown;
    RwTexture* m_pTextureCostumeSam;
    RwTexture* m_pTextureCostumeNex;
    int32 m_nChrMax;
    bool m_bTextureLoad;
};


CPlayerSelectWorkPool::CPlayerSelectWorkPool(void)
: m_fTimer(0.0f)
, m_status(STATUS_RUNNING)
, m_bTextureLoad(false)
, m_nChrMax(0)
{
    for (int32 i = 0; i < COUNT_OF(m_aCharacterInfo); ++i)
    {
        m_aCharacterInfo[i].m_bLocked = false;
        m_aCharacterInfo[i].m_bEnable = PLAYERID::IsEnabled(PLAYERID::VALUE(i));
        m_aCharacterInfo[i].m_iLockPlayer = -1;

        m_nChrMax = Clamp(++m_nChrMax, 0, 4);
    };

    for (int32 i = 0; i < COUNT_OF(m_aPlayerInfo); ++i)
    {
        PLAYERINFO* pPlayerInfo = &m_aPlayerInfo[i];

        std::memset(pPlayerInfo, 0x00, sizeof(*pPlayerInfo));
        
        if (i >= CController::Max())
            pPlayerInfo->m_iController = -1;
        else
            pPlayerInfo->m_iController = i;

        pPlayerInfo->m_iCursor = i;
        pPlayerInfo->m_iCursorDisplay = i;
        pPlayerInfo->m_iCharacterMax = GAMETYPES::PLAYERS_MAX;
        pPlayerInfo->m_iPlayerNo = i;

        for (int32 j = 0; j < COUNT_OF(PLAYERINFO::m_aStackCostume); ++j)
            pPlayerInfo->m_aStackCostume[j] = GAMETYPES::COSTUME_NONE;        
    }; 
};


CPlayerSelectWorkPool::~CPlayerSelectWorkPool(void)
{
    ;
};


void CPlayerSelectWorkPool::Attach(void)
{
	m_bTextureLoad = false;
    m_status = STATUS_RUNNING;
    m_fTimer = 0.0f;
};


void CPlayerSelectWorkPool::Detach(void)
{
    ;
};


void CPlayerSelectWorkPool::Move(void)
{
    m_fTimer += CScreen::TimerStride();
    
    for (int32 i = 0; i < COUNT_OF(m_aPlayerInfo); ++i)
    {
        PLAYERINFO* pPlayerInfo = &m_aPlayerInfo[i];
        int32 iController = pPlayerInfo->m_iController;

        //
        //  Check player enabled
        //
        if (iController == -1)
            continue;

        //
        //  Check if player locked
        //
        if (CController::IsLocked(iController))
            pPlayerInfo->m_state = PLAYERINFO::STATE_ACTIVE;
        else
            pPlayerInfo->m_state = PLAYERINFO::STATE_INACTIVE;

        pPlayerInfo->m_uAnimPressStartDuration = 30;
        if (pPlayerInfo->m_uAnimPressStartFrame < pPlayerInfo->m_uAnimPressStartDuration)
        {
            ++pPlayerInfo->m_uAnimPressStartFrame;
        };
        
        //
        //  Check if player active
        //
        if (pPlayerInfo->m_state != PLAYERINFO::STATE_ACTIVE)
            continue;

        if (pPlayerInfo->m_iCursorDisplay != pPlayerInfo->m_iCursor)
        {
            pPlayerInfo->m_uAnimCharChangeDuration = 15;
            if (pPlayerInfo->m_uAnimCharChangeFrame >= pPlayerInfo->m_uAnimCharChangeDuration)
                pPlayerInfo->m_iCursorDisplay = pPlayerInfo->m_iCursor;
            else
                ++pPlayerInfo->m_uAnimCharChangeFrame;
        }
		else
		{
			pPlayerInfo->m_uAnimCharChangeDuration = 15;
			pPlayerInfo->m_uAnimCharChangeFrame = 15;
		};

        //
        //  Check controller inptus
        //
        if (CController::GetDigitalTrigger(iController, CController::DIGITAL_OK))
        {
            if (m_status == STATUS_DECIDE)
            {
                m_status = STATUS_CONFIRM;
            }
            else
            {
                if (PushStackCharacter(i))
                {
                    UnselectCharacterSearch(i);
                    CGameSound::PlaySE(SDCODE_SE(4098));
                };
            };
        }
        else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_CANCEL))
        {
            CGameSound::PlaySE(SDCODE_SE(4097));
            if (m_status == STATUS_DECIDE)
                m_status = STATUS_RUNNING;

            PopStackCharacter(i);
            LimitCheck();
        }
        else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_LLEFT))
        {
            CGameSound::PlaySE(SDCODE_SE(4100));
            pPlayerInfo->m_CursorAnimDir = PLAYERINFO::ANIMDIR_LEFT;
            pPlayerInfo->m_uAnimCharChangeFrame = 0;
            pPlayerInfo->m_iCursorDisplay = pPlayerInfo->m_iCursor;

            do
            {
                pPlayerInfo->m_iCursor = InvClamp(--pPlayerInfo->m_iCursor, 0, COUNT_OF(m_aPlayerInfo) - 1);
            } while (!m_aCharacterInfo[ GetRealCursor(i) ].m_bEnable);
        }
        else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_LRIGHT))
        {
            CGameSound::PlaySE(SDCODE_SE(4100));
            pPlayerInfo->m_CursorAnimDir = PLAYERINFO::ANIMDIR_RIGHT;
            pPlayerInfo->m_uAnimCharChangeFrame = 0;
            pPlayerInfo->m_iCursorDisplay = pPlayerInfo->m_iCursor;

            do
            {
                pPlayerInfo->m_iCursor = InvClamp(++pPlayerInfo->m_iCursor, 0, COUNT_OF(m_aPlayerInfo) - 1);
            } while (!m_aCharacterInfo[ GetRealCursor(i) ].m_bEnable);
        }
        else if (CController::GetDigitalTrigger(iController, CController::DIGITAL_LUP) ||
            CController::GetDigitalTrigger(iController, CController::DIGITAL_LDOWN))
        {
            if (m_aCharacterInfo[ pPlayerInfo->m_iCursor + 4 ].m_bEnable)
                pPlayerInfo->m_bSecret = !pPlayerInfo->m_bSecret;
        }
        else if (CController::GetDigital(iController, CController::DIGITAL_L1))
        {
            m_aCostumeSelect[iController] = GAMETYPES::COSTUME_NEXUS;
        }
        else if (CController::GetDigital(iController, CController::DIGITAL_R1))
        {
            m_aCostumeSelect[iController] = GAMETYPES::COSTUME_SAMURAI;
        }
        else
        {
            m_aCostumeSelect[iController] = GAMETYPES::COSTUME_NONE;
        };
    };

    if (m_status == STATUS_RUNNING)
    {
        StatusCheck();
        PadCheck();
    };
};


void CPlayerSelectWorkPool::Draw(void)
{
    CSprite::PushRenderStates();

    for (int32 i = 0; i < COUNT_OF(m_aPlayerInfo); ++i)
    {
		bool bConnected = false;

		if (i >= CController::Max())
			bConnected = false;
		else
			bConnected = (CController::GetState(i) == CController::STATE_CONNECT);

        if (bConnected)
        {
            if (m_aPlayerInfo[i].m_state == PLAYERINFO::STATE_ACTIVE)
            {
                DrawBg(i, false);
                DrawChr(i);
                DrawBg(i, true);
                DrawWaku(i);
                DrawIcon(i);
                DrawNames(i);
				DrawWakuSelect(i);
                DrawCursor(i);
                DrawIconEx(i);
                DrawPlayerLabels(i);
            }
            else
            {
                DrawBg(i, false);
                DrawWaku(i);
                DrawPressStart(i);
            };
        }
		else
		{
			DrawBg(i, false);
			DrawWaku(i);
		};
    };

    if(m_status >= STATUS_DECIDE)
        DrawConfirmCheck();

    CSprite::PopRenderStates();
};


void CPlayerSelectWorkPool::DrawBg(int32 iPlayerNo, bool bCheckLock)
{
	PLAYERINFO* pPlayerInfo = &m_aPlayerInfo[iPlayerNo];

    if (pPlayerInfo->m_state == PLAYERINFO::STATE_ACTIVE)
    {
        if (bCheckLock)
        {
            if (!m_aCharacterInfo[pPlayerInfo->m_iCursor].m_bLocked)
                return;

            m_sprite.SetTexture(m_pTextureBg[0]);
            m_sprite.SetRGBA(55, 55, 55, 200);
        }
        else
        {
            m_sprite.SetTexture(m_pTextureBg[pPlayerInfo->m_iCursor + 1]);
            m_sprite.SetRGBA(255, 255, 255, 255);
        };
    }
    else
    {
        m_sprite.SetTexture(m_pTextureBg[0]);
        m_sprite.SetRGBA(255, 255, 255, 255);
    };
    
    m_sprite.ResetUV();
    m_sprite.SetOffset(0.5f, 0.5f);
    m_sprite.Move(-205.0f + (iPlayerNo * 140.0f), 1.0f);
    m_sprite.Resize(128.0f, 512.0f);
    m_sprite.Draw();
};


void CPlayerSelectWorkPool::DrawChr(int32 iPlayerNo)
{
    PLAYERINFO* pPlayerInfo = &m_aPlayerInfo[iPlayerNo];

    int32 iCursorOffsetMain = (pPlayerInfo->m_bSecret ? 4 : 0);
    int32 iCursorOffsetSub  = (pPlayerInfo->m_bSecret ? 0 : 4);
    
    float fAnimFrame    = float(pPlayerInfo->m_uAnimCharChangeFrame);
    float fAnimDuration = float(pPlayerInfo->m_uAnimCharChangeDuration);

    uint8 AlphaBasisOut     = uint8(Math::LinearTween(255.0f, -255.0f, fAnimFrame, fAnimDuration));
    uint8 AlphaBasisOutSub  = uint8(Math::LinearTween(127.0f, -127.0f, fAnimFrame, fAnimDuration));
    uint8 AlphaBasisIn      = uint8(Math::LinearTween(0.0f, 255.0f, fAnimFrame, fAnimDuration));
    uint8 AlphaBasisInSub   = uint8(Math::LinearTween(0.0f, 127.0f, fAnimFrame, fAnimDuration));

    float fPosX = (-205.0f + (iPlayerNo * 140.0f));
    float fPosY = 21.0f;
    float fSubOffset = 10.0f;
    float fPosSub = fPosX - fSubOffset;
	float move = 0.0f;

    if (pPlayerInfo->m_CursorAnimDir == PLAYERINFO::ANIMDIR_RIGHT)
        move = 10.0f;
    else
        move = -10.0f;

    //
    //  Move out character from current display position to right
    //  Move in charcter from left to current display position
    //
    float fPosOut = Math::LinearTween(fPosX, move, fAnimFrame, fAnimDuration);    
    float fPosIn = Math::LinearTween(fPosX + -move, move, fAnimFrame, fAnimDuration);

    //
    //  Init base for all chr portraits
    //
    m_sprite.ResetUV();
    m_sprite.Resize(128.0f, 256.0f);
    m_sprite.SetOffset(0.5f, 0.5f);

    if (AlphaBasisOut && AlphaBasisOutSub)
    {
        //
        //  Draw sub character silhouette that is fade out
        //
        m_sprite.SetTexture(m_pTextureChr[pPlayerInfo->m_iCursorDisplay + iCursorOffsetSub]);
        m_sprite.SetRGBA(0, 0, 0, AlphaBasisOutSub);
        m_sprite.Move(fPosOut - fSubOffset, fPosY);
        m_sprite.Draw();

        //
        //  Draw character portrait that is fade out
        //
        m_sprite.SetTexture(m_pTextureChr[pPlayerInfo->m_iCursorDisplay + iCursorOffsetMain]);
        m_sprite.SetRGBA(255, 255, 255, AlphaBasisOut);
        m_sprite.Move(fPosOut, fPosY);
        m_sprite.Draw();
    };

    if (AlphaBasisIn && AlphaBasisInSub)
    {
        //
        //  Draw sub character silhouette that is fade in
        //
        m_sprite.SetTexture(m_pTextureChr[pPlayerInfo->m_iCursor + iCursorOffsetSub]);
        m_sprite.SetRGBA(0, 0, 0, AlphaBasisInSub);
        m_sprite.Move(fPosIn - fSubOffset, fPosY);
        m_sprite.Draw();

        //
        //  Draw character portrait that is fade in
        //
        m_sprite.SetTexture(m_pTextureChr[pPlayerInfo->m_iCursor + iCursorOffsetMain]);
        m_sprite.SetRGBA(255, 255, 255, AlphaBasisIn);
        m_sprite.Move(fPosIn, fPosY);
        m_sprite.Draw();
    };    
};


void CPlayerSelectWorkPool::DrawWaku(int32 iPlayerNo)
{
    m_sprite.ResetUV();
    m_sprite.SetTexture(m_pTextureWaku);
    m_sprite.SetOffset(0.5f, 0.5f);
    m_sprite.SetRGBA(255, 255, 255, 255);
    m_sprite.Move(-205.0f + (iPlayerNo * 140.0f), 1.0f);
    m_sprite.Resize(256.0f, 512.0f);
    m_sprite.Draw();
};


void CPlayerSelectWorkPool::DrawIcon(int32 iPlayerNo)
{
    m_sprite.ResetUV();
    
    for (int32 i = 0; i < 4; ++i)
    {
        m_sprite.SetTexture(m_pTextureIcon[i]);
        m_sprite.SetOffset(0.5f, 0.5f);

        if (m_aCharacterInfo[i].m_bLocked)
            m_sprite.SetRGBA(55, 55, 55, 255);
        else
            m_sprite.SetRGBA(255, 255, 255, 255);

        int32 iOffset = (m_aPlayerInfo[iPlayerNo].m_bSecret ? 4 : 0);
        if (!m_aCharacterInfo[i + iOffset].m_bEnable)
            m_sprite.SetRGBA(55, 55, 55, 255);        

        m_sprite.Move((-213.0f + (iPlayerNo * 140.0f)) + i * 16.0f, -113.0f);
        m_sprite.Resize(16.0f, 16.0f);
        m_sprite.Draw();
    };

    m_sprite.SetTexture(m_pTextureIconSelect);
    m_sprite.SetOffset(0.5f, 0.5f);
    m_sprite.SetRGBA(200, 200, 200, 255);
    m_sprite.Resize(16.0f, 16.0f);
    m_sprite.Move(
        (-213.0f + (iPlayerNo * 140.0f)) + m_aPlayerInfo[iPlayerNo].m_iCursor * 16.0f,
        -113.0f
    );
    m_sprite.Draw();
};


void CPlayerSelectWorkPool::DrawCursor(int32 iPlayerNo)
{
    const float fHorOffset = 55.0f;
    const float fVerOffset = 30.0f;
    int32 iCursor = m_aPlayerInfo[iPlayerNo].m_iCursor;

    //
    //  Draw horizontal cursors
    //
    for (int32 i = 0; i < 4; ++i)
    {
        uint8 AlphaBasis = 255;
        
        if (i > 2)
        {
            AlphaBasis = uint8(127.0f - Math::Cos(m_fTimer * 13.5f) * 127.0f);
            RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
            RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
            RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDONE);
        };
        
        m_sprite.ResetUV();
        m_sprite.SetOffset(0.5f, 0.5f);
        m_sprite.SetRGBA(255, 255, 255, AlphaBasis);
        m_sprite.Resize(32.0f, 32.0f);
        
        m_sprite.SetTexture(m_pTextureCursorLeft);
        m_sprite.Move((-205.0f + (iPlayerNo * 140.0f)) - fHorOffset, 15.0f);
        m_sprite.Draw();

        m_sprite.SetTexture(m_pTextureCursorRight);
        m_sprite.Move((-205.0f + (iPlayerNo * 140.0f)) + fHorOffset, 15.0f);
        m_sprite.Draw();

        if (i > 2)
        {
            RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
            RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
            RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
        };
    };

    ASSERT(iCursor >= 0 && iCursor < (COUNT_OF(m_aCharacterInfo) / 2));
    if (!m_aCharacterInfo[4 + iCursor].m_bEnable)
        return;

    //
    //  Draw vertical cursors
    //
    for (int32 i = 0; i < 4; ++i)
    {
        uint8 AlphaBasis = 255;

        if (i > 2)
        {
            AlphaBasis = uint8(127.0f - Math::Cos(m_fTimer * 13.5f) * 127.0f);
            RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
            RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
            RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDONE);
        };
        
        m_sprite.ResetUV();
        m_sprite.SetRGBA(255, 255, 255, AlphaBasis);
        m_sprite.SetTexture(m_pTextureCursorUpDown);
        m_sprite.Resize(16.0f, 16.0f);
        m_sprite.SetOffset(0.5f, 0.5f);

        m_sprite.SetUV(0.0f, 0.0f, 0.5f, 1.0f);
        m_sprite.Move(-205.0f + (iPlayerNo * 140.0f), 3.0f);
        m_sprite.Draw();

        m_sprite.SetUV(0.5f, 0.0f, 1.0f, 1.0f);
        m_sprite.Move(-205.0f + (iPlayerNo * 140.0f), 33.0f);
        m_sprite.Draw();

        if (i > 2)
        {
            RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
            RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
            RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
        };
    };
};


void CPlayerSelectWorkPool::DrawNames(int32 iPlayerNo)
{
    m_sprite.ResetUV();
    m_sprite.SetTexture(m_pTextureName[GetRealCursor(iPlayerNo)]);
    m_sprite.SetOffset(0.5f, 0.5f);
    m_sprite.SetRGBA(255, 255, 255, 255);
    m_sprite.Resize(128.0f, 16.0f);
    m_sprite.Move(-205.0f + (iPlayerNo * 140.0f), 18.0f);
    m_sprite.Draw();
};


void CPlayerSelectWorkPool::DrawWakuSelect(int32 iPlayerNo)
{
    static const RwV2d s_aTurnAnchors[GAMETYPES::PLAYERS_MAX] =
    {
        { -214.0f, 50.0f    },
        { -201.0f, 83.0f    },
        { -215.0f, 110.0f   },
        { -207.0f, 139.0f   },
    };

    m_sprite.ResetUV();
    m_sprite.SetTexture(m_pTextureTurnNone);
    m_sprite.Resize(128.0f, 64.0f);
    m_sprite.SetOffset(0.5f, 0.5f);

    PLAYERINFO* pPlayerInfo = GetPlayerInfo(iPlayerNo);
    
    if ((pPlayerInfo->m_iCharacterNum < pPlayerInfo->m_iCharacterMax) &&
        (GetTotalCharNum() < COUNT_OF(m_aPlayerInfo)))
    {
        m_sprite.SetRGBA(255, 255, 255, 255);
        m_sprite.SetTexture(m_pTextureTurnNone);
        m_sprite.SetUV(
            0.0f,
            (pPlayerInfo->m_iCharacterNum * 64.0f) * (1.0f / 256.0f),
            1.0f,
            ((pPlayerInfo->m_iCharacterNum * 64.0f) + 64.0f) * (1.0f / 256.0f)
        );
        m_sprite.Move(
            s_aTurnAnchors[pPlayerInfo->m_iCharacterNum].x + (iPlayerNo * 140.0f),
            s_aTurnAnchors[pPlayerInfo->m_iCharacterNum].y
        );
        m_sprite.Draw();

        //
        //  Draw with additive alpha blending
        //
        RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
        RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
        RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDONE);
        m_sprite.SetRGBA(
            255,
            255,
            255,
            uint8(127.0f - Math::Cos(m_fTimer * 13.5f) * 127.0f)
        );
        m_sprite.Draw();
        RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
        RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
        RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    };


    for (int32 i = 0; i < pPlayerInfo->m_iCharacterNum; ++i)
    {
        float fTextureOffset = 0.0f;
        int32 iCharacterNo = GetCharacterIndex(pPlayerInfo->m_aStackPlayerID[i]);
        
        if (iCharacterNo >= 4)
        {
            iCharacterNo -= 4;
            fTextureOffset = 256.0f;
        };

        m_sprite.ResetUV();
        m_sprite.SetTexture(m_pTextureTurn[iCharacterNo]);
        m_sprite.SetRGBA(255, 255, 255, 255);
        m_sprite.SetUV(
            0.0f,
            ((i * 64.0f) + fTextureOffset) * (1.0f / 512.0f),
            1.0f,
            (((i * 64.0f) + fTextureOffset) + 64.0f) * (1.0f / 512.0f)
        );
        m_sprite.Move(
            s_aTurnAnchors[i].x + (iPlayerNo * 140.0f),
            s_aTurnAnchors[i].y
        );
        m_sprite.Draw();
    };
};


void CPlayerSelectWorkPool::DrawIconEx(int32 iPlayerNo)
{
    static const float fVerOffset = 35.0f;
    static const float fHorOffset = 22.5f;

    if (m_aCharacterInfo[m_aPlayerInfo[iPlayerNo].m_iCursor].m_bLocked)
        return;

    if (GetRealCursor(iPlayerNo) >= 4)
        return;

    m_sprite.ResetUV();
    m_sprite.SetOffset(0.5f, 0.5f);
    m_sprite.Resize(32.0f, 32.0f);

    int32 iCursor = m_aPlayerInfo[iPlayerNo].m_iCursor;

    if (IsCostumeTaken(iCursor, GAMETYPES::COSTUME_NEXUS))
    {
        m_sprite.SetTexture(m_pTextureCostumeNex);
        m_sprite.Move((-228.0f + (iPlayerNo * 140.0f)), -14.0f);
        if (m_aCostumeSelect[iPlayerNo] == GAMETYPES::COSTUME_NEXUS)
            m_sprite.SetRGBA(255, 255, 255, 255);
        else
            m_sprite.SetRGBA(0, 0, 0, 255);
        m_sprite.Draw();
    };

    if (IsCostumeTaken(iCursor, GAMETYPES::COSTUME_SAMURAI))
    {
        m_sprite.SetTexture(m_pTextureCostumeSam);
        m_sprite.Move((-185.0f + (iPlayerNo * 140.0f)), -14.0f);
        if (m_aCostumeSelect[iPlayerNo] == GAMETYPES::COSTUME_SAMURAI)
            m_sprite.SetRGBA(255, 255, 255, 255);
        else
            m_sprite.SetRGBA(0, 0, 0, 255);
        m_sprite.Draw();
    };    
};


void CPlayerSelectWorkPool::DrawPressStart(int32 iPlayerNo)
{    
    float fPressStartWH = 128.0f;

    PLAYERINFO* pPlayerInfo = GetPlayerInfo(iPlayerNo);
    
    fPressStartWH = Math::LinearTween(
        256.0f + 128.0f,
        -256.0f,
        float(pPlayerInfo->m_uAnimPressStartFrame),
        float(pPlayerInfo->m_uAnimPressStartDuration)
    );

    m_sprite.SetTexture(m_pTexturePressStart);
    m_sprite.SetOffset(0.5f, 0.5f);
    m_sprite.SetRGBA(255, 255, 255, 255);
    m_sprite.Move(-205.0f + (iPlayerNo * 140.0f), -53.0f);
    m_sprite.Resize(fPressStartWH, fPressStartWH);
    m_sprite.Draw();

    //
    //  Additive alpha blending
    //
    RENDERSTATE_PUSH(rwRENDERSTATEZWRITEENABLE, false);
    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDONE);   

    m_sprite.SetRGBA(
        255,
        255,
        255,
        uint8(127.0f - Math::Cos(m_fTimer * 13.5f) * 127.0f)
    );
    m_sprite.Draw();

    RENDERSTATE_POP(rwRENDERSTATEZWRITEENABLE);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);
    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);

    m_sprite.SetTexture(m_pTextureStartP[iPlayerNo]);
    m_sprite.SetOffset(0.5f, 0.5f);
    m_sprite.SetRGBA(255, 255, 255, 255);
    m_sprite.Move(-205.0f + (iPlayerNo * 140.0f), 16.0f);
    m_sprite.Resize(128.0f, 64.0f);
    m_sprite.Draw();
};


void CPlayerSelectWorkPool::DrawPlayerLabels(int32 iPlayerNo)
{
    m_sprite.ResetUV();
    m_sprite.SetOffset(0.5f, 0.5f);
    m_sprite.SetRGBA(255, 255, 255, 255);
    m_sprite.SetTexture(m_pTextureP[iPlayerNo]);
    m_sprite.Move(-248.0f + (iPlayerNo * 140.0f), -132.0f);
    m_sprite.Resize(64.0f, 32.0f);
    m_sprite.Draw();
};


void CPlayerSelectWorkPool::DrawConfirmCheck(void)
{
    m_sprite.ResetUV();
    m_sprite.SetTexture(m_pTextureWindow);
    m_sprite.SetOffset(0.5f, 0.5f);
    m_sprite.Move(0.0f, -60.0f);
    m_sprite.SetRGBA(255, 255, 255, 255);
    m_sprite.Resize(CSprite::m_fVirtualScreenW, 128.0f);
    m_sprite.Draw();

    CGameFont::SetRGBA(255, 255, 255, 255);

	CGameFont::SetHeight(CGameFont::GetScreenHeightEx(TYPEDEF::VSCR_H / 2.5f));
    CGameFont::Show(CGameText::GetText(GAMETEXT(762)), -220.0f, -80.0f);

    const wchar* pwszOk = CGameText::GetText(GAMETEXT(1226));
    const wchar* pwszCancel = CGameText::GetText(GAMETEXT(9));

    wchar wszBuffer[256];
    wszBuffer[0] = UTEXT('\0');

    CTextData::StrCpy(wszBuffer, pwszOk);
	CTextData::StrCat(wszBuffer, UTEXT("   "));
    CTextData::StrCat(wszBuffer, pwszCancel);

	CGameFont::SetHeight(CGameFont::GetScreenHeightEx(TYPEDEF::VSCR_H / 2.0f));
    CGameFont::Show(wszBuffer, -200.0f, -50.0f);
};


void CPlayerSelectWorkPool::TextureLoad(void)
{
    if (m_bTextureLoad)
        return;
    
    CTextureManager::SetCurrentTextureSet("charselect");
    
    m_pTextureWaku          = CTextureManager::GetRwTexture("chsel_waku");
    m_pTextureBg[0]         = CTextureManager::GetRwTexture("chsel_chbg_none");
    m_pTextureBg[1]         = CTextureManager::GetRwTexture("chsel_chbg_leo");
    m_pTextureBg[2]         = CTextureManager::GetRwTexture("chsel_chbg_rap");
    m_pTextureBg[3]         = CTextureManager::GetRwTexture("chsel_chbg_mic");
    m_pTextureBg[4]         = CTextureManager::GetRwTexture("chsel_chbg_don");
    m_pTextureChr[0]        = CTextureManager::GetRwTexture("chsel_chara_leo");
    m_pTextureChr[1]        = CTextureManager::GetRwTexture("chsel_chara_rap");
    m_pTextureChr[2]        = CTextureManager::GetRwTexture("chsel_chara_mic");
    m_pTextureChr[3]        = CTextureManager::GetRwTexture("chsel_chara_don");
    m_pTextureChr[4]        = CTextureManager::GetRwTexture("chsel_chara_sla");
    m_pTextureChr[5]        = CTextureManager::GetRwTexture("chsel_chara_cas");
    m_pTextureChr[6]        = CTextureManager::GetRwTexture("chsel_chara_kar");
    m_pTextureChr[7]        = CTextureManager::GetRwTexture("chsel_chara_spl");
    m_pTextureIconSelect    = CTextureManager::GetRwTexture("chsel_icon_select");
    m_pTextureIcon[0]       = CTextureManager::GetRwTexture("chsel_icon_leo");
    m_pTextureIcon[1]       = CTextureManager::GetRwTexture("chsel_icon_rap");
    m_pTextureIcon[2]       = CTextureManager::GetRwTexture("chsel_icon_mic");
    m_pTextureIcon[3]       = CTextureManager::GetRwTexture("chsel_icon_don");
    m_pTextureTurnNone      = CTextureManager::GetRwTexture("chsel_turn_none");
    m_pTextureTurn[0]       = CTextureManager::GetRwTexture("chsel_turn_leo");
    m_pTextureTurn[1]       = CTextureManager::GetRwTexture("chsel_turn_rap");
    m_pTextureTurn[2]       = CTextureManager::GetRwTexture("chsel_turn_mic");
    m_pTextureTurn[3]       = CTextureManager::GetRwTexture("chsel_turn_don");
    m_pTextureName[0]       = CTextureManager::GetRwTexture("chsel_name_leo");
    m_pTextureName[1]       = CTextureManager::GetRwTexture("chsel_name_rap");
    m_pTextureName[2]       = CTextureManager::GetRwTexture("chsel_name_mic");
    m_pTextureName[3]       = CTextureManager::GetRwTexture("chsel_name_don");
    m_pTextureName[4]       = CTextureManager::GetRwTexture("chsel_name_sla");
    m_pTextureName[5]       = CTextureManager::GetRwTexture("chsel_name_cas");
    m_pTextureName[6]       = CTextureManager::GetRwTexture("chsel_name_kar");
    m_pTextureName[7]       = CTextureManager::GetRwTexture("chsel_name_spl");    
#ifdef TARGET_PC    
    m_pTexturePressStart    = CTextureManager::GetRwTexture("chsel_pressstart_ps");
#else
#error Not implemented for current target
#endif    
    m_pTextureStartP[0]     = CTextureManager::GetRwTexture("chsel_start_p1");
    m_pTextureStartP[1]     = CTextureManager::GetRwTexture("chsel_start_p2");
    m_pTextureStartP[2]     = CTextureManager::GetRwTexture("chsel_start_p3");
    m_pTextureStartP[3]     = CTextureManager::GetRwTexture("chsel_start_p4");
    m_pTextureP[0]          = CTextureManager::GetRwTexture("chsel_p1");
    m_pTextureP[1]          = CTextureManager::GetRwTexture("chsel_p2");
    m_pTextureP[2]          = CTextureManager::GetRwTexture("chsel_p3");
    m_pTextureP[3]          = CTextureManager::GetRwTexture("chsel_p4");
    m_pTextureWindow        = CTextureManager::GetRwTexture("chsel_window");
    m_pTextureCursorLeft    = CTextureManager::GetRwTexture("chsel_ya_l");
    m_pTextureCursorRight   = CTextureManager::GetRwTexture("chsel_ya_r");
    m_pTextureCursorUpDown  = CTextureManager::GetRwTexture("chsel_ya_ud");
    m_pTextureCostumeSam    = CTextureManager::GetRwTexture("chsel_cos_sam");
    m_pTextureCostumeNex    = CTextureManager::GetRwTexture("chsel_cis_nex");

    m_bTextureLoad = true;
};


bool CPlayerSelectWorkPool::PushStackCharacter(int32 iPlayerNo)
{
    ASSERT(iPlayerNo >= 0 && iPlayerNo < COUNT_OF(m_aPlayerInfo));

    PLAYERINFO* pPlayerInfo = &m_aPlayerInfo[iPlayerNo];
    int32 iCursor = pPlayerInfo->m_iCursor;

    if (m_aCharacterInfo[iCursor].m_bLocked)
        return false;

    if (pPlayerInfo->m_iCharacterNum >= pPlayerInfo->m_iCharacterMax)
        return false;
    
    m_aCharacterInfo[iCursor].m_iLockPlayer = iPlayerNo;
    m_aCharacterInfo[iCursor].m_bLocked = true;

    if (IsCostumeTaken(iCursor, m_aCostumeSelect[iPlayerNo]))
        pPlayerInfo->m_aStackCostume[pPlayerInfo->m_iCharacterNum] = m_aCostumeSelect[iPlayerNo];
        
    iCursor += (pPlayerInfo->m_bSecret ? 4 : 0);    
    pPlayerInfo->m_aStackPlayerID[pPlayerInfo->m_iCharacterNum++] = GetCharacterID(iCursor);

    
    return true;
};


bool CPlayerSelectWorkPool::PopStackCharacter(int32 iPlayerNo)
{
    ASSERT(iPlayerNo >= 0 && iPlayerNo < COUNT_OF(m_aPlayerInfo));

    PLAYERINFO* pPlayerInfo = &m_aPlayerInfo[iPlayerNo];

    if (pPlayerInfo->m_iCharacterNum > 0)
    {
        int32 iChrIndex = GetCharacterIndex(pPlayerInfo->m_aStackPlayerID[pPlayerInfo->m_iCharacterNum - 1]);
        
        --pPlayerInfo->m_iCharacterNum;
        pPlayerInfo->m_aStackPlayerID[pPlayerInfo->m_iCharacterNum] = PLAYERID::ID_MAX;
        pPlayerInfo->m_aStackCostume[pPlayerInfo->m_iCharacterNum] = GAMETYPES::COSTUME_NONE;
        
		if (iChrIndex >= 4)
			iChrIndex -= 4;

        if(iChrIndex >= 0)
        {
            m_aCharacterInfo[iChrIndex].m_bLocked = false;
            m_aCharacterInfo[iChrIndex].m_iLockPlayer = -1;
        };

        return true;
    };

    return false;
};


void CPlayerSelectWorkPool::UnselectCharacterSearch(int32 iPlayerNo)
{
    PLAYERINFO* pPlayerInfo = &m_aPlayerInfo[iPlayerNo];

    if (GetTotalCharNum() >= GAMETYPES::PLAYERS_MAX)
        return;

    if (pPlayerInfo->m_iCharacterNum >= pPlayerInfo->m_iCharacterMax)
        return;

    pPlayerInfo->m_iCursorDisplay = pPlayerInfo->m_iCursor;
    
	int32 i = 0;
	do
	{

		pPlayerInfo->m_iCursor = InvClamp(++pPlayerInfo->m_iCursor, 0, COUNT_OF(m_aPlayerInfo) - 1);
		pPlayerInfo->m_CursorAnimDir = PLAYERINFO::ANIMDIR_RIGHT;
		pPlayerInfo->m_uAnimCharChangeFrame = 0;

		bool bEnable = m_aCharacterInfo[GetRealCursor(iPlayerNo)].m_bEnable;
		bool bLocked = m_aCharacterInfo[pPlayerInfo->m_iCursor].m_bLocked;

		if (bEnable && !bLocked)
			break;

	} while (++i < 4);
};


void CPlayerSelectWorkPool::Confirm(void)
{
	CGameData::PlayParam().ClearPlayer();

    for (int32 i = 0; i < COUNT_OF(m_aPlayerInfo); ++i)
    {
        PLAYERINFO* pPlayerInfo = &m_aPlayerInfo[i];
        
        for (int32 j = 0; j < pPlayerInfo->m_iCharacterNum; ++j)
        {
            CGameData::PlayParam().AddPlayerCharacter(
                pPlayerInfo->m_iPlayerNo,
                pPlayerInfo->m_aStackPlayerID[j],
                pPlayerInfo->m_aStackCostume[j]
            );
        };
    };
};


void CPlayerSelectWorkPool::LimitCheck(void)
{
    int32 iLockedCounter = 0;

    for (int32 i = 0; i < CController::Max(); ++i)
    {
        if (CController::IsLocked(i))
            ++iLockedCounter;
    };

    iLockedCounter = Clamp(iLockedCounter, 0, int32(GAMETYPES::PLAYERS_MAX));

    int32 iSelectMax = GAMETYPES::PLAYERS_MAX;
    if (iLockedCounter)
        iSelectMax = (GAMETYPES::PLAYERS_MAX - (iLockedCounter - 1));        

    for (int32 i = 0; i < COUNT_OF(m_aPlayerInfo); ++i)
    {
		while (m_aPlayerInfo[i].m_iCharacterNum > iSelectMax)
            PopStackCharacter(i);

        m_aPlayerInfo[i].m_iCharacterMax = iSelectMax;
    };
};


void CPlayerSelectWorkPool::StatusCheck(void)
{
    int32 iTotalCharacterNum = 0;

    for (int32 i = 0; i < COUNT_OF(m_aPlayerInfo); ++i)
        iTotalCharacterNum += m_aPlayerInfo[i].m_iCharacterNum;

    m_status = (iTotalCharacterNum >= m_nChrMax ? STATUS_DECIDE : STATUS_RUNNING);
};


void CPlayerSelectWorkPool::PadCheck(void)
{
    if (CController::GetDigitalTrigger(CController::CONTROLLER_UNLOCKED_ON_VIRTUAL, CController::DIGITAL_OK))
    {
        if (LockTriggeredController(CController::DIGITAL_OK) != -1)
        {
            LimitCheck();
            CGameSound::PlaySE(SDCODE_SE(4098));
        };
    };
};


PLAYERID::VALUE CPlayerSelectWorkPool::GetCharacterID(int32 iCursor) const
{
    ASSERT(iCursor >= 0 && iCursor < PLAYERID::ID_MAX);
    
    return PLAYERID::VALUE(iCursor);
};


int32 CPlayerSelectWorkPool::GetCharacterIndex(PLAYERID::VALUE PlayerID) const
{
    ASSERT(PlayerID >= 0 && PlayerID < PLAYERID::ID_MAX);

    return int32(PlayerID);
};


CPlayerSelectWorkPool::PLAYERINFO* CPlayerSelectWorkPool::GetPlayerInfo(int32 iPlayerNo)
{
    ASSERT(iPlayerNo >= 0 && iPlayerNo < COUNT_OF(m_aPlayerInfo));

    return &m_aPlayerInfo[iPlayerNo];
};


int32 CPlayerSelectWorkPool::GetRealCursor(int32 iPlayerNo) const
{
    ASSERT(iPlayerNo >= 0 && iPlayerNo < COUNT_OF(m_aPlayerInfo));
    return m_aPlayerInfo[iPlayerNo].m_iCursor + (m_aPlayerInfo[iPlayerNo].m_bSecret ? 4 : 0);
};


int32 CPlayerSelectWorkPool::GetTotalCharNum(void) const
{
    int32 iResult = 0;
    
    for (int32 i = 0; i < COUNT_OF(m_aPlayerInfo); ++i)
        iResult += m_aPlayerInfo[i].m_iCharacterNum;

    return iResult;
};


bool CPlayerSelectWorkPool::IsCostumeTaken(int32 iCursor, GAMETYPES::COSTUME Costume) const
{
    bool bResult = false;
    
    switch (Costume)
    {
    case GAMETYPES::COSTUME_SAMURAI:
        {
            switch (GetCharacterID(iCursor))
            {
            case PLAYERID::ID_LEO:
                bResult = CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_HOME_SAMURAI_LEO);
                break;

            case PLAYERID::ID_RAP:
                bResult = CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_HOME_SAMURAI_RAP);
                break;

            case PLAYERID::ID_MIC:
                bResult = CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_HOME_SAMURAI_MIC);
                break;

            case PLAYERID::ID_DON:
                bResult = CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_HOME_SAMURAI_DON);
                break;
            };
        }
        break;
        
    case GAMETYPES::COSTUME_NEXUS:
        {
            switch (GetCharacterID(iCursor))
            {
            case PLAYERID::ID_LEO:
                bResult = CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_HOME_NEXUS_LEO);
                break;

            case PLAYERID::ID_RAP:
                bResult = CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_HOME_NEXUS_RAP);
                break;

            case PLAYERID::ID_MIC:
                bResult = CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_HOME_NEXUS_MIC);
                break;

            case PLAYERID::ID_DON:
                bResult = CGameData::Record().Secret().IsUnlockedSecret(SECRETID::ID_HOME_NEXUS_DON);
                break;
            };
        }
        break;
    };

    return bResult;
};


bool CPlayerSelectWorkPool::IsEnded(void) const
{
    return (m_status > STATUS_DECIDE);
};


bool CPlayerSelectWorkPool::IsConfirmed(void) const
{
    return (m_status == STATUS_CONFIRM);
};


bool CPlayerSelectWorkPool::IsReturned(void) const
{
    return (m_status == STATUS_RETURN);
};


/*static*/ CProcess* CCharacterSelectSequence::Instance(void)
{
    return new CCharacterSelectSequence;
};


CCharacterSelectSequence::CCharacterSelectSequence(void)
: CAnim2DSequence("charselect")
, m_pWorkPool(nullptr)
{
    ;
};


CCharacterSelectSequence::~CCharacterSelectSequence(void)
{
    ASSERT(!m_pWorkPool);
};


bool CCharacterSelectSequence::OnAttach(const void* pParam)
{
    m_pWorkPool = new CPlayerSelectWorkPool;
    ASSERT(m_pWorkPool);
    m_pWorkPool->Attach();

#ifdef BUILD_TRIAL
    CTimeoutProcess::Enable(this, true);
    CTimeoutProcess::Start(this);
#endif    
    CGameData::Attribute().SetInteractive(true);
    
    UnlockAllControllers();
    m_pWorkPool->LimitCheck();

    return CAnim2DSequence::OnAttach(FILEID::ID_CHARSELECT);
};


void CCharacterSelectSequence::OnDetach(void)
{
    CAnim2DSequence::OnDetach();
    
    CGameData::Attribute().SetInteractive(false);

    if (m_pWorkPool)
    {
        delete m_pWorkPool;
        m_pWorkPool = nullptr;
    };
};


void CCharacterSelectSequence::OnMove(bool bRet, const void* pReturnValue)
{
    switch (m_animstep)
    {
    case ANIMSTEP_FADEIN:
        {
            m_pWorkPool->Move();
            
            if (!CScreenFade::IsFading())
                CGameSound::PlayBGM(SDCODE_BGM(12321));
        }
        break;
	
    case ANIMSTEP_DRAW:
        {
            m_pWorkPool->Move();

            if (m_pWorkPool->IsEnded())
            {
                CGameSound::FadeOut(CGameSound::FADESPEED_SLOW);
                BeginFadeout();
            };
        }
        break;

    case ANIMSTEP_END:
        {
            if (m_pWorkPool->IsConfirmed())
            {
                m_pWorkPool->Confirm();
                CGameData::Attribute().SetVirtualPad(CController::CONTROLLER_LOCKED_ON_VIRTUAL);
                Ret();
            }
            else if (m_pWorkPool->IsReturned())
            {
                Ret((const void*)PROCLABEL_SEQ_TITLE);
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


void CCharacterSelectSequence::OnDraw(void) const
{
    CAnim2DSequence::OnDraw();

    if (CAnim2DSequence::IsDrawing())
    {
        m_pWorkPool->TextureLoad();
        m_pWorkPool->Draw();
    };
};