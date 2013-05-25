// TBK Jewish Clock
// A Jewish calendar/zmanim watch face for the Pebble
// Copyright (C) 2013 Ary Tebeka contact@arytbk.net
// Open Source - feel free to use, modify, contribute
// Attribution and comments welcome
// Check our iOS apps:
// https://itunes.apple.com/us/artist/ary-tebeka/id375595955?uo=4
// https://itunes.apple.com/us/artist/tebeka-software-solutions/id285016307?uo=4

// NOTE: longitude is positive for East and negative for West
// Use http://itouchmap.com/latlong.html to find the values for your location

// Jerusalem
//#define LATITUDE    (31.752766)
//#define LONGITUDE 	(35.119600)

// RAANANA
//#define LATITUDE    (32.184781)
//#define LONGITUDE 	(34.871326)

// BNEI BRAK
 #define LATITUDE    (32.081261)
 #define LONGITUDE 	(34.826726)

// New York, NY
//#define LATITUDE    (40.714353)
//#define LONGITUDE 	(-74.005973)

// Palo Alto, CA
//#define LATITUDE    (37.441883)
//#define LONGITUDE 	(-122.143019)

// Los Angeles
//#define LATITUDE    (34.036658)
//#define LONGITUDE 	(-118.336637)

// ******** Enter your timezone here, for example, if you are GMT+2 (Israel), enter 2

// Israel
 #define TIMEZONE (2)

// USA East
//#define TIMEZONE (-5)

// USA West
//#define TIMEZONE (-8)

// ******** DAYLIGHT SAVING TIME
// Currently the Pebble SDK does not tell if Daily Saving Time is active, so must set this flag and recompile (yuk!). Use 0 if not-DST, use -1 if DST.
#define ISDST (-1)

//****************** ZMANIM *****************

// Comment this line to use Hour numbers instead of Zmanim
// #define USE_ZMANIM

// Comment this line to disable vibration alert before end of Mincha (sunset)
// If defined, it is the number of minutes before sunset for the alert
#define MINCHA_ALERT  (18)    // 18 minutes is also good for Shabbat candlelighting alert!
