#include "MovieText.hpp"

#ifdef TMNT2_BUILD_EU

#include "Game/System/2d/GameFont.hpp"
#include "Game/System/DataLoader/DataLoader.hpp"
#include "System/Common/Configure.hpp"
#include "System/Common/Screen.hpp"
#include "System/Common/Sprite.hpp"
#include "System/Common/Font.hpp"
#include "System/Common/System2D.hpp"
#include "System/Common/RenderState.hpp"

#include "crimwsfd.h"


#define LANGUAGE_ID_ENGLISH     (0)
#define LANGUAGE_ID_GERMAN      (1)
#define LANGUAGE_ID_ITALIAN     (2)
#define LANGUAGE_ID_FRENCH      (3)
#define LANGUAGE_ID_SPANISH     (4)


struct MOVIETEXT_LOADINFO
{
    FNAME  fname;
    MVNAME mvname;
};


struct MOVIETEXT_FILEHEADER
{
    uint32 magic;       // 'MTFH'
    int32  elementMax;
    char   unused[8];
};


struct MOVIETEXT_ELEMENT
{
    uint32  magic;      // 'MTID'
    uint32  size;
    uint32  languageId;
    uint32  frame;
    wchar   text[0];
};


static const MOVIETEXT_LOADINFO s_aMovieTextLoadInfo[] =
{
    {  FPATH("Common/MovieText/M01T01.lpac"),    MVPATH("m01t01.sfd")   }, // 0
    {  FPATH("Common/MovieText/M02N01.lpac"),    MVPATH("m02n01.sfd")   }, // 1
    {  FPATH("Common/MovieText/M02N02.lpac"),    MVPATH("m02n02.sfd")   }, // 2
    {  FPATH("Common/MovieText/M04N01.lpac"),    MVPATH("m04n01.sfd")   }, // 3
    {  FPATH("Common/MovieText/M05N01.lpac"),    MVPATH("m05n01p.sfd")  }, // 4
    {  FPATH("Common/MovieText/M06R01.lpac"),    MVPATH("m06r01.sfd")   }, // 5
    {  FPATH("Common/MovieText/M09N01.lpac"),    MVPATH("m09n01.sfd")   }, // 6
    {  FPATH("Common/MovieText/M10N01.lpac"),    MVPATH("m10n01.sfd")   }, // 7
    {  FPATH("Common/MovieText/M11J01.lpac"),    MVPATH("m11j01.sfd")   }, // 8
    {  FPATH("Common/MovieText/M12N01.lpac"),    MVPATH("m12n01.sfd")   }, // 9
    {  FPATH("Common/MovieText/M14N01.lpac"),    MVPATH("m14n01.sfd")   },// 10
    {  FPATH("Common/MovieText/M15N01.lpac"),    MVPATH("m15n01.sfd")   },// 11
    {  FPATH("Common/MovieText/M17N01.lpac"),    MVPATH("m17n01.sfd")   },// 12
    {  FPATH("Common/MovieText/M18FB01.lpac"),   MVPATH("m18fb01.sfd")  },// 13
    {  FPATH("Common/MovieText/M18FB02.lpac"),   MVPATH("m18fb02.sfd")  },// 14
    {  FPATH("Common/MovieText/M20FB01.lpac"),   MVPATH("m20fb01.sfd")  },// 15
    {  FPATH("Common/MovieText/M20FB02.lpac"),   MVPATH("m20fb02.sfd")  },// 16
    {  FPATH("Common/MovieText/M21N01.lpac"),    MVPATH("m21n01.sfd")   },// 17
    {  FPATH("Common/MovieText/M21N02.lpac"),    MVPATH("m21n02.sfd")   },// 18
    {  FPATH("Common/MovieText/M24N01.lpac"),    MVPATH("m24n01.sfd")   },// 19
    {  FPATH("Common/MovieText/M24N02.lpac"),    MVPATH("m24n02.sfd")   },// 20
    {  FPATH("Common/MovieText/M25J01.lpac"),    MVPATH("m25j01.sfd")   },// 21
    {  FPATH("Common/MovieText/M27N01.lpac"),    MVPATH("m27n01.sfd")   },// 22
    {  FPATH("Common/MovieText/M29N01.lpac"),    MVPATH("m29n01.sfd")   },// 23
    {  FPATH("Common/MovieText/M29N02.lpac"),    MVPATH("m29n02.sfd")   },// 24
    {  FPATH("Common/MovieText/M30NB01.lpac"),   MVPATH("m30nb01.sfd")  },// 25
    {  FPATH("Common/MovieText/M31NB01.lpac"),   MVPATH("m31nb01.sfd")  },// 26
    {  FPATH("Common/MovieText/M32R01.lpac"),    MVPATH("m32r01.sfd")   },// 27
    {  FPATH("Common/MovieText/M33J01.lpac"),    MVPATH("m33j01.sfd")   },// 28
    {  FPATH("Common/MovieText/M38NB01.lpac"),   MVPATH("m38nb01.sfd")  },// 29
    {  FPATH("Common/MovieText/M38NB02.lpac"),   MVPATH("m38nb02.sfd")  },// 30
    {  FPATH("Common/MovieText/M39N01.lpac"),    MVPATH("m39n01.sfd")   },// 31
    {  FPATH("Common/MovieText/M40OB01.lpac"),   MVPATH("m40ob01.sfd")  },// 32
    {  FPATH("Common/MovieText/M40OB02.lpac"),   MVPATH("m40ob02.sfd")  },// 33
    {  FPATH("Common/MovieText/M41NB01.lpac"),   MVPATH("m41nb01.sfd")  },// 34
    {  FPATH("Common/MovieText/M42NB01.lpac"),   MVPATH("m42nb01.sfd")  },// 35
    {  FPATH("Common/MovieText/M43N01.lpac"),    MVPATH("m43n01.sfd")   },// 36
    {  FPATH("Common/MovieText/M44NB01.lpac"),   MVPATH("m44nb01.sfd")  },// 37
    {  FPATH("Common/MovieText/M45N01.lpac"),    MVPATH("m45n01.sfd")   },// 38
    {  FPATH("Common/MovieText/M45N02.lpac"),    MVPATH("m45n02.sfd")   },// 39
    {  FPATH("Common/MovieText/M46R01.lpac"),    MVPATH("m46r01.sfd")   },// 40
    {  FPATH("Common/MovieText/M47OB01.lpac"),   MVPATH("m47ob01.sfd")  },// 41
    {  FPATH("Common/MovieText/M47OB02.lpac"),   MVPATH("m47ob02.sfd")  },// 42
    {  FPATH("Common/MovieText/M48N01.lpac"),    MVPATH("m48n01.sfd")   },// 43
    {  FPATH("Common/MovieText/M50NB01.lpac"),   MVPATH("m50nb01.sfd")  },// 44
    {  FPATH("Common/MovieText/M50NB02.lpac"),   MVPATH("m50nb02.sfd")  },// 45
    {  FPATH("Common/MovieText/M56NB01.lpac"),   MVPATH("m56nb01.sfd")  },// 46
    {  FPATH("Common/MovieText/M57NB01.lpac"),   MVPATH("m57nb01.sfd")  },// 47
    {  FPATH("Common/MovieText/M57NB02.lpac"),   MVPATH("m57nb02.sfd")  },// 48
    {  FPATH("Common/MovieText/M58OB01.lpac"),   MVPATH("m58ob01.sfd")  },// 49
    {  FPATH("Common/MovieText/M58OB02.lpac"),   MVPATH("m58ob02E.sfd") },// 50
    {  FPATH("Common/MovieText/M59S01.lpac"),    MVPATH("m59s01.sfd")   },// 51
    {  FPATH("Common/MovieText/M59S02.lpac"),    MVPATH("m59s02.sfd")   },// 52
    {  FPATH("Common/MovieText/M59S03.lpac"),    MVPATH("m59s03.sfd")   },// 53
    {  FPATH("Common/MovieText/M60X03.lpac"),    MVPATH("m60x03E.sfd")  },// 54
    {  FPATH("Common/MovieText/M62X01.lpac"),    MVPATH("m62x01E.sfd")  },// 55
    {  FNAME_INVALID,                            MVNAME_INVALID         },
};


