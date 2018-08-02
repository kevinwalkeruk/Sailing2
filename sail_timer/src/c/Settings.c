#include <pebble.h>
#include "Settings.h"

//#include <pebble.h>
//#include "Setup.h"
#include "Shared.h"
//#include "AlertWindow.h"
#include "Menu.h"

Window *settings_window;
TextLayer *settings_lable_layer;
TextLayer *settings_layer;
TextLayer *title_layer;

static Layer *s_image_layer;
static GBitmap *plus_image;
static GBitmap *minus_image;
static GBitmap *save_image;

char *DataTitle = "No Data";
int DataValue = 0;
char *DataText = "";
static char *DataType = "";
int DataAddress = 0;
int DataValueMax = 360;
int DataValueMin = 0;

int WindAngle = 0;
static int TackAngle = 0;
static int ShiftAlert = 0;

/*
void GetSavedData(){
  if (persist_exists(3)) {
    WindAngle = persist_read_int(3);
  } //True Wind
  if (persist_exists(4)) {
    TackAngle = persist_read_int(4);
  } //Tack Angle
  if (persist_exists(5)) {
    ShiftAlert = persist_read_int(5);
  } //Shift Alert
}
*/

/*
void GetData(){
  if (DataType ==1){
    DataTitle = "Wind Direction";
    DataValue = WindAngle;
    DataText = ConvertIntToChar(DataValue);
    DataValueMax = 360;
    DataValueMin = 0;
  }

  else if (DataType == 2){
    DataTitle = "Shift Alert";
    DataValue = ShiftAlert;
    DataText = ConvertIntToChar(DataValue);
    DataValueMax = 90;
    DataValueMin = 3;
  }
  else if (DataType == 3){
    DataTitle = "Tack Angle";
    DataValue = TackAngle;
    DataText = ConvertIntToChar(DataValue);
    DataValueMax = 90;
    DataValueMin = 10;
  }
  text_layer_set_text(setup_layer, DataText);
  text_layer_set_text(setup_lable_layer, DataTitle);
}
*/

/*
void SetData(){
  if (DataType == 1){
    WindAngle = DataValue;
  }
  else if (DataType == 2){
    ShiftAlert = DataValue;
  }
  else if (DataType == 3){
    TackAngle = DataValue;
  }
}
*/

//Clicks/////////////////////////////////////////////////////////////////////////////////
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Select!");
  persist_write_string(DataAddress, DataText);
  window_stack_remove(settings_window, true);
  SendSavedDataToWatch();
}
static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Up!");
  vibes_short_pulse();
  if (DataValue == DataValueMax){
    DataValue= DataValueMin;
  }
  else{
    DataValue += 1;
  }
  DataText = ConvertIntToChar(DataValue);
  text_layer_set_text(settings_layer, DataText);
}
static void up_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Up!");
  vibes_short_pulse();
  DataValue += 10;
  if ((DataValueMax - DataValueMin) > 10){
    if (DataValue > DataValueMax){
      DataValue = DataValueMin;
    }
  }
  DataText = ConvertIntToChar(DataValue);
  text_layer_set_text(settings_layer, DataText);
}
static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Down!");
  vibes_short_pulse();
  if (DataValue == DataValueMin){
    DataValue= DataValueMax;
  }
  else{
    DataValue -= 1;
  }
  DataText = ConvertIntToChar(DataValue);
  text_layer_set_text(settings_layer, DataText);
}
static void down_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Down!");
  vibes_short_pulse();
  DataValue -= 10;
  if ((DataValueMax - DataValueMin) > 10){
    if (DataValue < DataValueMin){
      DataValue = DataValueMax;
    }
  }
  DataText = ConvertIntToChar(DataValue);
  text_layer_set_text(settings_layer, DataText);
}
static void do_nothing(){}
static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_long_click_subscribe(BUTTON_ID_UP, 500, up_long_click_handler, do_nothing);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_long_click_subscribe(BUTTON_ID_DOWN, 500, down_long_click_handler, do_nothing);
  //window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler);
  //window_long_click_subscribe(BUTTON_ID_SELECT, 500, long_select_click_handler, long_select_click_handler);
}

