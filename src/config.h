// TBK Jewish Clock
// A Jewish calendar/zmanim watch face for the Pebble
// Copyright (C) 2013 Ary Tebeka contact@arytbk.net
// Open Source - feel free to use, modify, contribute
// Attribution and comments welcome
// Check our iOS apps:
// https://itunes.apple.com/us/artist/ary-tebeka/id375595955?uo=4
// https://itunes.apple.com/us/artist/tebeka-software-solutions/id285016307?uo=4

// uncomment the next line to show the watch as black on white
//#define REVERSED 1

// ******** DAYLIGHT SAVING TIME
// Currently the Pebble SDK does not tell if Daily Saving Time is active, so must set this flag and recompile (yuk!). Use 0 if not-DST, use -1 if DST.
#define ISDST (-1)

// Configure your location and timezone
// NOTE: longitude is positive for East and negative for West
// Use http://itouchmap.com/latlong.html to find the values for your location

// Jerusalem
//#define LATITUDE    (31.752766)
//#define LONGITUDE 	(35.119600)
//#define TIMEZONE (+2)

// RAANANA
//#define LATITUDE    (32.184781)
//#define LONGITUDE 	(34.871326)
//#define TIMEZONE (+2)

// BNEI BRAK
 #define LATITUDE    (32.081261)
 #define LONGITUDE 	(34.826726)
 #define TIMEZONE (+2)

// Paris
//#define LATITUDE    (48.856638)
//#define LONGITUDE   (2.352241)
//#define TIMEZONE (+1)

// New York, NY
//#define LATITUDE    (40.714353)
//#define LONGITUDE 	(-74.005973)
//#define TIMEZONE (-5)

// Toronto
//#define LATITUDE (43.6481)
//#define LONGITUDE (-79.4042)
//#define TIMEZONE (-5)

// Baysville
//#define LATITUDE (45.148232)
//#define LONGITUDE (-79.118831)
//#define TIMEZONE (-5)

// Palo Alto, CA
//#define LATITUDE    (37.441883)
//#define LONGITUDE 	(-122.143019)
//#define TIMEZONE (-8)

// Los Angeles
//#define LATITUDE    (34.036658)
//#define LONGITUDE 	(-118.336637)
//#define TIMEZONE (-8)

//****************** ZMANIM *****************
// NOT YET IMPLEMENTED
// Comment this line to use Hour numbers instead of Zmanim
// #define USE_ZMANIM

// ***************** ALERTS *****************
// Comment this line to disable vibration alert before end of Mincha (sunset)
// If defined, it is the number of minutes before sunset for the alert
#define MINCHA_ALERT  (18)    // 18 minutes is also good for Shabbat candlelighting alert!
// Comment to disable start of Mincha Gdola and Ktana alerts, plag and shkia (sunset) alerts
#define MINCHA_GEDOLA_ALERT
#define MINCHA_KETANA_ALERT
#define PLAG_ALERT
#define SUNSET_ALERT
