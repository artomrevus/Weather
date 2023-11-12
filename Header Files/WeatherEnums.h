#ifndef WEATHERENUMS_H
#define WEATHERENUMS_H


/** This enum allows you to easily work with months by using clear and convenient identifiers, such as January, February,
and so on, instead of numeric values. You can use this enumeration to work with months in applications that require date information. */
enum Month
{
    January = 1,
    February = 2,
    March = 3,
    April = 4,
    May = 5,
    June = 6,
    July = 7,
    August = 8,
    September = 9,
    October = 10,
    November = 11,
    December = 12,
    Unknown = 0
};


/** This enumeration makes it easy to identify and represent different wind directions in an application using clear identifiers
such as North, South, East, and others. You can use this enumeration to conveniently handle wind direction data in weather-related
applications or other situations where wind direction is important. */
enum WindDirection
{
    North = 1,
    South = 2,
    East = 3,
    West = 4,
    Northeast = 5,
    Northwest = 6,
    Southeast = 7,
    Southwest = 8,
    Undefined = 0
};


/** This enumeration makes it easy to identify and represent different seasons in applications with clear identifiers such as
Winter, Spring, Summer, and Autumn. You can use this enumeration to conveniently process season-related data in weather-related
applications, calendars, or other situations where it is important to take seasonal changes into account.*/
enum Season
{
    Winter = 1,
    Spring = 2,
    Summer = 3,
    Autumn = 4
};


#endif // WEATHERENUMS_H
