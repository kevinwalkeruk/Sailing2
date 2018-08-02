#pragma once

char *GetTimerText(int Timer, int Mode);

char *GetTime();

char *ConvertIntToChar(int IntData);

//JAVA/////////////////////////////////////////////////////////////////////////////
void SendCommandToJAVA(char *DataText);

typedef enum {
  AppKeyAlert,//0
  AppKeySpeed,//1
  AppKeyMaxSpeed,//2
  AppKeyMarks,//3
  AppKeyAngleToWind,//4
  AppKeyATWType,//5
  AppKeyBearing,//6
  AppKeyDirection,//7
  AppKeyShift,//8
  AppKeyShiftType,//9
} AppKey;


//SAVED SETTINGS////////////////////////////////////////////////////////////////

void CheckForSavedSettings();

void SendSavedDataToWatch();