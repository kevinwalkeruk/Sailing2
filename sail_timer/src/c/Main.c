#include <pebble.h>
#include "Main.h"
#include "Shared.h"
#include "Menu.h"
#include "Timer.h"
#include "Alert.h"

#define SOURCE_FOREGROUND 0
#define SOURCE_BACKGROUND 1

//Windows
static Window *main_window;

//Layers etc
static GBitmap *digit1_bitmap;
static GBitmap *digit2_bitmap;
static GBitmap *digit3_bitmap;
static GBitmap *symbol_bitmap;
static BitmapLayer *digit1_bitmap_layer;
static BitmapLayer *digit2_bitmap_layer;
static BitmapLayer *digit3_bitmap_layer;
static BitmapLayer *symbol_bitmap_layer;
static TextLayer *Time_layer;
static TextLayer *Data_Lable_Layer;
static TextLayer *Marks_layer;

static AppTimer *main_window_start_delay_timer;

int digit1 = 0;
int digit2 = 0;
int digit3 = 0;
int symbol = 0;

int TimeType = 0;
static char *TimeText = "";

char *Data = "999";
static int DataValue = 0;
char *DataLable = "Shift";
int DataType = 5; // 1 = speed, 2 = max speed, 3 = Wind
char *Speed = "0.0";
char *MaxSpeed = "0.0";
char *Marks = "0";
char *AngleToWind = "0.0";
char *ATWType = "ATW";
char *Bearing = "0";
char *Direction = "";
char *Shift = "";
char *ShiftType = "";

//Variables for backgroud worker
static int CountdownTime = 0;
static char *RaceTime = "00:00";
static bool AlertEnabled = false;
static bool AlertPaused=false;
static char *CurrentTime = "00:00";
static char *Alert = "";

int TimerMode = 0;

const int IMAGE_RESOURCE_IDS[10] = {
  RESOURCE_ID_IMAGE_NUM_0, RESOURCE_ID_IMAGE_NUM_1, RESOURCE_ID_IMAGE_NUM_2,
  RESOURCE_ID_IMAGE_NUM_3, RESOURCE_ID_IMAGE_NUM_4, RESOURCE_ID_IMAGE_NUM_5,
  RESOURCE_ID_IMAGE_NUM_6, RESOURCE_ID_IMAGE_NUM_7, RESOURCE_ID_IMAGE_NUM_8,
  RESOURCE_ID_IMAGE_NUM_9
};

const int S_IMAGE_RESOURCE_IDS[10] = {
  RESOURCE_ID_S_IMAGE_NUM_0, RESOURCE_ID_S_IMAGE_NUM_1, RESOURCE_ID_S_IMAGE_NUM_2,
  RESOURCE_ID_S_IMAGE_NUM_3, RESOURCE_ID_S_IMAGE_NUM_4, RESOURCE_ID_S_IMAGE_NUM_5,
  RESOURCE_ID_S_IMAGE_NUM_6, RESOURCE_ID_S_IMAGE_NUM_7, RESOURCE_ID_S_IMAGE_NUM_8,
  RESOURCE_ID_S_IMAGE_NUM_9
};

const int SYMBOL_IMAGE_RESOURCE_IDS[5] = {
  RESOURCE_ID_Blank_Symbol, RESOURCE_ID_Arrow_PD, RESOURCE_ID_Arrow_PU,
  RESOURCE_ID_Arrow_SD, RESOURCE_ID_Arrow_SU
};

