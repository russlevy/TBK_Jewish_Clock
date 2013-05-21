// TBK Hebrew Clock
// A Jewish calendar/zmanim watch face for the Pebble
// Copyright (C) 2013 Ary Tebeka contact@arytbk.net
// Open Source - feel free to use, modify, contribute
// Attribution and comments welcome

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "my_math.h"
#include "suncalc.h"
#include "xprintf.h"
#include "hebrewdate.h"
#include "config.h"

// App Configuration
#define MY_UUID { 0xB6, 0x79, 0x42, 0x92, 0xFA, 0x2C, 0x4B, 0x4B, 0x9A, 0xCC, 0xE8, 0xD6, 0x13, 0x68, 0x57, 0x56 }
PBL_APP_INFO(MY_UUID,
             "TBK Hebrew Clock", "Ary & Haim Tebeka",
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);
Window window;
AppContextRef appContext;

// Text Layers and associated strings
TextLayer timeLayer;          char timeString[]=        "00:00";
TextLayer sunriseLayer;       char sunriseString[]=     "00:00";
TextLayer sunsetLayer;        char sunsetString[]=      "00:00";
TextLayer hatsotLayer;        char hatsotString[]=      "00:00";
TextLayer dateLayer;          char dateString[]=        "Wednesday September 31";
TextLayer hebrewDateLayer;    char hebrewDateString[]=  "17 Cheshvan 5773";
TextLayer moonLayer;          char moonString[]=        " ";

// Other Layers
Layer lineLayer;

// Keep current time so its available in all functions
PblTm currentPblTime;

// Format string to use for times (must change according to 24h or 12h option)
char *timeFormat;

// Zmanim
float currentTime, sunriseTime, sunsetTime, hatsotTime, zmanHourDuration;

// Functions declarations (to allow for more readable code!
void handle_init(AppContextRef ctx);
void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t);
void updateWatch();
void dayHasChanged();
void hourHasChanged();
void minuteHasChanged();
void updateDate();
void updateHebrewDate();
void updateMoon();
void updateZmanim();
void line_layer_update_callback(Layer *me, GContext* ctx);
void adjustTimezone(float* time);
int tm2jd(PblTm *time);
int moon_phase(int jdn);
