#pragma once

#include "System/Common/Framework.hpp"


class CWebSystem;
class CWebGraphicsDevice;
class CWebSoundDevice;
class CWebInputsDevice;


class CWebFramework final : public CFramework
{    
public:
    CWebFramework(void);
    virtual ~CWebFramework(void);
    virtual bool Initialize(void) override;
    virtual void Terminate(void) override;
    virtual void Run(void) override;
    virtual void Move(void) override;
    virtual void Render(void) override;
    virtual void Flip(void) override;
    void SyncInput(void);
    void SyncFS(void);
    void OnScreenSizeChanged(int32 w, int32 h);
    void OnJoystickAdd(uint32 joystickID);
    void OnJoystickRem(uint32 joystickID);

private:
    CWebSystem*         m_pSystem;
    CWebGraphicsDevice* m_pWebGraphicsDevice;
    CWebSoundDevice*    m_pWebSoundDevice;
    CWebInputsDevice*   m_pWebInputsDevice;
};