// TBK Hebrew Clock
// A Jewish calendar/zmanim watch face for the Pebble
// Copyright (C) 2013 Ary Tebeka contact@arytbk.net
// Open Source - feel free to use, modify, contribute
// Attribution and comments welcome

// Parts from KP_Sun_Moon_Vibe_Clock - https://github.com/KarbonPebbler/KP_Sun_Moon_Vibe_Clock

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
  
  // ******************************************
  // Init Layers - top to bottom, left to right
  // ******************************************
  
  // Gregorian Day
  text_layer_init(&dayLayer, GRect(0, 0, 144, 25));
  text_layer_set_text_color(&dayLayer, GColorWhite);
  text_layer_set_background_color(&dayLayer, GColorClear);
  text_layer_set_text_alignment(&dayLayer, GTextAlignmentLeft);
  text_layer_set_font(&dayLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24));;
  layer_add_child(&window.layer, &dayLayer.layer);
  
  // Hebrew Day
  text_layer_init(&hDayLayer, GRect(0, 0, 144, 25));
  text_layer_set_text_color(&hDayLayer, GColorWhite);
  text_layer_set_background_color(&hDayLayer, GColorClear);
  text_layer_set_text_alignment(&hDayLayer, GTextAlignmentRight);
  text_layer_set_font(&hDayLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24));;
  layer_add_child(&window.layer, &hDayLayer.layer);
  
  //  Moon phase
  text_layer_init(&moonLayer, GRect(0, 10, 144, 168-115));
  text_layer_set_text_color(&moonLayer, GColorWhite);
  text_layer_set_background_color(&moonLayer, GColorClear);
  text_layer_set_font(&moonLayer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MOON_PHASES_SUBSET_30)));
  text_layer_set_text_alignment(&moonLayer, GTextAlignmentCenter);
  layer_add_child(&window.layer, &moonLayer.layer);
  
  // Gregorian Month
  text_layer_init(&monthLayer, GRect(0, 25, 144, 15));
  text_layer_set_text_color(&monthLayer, GColorWhite);
  text_layer_set_background_color(&monthLayer, GColorClear);
  text_layer_set_text_alignment(&monthLayer, GTextAlignmentLeft);
  text_layer_set_font(&monthLayer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));;
  layer_add_child(&window.layer, &monthLayer.layer);
  
  // Hebrew Month
  text_layer_init(&hMonthLayer, GRect(0, 25, 144, 15));
  text_layer_set_text_color(&hMonthLayer, GColorWhite);
  text_layer_set_background_color(&hMonthLayer, GColorClear);
  text_layer_set_text_alignment(&hMonthLayer, GTextAlignmentRight);
  text_layer_set_font(&hMonthLayer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));;
  layer_add_child(&window.layer, &hMonthLayer.layer);
  
  //  Time
  text_layer_init(&timeLayer, GRect(0, 40, 144, 50));
  text_layer_set_text_color(&timeLayer, GColorWhite);
  text_layer_set_text_alignment(&timeLayer, GTextAlignmentCenter);
  text_layer_set_background_color(&timeLayer, GColorClear);
  text_layer_set_font(&timeLayer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_49)));
  layer_add_child(&window.layer, &timeLayer.layer);
  
  // Line
  layer_init(&lineLayer, window.layer.frame);
  lineLayer.update_proc = &lineLayerUpdate;
  layer_add_child(&window.layer, &lineLayer);
  
  // Zman hours labels
  text_layer_init(&zmanHourLabelLayer, GRect(0, 100, 144, 15));
  text_layer_set_text_color(&zmanHourLabelLayer, GColorWhite);
  text_layer_set_background_color(&zmanHourLabelLayer, GColorClear);
  text_layer_set_text_alignment(&zmanHourLabelLayer, GTextAlignmentLeft);
  text_layer_set_font(&zmanHourLabelLayer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));;
  layer_add_child(&window.layer, &zmanHourLabelLayer.layer);
  text_layer_set_text(&zmanHourLabelLayer, zmanHourLabelString);
  
  text_layer_init(&nextHourLabelLayer, GRect(0, 100, 144, 15));
  text_layer_set_text_color(&nextHourLabelLayer, GColorWhite);
  text_layer_set_background_color(&nextHourLabelLayer, GColorClear);
  text_layer_set_text_alignment(&nextHourLabelLayer, GTextAlignmentRight);
  text_layer_set_font(&nextHourLabelLayer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));;
  layer_add_child(&window.layer, &nextHourLabelLayer.layer);
  text_layer_set_text(&nextHourLabelLayer, nextHourLabelString);
  
  // Sun Graph
  layer_init(&sunGraphLayer, GRect(72-sunSize/2, sunY, sunSize, sunSize));
  sunGraphLayer.update_proc = &sunGraphLayerUpdate;
  layer_set_clips(&sunGraphLayer, true);
  layer_add_child(&window.layer, &sunGraphLayer);
  
  // Zman hour number
  text_layer_init(&zmanHourLayer, GRect(0, 108, 144, 25));
  text_layer_set_text_color(&zmanHourLayer, GColorWhite);
  text_layer_set_background_color(&zmanHourLayer, GColorClear);
  text_layer_set_text_alignment(&zmanHourLayer, GTextAlignmentLeft);
  text_layer_set_font(&zmanHourLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24));;
  layer_add_child(&window.layer, &zmanHourLayer.layer);
  
  // Next zman hour
  text_layer_init(&nextHourLayer, GRect(0, 108, 144, 25));
  text_layer_set_text_color(&nextHourLayer, GColorWhite);
  text_layer_set_background_color(&nextHourLayer, GColorClear);
  text_layer_set_text_alignment(&nextHourLayer, GTextAlignmentRight);
  text_layer_set_font(&nextHourLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24));;
  layer_add_child(&window.layer, &nextHourLayer.layer);
  
  //  Sunrise hour
  text_layer_init(&sunriseLayer, window.layer.frame);
  text_layer_set_text_color(&sunriseLayer, GColorWhite);
  text_layer_set_background_color(&sunriseLayer, GColorClear);
  layer_set_frame(&sunriseLayer.layer, GRect(0, 145, 144, 30));
  text_layer_set_font(&sunriseLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(&sunriseLayer, GTextAlignmentLeft);
  layer_add_child(&window.layer, &sunriseLayer.layer);
  
  // Hatsot hour
  text_layer_init(&hatsotLayer, window.layer.frame);
  text_layer_set_text_color(&hatsotLayer, GColorWhite);
  text_layer_set_background_color(&hatsotLayer, GColorClear);
  layer_set_frame(&hatsotLayer.layer, GRect(0, 145, 144, 30));
  text_layer_set_font(&hatsotLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(&hatsotLayer, GTextAlignmentCenter);
  layer_add_child(&window.layer, &hatsotLayer.layer);
  
  //  Sunset hour
  text_layer_init(&sunsetLayer, window.layer.frame);
  text_layer_set_text_color(&sunsetLayer, GColorWhite);
  text_layer_set_background_color(&sunsetLayer, GColorClear);
  layer_set_frame(&sunsetLayer.layer, GRect(0, 145, 144, 30));
  text_layer_set_font(&sunsetLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(&sunsetLayer, GTextAlignmentRight);
  layer_add_child(&window.layer, &sunsetLayer.layer);
  
  updateWatch();  // update display at startup to avoid empty screen until next tick
}

// Draw line
void lineLayerUpdate(Layer *me, GContext* ctx) {
  (void)me;
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_line(ctx, GPoint(0, 97), GPoint(144, 97));
  graphics_draw_line(ctx, GPoint(0, 98), GPoint(144, 98));
}

// Draw sun graph
void sunGraphLayerUpdate(Layer *me, GContext* ctx)
{
  (void)me;
  // Draw white circle
  graphics_context_set_fill_color(ctx, GColorWhite);
  GPoint sunCenter = grect_center_point(&sunGraphLayer.frame);
  graphics_fill_circle(ctx, GPoint(sunSize/2, sunSize/2), sunSize/2);
  
  // Must fill night part with black
  graphics_context_set_fill_color(ctx, GColorBlack);
  gpath_init(&sun_path, &sun_path_info);
  gpath_move_to(&sun_path, GPoint(sunSize/2, sunSize/2));
  gpath_draw_filled(ctx, &sun_path);
}

void handle_deinit(AppContextRef ctx) {
  (void)ctx;
}

// ************* TICK HANDLER *****************
// Handles the system minute-tick, calls appropriate functions below
void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
  (void)t;
  appContext = ctx;
  updateWatch();
}

// ************* Update watch as needed, calls required update functions at the right times, and once at startup
void updateWatch() {
  static int currentYDay = -1;
  static int currentHour = -1;
  
  get_time(&currentPblTime);
  currentTime = ((float)currentPblTime.tm_hour) + ((float)currentPblTime.tm_min)/60.0;
  
  // call update functions as requires, daily first, then hourly, then minute
  if(currentPblTime.tm_yday != currentYDay) {  // Day has changed, or app just started
    currentYDay = currentPblTime.tm_yday;
    doEveryDay();
  }
  if(currentPblTime.tm_hour != currentHour) {  // Hour has changed, or app just started
    currentHour = currentPblTime.tm_hour;
    doEveryHour();
  }
  doEveryMinute();
}

// Called once per day at midnight, and once at startup
void doEveryDay() {
  updateDate();
  updateHebrewDate();
  updateMoonAndSun();
}

// Called once per hour when minute becomes 0, and once at startup
void doEveryHour() {
  
}

// Called once per minute, and once at startup
void doEveryMinute() {
  updateTime();
  updateZmanim();
}

// ******************************** Now the real works begins! ******************
// From least frequent updates to most frequent updates
// ******************************************************************************

// Update Gregorian Date
void updateDate() {
  static char *monthNames[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  xsprintf(dayString, "%d", currentPblTime.tm_mday);
  xsprintf(monthString, "%s", monthNames[currentPblTime.tm_mon]);
  text_layer_set_text(&dayLayer, dayString);
  text_layer_set_text(&monthLayer, monthString);
}

// Update Hebrew Date
void updateHebrewDate() {
  int julianDay = hdate_gdate_to_jd(currentPblTime.tm_mday, currentPblTime.tm_mon + 1, currentPblTime.tm_year + 1900);
  // Convert julian day to hebrew date
  int hDay, hMonth, hYear, hDayTishrey, hNextTishrey;
  hdate_jd_to_hdate(julianDay, &hDay, &hMonth, &hYear, &hDayTishrey, &hNextTishrey);
  char *hebrewMonthName = hdate_get_month_string(hMonth);
  xsprintf(hDayString, "%d",hDay);
  text_layer_set_text(&hDayLayer, hDayString);
  text_layer_set_text(&hMonthLayer, hebrewMonthName);
}

// Update MOON phase and Sun info
void updateMoonAndSun() {
  // ******************* MOON
  int moonphase_number = moon_phase(tm2jd(&currentPblTime));
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
  
  // ******************* SUN TIMES
  sunriseTime = calcSunRise(currentPblTime.tm_year, currentPblTime.tm_mon+1, currentPblTime.tm_mday, LATITUDE, LONGITUDE, 91.0f);
  sunsetTime = calcSunSet(currentPblTime.tm_year, currentPblTime.tm_mon+1, currentPblTime.tm_mday, LATITUDE, LONGITUDE, 91.0f);
	hatsotTime = (sunriseTime+sunsetTime)/2.0f;
  
  adjustTimezone(&sunriseTime);
  adjustTimezone(&sunsetTime);
	adjustTimezone(&hatsotTime);
  
  PblTm pblTime;
  pblTime.tm_min = (int)(60*(sunriseTime-((int)(sunriseTime))));
  pblTime.tm_hour = (int)sunriseTime;
  string_format_time(sunriseString, sizeof(sunriseString), timeFormat, &pblTime);
  text_layer_set_text(&sunriseLayer, sunriseString);
  
  pblTime.tm_min = (int)(60*(hatsotTime-((int)(hatsotTime))));
  pblTime.tm_hour = (int)hatsotTime;
  string_format_time(hatsotString, sizeof(hatsotString), timeFormat, &pblTime);
  text_layer_set_text(&hatsotLayer, hatsotString);
  
  pblTime.tm_min = (int)(60*(sunsetTime-((int)(sunsetTime))));
  pblTime.tm_hour = (int)sunsetTime;
  string_format_time(sunsetString, sizeof(sunsetString), timeFormat, &pblTime);
  text_layer_set_text(&sunsetLayer, sunsetString);
  
  // SUN GRAPHIC
  float rise2 = sunriseTime+12.0f;
  sun_path_info.points[1].x = (int16_t)(my_sin(rise2/24 * M_PI * 2) * 120);
  sun_path_info.points[1].y = -(int16_t)(my_cos(rise2/24 * M_PI * 2) * 120);
  float set2 =   sunsetTime+12.0f;
  sun_path_info.points[4].x = (int16_t)(my_sin(set2/24 * M_PI * 2) * 120);
  sun_path_info.points[4].y = -(int16_t)(my_cos(set2/24 * M_PI * 2) * 120);
}

// Update zmanim
void updateZmanim() {
  float zmanTime;
  int zn;
  if((currentTime >= sunriseTime) && (currentTime <= sunsetTime)) { // Day
    zmanHourDuration = (sunsetTime - sunriseTime)/12.0;
    zmanTime = (currentTime-sunriseTime)/zmanHourDuration;
    zn = (int) zmanTime;
    timeUntilNextHour = sunriseTime + (((float)(zn+1))*zmanHourDuration) - currentTime;
  } else {  // Night
    zmanHourDuration = (24.0 - (sunsetTime-sunriseTime))/12.0;
    if(currentTime < sunriseTime) {
      zmanTime = (12.0+currentTime-sunsetTime)/zmanHourDuration;
      zn = (int) zmanTime;
      timeUntilNextHour = sunsetTime + (((float)(zn+1))*zmanHourDuration) - currentTime - 12.0;
    } else {
      zmanTime = (currentTime - sunsetTime)/zmanHourDuration;
      zn = (int) zmanTime;
      timeUntilNextHour = sunsetTime + (((float)(zn+1))*zmanHourDuration) - currentTime;
    }
  }
  zmanHourNumber = zn+1;  // We start at hour # 1 not 0
  int nextHour = (int)timeUntilNextHour;
  int nextMinute = (int)((timeUntilNextHour - ((float)nextHour))*60);
  xsprintf(zmanHourString, "%d", zmanHourNumber);
  xsprintf(nextHourString, "%d:%02d",nextHour, nextMinute);
  text_layer_set_text(&zmanHourLayer, zmanHourString);
  text_layer_set_text(&nextHourLayer, nextHourString);
}

// Update time
void updateTime() {
  /*
  char time_text[]="00:00";
  string_format_time(time_text, sizeof(time_text), timeFormat, &currentPblTime);
  if (!clock_is_24h_style() && (time_text[0] == '0'))
  {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }
  */
  string_format_time(timeString, sizeof(timeString), timeFormat, &currentPblTime);
  text_layer_set_text(&timeLayer, timeString);
}

// ******************** Utility functions ****************

void adjustTimezone(float* time)
{
  // ****************** warning tm_idst is not implemented yet, find another way! ********************
  if (ISDST)  // Currently using DST flag in config.h
  {
    *time += 1.0;
  }
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

float get24HourAngle(int hours, int minutes)
{
  return (12.0f + hours + (minutes/60.0f)) / 24.0f;
}

