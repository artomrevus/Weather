#include "../Header Files/cwather.h"


CWather::CWather()
{
    weatherArr.reserve(1000);
}


CWather::CWather(QTableWidget * weatherTable, const int& rowCount)
{
    weatherArr.clear();
    for(int i = 0; i < rowCount; ++i)
    {
        int year = weatherTable->item(i, 0)->text().toInt();
        Month month = static_cast<Month>(weatherTable->item(i, 1)->text().toInt());
        unsigned day = weatherTable->item(i, 2)->text().toUInt();
        int t = weatherTable->item(i, 3)->text().toInt();
        unsigned pressure = weatherTable->item(i, 4)->text().toUInt();
        int humidity = weatherTable->item(i, 5)->text().toInt();
        WindDirection windDirection = convertTextToWindDir(weatherTable->item(i, 6)->text());

        weatherArr.push_back(weatherData(year, month, day, t, pressure, humidity, windDirection));
    }
}


CWather::CWather(const CWather& weather) : weatherArr(weather.weatherArr)
{}


CWather& CWather::operator=(const CWather& other)
{
    if (this == &other) {
        return *this;
    }

    weatherArr = other.weatherArr;

    return *this;
}


CWather::~CWather()
{
    weatherArr.clear();
}


void CWather::selectionSortByPressure(int startIndex, int endIndex)
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


void CWather::sortPressureBySeason()
{
    int startIndex = 0, endIndex = 0;
    for(int i = 0; i < weatherArr.size(); ++i)
    {
        if(isSeasonChanged(weatherArr[i].m_month, weatherArr[i].m_year, weatherArr[i+1].m_month, weatherArr[i+1].m_year)){
            startIndex = endIndex;
            endIndex = i + 1;
            selectionSortByPressure(startIndex, endIndex);
        }
    }
}


void CWather::completeTable(QTableWidget* weatherTable)
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


