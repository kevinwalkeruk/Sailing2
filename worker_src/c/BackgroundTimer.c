#include <pebble_worker.h>
#define SOURCE_FOREGROUND 0
#define SOURCE_BACKGROUND 1

int  CountTime = 360;
int RaceTime = 100;
int CountMin = 6;
int CountSec = 0;
double CountMinSec = 6.00;
char CountTimeText[100] = "";

static int BackgroundCommand = 0;
static int s_some_value = 3;
static int s_another_value = 4;

static int TimerMode = 1;
//1 = Prestart Stopped
//2 = Prestart Running
//3 = Racing
//4 = Race Complete

static void GetCountMinSec(){
  if (CountTime < 0 ){
    CountMin = 0;
    CountSec = 0;
    TimerMode = 3;
  }
  else if (CountTime < 60) {
    CountMin = 0;
    CountSec = CountTime;
  }
  else{
    CountMin = CountTime/60;
    CountSec = CountTime-(CountMin * 60);
  }
  CountMinSec = CountMin + (CountSec/100);
  int bufferSize= 4;
  snprintf(CountTimeText, bufferSize, "%d", (int)CountMin);
  strcat(CountTimeText, ":");
  if (CountSec<10){
    strcat(CountTimeText, "0");
  }
  char SecBuffer[5];
  snprintf(SecBuffer, bufferSize, "%d", (int)CountSec);
  strcat(CountTimeText, SecBuffer);
  //if (CountSec == 0){
  //    APP_LOG(APP_LOG_LEVEL_INFO, "Heart Beat (Each Min)!");
  //}
}

static void worker_message_handler(uint16_t type, AppWorkerMessage *message) {
  if(type == SOURCE_FOREGROUND) {
    // Get the data, if it was sent from the foreground
    BackgroundCommand = message->data0;
    //UP
    if(BackgroundCommand == 1){
      APP_LOG(APP_LOG_LEVEL_INFO, "Up!");
      //if (TimerCountUp == false){
      if (TimerMode < 3){
        if (CountTime<1141){
            CountTime=CountTime+60;
            GetCountMinSec();
      }
  }
    }
    //DOWN
    else if(BackgroundCommand == 2){
      APP_LOG(APP_LOG_LEVEL_INFO, "Down!");
      if (TimerMode < 3){
        if (CountTime>61){
          CountTime=CountTime-60;
          GetCountMinSec();
        }
      }
    }
    //SELECT
    else if(BackgroundCommand == 3){
      APP_LOG(APP_LOG_LEVEL_INFO, "Select!");
      if (TimerMode == 1){
        TimerMode = 2;
      }
      else if (TimerMode == 2){
        if (CountSec>30){
          CountTime=(CountMin+1)*60;
        }
        else if (CountSec<31){
          CountTime=CountMin*60;
        }   
        GetCountMinSec();
      }
      else if (TimerMode == 3){
        TimerMode = 4;
      }
    }
    //LONG SELECT
    else if(BackgroundCommand == 4){
      APP_LOG(APP_LOG_LEVEL_INFO, "Long Select!");
      CountTime=360;
      GetCountMinSec();
      TimerMode = 1;
    }
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  if (TimerMode == 3){
    CountTime ++;
  }
  else if (TimerMode == 2){
    CountTime --;
  }
  //Send Data
  // Construct a message to send
  AppWorkerMessage message = {
    //.data0 = RaceTime,
    .data1 = CountTime,
    .data2 = TimerMode
  };
  app_worker_send_message(SOURCE_BACKGROUND, &message);
  GetCountMinSec();
}

static void prv_init() {
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  app_worker_message_subscribe(worker_message_handler);
  APP_LOG(APP_LOG_LEVEL_INFO, "Background Started!");
}

static void prv_deinit() {
  tick_timer_service_unsubscribe();
  app_worker_message_unsubscribe();
  APP_LOG(APP_LOG_LEVEL_INFO, "Background Stopped!");
}

int main(void) {
  prv_init();
  worker_event_loop();
  prv_deinit();
}