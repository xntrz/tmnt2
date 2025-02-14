#pragma once


int32 LockTriggeredController(uint32 uButton);
void UnlockAllControllers(void);
int32 LockAndSearchController(int32 iController);
int32 FindTriggeredController(uint32 uButton, bool bUnlocked);
void EnableStickToDirButton(bool bEnable);
bool ControllerIsKeyboard(int32 iController);