#include "cwather.h"


CWather::CWather() :
    m_year(2000), m_month(Month::January), m_day(1),
    m_temperature(0), m_pressure(760), m_humidity(50), m_windDirection(WindDirection::North)
{}


CWather::CWather(int year, Month month, unsigned int day, int temperature, unsigned int pressure,
                int humidity, WindDirection windDirection) :
        m_year(year), m_month(month), m_day(day),
        m_temperature(temperature), m_pressure(pressure), m_humidity(humidity), m_windDirection(windDirection)
{}


CWather::CWather(const CWather& weather) :
        m_year(weather.m_year), m_month(weather.m_month), m_day(weather.m_day),
        m_temperature(weather.m_temperature), m_pressure(weather.m_pressure), m_humidity(weather.m_humidity),
    m_windDirection(weather.m_windDirection)
{}


CWather::~CWather()
{
    m_year = 0;
    m_month = Month::Unknown;
    m_day = 0;
    m_temperature = 0;
    m_pressure = 0;
    m_humidity = 0;
    m_windDirection = WindDirection::Undefined;
}


int CWather::temperature() const
{
    return m_temperature;
}


unsigned int CWather::pressure() const
{
    return m_pressure;
}


int CWather::humidity() const
{
    return m_humidity;
}


int CWather::year() const
{
    return m_year;
}


Month CWather::month() const
{
    return m_month;
}


unsigned int CWather::day() const
{
    return m_day;
}


WindDirection CWather::windDirection() const
{
    return m_windDirection;
}


QTextStream& operator>>(QTextStream& in, CWather& WeatherData)
{
    int monthInt;
    QString windDirectionStr;

    in >> WeatherData.m_year >> monthInt >> WeatherData.m_day
       >> WeatherData.m_temperature >> WeatherData.m_pressure >> WeatherData.m_humidity >> windDirectionStr;

    WeatherData.m_month = static_cast<Month>(monthInt);
    WeatherData.m_windDirection = convertTextToWindDir(windDirectionStr);

    return in;
}


QTextStream& operator<<(QTextStream& out, const CWather& WeatherData)
{
    out << WeatherData.m_year << " " << static_cast<int>(WeatherData.m_month) << " " << WeatherData.m_day
        << " " <<  WeatherData.m_temperature << " " << WeatherData.m_pressure << " " << WeatherData.m_humidity
        << " " << convertWindDirToText(WeatherData.m_windDirection);

    return out;
}


void selectionSortByPressure(std::vector<CWather>& weatherArr, int startIndex, int endIndex)
{
    int minIndex;
    for (int i = startIndex; i < endIndex - 1; ++i) {
        minIndex = i;
        for (int j = i + 1; j < endIndex; ++j) {
            if(weatherArr[j].m_pressure < weatherArr[minIndex].m_pressure) {
                minIndex = j;
            }
        }
        std::swap(weatherArr[i], weatherArr[minIndex]);
    }
}


void sortPressureBySeason(std::vector<CWather>& weatherArr)
{
    int startIndex = 0, endIndex = 0;
    for(int i = 0; i < weatherArr.size(); ++i)
    {
        if(isSeasonChanged(weatherArr[i].m_month, weatherArr[i].m_year, weatherArr[i+1].m_month, weatherArr[i+1].m_year)){
            startIndex = endIndex;
            endIndex = i + 1;
            selectionSortByPressure(weatherArr, startIndex, endIndex);
        }
    }
}


void completeTable(const std::vector<CWather>& weatherArr, QTableWidget* weatherTable)
{
    weatherTable->setRowCount(weatherArr.size());
    for (int i = 0; i < weatherArr.size(); ++i) {
        weatherTable->setItem(i, 0, new QTableWidgetItem(QString::number(weatherArr[i].m_year)));
        weatherTable->setItem(i, 1, new QTableWidgetItem(QString::number(static_cast<int>(weatherArr[i].m_month))));
        weatherTable->setItem(i, 2, new QTableWidgetItem(QString::number(weatherArr[i].m_day)));
        weatherTable->setItem(i, 3, new QTableWidgetItem(QString::number(weatherArr[i].m_temperature)));
        weatherTable->setItem(i, 4, new QTableWidgetItem(QString::number(weatherArr[i].m_pressure)));
        weatherTable->setItem(i, 5, new QTableWidgetItem(QString::number(weatherArr[i].m_humidity)));
        weatherTable->setItem(i, 6, new QTableWidgetItem(convertWindDirToText(weatherArr[i].m_windDirection)));
    }
}


Season getSeason(int month)
{
    if (month == 12 || month == 1 || month == 2) {
        return Season::Winter;
    } else if (month >= 3 && month <= 5) {
        return Season::Spring;
    } else if (month >= 6 && month <= 8) {
        return Season::Summer;
    } else {
        return Season::Autumn;
    }
}


bool isSeasonChanged(int month1, int year1, int month2, int year2)
{
    if(year1 == year2 - 1 && getSeason(static_cast<int>(month1)) == Season::Winter == getSeason(static_cast<int>(month2)))
        return false;
    else if(year1 != year2)
        return true;
    else if(getSeason(static_cast<int>(month1)) != getSeason(static_cast<int>(month2)))
        return true;
    else
        return false;
}



