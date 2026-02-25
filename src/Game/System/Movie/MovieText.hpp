#pragma once


#ifdef TMNT2_BUILD_EU

class CMovieText
{
public:
    struct UPDATEINFO
    {
        const void* pFrmObj;
    };

public:
    static void Initialize(void);
    static void Terminate(void);
    static void Read(void* pBuffer, uint32 uBufferSize);
    static bool LoadFor(MVNAME mvname);
    static bool IsLoadEnd(void);
    static void OnLoadEnd(void);
    static void Update(UPDATEINFO* pUpdateInfo);
    static void Draw(void);
};

#endif /* TMNT2_BUILD_EU */