static void layer_update_callback(Layer *layer, GContext* ctx) {
  GRect bounds = layer_get_bounds(s_image_layer);
  graphics_draw_bitmap_in_rect(ctx, plus_image, GRect((bounds.size.w-40),0 , 40, 40));
  graphics_draw_bitmap_in_rect(ctx, save_image, GRect((bounds.size.w-40),((bounds.size.h/2)-20) , 40, 40));
  graphics_draw_bitmap_in_rect(ctx, minus_image, GRect((bounds.size.w-40),(bounds.size.h-40) , 40, 40));
}

static void CloseEverything(){
  //text_layer_destroy(count_down_countdown_layer);
  //text_layer_destroy(count_down_countdown_lable_layer);
  //text_layer_destroy(count_down_time_layer);
  //text_layer_destroy(bearing_countdown_layer);
  gbitmap_destroy(plus_image);
  gbitmap_destroy(save_image);
  gbitmap_destroy(minus_image);
  layer_destroy(s_image_layer);
  window_destroy(settings_window);
  
  //menu_load_data();

  APP_LOG(APP_LOG_LEVEL_INFO, "Closed Everything!");
}

//void close_settings_window(){
//  APP_LOG(APP_LOG_LEVEL_WARNING, "Close Setup Window!");
//  window_stack_remove(settings_window, true);
//}


void settings_window_load(Window *window){
  //GetSavedData();
  DataText = ConvertIntToChar(DataValue);
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  //Images
  s_image_layer = layer_create(bounds);
  layer_set_update_proc(s_image_layer, layer_update_callback);
  layer_add_child(window_layer, s_image_layer);
  plus_image = gbitmap_create_with_resource(RESOURCE_ID_Plus);
  save_image = gbitmap_create_with_resource(RESOURCE_ID_Start);
  minus_image = gbitmap_create_with_resource(RESOURCE_ID_Minus);
    
  settings_lable_layer = text_layer_create(GRect(0, (bounds.size.h/2-40), bounds.size.w-40, 50));
  text_layer_set_background_color(settings_lable_layer, GColorClear);
  text_layer_set_text_color(settings_lable_layer, GColorBlack);
  text_layer_set_font(settings_lable_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_alignment(settings_lable_layer, GTextAlignmentCenter);
  text_layer_set_text(settings_lable_layer, DataTitle);
  layer_add_child(window_layer, text_layer_get_layer(settings_lable_layer));
  
  settings_layer = text_layer_create(GRect(0, (bounds.size.h/2-32), (bounds.size.w-40), 50));
  text_layer_set_background_color(settings_layer, GColorClear);
  text_layer_set_text_color(settings_layer, GColorBlack);
  text_layer_set_font(settings_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  text_layer_set_text_alignment(settings_layer, GTextAlignmentCenter);
  text_layer_set_text(settings_layer, DataText);
  layer_add_child(window_layer, text_layer_get_layer(settings_layer));
  
  title_layer = text_layer_create(GRect(0, 0, bounds.size.w-40, 50));
  text_layer_set_background_color(title_layer, GColorClear);
  text_layer_set_text_color(title_layer, GColorBlack);
  text_layer_set_font(title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(title_layer, GTextAlignmentCenter);
  text_layer_set_text(title_layer, "Setup");
  layer_add_child(window_layer, text_layer_get_layer(title_layer));
  
  window_set_click_config_provider(settings_window, click_config_provider);
  
  //GetData();
}

void settings_window_unload(Window *window){
  APP_LOG(APP_LOG_LEVEL_INFO, "Unload Settings Window!");
  CloseEverything();
}

//void alert_window_create(char *Alert, int Duration, int Type)
void settings_window_create(char *Title, char *Type, int Address){
  DataTitle = Title;
  persist_read_string(Address, DataText,sizeof(DataText));
  DataValue = atoi(DataText);
  DataType = Type;
  DataAddress = Address;
  settings_window = window_create();
  window_set_window_handlers(settings_window,(WindowHandlers){
    .load = settings_window_load,
    .unload = settings_window_unload
  });
}

void settings_window_distroy(){
  APP_LOG(APP_LOG_LEVEL_INFO, "Distroy Settings Window!");
  CloseEverything();
}

Window *settings_window_get_window(){
  return settings_window;
}