static bool                 s_bIsSubtitlesRequired = false;
static bool                 s_bIsSubtitlesEnabled = false;
static MVNAME               s_textMvName = MVNAME_INVALID;
static int32                s_elementMax = 0;
static int32                s_elementCur = 0;
static MOVIETEXT_ELEMENT**  s_ppMovieTextElement = nullptr;
static MOVIETEXT_ELEMENT*   s_pMovieTextElementCurrent = nullptr;
static RwV2d                s_textAreaSize = { 0.0f, 0.0f };
static RwV2d                s_textAreaPos = { 0.0f, 0.0f };
static RwV2d                s_textAreaOfs = { 0.0f, 0.0f };


static inline MOVIETEXT_ELEMENT* GetCurrentElement(void)
{
    if (s_elementCur < s_elementMax)
        return s_ppMovieTextElement[s_elementCur];

    return nullptr;
};


static inline MOVIETEXT_ELEMENT* GetNextElement(void)
{
    ++s_elementCur;
    return GetCurrentElement();
};


static void DrawSubtitleBorder(void)
{
    CSprite::PushRenderStates();

    RENDERSTATE_PUSH(rwRENDERSTATESRCBLEND, rwBLENDSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEDESTBLEND, rwBLENDINVSRCALPHA);
    RENDERSTATE_PUSH(rwRENDERSTATEVERTEXALPHAENABLE, true);

    RENDERSTATE_PUSH(rwRENDERSTATETEXTURERASTER, 0);

    const int32 colorPtMax = 4;

    // border left
    {
        CSprite spriteL;

        spriteL.SetPositionAndSize(-((s_textAreaSize.x * 0.5f) * 0.5f),
                                    (s_textAreaSize.y * 0.5f) + 140.0f,
                                    s_textAreaSize.x * 0.5f,
                                    s_textAreaSize.y);

        for (int32 i = 0; i < colorPtMax; ++i)
        {
            int32 alphaBasis = (16 - (80 * (i / 2)));
            spriteL.SetRGBA(i, 0, 0, 0, static_cast<uint8>(alphaBasis));
        };

        spriteL.Draw();
    }

    // border right
    {
        CSprite spriteR;

        spriteR.SetPositionAndSize(((s_textAreaSize.x * 0.5f) * 0.5f),
                                    (s_textAreaSize.y * 0.5f) + 140.0f,
                                    s_textAreaSize.x * 0.5f,
                                    s_textAreaSize.y);

        for (int32 i = 0; i < colorPtMax; ++i)
        {
            int32 alphaBasis = (16 - (80 * (i / 2)));
            spriteR.SetRGBA((colorPtMax - 1) - i, 0, 0, 0, static_cast<uint8>(alphaBasis));
        };

        spriteR.Draw();
    }

    RENDERSTATE_POP(rwRENDERSTATETEXTURERASTER);
    
    RENDERSTATE_POP(rwRENDERSTATEVERTEXALPHAENABLE);
    RENDERSTATE_POP(rwRENDERSTATEDESTBLEND);
    RENDERSTATE_POP(rwRENDERSTATESRCBLEND);

    CSprite::PopRenderStates();
};