static void UpdateDigits(){
  //Remove Digit Three
  layer_remove_from_parent(bitmap_layer_get_layer(symbol_bitmap_layer));
  bitmap_layer_destroy(symbol_bitmap_layer);
  gbitmap_destroy(symbol_bitmap);
  
  //Symbol Bitmap
  symbol_bitmap = gbitmap_create_with_resource(SYMBOL_IMAGE_RESOURCE_IDS[symbol]);
  symbol_bitmap_layer = bitmap_layer_create(GRect(121, 50, 24, 30));
  bitmap_layer_set_compositing_mode(symbol_bitmap_layer, GCompOpSet);
  bitmap_layer_set_bitmap(symbol_bitmap_layer, symbol_bitmap);
  layer_add_child(window_get_root_layer(main_window), bitmap_layer_get_layer(symbol_bitmap_layer));
  
  //Remove Digit Three
  layer_remove_from_parent(bitmap_layer_get_layer(digit3_bitmap_layer));
  bitmap_layer_destroy(digit3_bitmap_layer);
  gbitmap_destroy(digit3_bitmap);
  
  //Digit Three Bitmap
  digit3_bitmap = gbitmap_create_with_resource(S_IMAGE_RESOURCE_IDS[digit3]);
  digit3_bitmap_layer = bitmap_layer_create(GRect(121, 94, 24, 30));
  bitmap_layer_set_compositing_mode(digit3_bitmap_layer, GCompOpSet);
  bitmap_layer_set_bitmap(digit3_bitmap_layer, digit3_bitmap);
  layer_add_child(window_get_root_layer(main_window), bitmap_layer_get_layer(digit3_bitmap_layer));
  
  //Remove Digit Two
  layer_remove_from_parent(bitmap_layer_get_layer(digit2_bitmap_layer));
  bitmap_layer_destroy(digit2_bitmap_layer);
  gbitmap_destroy(digit2_bitmap);
  
  //Digit Two Bitmap
  digit2_bitmap = gbitmap_create_with_resource(IMAGE_RESOURCE_IDS[digit2]);
  digit2_bitmap_layer = bitmap_layer_create(GRect(60, 50, 60, 76));
  bitmap_layer_set_compositing_mode(digit2_bitmap_layer, GCompOpSet);
  bitmap_layer_set_bitmap(digit2_bitmap_layer, digit2_bitmap);
  layer_add_child(window_get_root_layer(main_window), bitmap_layer_get_layer(digit2_bitmap_layer));
  
  //Remove Digit One
  layer_remove_from_parent(bitmap_layer_get_layer(digit1_bitmap_layer));
  bitmap_layer_destroy(digit1_bitmap_layer);
  gbitmap_destroy(digit1_bitmap);
  
  //Digit One Bitmap
  digit1_bitmap = gbitmap_create_with_resource(IMAGE_RESOURCE_IDS[digit1]);
  digit1_bitmap_layer = bitmap_layer_create(GRect(0, 50, 60, 76));
  bitmap_layer_set_compositing_mode(digit1_bitmap_layer, GCompOpSet);
  bitmap_layer_set_bitmap(digit1_bitmap_layer, digit1_bitmap);
  layer_add_child(window_get_root_layer(main_window), bitmap_layer_get_layer(digit1_bitmap_layer));
}

void un_pause_alerts(){
  APP_LOG(APP_LOG_LEVEL_WARNING, "Alert Un-Paused");
  AlertPaused = false;  
}

