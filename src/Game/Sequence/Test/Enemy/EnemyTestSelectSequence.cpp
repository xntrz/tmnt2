#include "EnemyTestStageSequence.hpp"
#include "EnemyTestStageSeqState.hpp"
#include "EnemyTestMakeFileSequence.hpp"

#ifdef _DEBUG

#include "Game/Component/Enemy/EnemyID.hpp"
#include "Game/Sequence/Test/DebugUtils.hpp"
#include "Game/Sequence/Test/MakeFileSequence.hpp"
#include "Game/System/Misc/ScreenFade.hpp"
#include "Game/ProcessList.hpp"
#include "System/Common/Process/Sequence.hpp"
#include "System/Common/File/Filename.hpp"
#include "System/Common/File/FileID.hpp"

#include <vector>


class CEnemyTestSelectSequence : public CSequence
{
private:
    struct ENEMYINFO
    {
        const char*     name;
        ENEMYID::VALUE  id;
    };

public:
    CEnemyTestSelectSequence(void);
    virtual ~CEnemyTestSelectSequence(void);
    virtual bool OnAttach(const void* pParam) override;
    virtual void OnDetach(void) override;
    virtual void OnMove(bool bRet, const void* pReturnValue) override;
    virtual void OnDraw(void) const override;
    void OnMenuOk(int32 select);
    int32 FindInfoIndexByEnemyID(ENEMYID::VALUE id) const;
    int32 GetInfoIndexOfLatestImplID(void) const;
    ENEMYID::VALUE GetLatestImplEnemyID(void) const;
    int32 GetExtractIndexOfEnemyID(ENEMYID::VALUE enemyId) const;

private:
    static int32                    m_selectIndexTest;
    static int32                    m_selectIndexExtract;
    static int32                    m_selectIndexPrint;
    CDebugMenuCtrl                  m_menu;
    std::vector<ENEMYINFO>          m_aImplEnemyInfo;
    std::vector<const char*>        m_apszImplEnemyName;
    std::vector<ENEMYINFO>          m_aEnemyInfo;
    std::vector<const char*>        m_apszEnemyName;
    int32                           m_menuSelectBase;
    std::vector<const char*>        m_aszFilename;
};


/*static*/ int32 CEnemyTestSelectSequence::m_selectIndexTest = -1;
/*static*/ int32 CEnemyTestSelectSequence::m_selectIndexExtract = -1;
/*static*/ int32 CEnemyTestSelectSequence::m_selectIndexPrint = -1;


CEnemyTestSelectSequence::CEnemyTestSelectSequence(void)
: m_menu()
, m_aImplEnemyInfo()
, m_apszImplEnemyName()
, m_aEnemyInfo()
, m_apszEnemyName()
, m_menuSelectBase(0)
{
    size_t reserve = static_cast<size_t>(ENEMYID::ID_MAX);

    m_aImplEnemyInfo.reserve(reserve);
    m_apszImplEnemyName.reserve(reserve);

    m_aEnemyInfo.reserve(reserve);
    m_apszEnemyName.reserve(reserve);
};


CEnemyTestSelectSequence::~CEnemyTestSelectSequence(void)
{
    ;
};


bool CEnemyTestSelectSequence::OnAttach(const void* pParam)
{
    for (int32 i = 0; i < ENEMYID::ID_MAX; ++i)
    {
        ENEMYID::VALUE id = static_cast<ENEMYID::VALUE>(i);

        const char* pszName = ENEMYID::GetName(id);
        if (!pszName)
            continue;

        /* regist as "existing" enemy */
        m_aEnemyInfo.push_back({ pszName, id });
        m_apszEnemyName.push_back(pszName);

        if (!ENEMYID::GetImplementProgress(id))
            continue;

        /* regist as "implemented" enemy */
        m_aImplEnemyInfo.push_back({ pszName, id });
        m_apszImplEnemyName.push_back(pszName);
    };

    /* init select menu */
    m_menu.SetDispMax(20);
    m_menu.SetPos(100, 100);

    m_menu.AddText("All extracted files will be saved into executable dir");
    m_menu.AddSeparator(true);

    m_menuSelectBase = m_menu.GetSelectMax();

    /* init test enemy */
    int32 min = 0;
    int32 max = static_cast<int32>(m_apszImplEnemyName.size());
    int32 step = 1;

    int32 start = GetInfoIndexOfLatestImplID(); //FindInfoIndexByEnemyID(ENEMYID::ID_PURPLE_DRAGON_GANG);
    ASSERT(start != -1);

    m_menu.AddInt("Test Enemy", min, max, step, start, &m_apszImplEnemyName[0], [](int32 value, bool trigger) {
        m_selectIndexTest = value;
    });

	/* init extract enemy */
	min = 0;
	max = static_cast<int32>(m_apszEnemyName.size());
	step = 1;
    start = GetExtractIndexOfEnemyID(GetLatestImplEnemyID());

    m_menu.AddInt("Extract Enemy", min, max, step, start, &m_apszEnemyName[0], [](int32 value, bool trigger) {
        m_selectIndexExtract = value;
    });
    m_menu.AddTrigger("Extract all Enemies");
    m_menu.AddTrigger("Extract items");

    /* init file print */
    m_aszFilename.reserve(static_cast<size_t>(FILEID::ID_MAX));

    for (int32 i = 0; i < FILEID::ID_MAX; ++i)
    {
        static char s_aszFilenameBuff[FILEID::ID_MAX][256];
        s_aszFilenameBuff[i][0] = '\0';

        CFilename::Filename(s_aszFilenameBuff[i], i);
        
        m_aszFilename.push_back(s_aszFilenameBuff[i]);
    };

    min = 0;
    max = static_cast<int32>(m_aszFilename.size());
    step = 1;
    start = 0;

    m_menu.AddInt("Print file", min, max, step, start, &m_aszFilename[0], [](int32 value, bool trigger) {
        m_selectIndexPrint = value;
    });

    /* init effects */
    m_menu.AddTrigger("Extract all effects");
    
    /* init custom */
    m_menu.AddTrigger("Make enemy table");
    m_menu.AddTrigger("Make effect table");

    m_menu.AddTrigger("Exit");

    return true;
};