static void DrawSubtitleText(void)
{
    Rt2dBBox bbox;
    bbox.x =  s_textAreaPos.x + s_textAreaOfs.x;
    bbox.y = (s_textAreaPos.y + s_textAreaOfs.y) * -1.0f - s_textAreaSize.y;
    bbox.w =  s_textAreaSize.x;
    bbox.h =  s_textAreaSize.y;

    CGameFont::SetHeight(18.0f);
    CGameFont::SetRGBA({ 0xFF, 0xFF, 0xFF, 0xFF });
    CGameFont::SetIntergapSpacing(-0.02f);
    CGameFont::Flow(s_pMovieTextElementCurrent->text, &bbox, rt2dJUSTIFYCENTER);
    CGameFont::SetIntergapSpacing(0.0f);
};


/*static*/ void CMovieText::Initialize(void)
{
    float scrW = static_cast<float>(CScreen::Width());
    float scrH = static_cast<float>(CScreen::Height());

    s_textAreaSize = { 540.0f, 72.0f };

    s_textAreaPos = { -(scrW * 0.5f),
                      -(scrH * 0.5f) };

    s_textAreaOfs = { (scrW - 540.0f) * 0.5f,
                      (scrH * 0.5f) + 140.0f };

    ASSERT(s_ppMovieTextElement == nullptr);
    ASSERT(s_elementMax == 0);

    s_elementCur = 0;
    s_elementMax = 0;
    s_ppMovieTextElement = nullptr;
    s_pMovieTextElementCurrent = nullptr;
    s_bIsSubtitlesEnabled = false;

#ifdef _DEBUG
    bool bLangSpecified = CConfigure::CheckArg("lang");
    if (bLangSpecified)
        s_bIsSubtitlesRequired = true;
    else
        s_bIsSubtitlesRequired = (CConfigure::GetLanguage() != TYPEDEF::CONFIG_LANG_ENGLISH);    
#else /* _DEBUG */
    s_bIsSubtitlesRequired = (CConfigure::GetLanguage() != TYPEDEF::CONFIG_LANG_ENGLISH);
#endif /* _DEBUG */

    s_textMvName = MVNAME_INVALID;
};


