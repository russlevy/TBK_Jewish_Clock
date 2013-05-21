
/**
 @brief Compute Julian day from Gregorian date
 
 @author Yaacov Zamir (algorithm from Henry F. Fliegel and Thomas C. Van Flandern ,1968)
 
 @param day Day of month 1..31
 @param month Month 1..12
 @param year Year in 4 digits e.g. 2001
 @return the julian day number
 */
int
hdate_gdate_to_jd (int day, int month, int year);

/**
 @brief Converting from the Julian day to the Hebrew day
 
 @author Yaacov Zamir 2005
 
 @param jd Julian day
 @param day return Day of month 1..31
 @param month return Month 1..14 (13 - Adar 1, 14 - Adar 2)
 @param year return Year in 4 digits e.g. 2001
 @param jd_tishrey1 return the julian number of 1 Tishrey this year
 @param jd_tishrey1_next_year return the julian number of 1 Tishrey next year
 */
void
hdate_jd_to_hdate (int jd, int *day, int *month, int *year, int *jd_tishrey1, int *jd_tishrey1_next_year);

/**
 @brief Converting from the Julian day to the Gregorian date
 
 @author Yaacov Zamir (Algorithm, Henry F. Fliegel and Thomas C. Van Flandern ,1968)
 
 @param jd Julian day
 @param day return Day of month 1..31
 @param month return Month 1..12
 @param year return Year in 4 digits e.g. 2001
 */
void
hdate_jd_to_gdate (int jd, int *day, int *month, int *year);

/**
 @brief Days since Tishrey 3744
 
 @author Amos Shapir 1984 (rev. 1985, 1992) Yaacov Zamir 2003-2005
 
 @param hebrew_year The Hebrew year
 @return Number of days since 3,1,3744
 */
int
hdate_days_from_3744 (int hebrew_year);

/**
 @brief Return a static string, with name of month.
 
 @param month The number of the month 1..12 (1 - jan).
 @param short_form A short flag.
 @warning DEPRECATION: This function is now just a wrapper for
 hdate_string, and is subject to deprecation.
 [deprecation date 2011-12-28]
 */
char *
hdate_get_month_string (int month);
