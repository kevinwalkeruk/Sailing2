#include <pebble.h>
#include "Timer.h"
#include "Shared.h"
#include "Main.h"
#define SOURCE_FOREGROUND 0
#define SOURCE_BACKGROUND 1

Window *timer_window;
TextLayer *count_down_countdown_lable_layer;
TextLayer *count_down_countdown_layer;
TextLayer *count_down_time_layer;
//TextLayer *bearing_countdown_layer;
//TextLayer *count_down_bearing_lable_layer;

static Layer *s_image_layer;
static GBitmap *plus_image;
static GBitmap *minus_image;
static GBitmap *start_image;

int CountdownTime = 360;
double CountdownMinSec = 6.00;
int CountdownMin = 6;
int CountdownSec = 0;
bool CountdownRun = false;
bool CountupRun = false;

static int TimerMode = 0;
//1 = Prestart Stopped
//2 = Prestart Running
//3 = Racing
//4 = Race Complete

static int BackgroundCommand = 0;
//1 = Up
//2 = Down
//3 = Select
//4 = Long Select

void SendBackGroundCommand(int Test){
  // Construct a message to send
  AppWorkerMessage message = {
    .data0 = Test,
    //.data1 = s_another_value
  };
  // Send the data to the background app
  app_worker_send_message(SOURCE_FOREGROUND, &message);
}

char TimeText[100] = "";

static void CloseEverything(){
  app_worker_message_unsubscribe();
  text_layer_destroy(count_down_countdown_layer);
  text_layer_destroy(count_down_countdown_lable_layer);
  text_layer_destroy(count_down_time_layer);
  //text_layer_destroy(bearing_countdown_layer);
  gbitmap_destroy(plus_image);
  gbitmap_destroy(start_image);
  gbitmap_destroy(minus_image);
  layer_destroy(s_image_layer);
  window_destroy(timer_window);
  
  StartAppWorker();

  APP_LOG(APP_LOG_LEVEL_INFO, "Closed Everything!");
}

//GET BACKGROUND/////////////////////////////////////////////////////////////////////////////////
static void timer_worker_message_handler(uint16_t type, AppWorkerMessage *message) {
  if(type == SOURCE_BACKGROUND) {
    CountdownTime = message->data1;
    int NewTimerMode = message->data2;
    if (TimerMode != NewTimerMode){
      TimerMode = NewTimerMode;
      //1 = Prestart Stopped
      if (TimerMode == 1){
        APP_LOG(APP_LOG_LEVEL_INFO, "Timer Mode 1");
        text_layer_set_text(count_down_countdown_lable_layer, "Start Time");
      }
      //2 = Prestart Running
      else if (TimerMode == 2){
        APP_LOG(APP_LOG_LEVEL_INFO, "Timer Mode 2");
        text_layer_set_text(count_down_countdown_lable_layer, "Start Time");
      }
      //3 = Racing
      else if (TimerMode == 3){
        APP_LOG(APP_LOG_LEVEL_INFO, "Timer Mode 3");
        if (CountdownTime == 0){
          SendCommandToJAVA("RaceSt");
          close_timer_window();
        }
        
        text_layer_set_text(count_down_countdown_lable_layer, "Race Time");
      }
      //4 = Race Complete
      else if (TimerMode == 4){
        APP_LOG(APP_LOG_LEVEL_INFO, "Timer Mode 4");
        text_layer_set_text(count_down_countdown_lable_layer, "Race Finished");
      }
    }
    
    text_layer_set_text(count_down_countdown_layer, GetTimerText(CountdownTime, TimerMode));
    text_layer_set_text(count_down_time_layer, GetTime());
    if (CountdownTime == 30 && TimerMode == 2){
      window_stack_remove(timer_window, true);
    }
    else if (CountdownTime == 0){
      window_stack_remove(timer_window, true);
    }
  }
}
////////////////////////////////////////////////////////////////////////////////////////

//JAVA/////////////////////////////////////////////////////////////////////////////////
/*
static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  //APP_LOG(APP_LOG_LEVEL_INFO, "Data Received!");
  Tuple *AppKeyAngleToWind_tuple = dict_find(iter, AppKeyAngleToWind);
  if(AppKeyAngleToWind_tuple) {
    //APP_LOG(APP_LOG_LEVEL_INFO, "Angle to Wind Received!");
    char *AngleToWind = AppKeyAngleToWind_tuple->value->cstring;
    text_layer_set_text(bearing_countdown_layer, AngleToWind);
    //APP_LOG(APP_LOG_LEVEL_INFO, AngleToWind);
  }
}
*/
///////////////////////////////////////////////////////////////////////////////////////

//Clicks/////////////////////////////////////////////////////////////////////////////////
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  //APP_LOG(APP_LOG_LEVEL_INFO, "Select!");
  SendBackGroundCommand(3);
  vibes_short_pulse();
}
static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  //APP_LOG(APP_LOG_LEVEL_INFO, "Up!");
  SendBackGroundCommand(1);
  vibes_short_pulse();
}
static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  //APP_LOG(APP_LOG_LEVEL_INFO, "Down!");
  SendBackGroundCommand(2);
  //ControlTimerDown();
  vibes_short_pulse();

}
static void long_select_click_handler(ClickRecognizerRef recognizer, void *context) {
  //APP_LOG(APP_LOG_LEVEL_INFO, "Select Long!");
  SendBackGroundCommand(4);
  //ControlTimerLSelect();
  vibes_short_pulse();

}
static void back_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Back!");
  vibes_short_pulse();
  app_worker_message_unsubscribe();
  ////menu_window_create();
  ////window_stack_push(menu_window_get_window(), true);
  
  timer_window_distroy();
  //StartAppWorker();
}
static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  //window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT,1000, long_select_click_handler, long_select_click_handler);
}

