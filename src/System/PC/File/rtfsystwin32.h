
#ifndef RTFSWIN_H
#define RTFSWIN_H

/**
* \defgroup rtfswin Windows
* \ingroup  rtfsyst
*
* The Windows file system allows for synchronous and asynchronous file
* operations to be performed. This file system uses a driver internal
* read and write bufferring system.
* Asynchronous file operations are not supported on Windows Me/98/95.
*/

/**
* \defgroup rtfswindatatypes Data Types
* \ingroup  rtfswin
*
* Windows File System Data Types.
*/
#include <windows.h>

#define WIN_FS_MAX_PATH_LENGTH 256
#define WIN_SECTOR_SIZE_MAX    2048

typedef struct RtWinFile RtWinFile;
/**
* \ingroup rtfswindatatypes
* \struct RtWinFile
* File object used by the Windows file system. It describes the current state
* of a specific file on the Windows file system.
*/
struct RtWinFile
{
	RtFile       file;          /**< Parent \ref RtFile object. */
	HANDLE       fp;            /**< Windows file handle. */
	OVERLAPPED   overlap;       /**< Structure for dealing with asynchronous
								file loading. */
	HANDLE       eventHandle;   /**< Event handle for dealing with asynchronous
								file loading. */
};

typedef struct RtWinFileSystem RtWinFileSystem;
/**
* \ingroup rtfswindatatypes
* \struct RtWinFileSystem
* This represents the Windows file system. The Windows file system
* contains a number of Windows specific files. This number is specified
* during the initialization (see \ref RtWinFSystemInit).
*/
struct RtWinFileSystem
{
	RtFileSystem  wfs;          /**< Parent \ref RtFileSystem object. */
	RwUInt32      sectorSize;   /**< Sector size for this file system. */
	RtWinFile    *files;        /**< Windows file system files. */
};

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */


	RtFileSystem      *
		RtWinFSystemInit(RwInt32 maxNbOpenFiles, RwChar *deviceName, RwChar *fileSystemName);

#ifdef __cplusplus
}
#endif  /* cplusplus */

/*---- end: ./win/rtfswin.h----*/

extern CRITICAL_SECTION section;

#define RTFS_INITIALIZE_INTERRUPT_HANDLING InitializeCriticalSection(&section)
#define RTFS_DISABLE_INTERRUPT EnterCriticalSection(&section)
#define RTFS_ENABLE_INTERRUPT  LeaveCriticalSection(&section)
#define RTFS_SHUTDOWN_INTERRUPT_HANDLING DeleteCriticalSection(&section)

/*---- end: ./win/fsystplatform.h----*/

#endif /* RTFSWIN_H */


