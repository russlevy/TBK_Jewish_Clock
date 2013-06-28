// TBK Jewish Clock
// A Jewish calendar/zmanim watch face for the Pebble
// Copyright (C) 2013 Ary Tebeka contact@arytbk.net
// Open Source - feel free to use, modify, contribute
// Attribution and comments welcome

// Check our iOS apps:
// https://itunes.apple.com/us/artist/ary-tebeka/id375595955?uo=4
// https://itunes.apple.com/us/artist/tebeka-software-solutions/id285016307?uo=4

// Parts of this project are from KP_Sun_Moon_Vibe_Clock - https://github.com/KarbonPebbler/KP_Sun_Moon_Vibe_Clock and libhdate - http://libhdate.sourceforge.net

#include "TBK_Jewish_Clock.h"

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
  window_set_background_color(&window, kBackgroundColor);
  resource_init_current_app(&APP_RESOURCES);
  
  // Init time format string
  timeFormat = clock_is_24h_style()?"%R":"%I:%M";

  // Define fonts
  GFont tinyFont = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  GFont smallFont = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
  GFont mediumFont = fonts_get_system_font(FONT_KEY_GOTHIC_24);
  GFont mediumBoldFont = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
  GFont largeFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_49));
  GFont moonFont = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MOON_PHASES_SUBSET_30));
  
  // ******************************************
  // Init Layers - top to bottom, left to right
  // ******************************************
  
  // Gregorian Day
  initTextLayer(&dayLayer, 0, 0, 144, 25, kTextColor, GColorClear, GTextAlignmentLeft, mediumFont);

  // Hebrew Day
  initTextLayer(&hDayLayer, 0, 0, 144, 25, kTextColor, GColorClear, GTextAlignmentRight, mediumFont);
  
  //  Moon phase
//  initTextLayer(&moonLayer, 0, 10, 144, 168-115, kTextColor, GColorClear, GTextAlignmentCenter, moonFont);
  initTextLayer(&moonLayer, 56, 9, 32, 32, kTextColor, GColorClear, GTextAlignmentCenter, moonFont);

    
  // Gregorian Month
  initTextLayer(&monthLayer, 0, 25, 144, 15, kTextColor, GColorClear, GTextAlignmentLeft, smallFont);
  
  // Hebrew Month
  initTextLayer(&hMonthLayer, 0, 25, 144, 15, kTextColor, GColorClear, GTextAlignmentRight, smallFont);
  
  //  Time
  initTextLayer(&timeLayer, 0, 40, 144, 50, kTextColor, GColorClear, GTextAlignmentCenter, largeFont);
  
  // Line
  layer_init(&lineLayer, window.layer.frame);
  lineLayer.update_proc = &lineLayerUpdate;
  layer_add_child(&window.layer, &lineLayer);
  
  // Zman hours labels
  initTextLayer(&zmanHourLabelLayer, 0, 100, 144, 15, kTextColor, GColorClear, GTextAlignmentLeft, smallFont);
  text_layer_set_text(&zmanHourLabelLayer, zmanHourLabelString);
  initTextLayer(&nextHourLabelLayer, 0, 100, 144, 15, kTextColor, GColorClear, GTextAlignmentRight, smallFont);
  text_layer_set_text(&nextHourLabelLayer, nextHourLabelString);
  
  // Sun Graph
  layer_init(&sunGraphLayer, GRect(72-sunSize/2, sunY, sunSize+2, sunSize+2));
  sunGraphLayer.update_proc = &sunGraphLayerUpdate;
  layer_set_clips(&sunGraphLayer, true);
  layer_add_child(&window.layer, &sunGraphLayer);
  
  // Optional Alert message
  initTextLayer(&alertLayer, 0, 102, 144, 36, kBackgroundColor, kTextColor, GTextAlignmentCenter, mediumBoldFont);
  layer_remove_from_parent(&alertLayer.layer);  // don't show now!
  
  // Zman hour number
  initTextLayer(&zmanHourLayer, 0, 108, 144, 25, kTextColor, GColorClear, GTextAlignmentLeft, mediumFont);
  
  // Next zman hour
  initTextLayer(&nextHourLayer, 0, 108, 144, 25, kTextColor, GColorClear, GTextAlignmentRight, mediumFont);
  
  //  Sunrise hour
  initTextLayer(&sunriseLayer, 0, 145, 144, 30, kTextColor, GColorClear, GTextAlignmentLeft, tinyFont);
  
  // Hatsot hour
  initTextLayer(&hatsotLayer, 0, 145, 144, 30, kTextColor, GColorClear, GTextAlignmentCenter, tinyFont);
  
  //  Sunset hour
  initTextLayer(&sunsetLayer, 0, 145, 144, 30, kTextColor, GColorClear, GTextAlignmentRight, tinyFont);
  
  updateWatch();  // update display at startup to avoid empty screen until next tick
}

