#include "WebRwIDBFS.hpp"

#include "System/Web/WebTypedefs.hpp"

#include <emscripten.h>


struct INDEXEDDBFS_STATE
{
    char   szMountPath[256];
    bool   bSyncComplete;
    bool   bSyncResult;
};


static INDEXEDDBFS_STATE s_idbfsState;


EMSCRIPTEN_KEEPALIVE
extern "C" void webIDBFSSyncCallback(bool result)
{
    s_idbfsState.bSyncResult = result;
    s_idbfsState.bSyncComplete = true;
};


#define webIDBFSSyncCallbackName TOSTRING(webIDBFSSyncCallback)


static void webIDBFSMount(const char* pszPath)
{
    EM_ASM({
        FS.mkdir(UTF8ToString($0));
        FS.mount(IDBFS, {}, UTF8ToString($0));
    }, pszPath);
};


static void webIDBFSUnmount(const char* pszPath)
{
    EM_ASM({
        FS.unmount(UTF8ToString($0));
    }, pszPath);
};


static void webIDBFSRequestSync(bool bLoad)
{    
    s_idbfsState.bSyncComplete = false;

    EM_ASM({
        var callback = UTF8ToString($0);
        var isLoad = $1;

        FS.syncfs(isLoad, function(err) {
            ccall(callback, 'v', ['boolean'], [err ? false : true]);
        });
    }, webIDBFSSyncCallbackName, bLoad);
};


#define webIDBFSRequestLoadSync() \
    webIDBFSRequestSync(true)


#define webIDBFSRequestStoreSync() \
    webIDBFSRequestSync(false)


#define webIDBFSWaitSync(ms)                \
    while (!s_idbfsState.bSyncComplete) {   \
        emscripten_sleep(ms);               \
    }


static bool webIDBFSStartup(const char* pszMountPath)
{
    ASSERT(std::strlen(pszMountPath) < sizeof(s_idbfsState.szMountPath));
    std::strcpy(s_idbfsState.szMountPath, pszMountPath);

    s_idbfsState.bSyncComplete = true;
    s_idbfsState.bSyncResult = false;

    webIDBFSMount(s_idbfsState.szMountPath);
    webIDBFSRequestLoadSync();
    webIDBFSWaitSync(10);

    return true;
};


static void webIDBFSShutdown(void)
{
    webIDBFSRequestStoreSync();
    webIDBFSWaitSync(10);
    webIDBFSUnmount(s_idbfsState.szMountPath);
};


CWebRwIDBFS::CWebRwIDBFS(void)
: m_bSyncRequest(false)
, m_bResyncRequest(false)
{
    ;
};


CWebRwIDBFS::~CWebRwIDBFS(void)
{
    ;
};


bool CWebRwIDBFS::Initialize(const char* pszFSName, const char* pszDeviceName, const char* pszMountPath)
{
    if (webIDBFSStartup(pszMountPath))
        return CWebRwStdFS::Initialize(pszFSName, pszDeviceName);

    return false;
};


void CWebRwIDBFS::Terminate(void)
{
    webIDBFSShutdown();
};


bool CWebRwIDBFS::Sync(void)
{
    if (!m_bSyncRequest)
        return true;

    if (s_idbfsState.bSyncComplete)
    {
        m_bSyncRequest = false;

        if (m_bResyncRequest)
        {
            m_bResyncRequest = false;

            webIDBFSRequestStoreSync();
            m_bSyncRequest = true;
        };
    };

    return false;
};


RtFileSystemError CWebRwIDBFS::rwOpen(RtFileSystem* fs, RtFile* file, const RwChar* filename, RwUInt32 flags)
{
    filename = PathCorrection(filename);

    return CWebRwStdFS::rwOpen(fs, file, filename, flags);
};


void CWebRwIDBFS::rwClose(RtFile* file)
{
    CWebRwStdFS::rwClose(file);

    if (!m_bSyncRequest)
    {
        webIDBFSRequestStoreSync();
        m_bSyncRequest = true;
    }
    else
    {
        m_bResyncRequest = true;
    };
};


RwBool CWebRwIDBFS::rwExists(RtFileSystem* fs, const RwChar* filename)
{
    filename = PathCorrection(filename);

    return CWebRwStdFS::rwExists(fs, filename);
};


const RwChar* CWebRwIDBFS::PathCorrection(const RwChar* filename) const
{
    static RwChar s_szCorrectPath[256];
    s_szCorrectPath[0] = '\0';

    std::strcpy(s_szCorrectPath, s_idbfsState.szMountPath);

    if (filename[0] != '/')
        std::strcat(s_szCorrectPath, "/");

    std::strcat(s_szCorrectPath, filename);

    return s_szCorrectPath;
};