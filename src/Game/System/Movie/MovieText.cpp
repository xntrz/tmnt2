#include "MovieText.hpp"

#ifdef TMNT2_BUILD_EU

#include "Game/System/2d/GameFont.hpp"
#include "Game/System/DataLoader/DataLoader.hpp"
#include "System/Common/File/FileID.hpp"
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
    FILEID::VALUE  fileId;
    MOVIEID::VALUE movieId;
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
    {   FILEID::ID_M01T01,      MOVIEID::ID_M01T01      }, // 0
    {   FILEID::ID_M02N01,      MOVIEID::ID_M02N01      }, // 1
    {   FILEID::ID_M02N02,      MOVIEID::ID_M02N02      }, // 2
    {   FILEID::ID_M04N01,      MOVIEID::ID_M04N01      }, // 3
    {   FILEID::ID_M05N01,      MOVIEID::ID_M05N01P     }, // 4
    {   FILEID::ID_M06R01,      MOVIEID::ID_M06R01      }, // 5
    {   FILEID::ID_M09N01,      MOVIEID::ID_M09N01      }, // 6
    {   FILEID::ID_M10N01,      MOVIEID::ID_M10N01      }, // 7
    {   FILEID::ID_M11J01,      MOVIEID::ID_M11J01      }, // 8
    {   FILEID::ID_M12N01,      MOVIEID::ID_M12N01      }, // 9
    {   FILEID::ID_M14N01,      MOVIEID::ID_M14N01      }, // 10
    {   FILEID::ID_M15N01,      MOVIEID::ID_M15N01      }, // 11
    {   FILEID::ID_M17N01,      MOVIEID::ID_M17N01      }, // 12
    {   FILEID::ID_M18FB01,     MOVIEID::ID_M18FB01     }, // 13
    {   FILEID::ID_M18FB02,     MOVIEID::ID_M18FB02     }, // 14
    {   FILEID::ID_M20FB01,     MOVIEID::ID_M20FB01     }, // 15
    {   FILEID::ID_M20FB02,     MOVIEID::ID_M20FB02     }, // 16
    {   FILEID::ID_M21N01,      MOVIEID::ID_M21N01      }, // 17
    {   FILEID::ID_M21N02,      MOVIEID::ID_M21N02      }, // 18
    {   FILEID::ID_M24N01,      MOVIEID::ID_M24N01      }, // 19
    {   FILEID::ID_M24N02,      MOVIEID::ID_M24N02      }, // 20
    {   FILEID::ID_M25J01,      MOVIEID::ID_M25J01      }, // 21
    {   FILEID::ID_M27N01,      MOVIEID::ID_M27N01      }, // 22
    {   FILEID::ID_M29N01,      MOVIEID::ID_M29N01      }, // 23
    {   FILEID::ID_M29N02,      MOVIEID::ID_M29N02      }, // 24
    {   FILEID::ID_M30NB01,     MOVIEID::ID_M30NB01     }, // 25
    {   FILEID::ID_M31NB01,     MOVIEID::ID_M31NB01     }, // 26
    {   FILEID::ID_M32R01,      MOVIEID::ID_M32R01      }, // 27
    {   FILEID::ID_M33J01,      MOVIEID::ID_M33J01      }, // 28
    {   FILEID::ID_M38NB01,     MOVIEID::ID_M38NB01     }, // 29
    {   FILEID::ID_M38NB02,     MOVIEID::ID_M38NB02     }, // 30
    {   FILEID::ID_M39N01,      MOVIEID::ID_M39N01      }, // 31
    {   FILEID::ID_M40OB01,     MOVIEID::ID_M40OB01     }, // 32
    {   FILEID::ID_M40OB02,     MOVIEID::ID_M40OB02     }, // 33
    {   FILEID::ID_M41NB01,     MOVIEID::ID_M41NB01     }, // 34
    {   FILEID::ID_M42NB01,     MOVIEID::ID_M42NB01     }, // 35
    {   FILEID::ID_M43N01,      MOVIEID::ID_M43N01      }, // 36
    {   FILEID::ID_M44NB01,     MOVIEID::ID_M44NB01     }, // 37
    {   FILEID::ID_M45N01,      MOVIEID::ID_M45N01      }, // 38
    {   FILEID::ID_M45N02,      MOVIEID::ID_M45N02      }, // 39
    {   FILEID::ID_M46R01,      MOVIEID::ID_M46R01      }, // 40
    {   FILEID::ID_M47OB01,     MOVIEID::ID_M47OB01     }, // 41
    {   FILEID::ID_M47OB02,     MOVIEID::ID_M47OB02     }, // 42
    {   FILEID::ID_M48N01,      MOVIEID::ID_M48N01      }, // 43
    {   FILEID::ID_M50NB01,     MOVIEID::ID_M50NB01     }, // 44
    {   FILEID::ID_M50NB02,     MOVIEID::ID_M50NB02     }, // 45
    {   FILEID::ID_M56NB01,     MOVIEID::ID_M56NB01     }, // 46
    {   FILEID::ID_M57NB01,     MOVIEID::ID_M57NB01     }, // 47
    {   FILEID::ID_M57NB02,     MOVIEID::ID_M57NB02     }, // 48
    {   FILEID::ID_M58OB01,     MOVIEID::ID_M58OB01     }, // 49
    {   FILEID::ID_M58OB02,     MOVIEID::ID_M58OB02E    }, // 50
    {   FILEID::ID_M59S01,      MOVIEID::ID_M59S01      }, // 51
    {   FILEID::ID_M59S02,      MOVIEID::ID_M59S02      }, // 52
    {   FILEID::ID_M59S03,      MOVIEID::ID_M59S03      }, // 53
    {   FILEID::ID_M60X03,      MOVIEID::ID_M60X03E     }, // 54
    {   FILEID::ID_M62X01,      MOVIEID::ID_M62X01E     }, // 55

    {   FILEID::ID_INVALID,     MOVIEID::ID_MAX         }, // 56
};


static bool                 s_bIsSubtitlesRequired = false;
static bool                 s_bIsSubtitlesEnabled = false;
static MOVIEID::VALUE       s_subtitlesMovieId = MOVIEID::ID_MAX;
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

    s_subtitlesMovieId = MOVIEID::ID_MAX;
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
    s_subtitlesMovieId = MOVIEID::ID_MAX;
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


/*static*/ bool CMovieText::LoadFor(MOVIEID::VALUE movieId)
{
    if (!s_bIsSubtitlesRequired)
        return false;

    s_subtitlesMovieId = movieId;

    for (int32 i = 0; i < COUNT_OF(s_aMovieTextLoadInfo); ++i)
    {
        if (s_aMovieTextLoadInfo[i].movieId != movieId)
            continue;

        if (s_aMovieTextLoadInfo[i].fileId != FILEID::ID_INVALID)
        {
            CDataLoader::Regist(s_aMovieTextLoadInfo[i].fileId);
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
    if (!s_bIsSubtitlesRequired)
        return;
    
    if (!s_bIsSubtitlesEnabled)
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
    if (!(s_bIsSubtitlesRequired &&
          s_bIsSubtitlesEnabled  &&
          s_ppMovieTextElement   &&
          s_elementMax           &&
          s_pMovieTextElementCurrent))
        return;

    if (s_pMovieTextElementCurrent->text[0] == UTEXT('\0'))
        return;

    DrawSubtitleBorder();
    DrawSubtitleText();
};

#endif /* TMNT2_BUILD_EU */