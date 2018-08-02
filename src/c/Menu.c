#include <pebble.h>
#include "Menu.h"
#include "Main.h"
#include "Settings.h"
#include "Shared.h"

#define NUM_MENU_SECTIONS 2
#define NUM_FIRST_MENU_ITEMS 2
#define NUM_SECOND_MENU_ITEMS 1
//#define NUM_THIRD_MENU_ITEMS 10

//144 x 168

//static AppTimer *load_settings_timer;
static Window *menu_window;
static SimpleMenuLayer *s_simple_menu_layer;
static SimpleMenuSection s_menu_sections[NUM_MENU_SECTIONS];
static SimpleMenuItem s_first_menu_items[NUM_FIRST_MENU_ITEMS];
static SimpleMenuItem s_second_menu_items[NUM_SECOND_MENU_ITEMS];
//static SimpleMenuItem s_third_menu_items[NUM_THIRD_MENU_ITEMS];
//static GBitmap *timer_icon_image;
//static GBitmap *sail_icon_image;
//static GBitmap *angle_icon_image;

//static uint32_t ShiftAngleKey = 0;
//static uint32_t WindAngleKey = 4;

int DefaultTackAngleValue = 0;
char *DefaultTackAngle = "";
int DefaultWindAngleValue = 0;
char *DefaultWindAngle = "0";

char *MaxSpeedLeg0 = "0.0";
char *MaxSpeedLeg1 = "0.0";
char *MaxSpeedLeg2 = "0.0";
char *MaxSpeedLeg3 = "0.0";
char *MaxSpeedLeg4 = "0.0";
char *MaxSpeedLeg5 = "0.0";
char *MaxSpeedLeg6 = "0.0";
char *MaxSpeedLeg7 = "0.0";
char *MaxSpeedLeg8 = "0.0";
char *MaxSpeedLeg9 = "0.0";

void menu_load_data(){
  /*
  DefaultTackAngleValue = persist_read_int(0);
  s_second_menu_items[0] = (SimpleMenuItem) {
    .title = "Tack Angle2",
    .subtitle = DefaultTackAngle,
    //.icon = PBL_IF_RECT_ELSE(timer_icon_image, NULL),
    //.callback = tack_angle_callback,
  };
  s_second_menu_items[1] = (SimpleMenuItem) {
    .title = "Wind Angle",
    .subtitle = DefaultWindAngle,
    //.icon = PBL_IF_RECT_ELSE(timer_icon_image, NULL),
    //.callback = tack_angle_callback,
  };
  */
}


static void cancel_callback(){
  APP_LOG(APP_LOG_LEVEL_INFO, "Cancelled!");
  window_stack_remove(menu_window, true);
}
static void close_callback(){
  APP_LOG(APP_LOG_LEVEL_INFO, "Close Timer & Exit!");
  app_worker_kill();
  //deinit();
  window_stack_remove(menu_window, true);
  //window_stack_remove(main_window, true);
  CloseMainWindow();
}
static void Setup_callback(){
  APP_LOG(APP_LOG_LEVEL_INFO, "Load Setup Window!");
  //setup_window_create();
  //window_stack_push(setup_window_get_window(), true);
  window_stack_remove(menu_window, true);
}
static void stop_timer_callback(){
  APP_LOG(APP_LOG_LEVEL_INFO, "Stop Back Ground Timer!");
  app_worker_kill();
  window_stack_remove(menu_window, true);
}
static void tack_angle_callback(){
  APP_LOG(APP_LOG_LEVEL_INFO, "Open Settings - Tack Angle!");
  settings_window_create("Tack Angle", "Angle",10);
  //alert_window_create(Alert, 5000, 1);
  window_stack_remove(menu_window, true);
  window_stack_push(settings_window_get_window(), true);
}
static void wind_angle_callback(){
  APP_LOG(APP_LOG_LEVEL_INFO, "Open Settings - Wind Angle!");
  settings_window_create("Wind Angle", "Angle",1);
  //alert_window_create(Alert, 5000, 1);
  window_stack_remove(menu_window, true);
  window_stack_push(settings_window_get_window(), true);
}

// Window Load event


