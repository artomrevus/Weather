#ifndef CWATHER_H
#define CWATHER_H

#include "WeatherEnums.h"
#include <QTableWidget>
#include <queue>
#include <QtCharts>
#include <QDialog>


/// Define a tuple to represent the date
using date = std::tuple<unsigned, Month, int>;


/// This class is designed to work with weather data and a weather table to represent it.
class CWather
{


// -------------------------------------------------------------------------------------------------------------------------


public:

// (Public) Constructors & destructor section:


    /// Default constructor
    CWather();


    /** @brief Constructor with parameters
     *
     * Constructor for retrieving weather data from a weather table with 7 columns (year, month, day, t, pressure, humidity,
    wind direction).
     *
     * @param weatherTable - Table for retrieving data.
     * @param rowCount - The number of rows (starting from 0) to be read and retrieved.
     */
    CWather(QTableWidget * weatherTable, const int& rowCount);


    /// Copy constructor
    CWather(const CWather& weather);


    /// Default destructor
    ~CWather();


// -------------------------------------------------------------------------------------------------------------------------


// (Public) Methods section:


    /** Determine if the weather data set meets all requirements (1 <= month <= 12, 1 <= day <= 31, 0 < pressure, 0 <= humidity <= 100
    & Wind direction != Undefined).
     *
     * @return True if the weather data meets all requirements, False otherwise.
     */
    bool isWeatherDataValid();


    /// Within a season (3 months), sort the records by Pressure.
    void sortPressureBySeason();


    /** @brief Fills the weather table.
     *
     * This method fills a table with 7 columns (year, month, day, t, pressure, humidity, wind direction) using the previously
    added weather data.
     *
     * @param weatherTable - Weather table neet to be filled.
     */
    void completeTable(QTableWidget* weatherTable);


    /** @brief Build a weather graph
     *
     * Draws a weather graph with weather data on the y-axis and dates on the x-axis.
     *
     * @param getWeatherData - Function that takes the index of a table row and returns data from this row about one of the
    weather parameters (temperature, pressure or humidity).
     * @param graphTitle - Title of the graph that is being built.
     */
    void buildWeatherGraph(std::function<int(int)> getWeatherData, const QString& graphTitle);


    /** Finds the indixes of weather "array" elements during which the wind direction did not change.
     *
     * @return A vector of vectors from the found indexes (each "internal" vector is a sequential weather indices when the wind
    direction did not change).
     */
    std::vector<std::vector<unsigned>> findDaysWindNotChange();


    /** Calculate the average temperature in the weather array.
     *
     * @return The average temperature in the weather array.
     */
    double getAvgTemperature();


    /** Calculate the average pressure in the weather array.
     *
     * @return The average pressure in the weather array.
     */
    double getAvgPressure();


    /** Finds the days when the humidity is highest.
     *
     * @return Vector of dates when the humidity is highest.
     */
    std::vector<QDate> getHighestHumidityDays();


    /** Finds periods when the temperature and pressure changed within certain percentages.
     *
     * @param tRangePct - Percentage points within which the temperature can change (+-tRangePct).
     * @param psreRangePct - Percentage points within which the pressure can change (+-psreRangePct).
     *
     * @return Periods when the weather changes only within specified limits.
     */
    std::vector<CWather> findPeriodTemperatureAndPressureChangeWithinRange(double tRangePct, double psreRangePct);


    /** Retrieve weather data for a specific period between two dates.
     *
     * @param startDate - start date of the period.
     * @param endDate - end date of the period.
     *
     * @return Weather data only for the specified period.
     */
    CWather getWeatherByPeriod(QDate startDate, QDate endDate);


    /// Forecasts the weather for the next month and adds it to the existing weather data (at the end).
    void forecastWeatherForNextMonth();


    /** Used to get the number of days for which weather data was added.
     *
     * @return number of days for which weather data was added.
     */
    int getWeatherSize();


    /** Used to get the temperature for a certain day.
     *
     * @param index - The index (row number) by which you want to get the temperature.
     */
    int getTemperature(int index);


    /** Used to get the pressure for a certain day.
     *
     * @param index - The index (row number) by which you want to get the pressure.
     */
    unsigned getPressure(int index);


    /** Used to get the humidity for a certain day.
     *
     * @param index - The index (row number) by which you want to get the humidity.
     */
    int getHumidity(int index);


// -------------------------------------------------------------------------------------------------------------------------


// (Public) Operators section:


