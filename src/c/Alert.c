#include <pebble.h>
#include "Alert.h"
//#include "Sail.h"
#include "Main.h"

Window *alert_window;
TextLayer *alert_text_layer;
char *AlertText;
static AppTimer *alert_timer;
int AlertType = 0;
static int AlertTimeOut = 1000;

static void CloseEverything(){
  //APP_LOG(APP_LOG_LEVEL_INFO, "Closing Everything!");
  text_layer_destroy(alert_text_layer);
  window_destroy(alert_window);
  //APP_LOG(APP_LOG_LEVEL_INFO, "Closed Everything!");
  un_pause_alerts();
}

static void alert_timer_callback(){
  //APP_LOG(APP_LOG_LEVEL_INFO, "Alert Window Timed Out!");
  window_stack_remove(alert_window, true);
}

void alert_window_load(Window *window){
  //APP_LOG(APP_LOG_LEVEL_INFO, "Loading Alert Window!");
  
  //Create Custom Font
  static GFont arial_black_font_50;
  arial_black_font_50 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_Arial_Black_50));
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // Alert Text Layer
  alert_text_layer = text_layer_create(GRect(0, 0, (bounds.size.w), (bounds.size.h)));
  text_layer_set_background_color(alert_text_layer, GColorClear);
  text_layer_set_text_color(alert_text_layer, GColorBlack);
  text_layer_set_font(alert_text_layer, arial_black_font_50);
  text_layer_set_text_alignment(alert_text_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(alert_text_layer, GTextOverflowModeWordWrap);
  text_layer_set_text(alert_text_layer, AlertText);
  //APP_LOG(APP_LOG_LEVEL_INFO, "Alert Text");
  //APP_LOG(APP_LOG_LEVEL_INFO, AlertText);
  layer_add_child(window_layer, text_layer_get_layer(alert_text_layer));
  //APP_LOG(APP_LOG_LEVEL_INFO, "Alert Window Loaded!");
  alert_timer = app_timer_register(AlertTimeOut, alert_timer_callback, NULL);
  
  if (AlertType == 1){
    vibes_double_pulse();
  }
  if (AlertType == 2){
    vibes_long_pulse();
    vibes_long_pulse();
  }
  
}

void alert_window_unload(Window *window){
  //APP_LOG(APP_LOG_LEVEL_INFO, "Unload Alert Window!");
  CloseEverything();
}

void alert_create(char *Alert, int Duration, int Type){
  //APP_LOG(APP_LOG_LEVEL_WARNING, Alert);
  AlertText=Alert;
  AlertTimeOut = Duration;
  AlertType = Type;
  alert_window = window_create();
  window_set_window_handlers(alert_window,(WindowHandlers){
    .load = alert_window_load,
    .unload = alert_window_unload
  });
}

Window *alert_get_window(){
  return alert_window;
}