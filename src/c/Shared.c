#include <pebble.h>
#include "Shared.h"

static AppTimer *send_java_delay_timer;
static char *JAVAData = "";

char *GetTimerText(int Timer, int Mode){
  static int CountdownMin = 6;
  static int CountdownSec = 0;
  static char TimeText[100] = "";
  if (Timer < 60) {
    CountdownMin = 0;
    CountdownSec = Timer;
  }
  else{
    CountdownMin = Timer/60;
    CountdownSec = Timer-(CountdownMin * 60);
  }
  double CountdownMinSec = CountdownMin + (CountdownSec/100);
  int bufferSize= 4;
  snprintf(TimeText, bufferSize, "%d", (int)CountdownMin);
  strcat(TimeText, ":");
  if (CountdownSec == 0){
    strcat(TimeText, "00");
  }
  else if (CountdownSec == 1){
    strcat(TimeText, "01");
  }
  else if (CountdownSec == 2){
    strcat(TimeText, "02");
  }
  else if (CountdownSec == 3){
    strcat(TimeText, "03");
  }
  else if (CountdownSec == 4){
    strcat(TimeText, "04");
  }
  else if (CountdownSec == 5){
    strcat(TimeText, "05");
  }
  else if (CountdownSec == 6){
    strcat(TimeText, "06");
  }
  else if (CountdownSec == 7){
    strcat(TimeText, "07");
  }
  else if (CountdownSec == 8){
    strcat(TimeText, "08");
  }
  else if (CountdownSec == 9){
    strcat(TimeText, "09");
  }
  else{
    char SecBuffer[5];
    snprintf(SecBuffer, bufferSize, "%d", (int)CountdownSec);
    strcat(TimeText, SecBuffer);
  }
  
  //VIBRATIONS
  //int TimerMode = 2;
  if (Mode == 2){
    if (Timer == 0){
      APP_LOG(APP_LOG_LEVEL_INFO, "Double Long Vibe");
      vibes_long_pulse();
      vibes_long_pulse();
      //ResetMaxSpeed();
      //ResetLaps();
      //ResetRaceStart();
    }
    else if (CountdownSec == 0){
      APP_LOG(APP_LOG_LEVEL_INFO, "Long Vibe");
      vibes_long_pulse();
    }
    else if (CountdownSec == 30){
      APP_LOG(APP_LOG_LEVEL_INFO, "Short Vibe");
      vibes_short_pulse();
    }
    else if (CountdownMin == 0){
      if (CountdownSec < 16){
        APP_LOG(APP_LOG_LEVEL_INFO, "Short Vibe");
        vibes_short_pulse();
      }
      else if (CountdownSec == 10){
        APP_LOG(APP_LOG_LEVEL_INFO, "Long Vibe");
        vibes_long_pulse();
      }
      else if (CountdownSec == 15){
        APP_LOG(APP_LOG_LEVEL_INFO, "Long Vibe");
        vibes_long_pulse();
      }
    }
  }
  //APP_LOG(APP_LOG_LEVEL_WARNING, TimeText);
  return TimeText;
}


char *GetTime(){
  //char *TimeText = "";
  static char TimeText[10];
  time_t temp = time(NULL);
  struct tm *current_time = localtime(&temp);
  strftime(TimeText, sizeof(TimeText), clock_is_24h_style() ?
                                          "%T" : "%T", current_time);
  //APP_LOG(APP_LOG_LEVEL_WARNING, TimeText);
  return TimeText;
}

char *ConvertIntToChar(int IntData){
  APP_LOG(APP_LOG_LEVEL_WARNING, "ConvertIntToChar Called!");
  static char buf[] = "00000000000";
  snprintf(buf, sizeof(buf), "%d", IntData);
  return buf;
}


//JAVA////////////////////////////////////////////////////////////////////////////////////////////////
/*
0 = Default Tack Angle
1 = Wind Angle
*/

void SendCommandToJAVA(char *DataText){
  APP_LOG(APP_LOG_LEVEL_INFO, "Send data to JAVA");
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  if (iter == NULL){
    APP_LOG(APP_LOG_LEVEL_ERROR, "Iter is Null Failed to send");
    return;
  }
  dict_write_cstring(iter, 100, DataText);
  dict_write_end(iter);
  app_message_outbox_send();
  //send_java_delay_timer = app_timer_register(5000, SendToJAVA, NULL);
}



//SAVED SETTINGS///////////////////////////////////////////////////////
void CheckForSavedSettings(){
APP_LOG(APP_LOG_LEVEL_INFO, "Checking for saved data!");
  if (persist_exists(10)) { } //Default Tack Angle
  else {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Default Tack Angle Doesn't Exists, creating new!");
    persist_write_string(10, "90");
  }
  /*
  if (persist_exists(0)) { } //Default Max Speed
  else {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Max Speed Leg 0 Doesn't Exists, creating new!");
    persist_write_string(0, "0.0");
  }
  if (persist_exists(1)) { } //Default Max Speed
  else {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Max Speed Leg 1 Doesn't Exists, creating new!");
    persist_write_string(1, "0.0");
  }
  if (persist_exists(2)) { } //Default Max Speed
  else {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Max Speed Leg 2 Doesn't Exists, creating new!");
    persist_write_string(2, "0.0");
  }
  if (persist_exists(3)) { } //Default Max Speed
  else {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Max Speed Leg 3 Doesn't Exists, creating new!");
    persist_write_string(3, "0.0");
  }
  if (persist_exists(4)) { } //Default Max Speed
  else {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Max Speed Leg 4 Doesn't Exists, creating new!");
    persist_write_string(4, "0.0");
  }
  if (persist_exists(5)) { } //Default Max Speed
  else {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Max Speed Leg 5 Doesn't Exists, creating new!");
    persist_write_string(5, "0.0");
  }
  if (persist_exists(6)) { } //Default Max Speed
  else {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Max Speed Leg 6 Doesn't Exists, creating new!");
    persist_write_string(6, "0.0");
  }
  if (persist_exists(7)) { } //Default Max Speed
  else {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Max Speed Leg 7 Doesn't Exists, creating new!");
    persist_write_string(7, "0.0");
  }
  if (persist_exists(8)) { } //Default Max Speed
  else {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Max Speed Leg 8 Doesn't Exists, creating new!");
    persist_write_string(8, "0.0");
  }
  if (persist_exists(9)) { } //Default Max Speed
  else {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Max Speed Leg 9 Doesn't Exists, creating new!");
    persist_write_string(9, "0.0");
  }
  */
}
    /*
void SendSavedDataToWatch1(){
  char CommandString[] = "               ";
  //Wind Angle
  int SendDefaultWindAngle = persist_read_int(1);
  snprintf(CommandString, sizeof(CommandString), "%s%d", "SetTack",SendDefaultWindAngle);
  SendCommandToJAVA(CommandString);
  APP_LOG(APP_LOG_LEVEL_INFO, "Wind Angle Sent!");
  //send_java_delay_timer = app_timer_register(200, SendSavedDataToWatch, NULL);
}
*/

void SendSavedDataToWatch(){
  char CommandString[] = "               ";
  //Tack Angle
  char *DefaultTackAngle = "0";
  persist_read_string(10, DefaultTackAngle, sizeof(DefaultTackAngle));
  snprintf(CommandString, sizeof(CommandString), "%s%s", "SetTack",DefaultTackAngle);
  SendCommandToJAVA(CommandString);
  APP_LOG(APP_LOG_LEVEL_INFO, "Tack Angle Sent!");
  //send_java_delay_timer = app_timer_register(200, SendSavedDataToWatch1, NULL);
}