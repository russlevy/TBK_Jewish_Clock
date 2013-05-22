// TBK Hebrew Clock
// A Jewish calendar/zmanim watch face for the Pebble
// Copyright (C) 2013 Ary Tebeka contact@arytbk.net
// Open Source - feel free to use, modify, contribute
// Attribution and comments welcome

// Parts from KP_Sun_Moon_Vibe_Clock - https://github.com/KarbonPebbler/KP_Sun_Moon_Vibe_Clock

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

// ******************************************
// Layers - top to bottom, left to right
// ******************************************
TextLayer dayLayer;           char dayString[]=           "17";
TextLayer hDayLayer;          char hDayString[]=          "13";

TextLayer moonLayer;          char moonString[]=          " ";

TextLayer monthLayer;         char monthString[]=         "May";
TextLayer hMonthLayer;        // char hMonthString[]=      "Cheshvan";

TextLayer timeLayer;          char timeString[]=          "00:00";
Layer lineLayer;

TextLayer zmanHourLabelLayer; char zmanHourLabelString[]= "Hour #";
TextLayer nextHourLabelLayer; char nextHourLabelString[]= "Next In:";

Layer sunGraphLayer;
const int sunY = 107;
const int sunSize = 36;

TextLayer zmanHourLayer;      char zmanHourString[]=      "11";
TextLayer nextHourLayer;      char nextHourString[]=      "01:07:00";

TextLayer sunriseLayer;       char sunriseString[]=       "00:00";
TextLayer sunsetLayer;        char sunsetString[]=        "00:00";
TextLayer hatsotLayer;        char hatsotString[]=        "00:00";

// Keep current time so its available in all functions
PblTm currentPblTime;
float currentTime;    // time as a float ( = hour + minutes/60 )

// Format string to use for times (must change according to 24h or 12h option)
char *timeFormat;

// Zmanim
float sunriseTime, sunsetTime, hatsotTime, zmanHourDuration, timeUntilNextHour;
int zmanHourNumber;

// Sun path
GPath sun_path;
GPathInfo sun_path_info = {
  5,
  (GPoint []) {
    {0, 0},
    {-73, +84}, //replaced by sunrise angle
    {-73, +84}, //bottom left
    {+73, +84}, //bottom right
    {+73, +84}, //replaced by sunset angle
  }
};

// Functions declarations (to allow for more readable code!
void handle_init(AppContextRef ctx);
void lineLayerUpdate(Layer *me, GContext* ctx);
void sunGraphLayerUpdate(Layer *me, GContext* ctx);

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t);
void updateWatch();

void doEveryDay();
void doEveryHour();
void doEveryMinute();
void updateTime();
void updateDate();
void updateHebrewDate();
void updateMoonAndSun();
void updateZmanim();

void adjustTimezone(float* time);
int tm2jd(PblTm *time);
int moon_phase(int jdn);
float get24HourAngle(int hours, int minutes);