//GET BACKGROUND/////////////////////////////////////////////////////////////////////////////////
static void worker_message_handler(uint16_t type, AppWorkerMessage *message) {
  if(type == SOURCE_BACKGROUND) {
    CountdownTime = message->data1;
    TimerMode = message->data2;
    RaceTime = GetTimerText(CountdownTime, TimerMode);
  }
  if (CountdownTime == 0){
    AlertEnabled = true;
    SendCommandToJAVA("RaceSt");
  }
  //Current Time
  if (TimeType == 1){
    TimeText = GetTime();
    text_layer_set_font(Time_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  }
  else{
    //TimeText = GetTimeShort();
    TimeText = GetTimerText(CountdownTime, TimerMode);
    text_layer_set_font(Time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  }
  
  text_layer_set_text(Time_layer, TimeText);
  
  if (CountdownTime <= 30 && TimerMode < 3){
    text_layer_set_text(Data_Lable_Layer, "Time");
    if (CountdownTime == 0){
      digit1 = 0;
      digit2 = 0;
      digit3 = 0;
    }
    else if(CountdownTime < 10){
      digit1 = 0;
      digit2 = CountdownTime;
      digit3 = 0;
    }
    else if(CountdownTime < 31){
      digit1 = CountdownTime / 10;
      digit2 = CountdownTime - (digit1 * 10);
      digit3 = 0;
    }
    else {
      digit1 = 0;
      digit2 = 0;
      digit3 = 0;
    }
    UpdateDigits();
  }
  
}

static void SendBackGroundCommand(int Test){
  // Construct a message to send
  AppWorkerMessage message = {
    .data0 = Test,
    //.data1 = s_another_value
  };
  // Send the data to the background app
  app_worker_send_message(SOURCE_FOREGROUND, &message);
}
////////////////////////////////////////////////////////////////////////////////////////

//JAVA/////////////////////////////////////////////////////////////////////////////////
static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  
  Tuple *AppKeyAlert_tuple = dict_find(iter, AppKeyAlert);
  Tuple *AppKeySpeed_tuple = dict_find(iter, AppKeySpeed);
  Tuple *AppKeyMaxSpeed_tuple = dict_find(iter, AppKeyMaxSpeed);
  Tuple *AppKeyMarks_tuple = dict_find(iter,AppKeyMarks);
  Tuple *AppKeyAngleToWind_tuple = dict_find(iter,AppKeyAngleToWind);
  Tuple *AppKeyATWType_turple = dict_find(iter, AppKeyATWType);
  Tuple *AppKeyBearing_tuple = dict_find(iter,AppKeyBearing);
  Tuple *AppKeyDirection_tuple = dict_find(iter,AppKeyDirection);
  Tuple *AppKeyShift_tuple = dict_find(iter,AppKeyShift);
  Tuple *AppKeyShiftType_tuple = dict_find(iter,AppKeyShiftType);
  
  if(AppKeyAlert_tuple) {
    Alert = AppKeyAlert_tuple->value->cstring;
    if (strcmp(Alert, "No Alert")  != 0){
      APP_LOG(APP_LOG_LEVEL_WARNING, Alert);
      if (AlertEnabled==true && AlertPaused ==false){
        AlertPaused = true;
        alert_create(Alert, 5000, 1);
        window_stack_push(alert_get_window(), true);
      }
    }
  }
  
  if(AppKeySpeed_tuple) {
    Speed = AppKeySpeed_tuple->value->cstring;
  }
  if(AppKeyMaxSpeed_tuple) {
    MaxSpeed = AppKeyMaxSpeed_tuple->value->cstring;
  }
  if(AppKeyMarks_tuple) {
    Marks = AppKeyMarks_tuple->value->cstring;
    text_layer_set_text(Marks_layer, Marks);
  }
  if(AppKeyAngleToWind_tuple) {
    AngleToWind = AppKeyAngleToWind_tuple->value->cstring;
  }
  if(AppKeyATWType_turple) {
    ATWType = AppKeyATWType_turple->value->cstring;
  }
  if(AppKeyBearing_tuple) {
    Bearing = AppKeyBearing_tuple->value->cstring;
  }
  if(AppKeyDirection_tuple) {
    Direction = AppKeyDirection_tuple->value->cstring;
  }
  if(AppKeyShift_tuple) {
    Shift = AppKeyShift_tuple->value->cstring;
  }
  if(AppKeyShiftType_tuple) {
    ShiftType = AppKeyShiftType_tuple->value->cstring;
  }

  /*
  if(AppKeyMarks_tuple) {
    Marks = AppKeyMarks_tuple->value->cstring;
    if (SpeedType == 1){
      text_layer_set_text(Marks_layer, "M");
    }
    else{
      text_layer_set_text(Marks_layer, Marks);
    }
  }
  */
  
  /*
  if(AppKeyMaxSpeedValue_tuple) {
    char *MaxSpeedText = AppKeyMaxSpeedValue_tuple->value->cstring;
    int NewMaxSpeedValue = atoi(MaxSpeedText);
    if (MaxSpeedValue < NewMaxSpeedValue){
      MaxSpeedValue = NewMaxSpeedValue;
      SaveMaxSpeed(MaxSpeedValue);
    }
  }  
  */
  
  if (CountdownTime > 30 || TimerMode > 2){
    if (DataType == 1){
      Data = Speed;
    } //Speed
    else if(DataType == 2){
      Data = MaxSpeed;
    } //Max Speed
    else if(DataType == 3){
      //text_layer_set_text(Data_Lable_Layer, AngleToWind);
      Data = AngleToWind;
      DataLable = ATWType;
      //text_layer_set_text(Data_Lable_Layer, Direction);
    } //Wind
    else if(DataType == 4){
      //text_layer_set_text(Data_Lable_Layer, Bearing);
      Data = Bearing;
    } //Bearing
    else if(DataType==5){
      Data = Shift;
      DataLable = ShiftType;
    }
    //if(DataType == 3){
      //text_layer_set_text(Data_Lable_Layer, Direction);
    //} Wind
    //else{
    text_layer_set_text(Data_Lable_Layer, DataLable);
    //}
      
    DataValue = atoi(Data);
        
    if (DataValue == 0){
      digit1 = 0;
      digit2 = 0;
      digit3 = 0;
    }
    else if(DataValue < 10){
      digit1 = 0;
      digit2 = 0;
      digit3 = DataValue;
    }
    else if(DataValue < 100){
      digit1 = 0;
      digit2 = DataValue/10;
      digit3 = DataValue - (digit2 * 10);
    }
    else if(DataValue < 1000){
      digit1 =  DataValue/100;
      digit2 = (DataValue - (digit1*100))/10;
      digit3 = DataValue - (digit1*100) - (digit2*10);
    }
    else {
      digit1 = 9;
      digit2 = 9;
      digit3 = 9;
    }
    if(strcmp("PD", Direction) == 0){symbol = 1;}
    else if(strcmp("PU", Direction) == 0){symbol = 2;}
    else if(strcmp("SD", Direction) == 0){symbol = 3;}
    else if(strcmp("SU", Direction) == 0){symbol = 4;}
    else{symbol = 0;}
    
    UpdateDigits();
  }

}
///////////////////////////////////////////////////////////////////////////////////////

//BUTTONS///////////////////////////////////////////////////////////////////////////////////////
static void Oldup_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Up! - Plus Min");
  SendBackGroundCommand(1);
  vibes_short_pulse();
} //+ Min
static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Long Up! - Reset Max Speed");
  if (DataType == 1){
    
  }
  else if (DataType == 2){
    SendCommandToJAVA("RSTMaxSpd");
  }
  else if (DataType == 3){
    SendCommandToJAVA("SetWind");
  }
  else if (DataType == 4){
    SendCommandToJAVA("SetWind");
  }
  else if (DataType == 5){
    SendCommandToJAVA("SetWind");
  }
  vibes_short_pulse();
} // Set / Reset