static void menu_window_load(Window *window) {
  persist_read_string(10, DefaultTackAngle,sizeof(DefaultTackAngle));
  /*
  persist_read_string(0, MaxSpeedLeg0,sizeof(MaxSpeedLeg0));
  persist_read_string(1, MaxSpeedLeg1,sizeof(MaxSpeedLeg1));
  persist_read_string(2, MaxSpeedLeg2,sizeof(MaxSpeedLeg2));
  persist_read_string(3, MaxSpeedLeg3,sizeof(MaxSpeedLeg3));
  persist_read_string(4, MaxSpeedLeg4,sizeof(MaxSpeedLeg4));
  persist_read_string(5, MaxSpeedLeg5,sizeof(MaxSpeedLeg5));
  persist_read_string(6, MaxSpeedLeg6,sizeof(MaxSpeedLeg6));
  persist_read_string(7, MaxSpeedLeg7,sizeof(MaxSpeedLeg7));
  persist_read_string(8, MaxSpeedLeg8,sizeof(MaxSpeedLeg8));
  persist_read_string(9, MaxSpeedLeg9,sizeof(MaxSpeedLeg9));
  */
  
  APP_LOG(APP_LOG_LEVEL_INFO, "Menu Loading!");
  //timer_icon_image = gbitmap_create_with_resource(RESOURCE_ID_Timer);
  //sail_icon_image = gbitmap_create_with_resource(RESOURCE_ID_Sail);
  //angle_icon_image = gbitmap_create_with_resource(RESOURCE_ID_Angle);
  
  //int num_a_items = 0;
  s_first_menu_items[0] = (SimpleMenuItem) {
    .title = "Cancel",
    //.subtitle = "Here's a subtitle",
    //.icon = PBL_IF_RECT_ELSE(sail_icon_image, NULL),
    .callback = cancel_callback,
  };
  s_first_menu_items[1] = (SimpleMenuItem) {
    .title = "Stop Timer & Exit",
    //.subtitle = "Here's a subtitle",
    //.icon = PBL_IF_RECT_ELSE(timer_icon_image, NULL),
    .callback = close_callback,
  };
  
  s_second_menu_items[0] = (SimpleMenuItem) {
    .title = "Tack Angle",
    .subtitle =  DefaultTackAngle,
    //.icon = PBL_IF_RECT_ELSE(timer_icon_image, NULL),
    .callback = tack_angle_callback,
  };
  /*
  s_second_menu_items[1] = (SimpleMenuItem) {
    .title = "Wind Angle",
    .subtitle = "Not Used",//DefaultWindAngle,
  };
  s_third_menu_items[0] = (SimpleMenuItem) {
    .title = "Max Speed Leg 1",
    .subtitle = MaxSpeedLeg0,
  };
  s_third_menu_items[1] = (SimpleMenuItem) {
    .title = "Max Speed Leg 2",
    .subtitle = MaxSpeedLeg1,
  };
  s_third_menu_items[2] = (SimpleMenuItem) {
    .title = "Max Speed Leg 3",
    .subtitle = MaxSpeedLeg2,
  };
  s_third_menu_items[3] = (SimpleMenuItem) {
    .title = "Max Speed Leg 4",
    .subtitle = MaxSpeedLeg3,
  };
  s_third_menu_items[4] = (SimpleMenuItem) {
    .title = "Max Speed Leg 5",
    .subtitle = MaxSpeedLeg4,
  };
  s_third_menu_items[5] = (SimpleMenuItem) {
    .title = "Max Speed Leg 6",
    .subtitle = MaxSpeedLeg5,
  };
  s_third_menu_items[6] = (SimpleMenuItem) {
    .title = "Max Speed Leg 7",
    .subtitle = MaxSpeedLeg6,
  };
  s_third_menu_items[7] = (SimpleMenuItem) {
    .title = "Max Speed Leg 8",
    .subtitle = MaxSpeedLeg7,
  };
  s_third_menu_items[8] = (SimpleMenuItem) {
    .title = "Max Speed Leg 9",
    .subtitle = MaxSpeedLeg8,
  };
  s_third_menu_items[9] = (SimpleMenuItem) {
    .title = "Max Speed Leg 10",
    .subtitle = MaxSpeedLeg9,
  };
  */

  s_menu_sections[0] = (SimpleMenuSection) {
    .title = PBL_IF_RECT_ELSE("              Menu", NULL),
    .num_items = NUM_FIRST_MENU_ITEMS,
    .items = s_first_menu_items,
  };
  s_menu_sections[1] = (SimpleMenuSection) {
    .title = PBL_IF_RECT_ELSE("           Settings", NULL),
    .num_items = NUM_SECOND_MENU_ITEMS,
    .items = s_second_menu_items,
  };
  /*
  s_menu_sections[2] = (SimpleMenuSection) {
    .title = PBL_IF_RECT_ELSE("           Max Speed", NULL),
    .num_items = NUM_THIRD_MENU_ITEMS,
    .items = s_third_menu_items,
  };
  */
  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  s_simple_menu_layer = simple_menu_layer_create(bounds, window, s_menu_sections, NUM_MENU_SECTIONS, NULL);
  layer_add_child(window_layer, simple_menu_layer_get_layer(s_simple_menu_layer));
  
  app_worker_launch();
}

void menu_window_unload(Window *window) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Menu Window Unloading!");
  simple_menu_layer_destroy(s_simple_menu_layer);
  //gbitmap_destroy(timer_icon_image);
  //gbitmap_destroy(angle_icon_image);
  //gbitmap_destroy(sail_icon_image);
  SendSavedDataToWatch();
}

void menu_window_create(){
  menu_window = window_create();
  window_set_window_handlers(menu_window,(WindowHandlers){
    .load = menu_window_load,
    .unload = menu_window_unload
  });
}

Window *menu_window_get_window(){
  return menu_window;
}