/*static*/ void CMovieText::Terminate(void)
{
    if (s_ppMovieTextElement)
    {
        for (int32 i = 0; i < s_elementMax; ++i)
        {
            if (s_ppMovieTextElement[i])
            {
                delete[] s_ppMovieTextElement[i];
                s_ppMovieTextElement[i] = nullptr;
            };
        };

        delete[] s_ppMovieTextElement;
        s_ppMovieTextElement = nullptr;
    };

    s_elementCur = 0;
    s_elementMax = 0;
    s_pMovieTextElementCurrent = nullptr;
    s_bIsSubtitlesRequired = false;
    s_bIsSubtitlesEnabled = false;
    s_textMvName = MVNAME_INVALID;
};


/*static*/ void CMovieText::Read(void* pBuffer, uint32 uBufferSize)
{
    ASSERT(pBuffer != nullptr);
    ASSERT(uBufferSize > 0);

    /* read header & init elements array */
    MOVIETEXT_FILEHEADER* pFileHdr = static_cast<MOVIETEXT_FILEHEADER*>(pBuffer);

    static char magicHdr[] = { 'M', 'T', 'F', 'H' };
    static_assert(sizeof(magicHdr) == sizeof(pFileHdr->magic), "sizeof should match");

    ASSERT(pFileHdr->magic == *reinterpret_cast<uint32*>(magicHdr));

    s_elementMax         = pFileHdr->elementMax;
    s_ppMovieTextElement = new MOVIETEXT_ELEMENT*[s_elementMax];

    for (int32 i = 0; i < s_elementMax; ++i)
        s_ppMovieTextElement[i] = nullptr;

    /* init language id from config language */
    uint32 languageId = LANGUAGE_ID_ENGLISH;
    TYPEDEF::CONFIG_LANG language = CConfigure::GetLanguage();
    switch (language)
    {
    case TYPEDEF::CONFIG_LANG_ENGLISH: languageId = LANGUAGE_ID_ENGLISH; break;
    case TYPEDEF::CONFIG_LANG_GERMAN:  languageId = LANGUAGE_ID_GERMAN;  break;
    case TYPEDEF::CONFIG_LANG_FRENCH:  languageId = LANGUAGE_ID_FRENCH;  break;
    case TYPEDEF::CONFIG_LANG_SPANISH: languageId = LANGUAGE_ID_SPANISH; break;
    case TYPEDEF::CONFIG_LANG_ITALIAN: languageId = LANGUAGE_ID_ITALIAN; break;
    default: ASSERT(false); break;
    };

    /* read elements */
    MOVIETEXT_ELEMENT* pFileElm = reinterpret_cast<MOVIETEXT_ELEMENT*>(++pFileHdr);

    for (int32 i = 0; i < s_elementMax;)
    {
        static char magicElm[] = { 'M', 'T', 'I', 'D' };
        static_assert(sizeof(magicElm) == sizeof(pFileElm->magic), "sizeof should match");

        ASSERT(pFileElm->magic == *reinterpret_cast<uint32*>(magicElm));

        if (pFileElm->languageId == languageId)
        {
            uint8* pRawData = new uint8[pFileElm->size];
            std::memcpy(pRawData, pFileElm, static_cast<size_t>(pFileElm->size));

            s_ppMovieTextElement[i++] = reinterpret_cast<MOVIETEXT_ELEMENT*>(pRawData);            
        };

        pFileElm = reinterpret_cast<MOVIETEXT_ELEMENT*>(reinterpret_cast<uint8*>(pFileElm) + pFileElm->size);
        ASSERT(reinterpret_cast<uint8*>(pFileElm) <= (static_cast<uint8*>(pBuffer) + uBufferSize));
    };
};