static void Oldselect_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Select! - Start / Sync");
  SendBackGroundCommand(3);
  vibes_short_pulse();
}
static void long_select_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Long Select! - Reset Timer");
  SendBackGroundCommand(4);
  //vibes_short_pulse();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Down! - Open Timer");
  app_worker_message_unsubscribe();
  timer_window_create();
  window_stack_push(timer_window_get_window(), true);
  vibes_short_pulse();
} // - Min
static void Olddown_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Down! - Minus Min");
  SendBackGroundCommand(2);
  vibes_short_pulse();
} // - Min
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
APP_LOG(APP_LOG_LEVEL_INFO, "Select! - Change Data");
  DataType++;
  if (DataType == 1){
    DataLable = "Spd";
  } //Speed
  else if(DataType == 2){
    DataLable = "Max";
  } //Max Speed
  else if(DataType == 3){
    DataLable = "ATW";
  } //Wind
  else if(DataType == 4){
    DataLable = "Bear";
  } //Bearing
  else if(DataType == 5){
    DataLable = "Shift";
  }
  else{
    DataLable = "Spd";
    DataType = 1;
  }
  text_layer_set_text(Data_Lable_Layer, DataLable);
  vibes_short_pulse();
} // Change Data

static void back_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Back! - Menu");
  
  menu_window_create();
  window_stack_push(menu_window_get_window(), true);
  vibes_short_pulse();
} // Back