    /// Copy assignment operator: Copies data from another object 'other' into the current object.
    CWather& operator=(const CWather& other);


    /// Overriding the >> operation for reading data from a file using QTextStream.
    friend QTextStream& operator>>(QTextStream& inFile, CWather &weather);


    /// Overriding the << operation for writing data to a file using QTextStream.
    friend QTextStream& operator<<(QTextStream& out, const CWather& weather);


// -------------------------------------------------------------------------------------------------------------------------


private:

// (Private) class field:


    /// This struct represents weather-related data.
    struct weatherData
    {
        /// An integer representing the year.
        int m_year;
        /// Enum representing the month (1 - 12).
        Month m_month;
        /// An unsigned integer representing the day (1 - 31).
        unsigned m_day;
        /// An integer representing temperature (in degrees Celsius).
        int m_temperature;
        /// An unsigned integer pressure data (in "millimeters of mercury" - mmHg).
        unsigned m_pressure;
        /// An integer representing humidity data (in per cent).
        int m_humidity;
        WindDirection m_windDirection;

        /// A default weatherData constructor that initializes all members to default values.
        weatherData
        ():
        m_year(0), m_month(Month::Unknown), m_day(0),
        m_temperature(0), m_pressure(0), m_humidity(0), m_windDirection(WindDirection::Undefined)
        {}

        /// Parameterized weatherData constructor that allows you to set specific values when creating an instance of the struct.
        weatherData
        (int year, Month month, unsigned int day, int temperature, unsigned int pressure, int humidity, WindDirection windDirection):
        m_year(year), m_month(month), m_day(day), m_temperature(temperature), m_pressure(pressure),
        m_humidity(humidity), m_windDirection(windDirection)
        {}
    };


    /// A vector for storing multiple instances of the weatherData struct (to represent the weather for many days).
    std::vector<weatherData> weatherArr;


// -------------------------------------------------------------------------------------------------------------------------


// (Private) Methods section:


    /** @brief Sorting the weather range (specified by parameters) by pressure.
     *
     * This function performs selection sort within the specified range of the weather "array" based on the pressure values.
    It sorts the elements in ascending order by pressure.
     *
     * @param startIndex - The index representing the start of the range to be sorted.
     * @param endIndex - The index representing the end of the range to be sorted.
     */
    void selectionSortByPressure(int startIndex, int endIndex);


    /** @brief Adds data about one day of weather.
     *
     * This function adds a weatherData object to existing weather data (to the end).
     *
     * @param wData - A weatherData object containing weather data for one day.
     */
    void pushWeatherDataEnd(const weatherData& wData);



// -------------------------------------------------------------------------------------------------------------------------

};

// -------------------------------------------------------------------------------------------------------------------------


// Methods to help you work with CWather class:


/** Returns the season based on the given month.
 *
 * @param month - An integer representing the month (1 to 12).
 *
 * @return The corresponding Season enum value (e.g., Season::Spring).
 */
Season getSeason(int month);


/** Checks if the season has changed between two given dates.
 *
 * @param month1, year1 - The month and year of the first date.
 * @param month2, year2 - The month and year of the second date.
 *
 * @return True if the season has changed between the two dates; otherwise, false.
 */
bool isSeasonChanged(int month1, int year1, int month2, int year2);


/** Returns the number of days in a specific month of a given year.
 *
 * @param month - The Month enum value representing the month.
 * @param year - The year for which you want to know the number of days.
 *
 * @return The number of days in the specified month and year.
 */
int getNumDaysInMonth(Month month, int year);


/** Returns the next month based on the current month.
 *
 * @param currentMonth - The Month enum value representing the current month.
 *
 * @return The Month enum value representing the next month.
 */
Month getNextMonth(Month currentMonth);


/** Converts a text representation to a wind direction.
 *
 * @param text - QString containing the text representation of a wind direction.
 *
 * @return The corresponding WindDirection enum value (e.g., WindDirection::North).
 */
WindDirection convertTextToWindDir(QString text);


/** Converts a wind direction to its text representation.
 *
 * @param direction - The WindDirection enum value.
 *
 * @return QString containing the text representation of the wind direction.
 */
QString convertWindDirToText(WindDirection direction);


/** Calculates the percentage of a given number.
 *
 * @param digit - The number for which you want to calculate the percentage.
 *
 * @return The percentage value as a double.
 */
double getPercentageOf(double digit);


// -------------------------------------------------------------------------------------------------------------------------

#endif // CWATHER_H