void CEnemyTestSelectSequence::OnDetach(void)
{
    ;
};


void CEnemyTestSelectSequence::OnMove(bool bRet, const void* pReturnValue)
{
    if (bRet)
        CScreenFade::BlackIn();

    m_menu.Period();

    CDebugMenuCtrl::RESULT result = m_menu.GetResult();
    switch (result)
    {
    case CDebugMenuCtrl::RESULT_OK:
        OnMenuOk(m_menu.GetSelect() - m_menuSelectBase);
        break;

    default:
        break;
    };
};


void CEnemyTestSelectSequence::OnDraw(void) const
{
    m_menu.Draw();
};


void CEnemyTestSelectSequence::OnMenuOk(int32 select)
{
    switch (select)
    {
    case 0:
        {
            ASSERT(m_selectIndexTest >= 0);
            ASSERT(m_selectIndexTest < static_cast<int32>(m_aImplEnemyInfo.size()));

            ENEMYID::VALUE id = m_aImplEnemyInfo[static_cast<size_t>(m_selectIndexTest)].id;

            Call(PROCLABEL_SEQ_ENEMYTESTSTAGE, reinterpret_cast<const void*>(id));
        }
        break;

    case 1:
        {
            ASSERT(m_selectIndexExtract >= 0);
            ASSERT(m_selectIndexExtract < static_cast<int32>(m_aEnemyInfo.size()));

            ENEMYID::VALUE id = m_aEnemyInfo[static_cast<size_t>(m_selectIndexExtract)].id;

            EnemyTestMakeFileEnemy(this, id);
        }
        break;

    case 2:
        EnemyTestMakeFileEnemyAll(this);
        break;

    case 3:
        EnemyTestMakeFileItems(this);
        break;

    case 4:
        PrintLPACFile(this, m_selectIndexPrint);
        break;

    case 5:
        EnemyTestMakeFileEffectAll(this);
        break;

    case 6:
        EnemyTestMakeEnemyTable(this);
        break;

    case 7:
        EnemyTestMakeEffectTable(this);
        break;

    case 8:
        Ret();
        break;

    default:
        ASSERT(false);
        break;
    };
};


int32 CEnemyTestSelectSequence::FindInfoIndexByEnemyID(ENEMYID::VALUE id) const
{
    for (const ENEMYINFO& it : m_aImplEnemyInfo)
    {
        if (it.id == id)
        {
            std::ptrdiff_t diff = (&it - &m_aImplEnemyInfo[0]);
            return static_cast<int32>(diff);
        };
    };

    return -1;
};


int32 CEnemyTestSelectSequence::GetInfoIndexOfLatestImplID() const
{
    std::ptrdiff_t diff = (&m_aImplEnemyInfo[m_aImplEnemyInfo.size() - 2] - &m_aImplEnemyInfo[0]);
    return static_cast<int32>(diff);
};


ENEMYID::VALUE CEnemyTestSelectSequence::GetLatestImplEnemyID(void) const
{
    if (m_aImplEnemyInfo.empty())
        return ENEMYID::ID_DUMMY;

    // except fugitoid
    return (*(m_aImplEnemyInfo.end() - 2)).id;
};


int32 CEnemyTestSelectSequence::GetExtractIndexOfEnemyID(ENEMYID::VALUE enemyId) const
{
    for (const ENEMYINFO& it : m_aEnemyInfo)
    {
        if (it.id == enemyId)
        {
            std::ptrdiff_t diff = (&it - &m_aEnemyInfo[0]);
            return static_cast<int32>(diff);
        };
    };

    return -1;
};


class CProcess* EnemyTestSelectSequenceInstance(void)
{
    return new CEnemyTestSelectSequence;
};

#endif /* _DEBUG */