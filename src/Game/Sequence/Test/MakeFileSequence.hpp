#pragma once

#ifdef _DEBUG

void ExtractLPACFile(class CSequence* pCaller, int32 fileId, const char* pszFilename);
void PrintLPACFile(class CSequence* pCaller, int32 fileId);

typedef void(*LPAC_ENUM_CB)(int32 fileId, uint32 fileSize, const char* pszFileName, void* pParam);
void EnumLPACFile(class CSequence* pCaller, int32 fileId, LPAC_ENUM_CB cb, void* pParam = nullptr);

class CProcess* MakeFileSequenceInstance(void);

#endif /* _DEBUG */