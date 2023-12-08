#pragma once


class CRwFileSystem;


class CRwFileSystemManager final
{
public:
    bool Initialize(void);
    void Terminate(void);
    
private:
    CRwFileSystem* m_pFileSystem;
};