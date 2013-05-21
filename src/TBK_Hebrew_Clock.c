#include "TBK_Hebrew_Clock.h"

// MAIN - starts the app
void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    // Handle app start
    .init_handler = &handle_init,
    // Handle time updates
    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }
  };
  app_event_loop(params, &handlers);
}

// Handles the system init event - called once at startup
void handle_init(AppContextRef ctx) {
  appContext = ctx;
  window_init(&window, "Window Name");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);
  resource_init_current_app(&APP_RESOURCES);
  
  // Init time format string
  timeFormat = clock_is_24h_style()?"%R":"%I:%M";
  
  // Init the text layer used to show the hebrew date
  text_layer_init(&hebrewDateLayer, GRect(0, 0, 144, 25));
  text_layer_set_text_color(&hebrewDateLayer, GColorWhite);
  text_layer_set_background_color(&hebrewDateLayer, GColorClear);
  text_layer_set_text_alignment(&hebrewDateLayer, GTextAlignmentCenter);
  text_layer_set_font(&hebrewDateLayer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  layer_add_child(&window.layer, &hebrewDateLayer.layer);
  
  // Init the text layer used to show the time
  text_layer_init(&timeLayer, GRect(0, 25, 144, 50));
  text_layer_set_text_color(&timeLayer, GColorWhite);
  text_layer_set_text_alignment(&timeLayer, GTextAlignmentCenter);
  text_layer_set_background_color(&timeLayer, GColorClear);
  text_layer_set_font(&timeLayer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_49)));
  layer_add_child(&window.layer, &timeLayer.layer);
  
  // Init the text layer used to show the moon phase
  text_layer_init(&moonLayer, GRect(0, 85, 144 /* width */, 168-115 /* height */));
  text_layer_set_text_color(&moonLayer, GColorWhite);
  text_layer_set_background_color(&moonLayer, GColorClear);
  text_layer_set_font(&moonLayer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MOON_PHASES_SUBSET_30)));
  text_layer_set_text_alignment(&moonLayer, GTextAlignmentCenter);
  layer_add_child(&window.layer, &moonLayer.layer);
  
  // Init the text layer used to show the Sunrise hour
  text_layer_init(&sunriseLayer, window.layer.frame);
  text_layer_set_text_color(&sunriseLayer, GColorWhite);
  text_layer_set_background_color(&sunriseLayer, GColorClear);
  layer_set_frame(&sunriseLayer.layer, GRect(0, 145, 144, 30));
  text_layer_set_font(&sunriseLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(&sunriseLayer, GTextAlignmentLeft);
  layer_add_child(&window.layer, &sunriseLayer.layer);
  
  // Init the text layer used to show theHatsot hour
  text_layer_init(&hatsotLayer, window.layer.frame);
  text_layer_set_text_color(&hatsotLayer, GColorWhite);
  text_layer_set_background_color(&hatsotLayer, GColorClear);
  layer_set_frame(&hatsotLayer.layer, GRect(0, 145, 144, 30));
  text_layer_set_font(&hatsotLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(&hatsotLayer, GTextAlignmentCenter);
  layer_add_child(&window.layer, &hatsotLayer.layer);
  
  // Init the text layer used to show the Sunset hour
  text_layer_init(&sunsetLayer, window.layer.frame);
  text_layer_set_text_color(&sunsetLayer, GColorWhite);
  text_layer_set_background_color(&sunsetLayer, GColorClear);
  layer_set_frame(&sunsetLayer.layer, GRect(0, 145, 144, 30));
  text_layer_set_font(&sunsetLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(&sunsetLayer, GTextAlignmentRight);
  layer_add_child(&window.layer, &sunsetLayer.layer);
  
  updateWatch();  // update display at startup to avoid empty screen until next tick
}

// Handles the system minute-tick, calls appropriate functions below
void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
  (void)t;
  appContext = ctx;
  updateWatch();
}

// Update watch as needed
void updateWatch() {
  static int currentYDay = -1;
  static int currentHour = -1;
  
  PblTm currentTime;
  get_time(&currentTime);
  
  // call update functions as requires, daily first, then hourly, then minute
  if(currentTime.tm_yday != currentYDay) {  // Day has changed, or app just started
    currentYDay = currentTime.tm_yday;
    dayHasChanged(&currentTime);
  }
  if(currentTime.tm_hour != currentHour) {  // Hour has changed, or app just started
    currentHour = currentTime.tm_hour;
    hourHasChanged(&currentTime);
  }
  minuteHasChanged(&currentTime);
}

// Called once per day at midnight, and once at startup
void dayHasChanged(PblTm *currentTime) {
  // Update moon phase
  int moonphase_number = moon_phase(tm2jd(currentTime));
  // correct for southern hemisphere
  if ((moonphase_number > 0) && (LATITUDE < 0))
    moonphase_number = 28 - moonphase_number;
  // select correct font char
  if (moonphase_number == 14)
  {
    moonString[0] = (unsigned char)(48);
  } else if (moonphase_number == 0) {
    moonString[0] = (unsigned char)(49);
  } else if (moonphase_number < 14) {
    moonString[0] = (unsigned char)(moonphase_number+96);
  } else {
    moonString[0] = (unsigned char)(moonphase_number+95);
  }
  text_layer_set_text(&moonLayer, moonString);
}

// Called once per hour when minute becomes 0, and once at startup
void hourHasChanged(PblTm *currentTime) {
  
}

// Called once per minute, and once at startup
void minuteHasChanged(PblTm *currentTime) {
  // Display Time
  string_format_time(timeString, sizeof(timeString), timeFormat, currentTime);
  text_layer_set_text(&timeLayer, timeString);
}

// Utility functions

void adjustTimezone(float* time)
{
  *time += TIMEZONE;
  if (*time > 24) *time -= 24;
  if (*time < 0) *time += 24;
}

//return julian day number for time
int tm2jd(PblTm *time)
{
  int y,m;
  y = time->tm_year + 1900;
  m = time->tm_mon + 1;
  return time->tm_mday-32075+1461*(y+4800+(m-14)/12)/4+367*(m-2-(m-14)/12*12)/12-3*((y+4900+(m-14)/12)/100)/4;
}

int moon_phase(int jdn)
{
  double jd;
  jd = jdn-2451550.1;
  jd /= 29.530588853;
  jd -= (int)jd;
  return (int)(jd*27 + 0.5); /* scale fraction from 0-27 and round by adding 0.5 */
}