void CWather::buildWeatherGraph(std::function<int (int)> getWeatherData, const QString &graphTitle)
{
    if(weatherArr.size() < 3){
        QMessageBox::information(nullptr, "Not enough data.", "Data is required to build the graph."
                                " Please add 3 or more rows to the table and save it.", QMessageBox::Ok);
        return;
    }

    QLineSeries* series = new QLineSeries();

    //додавання точок
    for(int i = 0; i < weatherArr.size(); ++i){
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

    for(int i = 0; i < weatherArr.size(); ++i){
        QString date = QString::asprintf("%02d.%02d", static_cast<int>(weatherArr[i].m_month), weatherArr[i].m_year);
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


std::vector<std::vector<unsigned>> CWather::findDaysWindNotChange()
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


double CWather::getAvgTemperature()
{
    double sum = 0;
    for (int i = 0; i < weatherArr.size(); ++i) {
        sum += weatherArr[i].m_temperature;
    }

    return qRound(sum / weatherArr.size() * 100.0) / 100.0;
}


double CWather::getAvgPressure()
{
    double sum = 0;
    for (int i = 0; i < weatherArr.size(); ++i) {
        sum += weatherArr[i].m_pressure;
    }

    return qRound(sum / weatherArr.size() * 100.0) / 100.0;
}


std::vector<QDate> CWather::getHighestHumidityDays()
{
    std::vector<QDate> highestHumDaysArr;

    int maxHumidity = 0;
    for (int i = 0; i < weatherArr.size(); ++i) {
        if(weatherArr[i].m_humidity > maxHumidity){
            maxHumidity = weatherArr[i].m_humidity;
        }
    }

    for (int i = 0; i < weatherArr.size(); ++i){
        if(weatherArr[i].m_humidity == maxHumidity){
            highestHumDaysArr.push_back(QDate(weatherArr[i].m_year, weatherArr[i].m_month, weatherArr[i].m_day));
        }
    }

    return highestHumDaysArr;
}


std::vector<CWather> CWather::findPeriodTemperatureAndPressureChangeWithinRange(double tRangePct, double psreRangePct)
{
    std::vector<CWather> periodsArr;

    int currEl = 0, periodIndex = 0;
    while(currEl < weatherArr.size())
    {
        if(periodsArr.size() < periodIndex + 1)
        {
            CWather periodWeather;
            periodWeather.pushWeatherDataEnd(weatherArr[currEl]);
            periodsArr.push_back(periodWeather);
        }
        else if(fabs(periodsArr[periodIndex].getAvgPressure() - (double)weatherArr[currEl].m_pressure) <= getPercentageOf(periodsArr[periodIndex].getAvgPressure()) * psreRangePct
                && fabs(periodsArr[periodIndex].getAvgTemperature() - (double)weatherArr[currEl].m_temperature) <= getPercentageOf(periodsArr[periodIndex].getAvgTemperature()) * tRangePct)
        {
            periodsArr[periodIndex].pushWeatherDataEnd(weatherArr[currEl]);
        }
        else
        {
            periodIndex++;
            continue;
        }

        currEl++;
    }

    for (auto it = periodsArr.begin(); it != periodsArr.end(); ) {
        if ((*it).weatherArr.size() < 3) {
            it = periodsArr.erase(it);
        } else {
            ++it;
        }
    }

    return periodsArr;
}


CWather CWather::getWeatherByPeriod(QDate startDate, QDate endDate)
{
    CWather weatherPeriod;

    for (int i = 0; i < weatherArr.size(); ++i) {
        QDate currentTableDate = QDate(weatherArr[i].m_year, weatherArr[i].m_month, weatherArr[i].m_day);
        if(currentTableDate >= startDate && currentTableDate <= endDate)
        {
            weatherPeriod.weatherArr.push_back(weatherArr[i]);
        }
    }

    return weatherPeriod;
}


void CWather::forecastWeatherForNextMonth()
{
    int lastElIndex = weatherArr.size() - 1;

    Month month = getNextMonth(weatherArr[lastElIndex].m_month);
    int year = month == Month::January ? weatherArr[lastElIndex].m_year + 1 : weatherArr[lastElIndex].m_year;
    int temperature;
    unsigned pressure;
    int humidity;
    WindDirection windDirection;

    for (int day = 1; day <= getNumDaysInMonth(month, year); ++day)
    {
        if(month == Month::December)
        {
            temperature = QRandomGenerator::global()->bounded(-5, 6);
            pressure = QRandomGenerator::global()->bounded(770, 781);
            humidity = QRandomGenerator::global()->bounded(30, 41);
            windDirection = static_cast<WindDirection>(QRandomGenerator::global()->bounded(1, 9));
        }
        else if(month == Month::January)
        {
            temperature = QRandomGenerator::global()->bounded(-10, 1);
            pressure = QRandomGenerator::global()->bounded(780, 791);
            humidity = QRandomGenerator::global()->bounded(20, 31);
            windDirection = static_cast<WindDirection>(QRandomGenerator::global()->bounded(1, 9));
        }
        else if(month == Month::February)
        {
            temperature = QRandomGenerator::global()->bounded(0, 11);
            pressure = QRandomGenerator::global()->bounded(760, 771);
            humidity = QRandomGenerator::global()->bounded(30, 46);
            windDirection = static_cast<WindDirection>(QRandomGenerator::global()->bounded(1, 9));
        }
        else if(month == Month::March)
        {
            temperature = QRandomGenerator::global()->bounded(10, 16);
            pressure = QRandomGenerator::global()->bounded(750, 761);
            humidity = QRandomGenerator::global()->bounded(45, 56);
            windDirection = static_cast<WindDirection>(QRandomGenerator::global()->bounded(1, 9));
        }
        else if(month == Month::April)
        {
            temperature = QRandomGenerator::global()->bounded(16, 21);
            pressure = QRandomGenerator::global()->bounded(740, 751);
            humidity = QRandomGenerator::global()->bounded(55, 66);
            windDirection = static_cast<WindDirection>(QRandomGenerator::global()->bounded(1, 9));
        }
        else if(month == Month::May){
            temperature = QRandomGenerator::global()->bounded(20, 26);
            pressure = QRandomGenerator::global()->bounded(730, 741);
            humidity = QRandomGenerator::global()->bounded(65, 71);
            windDirection = static_cast<WindDirection>(QRandomGenerator::global()->bounded(1, 9));
        }
        else if(month == Month::June)
        {
            temperature = QRandomGenerator::global()->bounded(20, 31);
            pressure = QRandomGenerator::global()->bounded(721, 731);
            humidity = QRandomGenerator::global()->bounded(65, 76);
            windDirection = static_cast<WindDirection>(QRandomGenerator::global()->bounded(1, 9));
        }
        else if(month == Month::July)
        {
            temperature = QRandomGenerator::global()->bounded(25, 36);
            pressure = QRandomGenerator::global()->bounded(710, 721);
            humidity = QRandomGenerator::global()->bounded(70, 81);
            windDirection = static_cast<WindDirection>(QRandomGenerator::global()->bounded(1, 9));
        }
        else if(month == Month::August)
        {
            temperature = QRandomGenerator::global()->bounded(20, 31);
            pressure = QRandomGenerator::global()->bounded(721, 731);
            humidity = QRandomGenerator::global()->bounded(62, 76);
            windDirection = static_cast<WindDirection>(QRandomGenerator::global()->bounded(1, 9));
        }
        else if(month == Month::September)
        {
            temperature = QRandomGenerator::global()->bounded(15, 21);
            pressure = QRandomGenerator::global()->bounded(736, 751);
            humidity = QRandomGenerator::global()->bounded(60, 71);
            windDirection = static_cast<WindDirection>(QRandomGenerator::global()->bounded(1, 9));
        }
        else if(month == Month::October)
        {
            temperature = QRandomGenerator::global()->bounded(10, 16);
            pressure = QRandomGenerator::global()->bounded(751, 766);
            humidity = QRandomGenerator::global()->bounded(50, 61);
            windDirection = static_cast<WindDirection>(QRandomGenerator::global()->bounded(1, 9));
        }
        else if(month == Month::November)
        {
            temperature = QRandomGenerator::global()->bounded(5, 11);
            pressure = QRandomGenerator::global()->bounded(760, 771);
            humidity = QRandomGenerator::global()->bounded(40, 51);
            windDirection = static_cast<WindDirection>(QRandomGenerator::global()->bounded(1, 9));
        }

        weatherArr.push_back(weatherData(year, month, day, temperature, pressure, humidity, windDirection));
    }
}


int CWather::getWeatherSize()
{
    return weatherArr.size();
}


int CWather::getTemperature(int index)
{
    return weatherArr[index].m_temperature;
}


unsigned int CWather::getPressure(int index)
{
    return weatherArr[index].m_pressure;
}


int CWather::getHumidity(int index)
{
    return weatherArr[index].m_humidity;
}


QTextStream& operator>>(QTextStream &inFile, CWather &weather)
{
    weather.weatherArr.clear();

    while (!inFile.atEnd())
    {
        int month;
        QString windDirection;

        CWather::weatherData wData;

        inFile >> wData.m_year >> month >> wData.m_day
        >> wData.m_temperature >> wData.m_pressure >> wData.m_humidity >> windDirection;

        wData.m_month = static_cast<Month>(month);
        wData.m_windDirection = convertTextToWindDir(windDirection);

        weather.weatherArr.push_back(wData);
    }

    return inFile;
}


void CWather::pushWeatherDataEnd(const weatherData &wData)
{
    weatherArr.push_back(wData);
}


QTextStream& operator<<(QTextStream& out, const CWather& weather)
{   
    for (int i = 0; i < weather.weatherArr.size(); ++i)
    {
        out << weather.weatherArr[i].m_year << " " << static_cast<int>(weather.weatherArr[i].m_month) << " " << weather.weatherArr[i].m_day
            << " " <<  weather.weatherArr[i].m_temperature << " " << weather.weatherArr[i].m_pressure << " " << weather.weatherArr[i].m_humidity
            << " " << convertWindDirToText(weather.weatherArr[i].m_windDirection);

        if(i != weather.weatherArr.size() - 1){
            out << "\n";
        }
    }

    return out;
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


int getNumDaysInMonth(Month month, int year)
{
    switch (month)
    {
    case Month::January:
    case Month::March:
    case Month::May:
    case Month::July:
    case Month::August:
    case Month::October:
    case Month::December:
        return 31;

    case Month::April:
    case Month::June:
    case Month::September:
    case Month::November:
        return 30;

    case Month::February:
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
            return 29;
        } else {
            return 28;
        }

    default:
        return 0; // Невідомий місяць
    }
}


Month getNextMonth(Month currentMonth) {
    if (currentMonth == Month::December) {
        return Month::January;
    } else {
        return static_cast<Month>(static_cast<int>(currentMonth) + 1);
    }
}
