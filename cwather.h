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
    CWather(int year, Month month, unsigned day,
            int temperature, unsigned pressure, int humidity, WindDirection windDirection);
    CWather(const CWather& weather);
    ~CWather();

    friend void selectionSortByPressure(std::vector<CWather>& weatherArr, int startIndex, int endIndex);
    friend void sortPressureBySeason(std::vector<CWather>& weatherArr);
    friend void completeTable(const std::vector<CWather>& weatherArr, QTableWidget * weatherTable);
    friend void buildWeatherGraph(const std::vector<CWather>& weatherArr, std::function<int(int)> getWeatherData, const QString& graphTitle);
    friend std::vector<std::vector<unsigned>> findDaysWindNotChange(std::vector<CWather>& weatherArr);
    friend double getAvgTemperature(const std::vector<CWather>& weatherArr);
    friend double getAvgPressure(const std::vector<CWather>& weatherArr);
    friend std::vector<QDate> getHighestHumidityDays(const std::vector<CWather>& findHighestHumArr);
    friend std::vector<std::vector<CWather>> findPeriodTemperatureAndPressureChangeWithinRange(const std::vector<CWather>& weatherArr,
                                                                double temperatureRangePct, double pressureRangePct);


   // void ForecastWeatherNextMonth(std::vector<CWather>& weather);

    // Перевизначення операції >> для зчитування з файлу
    friend QTextStream& operator>>(QTextStream& in, CWather& WeatherData);

     // Перевизначення операції >> для запису у файл.
    friend QTextStream& operator<<(QTextStream& out, const CWather& WeatherData);

    int temperature() const;
    unsigned int pressure() const;
    int humidity() const;
    int year() const;
    Month month() const;
    unsigned int day() const;
    WindDirection windDirection() const;

private:
    int m_year;
    Month m_month;
    unsigned m_day;
    int m_temperature;
    unsigned m_pressure;
    int m_humidity;
    WindDirection m_windDirection;
};


Season getSeason(int month);
bool isSeasonChanged(int month1, int year1, int month2, int year2);
WindDirection convertTextToWindDir(QString text);
QString convertWindDirToText(WindDirection direction);
double getPercentageOf(double digit);


#endif // CWATHER_H