/*static*/ bool CMovieText::LoadFor(MVNAME mvname)
{
    if (!s_bIsSubtitlesRequired)
        return false;

    s_textMvName = mvname;

    for (int32 i = 0; i < COUNT_OF(s_aMovieTextLoadInfo); ++i)
    {
        if (!MVNAME_EQUAL(s_aMovieTextLoadInfo[i].mvname, mvname))
            continue;

        if (!FNAME_EQUAL(s_aMovieTextLoadInfo[i].fname, FNAME_INVALID))
        {
            CDataLoader::Regist(s_aMovieTextLoadInfo[i].fname);
            return true;
        };
    };

    s_bIsSubtitlesRequired = false;

    return false;
};


/*static*/ bool CMovieText::IsLoadEnd(void)
{
    if (s_bIsSubtitlesRequired)
        return CDataLoader::IsLoadEnd();

    return true;
};


/*static*/ void CMovieText::OnLoadEnd(void)
{
    if (s_bIsSubtitlesRequired &&
        s_ppMovieTextElement   &&
        s_elementMax)
    {
        s_pMovieTextElementCurrent = nullptr;
        s_elementCur = 0;
        s_bIsSubtitlesEnabled = true;
    };
};


/*static*/ void CMovieText::Update(UPDATEINFO* pUpdateInfo)
{
    if (!(s_bIsSubtitlesRequired && s_bIsSubtitlesEnabled))
        return;

    ASSERT(pUpdateInfo != nullptr);
    ASSERT(pUpdateInfo->pFrmObj != nullptr);

    const MwsfdFrmObj* pFrameObj = static_cast<const MwsfdFrmObj*>(pUpdateInfo->pFrmObj);

    if (!pFrameObj->bufadr)
        return;

    if ((pFrameObj->fps == -1) ||
        (pFrameObj->fno == -1))
        return;

    float fFNO = static_cast<float>(pFrameObj->fno);
    float fFPS = static_cast<float>(pFrameObj->fps);

    uint32 frame = static_cast<uint32>(fFNO / (fFPS * (1.0f / 1000.0f)) * 100.0f);

    if (!s_pMovieTextElementCurrent || (s_pMovieTextElementCurrent->frame < frame))
    {
        MOVIETEXT_ELEMENT* pElement = GetCurrentElement();
        if (pElement && (pElement->frame < frame))
        {
            s_pMovieTextElementCurrent = pElement;
            GetNextElement();
        };
    };
};


/*static*/ void CMovieText::Draw(void)
{
    if (!(s_bIsSubtitlesRequired && s_bIsSubtitlesEnabled))
        return;

    if (!s_ppMovieTextElement)
        return;

    if (s_elementMax <= 0)
        return;

    if (!s_pMovieTextElementCurrent)
        return;

    if (s_pMovieTextElementCurrent->text[0] == UTEXT('\0'))
        return;

    DrawSubtitleBorder();
    DrawSubtitleText();
};

#endif /* TMNT2_BUILD_EU */