static void vibrate_click_handler(ClickRecognizerRef recognizer, void *context){vibes_short_pulse();}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler); // Reset / Set
  //window_long_click_subscribe(BUTTON_ID_UP,500, long_up_click_handler, vibrate_click_handler); //Reset Max Speed
  //window_multi_click_subscribe(BUTTON_ID_UP, 2, 0, 300, false, reset_max_speed_click_handler);//Reset Max Speed
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler); // Change Data
  //window_multi_click_subscribe(BUTTON_ID_SELECT, 2, 0, 300, false, double_select_click_handler);//Change Data
  //window_long_click_subscribe(BUTTON_ID_SELECT,500, vibrate_click_handler, long_select_click_handler); //Reset Timer
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler); // Timer Window
  //window_single_click_subscribe(BUTTON_ID_DOWN, back_click_handler); // -Min
  //window_long_click_subscribe(BUTTON_ID_DOWN,500, long_down_click_handler, vibrate_click_handler);
  //window_multi_click_subscribe(BUTTON_ID_DOWN, 2, 0, 300, false, double_down_click_handler);//Set Wind
  
  window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler); // Menu

  
}
//////////////////////////////////////////////////////////////////////////////////////////////////

void StartAppWorker(){
  APP_LOG(APP_LOG_LEVEL_INFO, "Main Start AppWorker!");
  app_worker_message_subscribe(worker_message_handler);
  app_worker_launch();
}

static void main_window_start_delay_callback(){
  APP_LOG(APP_LOG_LEVEL_INFO, "Delayed Start!");
  //window_set_click_config_provider(sail_window, click_config_provider);
  //app_message_register_inbox_received(inbox_received_handler);
  //app_message_open(256, 256);
  //app_worker_message_subscribe(worker_message_handler);
  //app_worker_launch();
  CheckForSavedSettings();
  StartAppWorker();
  //SendSavedSetting();
  SendSavedDataToWatch();
}



