#pragma once

#ifdef _DEBUG

/**
 *  Since retail game files are packed into LPAC containers this sequence allows to
 *  print it or extract specified file (and then load it from disk for tests separately e.g enemies, gimmicks and so on)
 *
 *  NOTE: This process works as sequence in single request mode due CDataLoader completion intercept
 *        Posting request via Print or Extract funcs will call sequence and returns to your sequence on completion
 */

void ExtractLPACFile(class CSequence* pCaller, int32 fileId, const char* pszFilename);
void PrintLPACFile(class CSequence* pCaller, int32 fileId);

typedef void(*LPAC_ENUM_CB)(int32 fileId, uint32 fileSize, const char* pszFileName, void* pParam);
void EnumLPACFile(class CSequence* pCaller, int32 fileId, LPAC_ENUM_CB cb, void* pParam = nullptr);

class CProcess* MakeFileSequenceInstance(void);

#endif /* _DEBUG */