static void layer_update_callback(Layer *layer, GContext* ctx) {
  GRect bounds = layer_get_bounds(s_image_layer);
  graphics_draw_bitmap_in_rect(ctx, plus_image, GRect((bounds.size.w-40),0 , 40, 40));
  graphics_draw_bitmap_in_rect(ctx, start_image, GRect((bounds.size.w-40),((bounds.size.h/2)-20) , 40, 40));
  graphics_draw_bitmap_in_rect(ctx, minus_image, GRect((bounds.size.w-40),(bounds.size.h-40) , 40, 40));
}

void close_timer_window(){
  APP_LOG(APP_LOG_LEVEL_WARNING, "Close Timer Window999!");
  CountdownTime = 1;
  window_stack_remove(timer_window, true);
}

void timer_window_load(Window *window){
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  //Images
  s_image_layer = layer_create(bounds);
  layer_set_update_proc(s_image_layer, layer_update_callback);
  layer_add_child(window_layer, s_image_layer);
  plus_image = gbitmap_create_with_resource(RESOURCE_ID_Plus);
  start_image = gbitmap_create_with_resource(RESOURCE_ID_Start);
  minus_image = gbitmap_create_with_resource(RESOURCE_ID_Minus);
    
  // COUNT DOWN Lable
  count_down_countdown_lable_layer = text_layer_create(GRect(0, (bounds.size.h/2-40), bounds.size.w-40, 50));
  text_layer_set_background_color(count_down_countdown_lable_layer, GColorClear);
  text_layer_set_text_color(count_down_countdown_lable_layer, GColorBlack);
  text_layer_set_font(count_down_countdown_lable_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_alignment(count_down_countdown_lable_layer, GTextAlignmentCenter);
  text_layer_set_text(count_down_countdown_lable_layer, "Start Time");
  layer_add_child(window_layer, text_layer_get_layer(count_down_countdown_lable_layer));
  
  // COUNT DOWN TEXT
  count_down_countdown_layer = text_layer_create(GRect(0, (bounds.size.h/2-32), (bounds.size.w-40), 50));
  text_layer_set_background_color(count_down_countdown_layer, GColorClear);
  text_layer_set_text_color(count_down_countdown_layer, GColorBlack);
  text_layer_set_font(count_down_countdown_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  //text_layer_set_font(count_down_countdown_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_text_alignment(count_down_countdown_layer, GTextAlignmentCenter);
  //GetCountdownMinSec();
  text_layer_set_text(count_down_countdown_layer, "6:00");
  layer_add_child(window_layer, text_layer_get_layer(count_down_countdown_layer));
  
  // Time Text
  count_down_time_layer = text_layer_create(GRect(0, 0, bounds.size.w-40, 50));
  text_layer_set_background_color(count_down_time_layer, GColorClear);
  text_layer_set_text_color(count_down_time_layer, GColorBlack);
  text_layer_set_font(count_down_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(count_down_time_layer, GTextAlignmentCenter);
  text_layer_set_text(count_down_time_layer, "Time");
  layer_add_child(window_layer, text_layer_get_layer(count_down_time_layer));
  
  // Bearing Lable
  /*
  count_down_bearing_lable_layer = text_layer_create(GRect(0, (bounds.size.h-60), bounds.size.w-40, 50));
  text_layer_set_background_color(count_down_bearing_lable_layer, GColorClear);
  text_layer_set_text_color(count_down_bearing_lable_layer, GColorBlack);
  text_layer_set_font(count_down_bearing_lable_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_alignment(count_down_bearing_lable_layer, GTextAlignmentCenter);
  text_layer_set_text(count_down_bearing_lable_layer, "Angle to Wind");
  layer_add_child(window_layer, text_layer_get_layer(count_down_bearing_lable_layer));
  */
  
  // Bearing TEXT
  /*
  bearing_countdown_layer = text_layer_create(GRect(0, (bounds.size.h-52), (bounds.size.w-40), 50));
  text_layer_set_background_color(bearing_countdown_layer, GColorClear);
  text_layer_set_text_color(bearing_countdown_layer, GColorBlack);
  text_layer_set_font(bearing_countdown_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  text_layer_set_text_alignment(bearing_countdown_layer, GTextAlignmentCenter);
  text_layer_set_text(bearing_countdown_layer, "000");
  layer_add_child(window_layer, text_layer_get_layer(bearing_countdown_layer));
*/
  
  //app_message_register_inbox_received(inbox_received_handler);
  //app_message_open(256, 256);
  window_set_click_config_provider(timer_window, click_config_provider);
  
  // Subscribe to get AppWorkerMessages
  //app_worker_message_subscribe(timer_worker_message_handler);
  
  //timer_window_start_delay_timer = app_timer_register(500, timer_window_start_delay_timer_callback, NULL);
  app_worker_message_subscribe(timer_worker_message_handler);
  app_worker_launch();
}

void timer_window_unload(Window *window){
  APP_LOG(APP_LOG_LEVEL_INFO, "Unload Timer Window!");
  CloseEverything();
}

void timer_window_create(){
  timer_window = window_create();
  window_set_window_handlers(timer_window,(WindowHandlers){
    .load = timer_window_load,
    .unload = timer_window_unload
  });
  APP_LOG(APP_LOG_LEVEL_INFO, "Background App Starting!");
  app_worker_launch();
}

void timer_window_distroy(){
  APP_LOG(APP_LOG_LEVEL_INFO, "Distroy Timer Window!");
  CloseEverything();
}

Window *timer_window_get_window(){
  return timer_window;
}