static void main_window_load(Window *window) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Main Window Loading!");
  
  //sail_window = window_create();
  Layer *window_layer = window_get_root_layer(main_window);
  GRect bounds = layer_get_bounds(window_layer);
  window_set_background_color(main_window, GColorBlack);
  
  //Digit One Bitmap
  digit1_bitmap = gbitmap_create_with_resource(IMAGE_RESOURCE_IDS[digit1]);
  digit1_bitmap_layer = bitmap_layer_create(GRect(0, 50, 60, 76));
  bitmap_layer_set_compositing_mode(digit1_bitmap_layer, GCompOpSet);
  bitmap_layer_set_bitmap(digit1_bitmap_layer, digit1_bitmap);
  layer_add_child(window_get_root_layer(main_window), bitmap_layer_get_layer(digit1_bitmap_layer));

  //Digit Two Bitmap
  digit2_bitmap = gbitmap_create_with_resource(IMAGE_RESOURCE_IDS[digit2]);
  digit2_bitmap_layer = bitmap_layer_create(GRect(60, 50, 60, 76));
  bitmap_layer_set_compositing_mode(digit2_bitmap_layer, GCompOpSet);
  bitmap_layer_set_bitmap(digit2_bitmap_layer, digit2_bitmap);
  layer_add_child(window_get_root_layer(main_window), bitmap_layer_get_layer(digit2_bitmap_layer));
  
  //Digit Three Bitmap
  digit3_bitmap = gbitmap_create_with_resource(S_IMAGE_RESOURCE_IDS[digit3]);
  digit3_bitmap_layer = bitmap_layer_create(GRect(121, 94, 24, 30));
  bitmap_layer_set_compositing_mode(digit3_bitmap_layer, GCompOpSet);
  bitmap_layer_set_bitmap(digit3_bitmap_layer, digit3_bitmap);
  layer_add_child(window_get_root_layer(main_window), bitmap_layer_get_layer(digit3_bitmap_layer));
  
  //Symbol Bitmap
  symbol_bitmap = gbitmap_create_with_resource(SYMBOL_IMAGE_RESOURCE_IDS[symbol]);
  symbol_bitmap_layer = bitmap_layer_create(GRect(121, 50, 24, 30));
  bitmap_layer_set_compositing_mode(symbol_bitmap_layer, GCompOpSet);
  bitmap_layer_set_bitmap(symbol_bitmap_layer, symbol_bitmap);
  layer_add_child(window_get_root_layer(main_window), bitmap_layer_get_layer(symbol_bitmap_layer));

  //Speed Text
  Data_Lable_Layer = text_layer_create(GRect(0, -5, 105, 55));
  text_layer_set_background_color(Data_Lable_Layer, GColorBlack);
  text_layer_set_text_color(Data_Lable_Layer, GColorWhite);
  text_layer_set_font(Data_Lable_Layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(Data_Lable_Layer, GTextAlignmentLeft);
  text_layer_set_text(Data_Lable_Layer, DataLable);
  layer_add_child(window_layer, text_layer_get_layer(Data_Lable_Layer));
  
  //Marks Text
  Marks_layer = text_layer_create(GRect(100, -5, bounds.size.w-100, 45));
  text_layer_set_background_color(Marks_layer, GColorBlack);
  text_layer_set_text_color(Marks_layer, GColorWhite);
  text_layer_set_font(Marks_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(Marks_layer, GTextAlignmentRight);
  text_layer_set_text(Marks_layer, "0");
  layer_add_child(window_layer, text_layer_get_layer(Marks_layer));
  
  // Race Time Text
  Time_layer = text_layer_create(GRect(0, 126, bounds.size.w, 50));
  text_layer_set_background_color(Time_layer, GColorBlack);
  text_layer_set_text_color(Time_layer, GColorWhite);
  text_layer_set_font(Time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(Time_layer, GTextAlignmentCenter);
  text_layer_set_text(Time_layer, "0:00");
  layer_add_child(window_layer, text_layer_get_layer(Time_layer));
  
  //sail_window_start_delay_timer = app_timer_register(2000, sail_window_start_delay_timer_callback, NULL);
  
  
  //app_worker_message_subscribe(worker_message_handler);
  //app_worker_launch();
  
  window_set_click_config_provider(main_window, click_config_provider);
  
  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(256, 256);
  
  main_window_start_delay_timer = app_timer_register(500, main_window_start_delay_callback, NULL);
  
  APP_LOG(APP_LOG_LEVEL_INFO, "Main Window Loaded!");
}

static void main_window_unload(Window *window) {
  
}


void init() {
  APP_LOG(APP_LOG_LEVEL_INFO, "Main Window Init!");
  main_window = window_create();
  window_set_window_handlers(main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(main_window, true);
  app_message_open(256,256);
  //load_settings_timer = app_timer_register(1000, load_settings_timer_callback, NULL);
}

//static void deinit() {
void deinit() {
  APP_LOG(APP_LOG_LEVEL_INFO, "Main Window Deinit!");
  window_destroy(main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

Window *main_window_get_window(){
  return main_window;
}

void CloseMainWindow(){
  window_stack_remove(main_window, true);
}