void buildWeatherGraph(const std::vector<CWather>& weather, std::function<int(int)> getWeatherData, const QString& graphTitle)
{
    if(weather.size() < 3){
        QMessageBox::information(nullptr, "Not enough data.", "Data is required to build the graph."
                                " Please add 3 or more rows to the table and save it.", QMessageBox::Ok);
        return;
    }

    QLineSeries* series = new QLineSeries();

    //додавання точок
    for(int i = 0; i < weather.size(); ++i){
        series->append(i, getWeatherData(i));
    }

    //вивід точок на графік
    QChart* chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();

    chart->setTitle(graphTitle);

    chart->setAnimationOptions(QChart::AllAnimations);

    QCategoryAxis *axisX = new QCategoryAxis();
    axisX->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);

    for(int i = 0; i < weather.size(); ++i){
        QString date = QString::asprintf("%02d.%02d", static_cast<int>(weather[i].month()), weather[i].year());
        axisX->append(date, i);
    }

    chart->setAxisX(axisX, series);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QDialog *dialog = new QDialog;
    dialog->setMinimumSize(800, 500);

    QVBoxLayout *layout = new QVBoxLayout; // Використовуйте розміщення для вмісту
    dialog->setLayout(layout); // Встановіть розміщення для діалогового вікна
    layout->addWidget(chartView); // Додайте QChartView до розміщення

    dialog->exec();
}

// Повертає вектор з векторів індексів Weather коли не змінюється напрям вітру.
std::vector<std::vector<unsigned>> findDaysWindNotChange(std::vector<CWather>& weatherArr)
{
    std::vector<std::vector<unsigned>> windNotChangeArr;

    for (int i = 0; i < weatherArr.size(); ++i){
        if(weatherArr[i].m_windDirection == weatherArr[i+1].m_windDirection)
        {
            std::vector<unsigned> indexArr;

            indexArr.push_back(i);
            while(weatherArr[i].m_windDirection == weatherArr[i+1].m_windDirection){
                indexArr.push_back(i+1);
                i++;
            }

            windNotChangeArr.push_back(indexArr);
        }
    }

    return windNotChangeArr;
}


double getAvgTemperature(const std::vector<CWather>& weatherArr)
{
    double sum = 0;
    for (int i = 0; i < weatherArr.size(); ++i) {
        sum += weatherArr[i].temperature();
    }

    return qRound(sum / weatherArr.size() * 100.0) / 100.0;
}


double getAvgPressure(const std::vector<CWather>& weatherArr)
{
    double sum = 0;
    for (int i = 0; i < weatherArr.size(); ++i) {
        sum += weatherArr[i].pressure();
    }

    return qRound(sum / weatherArr.size() * 100.0) / 100.0;
}


std::vector<QDate> getHighestHumidityDays(const std::vector<CWather>& findHighestHumArr)
{
    std::vector<QDate> highestHumDaysArr;

    int maxHumidity = 0;
    for (int i = 0; i < findHighestHumArr.size(); ++i) {
        if(findHighestHumArr[i].humidity() > maxHumidity){
            maxHumidity = findHighestHumArr[i].humidity();
        }
    }

    for (int i = 0; i < findHighestHumArr.size(); ++i){
        if(findHighestHumArr[i].humidity() == maxHumidity){
            highestHumDaysArr.push_back(QDate(findHighestHumArr[i].year(), findHighestHumArr[i].month(), findHighestHumArr[i].day()));
        }
    }

    return highestHumDaysArr;
}


std::vector<std::vector<CWather>> findPeriodTemperatureAndPressureChangeWithinRange
    (const std::vector<CWather>& weatherArr, double temperatureRangePct, double pressureRangePct)
{
    std::vector<std::vector<CWather>> periodsArr;

    int currEl = 0, periodIndex = 0;
    while(currEl < weatherArr.size())
    {
        if(periodsArr.size() < periodIndex + 1)
        {
            std::vector<CWather> periodWeatherArr;
            periodWeatherArr.push_back(weatherArr[currEl]);
            periodsArr.push_back(periodWeatherArr);
        }
        else if(fabs(getAvgPressure(periodsArr[periodIndex]) - (double)weatherArr[currEl].pressure()) <= getPercentageOf(getAvgPressure(periodsArr[periodIndex])) * pressureRangePct
                && fabs(getAvgTemperature(periodsArr[periodIndex]) - (double)weatherArr[currEl].temperature()) <= getPercentageOf(getAvgTemperature(periodsArr[periodIndex])) * temperatureRangePct)
        {
            periodsArr[periodIndex].push_back(weatherArr[currEl]);
        }
        else
        {
            periodIndex++;
            continue;
        }

        currEl++;
    }

    for (auto it = periodsArr.begin(); it != periodsArr.end(); ) {
        if ((*it).size() < 3) {
            it = periodsArr.erase(it);
        } else {
            ++it;
        }
    }

    return periodsArr;
}


WindDirection convertTextToWindDir(QString text)
{
    if(text == "N"){
        return North;
    } else if(text == "S"){
        return South;
    } else if(text == "E"){
        return East;
    } else if(text == "W"){
        return West;
    } else if(text == "NE"){
        return Northeast;
    } else if(text == "NW"){
        return Northwest;
    } else if(text == "SE"){
        return Southeast;
    } else if(text == "SW"){
        return Southwest;
    } else{
        return Undefined;
    }
}


QString convertWindDirToText(WindDirection direction)
{
    if(direction == North){
        return "N";
    } else if(direction == South){
        return "S";
    } else if(direction == East){
        return "E";
    } else if(direction == West){
        return "W";
    } else if(direction == Northeast){
        return "NE";
    } else if(direction == Northwest){
        return "NW";
    } else if(direction == Southeast){
        return "SE";
    } else if(direction == Southwest){
        return "SW";
    } else{
        return "Undefined";
    }
}


double getPercentageOf(double digit)
{
    return digit / 100;
}
