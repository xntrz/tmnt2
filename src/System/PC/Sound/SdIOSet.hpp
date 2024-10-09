#pragma once


void SdIOSetInitialize(void);
void SdIOSetTerminate(void);
void SdIOSetTask(void);
void SdIOSetKeyOff(int32 _ch);
void SdIOSetKeyOn(int32 _ch);
void SdIOSetAds(int32 _ch, int32 _ar, int32 _dr, int32 _sl);
void SdIOSetSrRr(int32 _ch, int32 _sr, int32 _rr);
void SdIOSetSr(int32 _ch, int32 _sr);
void SdIOSetRr(int32 _ch, int32 _rr);
void SdIOSetVolume(int32 _ch, int32 _lvol, int32 _rvol);
void SdIOSetSsAddress(int32 _ch, int32 _address);
void SdIOSetFreq(int32 _ch, int32 _freq);