#pragma once


void SdIOSetInitialize(void);
void SdIOSetTerminate(void);
void SdIOSetTask(void);
void SdIOSetKeyOff(int32 ch);
void SdIOSetKeyOn(int32 ch);
void SdIOSetAds(int32 ch, uint8 ar, uint8 dr, uint8 sl);
void SdIOSetSrRr(int32 ch, uint8 sr, uint8 rr);
void SdIOSetSr(int32 ch, uint8 sr);
void SdIOSetRr(int32 ch, uint8 rr);
void SdIOSetEffectOn(int32 ch);
void SdIOSetEffectOff(int32 ch);
void SdIOSetVolume(int32 ch, int32 lvol, int32 rvol);
void SdIOSetSsAddress(int32 ch, int32 addr);
void SdIOSetFreq(int32 ch, int32 freq);
void SdIOSetEffectMode(int32 ch, int32 mode);