// TBK Jewish Clock
// A Jewish calendar/zmanim watch face for the Pebble
// Copyright (C) 2013 Ary Tebeka contact@arytbk.net
// Open Source - feel free to use, modify, contribute
// Attribution and comments welcome

// Check our iOS apps:
// https://itunes.apple.com/us/artist/ary-tebeka/id375595955?uo=4
// https://itunes.apple.com/us/artist/tebeka-software-solutions/id285016307?uo=4

// Parts of this project are from KP_Sun_Moon_Vibe_Clock - https://github.com/KarbonPebbler/KP_Sun_Moon_Vibe_Clock and libhdate - http://libhdate.sourceforge.net

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
             "TBK Jewish Clock", "Ary Tebeka",
             1, 3, /* App version */
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
const int sunY = 104;
const int sunSize = 38;

#ifdef ZMANIM
TextLayer currentZmanLayer;   char currentZmanString[]=   "Mincha Gedola";
TextLayer EndOfZmanLayer;     char endOfZmanString[]=     "00:00";
#else
TextLayer zmanHourLayer;      char zmanHourString[]=      "11";
TextLayer nextHourLayer;      char nextHourString[]=      "01:07:00";
#endif

TextLayer alertLayer;   char alertString[]=    "SUNSET IN 000mn";
const VibePattern alertPattern = {
  .durations = (uint32_t []) {200, 100, 200, 100, 200},
  .num_segments = 5
};

TextLayer sunriseLayer;       char sunriseString[]=       "00:00";
TextLayer sunsetLayer;        char sunsetString[]=        "00:00";
TextLayer hatsotLayer;        char hatsotString[]=        "00:00";

// Keep current time so its available in all functions
PblTm currentPblTime;
//float currentTime;    // time as a float ( = hour + minutes/60 )
int currentTime;

// Format string to use for times (must change according to 24h or 12h option)
char *timeFormat;

// Zmanim as minutes from midnight
//float sunriseTime, sunsetTime, hatsotTime, zmanHourDuration, timeUntilNextHour;
int sunriseTime, sunsetTime, hatsotTime, timeUntilNextHour;
// current zman hour number
int zmanHourNumber;
// zman hour duration
float zmanHourDuration;

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
void checkAlerts();

void initTextLayer(TextLayer *theLayer, int x, int y, int w, int h, GColor textColor, GColor backgroundColor, GTextAlignment alignment, GFont theFont);
void adjustTimezone(int* time);
int tm2jd(PblTm *time);
int moon_phase(int jdn);
int hours2Minutes(float theTime);
float minutes2Hours(int theTime);
void displayTime(int theTime, TextLayer *theLayer, char *theString, int maxSize);