// Draw line
void lineLayerUpdate(Layer *me, GContext* ctx) {
  (void)me;
  graphics_context_set_stroke_color(ctx, kTextColor);
  graphics_draw_line(ctx, GPoint(0, 97), GPoint(144, 97));
  graphics_draw_line(ctx, GPoint(0, 98), GPoint(144, 98));
}

// Draw sun graph
void sunGraphLayerUpdate(Layer *me, GContext* ctx)
{
  (void)me;
    
    // Fill layer with black
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_rect(ctx, GRect(0, 0, sunSize+2, sunSize+2), 0, 0);
    
  GPoint sunCenter = GPoint(sunSize/2, sunSize/2);
  // Draw white filled circle
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, sunCenter, sunSize/2);
  
  // Must fill night part with black
  graphics_context_set_fill_color(ctx, GColorBlack);
  gpath_init(&sun_path, &sun_path_info);
  gpath_move_to(&sun_path, sunCenter);
  gpath_draw_filled(ctx, &sun_path);
  
  // Draw hand/needle at current time
  // Black if day, white if night
  if((currentTime >= sunriseTime) && (currentTime <= sunsetTime)) { // Day
    graphics_context_set_stroke_color(ctx, GColorBlack);
  } else {  // night
    graphics_context_set_stroke_color(ctx, GColorWhite);
  }
  float angle = (18.0 - minutes2Hours(currentTime))/24.0 * 2.0 * M_PI;
  GPoint toPoint = GPoint(sunCenter.x + my_cos(angle)*sunSize/2, sunCenter.y - my_sin(angle)*sunSize/2);
  graphics_draw_line(ctx, sunCenter, toPoint);
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
  currentTime = (currentPblTime.tm_hour * 60) + currentPblTime.tm_min;
  
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
  // Must update Sun Graph rendering
  layer_mark_dirty(&sunGraphLayer);
  checkAlerts();
}

// ******************************** Now the real work begins! ******************
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
    hebrewDayNumber = hDay;
  char *hebrewMonthName = hdate_get_month_string(hMonth);
  xsprintf(hDayString, "%d",hDay);
  text_layer_set_text(&hDayLayer, hDayString);
  text_layer_set_text(&hMonthLayer, hebrewMonthName);
}

// Update MOON phase and Sun info
void updateMoonAndSun() {
  // ******************* MOON
    
    // Moonphase font:
    // A-Z phases on white background
    // a-z phases on black background
    // 0 = new moon
    
    
    float jDay = (float) (hebrewDayNumber - 1);     // 0 to 29
    float mPhase = jDay * 26.0 / 29.0;  // 0 to 26
    int moonphase_number = (int)mPhase;
    // correct for southern hemisphere
    if ((moonphase_number > 0) && (LATITUDE < 0)) {
        moonphase_number = 26 - moonphase_number;
    }

    char moonChar;
    
    if(moonphase_number < 1) {    // new moon
        moonChar = '0';
    } else {
        int offset = moonphase_number - 1;
#ifndef REVERSED
        // Black background we must use the opposite phase direction...
        if(offset >= 13) {
            offset -= 13;
        } else {
            offset += 13;
        }
#endif
        moonChar = 'A' + offset;
    }
    moonString[0] = moonChar;
    
    /*
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
     */
     
  text_layer_set_text(&moonLayer, moonString);
  
  // ******************* SUN TIMES
  sunriseTime = hours2Minutes(calcSunRise(currentPblTime.tm_year, currentPblTime.tm_mon+1, currentPblTime.tm_mday, LATITUDE, LONGITUDE, 91.0f));
  sunsetTime = hours2Minutes(calcSunSet(currentPblTime.tm_year, currentPblTime.tm_mon+1, currentPblTime.tm_mday, LATITUDE, LONGITUDE, 91.0f));
	hatsotTime = (sunriseTime+sunsetTime)/2;
  
  adjustTimezone(&sunriseTime);
  adjustTimezone(&sunsetTime);
	adjustTimezone(&hatsotTime);
  
  displayTime(sunriseTime, &sunriseLayer, sunriseString, sizeof(sunriseString));
  displayTime(hatsotTime, &hatsotLayer, hatsotString, sizeof(hatsotString));
  displayTime(sunsetTime, &sunsetLayer, sunsetString, sizeof(sunsetString));
  
  // SUN GRAPHIC
  float rise2 = minutes2Hours(sunriseTime)+12.0f;
  sun_path_info.points[1].x = (int16_t)(my_sin(rise2/24 * M_PI * 2) * 120);
  sun_path_info.points[1].y = -(int16_t)(my_cos(rise2/24 * M_PI * 2) * 120);
  float set2 =  minutes2Hours(sunsetTime)+12.0f;
  sun_path_info.points[4].x = (int16_t)(my_sin(set2/24 * M_PI * 2) * 120);
  sun_path_info.points[4].y = -(int16_t)(my_cos(set2/24 * M_PI * 2) * 120);
}

