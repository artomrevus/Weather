#ifndef CWATHER_H
#define CWATHER_H
#include "WeatherEnums.h"
#include <QTableWidget>
#include <queue>
#include <QtCharts>
#include <QDialog>

using date = std::tuple<unsigned, Month, int>;

class CWather
{
public:

    CWather();
    CWather(QTableWidget * weatherTable, const int& rowCount);
    CWather(const CWather& weather);
    CWather& operator=(const CWather& other);

    ~CWather();

    void selectionSortByPressure(int startIndex, int endIndex);
    void sortPressureBySeason();
    void completeTable(QTableWidget* weatherTable);
    void buildWeatherGraph(std::function<int(int)> getWeatherData, const QString& graphTitle);
    std::vector<std::vector<unsigned>> findDaysWindNotChange();
    double getAvgTemperature();
    double getAvgPressure();
    std::vector<QDate> getHighestHumidityDays();
    std::vector<CWather> findPeriodTemperatureAndPressureChangeWithinRange(double tRangePct, double psreRangePct);
    CWather getWeatherByPeriod(QDate startDate, QDate endDate);


   // void ForecastWeatherNextMonth(std::vector<CWather>& weather);

    // Перевизначення операції >> для зчитування з файлу
    friend QTextStream& operator>>(QTextStream& inFile, CWather &weather);

     // Перевизначення операції >> для запису у файл.
    friend QTextStream& operator<<(QTextStream& out, const CWather& weather);

    int getWeatherSize();
    int getTemperature(int index);
    unsigned getPressure(int index);
    int getHumidity(int index);

private:
    struct weatherData
    {
        int m_year;
        Month m_month;
        unsigned m_day;
        int m_temperature;
        unsigned m_pressure;
        int m_humidity;
        WindDirection m_windDirection;

        weatherData
        ():
        m_year(0), m_month(Month::Unknown), m_day(0),
        m_temperature(0), m_pressure(0), m_humidity(0), m_windDirection(WindDirection::Undefined)
        {}

        weatherData
        (int year, Month month, unsigned int day, int temperature, unsigned int pressure, int humidity, WindDirection windDirection):
        m_year(year), m_month(month), m_day(day), m_temperature(temperature), m_pressure(pressure),
        m_humidity(humidity), m_windDirection(windDirection)
        {}
    };

    std::vector<weatherData> weatherArr;

    void pushWeatherDataEnd(const weatherData& wData);
};


Season getSeason(int month);
bool isSeasonChanged(int month1, int year1, int month2, int year2);
WindDirection convertTextToWindDir(QString text);
QString convertWindDirToText(WindDirection direction);
double getPercentageOf(double digit);


#endif // CWATHER_H