// Update zmanim
void updateZmanim() {
  zmanHourDuration = (minutes2Hours(sunsetTime) - minutes2Hours(sunriseTime)) / 12.0; // in hours
  int startTime = sunriseTime;
  int theTime = currentTime;
  if((currentTime < sunriseTime) || (currentTime > sunsetTime)) { // Night
    startTime = sunsetTime;
    zmanHourDuration = 2.0 - zmanHourDuration; // Day hour + Night hour = 2
    if(currentTime < sunriseTime) theTime += 24*60;
  }
  float zh = ((float)(theTime - startTime))/60.0/zmanHourDuration;
  zmanHourNumber = (int)zh + 1;
  float hoursUntilNext = ((float)zmanHourNumber)*zmanHourDuration;
  timeUntilNextHour = startTime + ((int)(hoursUntilNext * 60.0)) - theTime;

  int nextHour = timeUntilNextHour / 60;
  int nextMinute = timeUntilNextHour % 60;
  xsprintf(zmanHourString, "%d", zmanHourNumber);
  xsprintf(nextHourString, "%d:%02d",nextHour, nextMinute);
  text_layer_set_text(&zmanHourLayer, zmanHourString);
  text_layer_set_text(&nextHourLayer, nextHourString);
}

// **** Check if alert needed and show/vibrate if needed
void checkAlerts() {
  static int mustRemove = 0;
  int mustAlert = 0;
  
  if(mustRemove) {
    layer_remove_from_parent(&alertLayer.layer);
    mustRemove=0;
  }

#ifdef MINCHA_GEDOLA_ALERT
  if(currentTime == (hatsotTime + ((int)(zmanHourDuration*60.0*0.5)))) {  // half an hour after midday
    mustAlert = -1;
    xsprintf(alertString, "MINCHA-G", MINCHA_ALERT);
  }
#endif

  
#ifdef MINCHA_KETANA_ALERT
  if(currentTime == (sunsetTime - ((int)(zmanHourDuration*60.0*2.5)))) {  // 2,5 hours before sunset
    mustAlert = -1;
    xsprintf(alertString, "MINCHA-K", MINCHA_ALERT);
  }
#endif

#ifdef MINCHA_GEDOLA_ALERT
  if(currentTime == (sunsetTime - MINCHA_ALERT)) { // this is the minute for the alert
    mustAlert = -1;
    xsprintf(alertString, "SUNSET-%dmn", MINCHA_ALERT);
  }
#endif
    
#ifdef PLAG_ALERT
    if(currentTime == (sunsetTime - ((int)(zmanHourDuration*60.0*1.25)))) {  // 1 and a quarter hour before sunset
        mustAlert = -1;
        xsprintf(alertString, "PLAG", MINCHA_ALERT);
    }
#endif
    
#ifdef SUNSET_ALERT
    if(currentTime == (sunsetTime)) {
        mustAlert = -1;
        xsprintf(alertString, "SUNSET NOW!", MINCHA_ALERT);
    }
#endif

  if(mustAlert) {
    mustAlert = -1;
    text_layer_set_text(&alertLayer, alertString);
    layer_add_child(&window.layer, &alertLayer.layer);  // show message
    vibes_enqueue_custom_pattern(alertPattern);  // Vibrate
    mustRemove=-1;
  }
}

// Update time
void updateTime() {
  displayTime(currentTime, &timeLayer, timeString, sizeof(timeString));
}

// ******************** Utility functions ****************

// Initializes a text layer
void initTextLayer(TextLayer *theLayer, int x, int y, int w, int h, GColor textColor, GColor backgroundColor, GTextAlignment alignment, GFont theFont) {
  text_layer_init(theLayer, GRect(x, y, w, h));
  text_layer_set_text_color(theLayer, textColor);
  text_layer_set_background_color(theLayer, backgroundColor);
  text_layer_set_text_alignment(theLayer, alignment);
  text_layer_set_font(theLayer, theFont);
  layer_add_child(&window.layer, &theLayer->layer);
}

void adjustTimezone(int* time)  // time as minutes since midnight
{
  // ****************** warning tm_idst is not implemented yet, currently using a compile flag, find another way! ********************
  if (ISDST)  // Currently using DST flag in config.h
  {
    *time += 60;
  }
  *time += (TIMEZONE*60);
  if (*time >= (24*60)) *time -= (24*60);
  if (*time < 0) *time += (24*60);
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

int hours2Minutes(float theTime) {
  int hours = (int)theTime;
  int minutes = (int)((theTime - hours)*60.0);
  return (hours * 60) + minutes;
}

float minutes2Hours(int theTime) {
  return ((float)(theTime))/60.0;
}

void displayTime(int theTime, TextLayer *theLayer, char *theString, int maxSize){
  PblTm thePblTime;
  thePblTime.tm_hour = theTime / 60;
  thePblTime.tm_min = theTime % 60;
  string_format_time(theString, maxSize, timeFormat, &thePblTime);
  text_layer_set_text(theLayer, theString);
}
