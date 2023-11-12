#include "../Header Files/cwather.h"


// Default constructor
CWather::CWather()
{
    weatherArr.reserve(1000);
}


// Constructor for retrieving weather data from a weather table with 7 columns (year, month, day, t, pressure, humidity, wind direction).
CWather::CWather(QTableWidget * weatherTable, const int& rowCount)
{
    weatherArr.clear();

    // Iterate through each row in the weather table.
    for(int i = 0; i < rowCount; ++i)
    {
        // Retrieve values from the weather table and convert them to appropriate types.
        int year = weatherTable->item(i, 0)->text().toInt();
        Month month = static_cast<Month>(weatherTable->item(i, 1)->text().toInt());
        unsigned day = weatherTable->item(i, 2)->text().toUInt();
        int t = weatherTable->item(i, 3)->text().toInt();
        unsigned pressure = weatherTable->item(i, 4)->text().toUInt();
        int humidity = weatherTable->item(i, 5)->text().toInt();
        WindDirection windDirection = convertTextToWindDir(weatherTable->item(i, 6)->text());

        // Create a new weather data entry and add it to the array.
        weatherArr.push_back(weatherData(year, month, day, t, pressure, humidity, windDirection));
    }
}


// Copy constructor
CWather::CWather(const CWather& weather) : weatherArr(weather.weatherArr)
{}


// Copy assignment operator
CWather& CWather::operator=(const CWather& other)
{
    if (this == &other) {
        return *this;
    }

    weatherArr = other.weatherArr;

    return *this;
}


// Default destructor
CWather::~CWather()
{
    weatherArr.clear();
}


// Determine if the weather data set meets all requirements.
bool CWather::isWeatherDataValid()
{
    // Iterate through each weather data entry in the array.
    for (const weatherData& wData: weatherArr)
    {
        int intMonth = static_cast<int>(wData.m_month);
        int day = wData.m_day;
        int pressure = wData.m_pressure;
        int humidity = wData.m_humidity;
        WindDirection windDir = wData.m_windDirection;

        // Check if the values are within valid ranges.
        if((intMonth < 1) || (intMonth > 12) || (day < 1) || (day > 31) || (pressure < 1) || (humidity < 0) || (humidity > 100)
            || (windDir == Undefined))
        {
            return false;
        }
    }

    return true;
}


// Sorting the weather range (specified by parameters) by pressure.
void CWather::selectionSortByPressure(int startIndex, int endIndex)
{
    int minIndex;

    // Iterate through the specified weather range using selection sort.
    for (int i = startIndex; i < endIndex - 1; ++i)
    {
        minIndex = i;

        // Find the minimum element in the remaining unsorted array.
        for (int j = i + 1; j < endIndex; ++j) {
            if(weatherArr[j].m_pressure < weatherArr[minIndex].m_pressure) {
                minIndex = j;
            }
        }

        std::swap(weatherArr[i], weatherArr[minIndex]);
    }
}


// Within a season (3 months), sort the records by Pressure.
void CWather::sortPressureBySeason()
{
    int startIndex = 0, endIndex = 0;

    // Iterate through the weather data array.
    for(int i = 0; i < weatherArr.size(); ++i)
    {
        if(isSeasonChanged(weatherArr[i].m_month, weatherArr[i].m_year, weatherArr[i+1].m_month, weatherArr[i+1].m_year)){

            // Find the start and end index for the current season.
            startIndex = endIndex;
            endIndex = i + 1;

            // Use selection sort to sort the records by Pressure within the season.
            selectionSortByPressure(startIndex, endIndex);
        }
    }
}


// Fills the weather table.
void CWather::completeTable(QTableWidget* weatherTable)
{
    // Set the number of rows in the table to match the size of the weather data array.
    weatherTable->setRowCount(weatherArr.size());

    // Populate the table with weather data.
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


// Draws a weather graph with weather data on the y-axis and dates on the x-axis.
void CWather::buildWeatherGraph(std::function<int (int)> getWeatherData, const QString &graphTitle)
{
    // Check if there is enough data to build the graph.
    if(weatherArr.size() < 3){
        QMessageBox::information(nullptr, "Not enough data.", "Data is required to build the graph."
                                " Please add 3 or more rows to the table and save it.", QMessageBox::Ok);
        return;
    }

    // Create a new line series for the graph.
    QLineSeries* series = new QLineSeries();

    // Add points to the series.
    for(int i = 0; i < weatherArr.size(); ++i){
        series->append(i, getWeatherData(i));
    }

    // Create a new chart and add the series to it.
    QChart* chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();

    chart->setTitle(graphTitle);

    chart->setAnimationOptions(QChart::AllAnimations);

    // Create axis x for the dates.
    QCategoryAxis *axisX = new QCategoryAxis();
    axisX->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);

    // Append dates to the axis.
    for(int i = 0; i < weatherArr.size(); ++i){
        QString date = QString::asprintf("%02d.%02d", static_cast<int>(weatherArr[i].m_month), weatherArr[i].m_year);
        axisX->append(date, i);
    }

    // Set the X-axis for the chart.
    chart->setAxisX(axisX, series);

    // Create a chart view and set rendering options.
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Create a dialog to display the chart.
    QDialog *dialog = new QDialog;
    dialog->setMinimumSize(800, 500);

    // Create a layout for the dialog and add the chart view to it.
    QVBoxLayout *layout = new QVBoxLayout;
    dialog->setLayout(layout);
    layout->addWidget(chartView);

    // Show the dialog.
    dialog->exec();
}


// Finds the indixes of weather "array" elements during which the wind direction did not change.
std::vector<std::vector<unsigned>> CWather::findDaysWindNotChange()
{
    // Vector to store vectors of indices where the wind direction did not change.
    std::vector<std::vector<unsigned>> windNotChangeArr;

    // Iterate through the weather array to find consecutive days with the same wind direction.
    for (int i = 0; i < weatherArr.size() - 1; ++i) {
        // Check if the wind direction for the current day is the same as the next day.
        if (weatherArr[i].m_windDirection == weatherArr[i + 1].m_windDirection) {
            // Vector to store indices of consecutive days with the same wind direction.
            std::vector<unsigned> indexArr;

            // Add the index of the current day to the vector.
            indexArr.push_back(i);

            // Continue adding indices while the wind direction remains the same.
            while (weatherArr[i].m_windDirection == weatherArr[i + 1].m_windDirection) {
                indexArr.push_back(i + 1);
                i++;
            }

            // Add the vector of indices to the main vector.
            windNotChangeArr.push_back(indexArr);
        }
    }

    return windNotChangeArr;
}


// Calculate the average temperature in the weather array.
double CWather::getAvgTemperature()
{
    // Variable to store the sum of temperature values.
    double sum = 0;

    // Calculate the sum of temperature values in the weather array.
    for (int i = 0; i < weatherArr.size(); ++i) {
        sum += weatherArr[i].m_temperature;
    }

    // Calculate and return the average temperature rounded to two decimal places.
    return qRound(sum / weatherArr.size() * 100.0) / 100.0;
}


// Calculate the average pressure in the weather array.
double CWather::getAvgPressure()
{
    // Variable to store the sum of pressure values.
    double sum = 0;

    // Calculate the sum of pressure values in the weather array.
    for (int i = 0; i < weatherArr.size(); ++i) {
        sum += weatherArr[i].m_pressure;
    }

    // Calculate and return the average pressure rounded to two decimal places.
    return qRound(sum / weatherArr.size() * 100.0) / 100.0;
}


// Calculate the average humidity in the weather array.
std::vector<QDate> CWather::getHighestHumidityDays()
{
    // Vector to store dates with the highest humidity.
    std::vector<QDate> highestHumDaysArr;

    // Variable to store the maximum humidity value.
    int maxHumidity = 0;

    // Find the maximum humidity value in the weather array.
    for (int i = 0; i < weatherArr.size(); ++i) {
        if (weatherArr[i].m_humidity > maxHumidity) {
            maxHumidity = weatherArr[i].m_humidity;
        }
    }

    // Find dates with the highest humidity and add them to the vector.
    for (int i = 0; i < weatherArr.size(); ++i) {
        if (weatherArr[i].m_humidity == maxHumidity) {
            highestHumDaysArr.push_back(QDate(weatherArr[i].m_year, weatherArr[i].m_month, weatherArr[i].m_day));
        }
    }

    return highestHumDaysArr;
}


// Finds periods when the temperature and pressure changed within certain percentages.
std::vector<CWather> CWather::findPeriodTemperatureAndPressureChangeWithinRange(double tRangePct, double psreRangePct)
{
    // Vector to store periods with temperature and pressure changes within the specified percentages.
    std::vector<CWather> periodsArr;

    // Initialize variables to keep track of the current element in weatherArr and the current period index.
    int currEl = 0, periodIndex = 0;

    // Iterate through weatherArr to find periods with desired changes.
    while (currEl < weatherArr.size())
    {
        // Check if periodsArr needs a new period at the current index.
        if (periodsArr.size() < periodIndex + 1)
        {
            CWather periodWeather;
            periodWeather.pushWeatherDataEnd(weatherArr[currEl]);
            periodsArr.push_back(periodWeather);
        }
        // Check if the current weather data fits within the percentage change criteria.
        else if (fabs(periodsArr[periodIndex].getAvgPressure() - static_cast<double>(weatherArr[currEl].m_pressure))
                     <= getPercentageOf(periodsArr[periodIndex].getAvgPressure()) * psreRangePct
                 && fabs(periodsArr[periodIndex].getAvgTemperature() - static_cast<double>(weatherArr[currEl].m_temperature))
                        <= getPercentageOf(periodsArr[periodIndex].getAvgTemperature()) * tRangePct)
        {
            // Add the current weather data to the current period.
            periodsArr[periodIndex].pushWeatherDataEnd(weatherArr[currEl]);
        }
        else
        {
            // Move to the next period index.
            periodIndex++;
            continue;
        }

        // Move to the next element in weatherArr.
        currEl++;
    }

    // Remove periods with less than 3 weather data entries.
    for (auto it = periodsArr.begin(); it != periodsArr.end(); ) {
        if ((*it).weatherArr.size() < 3) {
            it = periodsArr.erase(it);
        } else {
            ++it;
        }
    }

    return periodsArr;
}


// Retrieve weather data for a specific period between two dates.
CWather CWather::getWeatherByPeriod(QDate startDate, QDate endDate)
{
    // Create a new instance of CWather to store weather data for the specified period.
    CWather weatherPeriod;

    // Loop through the existing weatherArr to find data within the specified date range.
    for (int i = 0; i < weatherArr.size(); ++i) {
        // Convert the year, month, and day of the current weather data to QDate.
        QDate currentTableDate = QDate(weatherArr[i].m_year, weatherArr[i].m_month, weatherArr[i].m_day);

        // Check if the currentTableDate is within the specified date range.
        if (currentTableDate >= startDate && currentTableDate <= endDate) {
            // Add the weather data to the weatherPeriod's weatherArr.
            weatherPeriod.weatherArr.push_back(weatherArr[i]);
        }
    }

    return weatherPeriod;
}


// Forecasts the weather for the next month and adds it to the existing weather data (at the end).
void CWather::forecastWeatherForNextMonth()
{
    int lastElIndex = weatherArr.size() - 1;

    // Calculating the next month and year based on the last element in the vector.
    Month month = getNextMonth(weatherArr[lastElIndex].m_month);
    int year = month == Month::January ? weatherArr[lastElIndex].m_year + 1 : weatherArr[lastElIndex].m_year;

    // Variables to store forecasted weather data.
    int temperature;
    unsigned pressure;
    int humidity;
    WindDirection windDirection;

    // Looping through each day in the next month.
    for (int day = 1; day <= getNumDaysInMonth(month, year); ++day)
    {
        // Generating forecasted weather data based on the month.
        switch (month) {
        case Month::December:
            temperature = QRandomGenerator::global()->bounded(-5, 6);
            pressure = QRandomGenerator::global()->bounded(770, 781);
            humidity = QRandomGenerator::global()->bounded(30, 41);
            break;
        case Month::January:
            temperature = QRandomGenerator::global()->bounded(-10, 1);
            pressure = QRandomGenerator::global()->bounded(780, 791);
            humidity = QRandomGenerator::global()->bounded(20, 31);
            break;
        case Month::February:
            temperature = QRandomGenerator::global()->bounded(0, 11);
            pressure = QRandomGenerator::global()->bounded(760, 771);
            humidity = QRandomGenerator::global()->bounded(30, 46);
            break;
        case Month::March:
            temperature = QRandomGenerator::global()->bounded(10, 16);
            pressure = QRandomGenerator::global()->bounded(750, 761);
            humidity = QRandomGenerator::global()->bounded(45, 56);
            break;
        case Month::April:
            temperature = QRandomGenerator::global()->bounded(16, 21);
            pressure = QRandomGenerator::global()->bounded(740, 751);
            humidity = QRandomGenerator::global()->bounded(55, 66);
            break;
        case Month::May:
            temperature = QRandomGenerator::global()->bounded(20, 26);
            pressure = QRandomGenerator::global()->bounded(730, 741);
            humidity = QRandomGenerator::global()->bounded(65, 71);
            break;
        case Month::June:
            temperature = QRandomGenerator::global()->bounded(20, 31);
            pressure = QRandomGenerator::global()->bounded(721, 731);
            humidity = QRandomGenerator::global()->bounded(65, 76);
            break;
        case Month::July:
            temperature = QRandomGenerator::global()->bounded(25, 36);
            pressure = QRandomGenerator::global()->bounded(710, 721);
            humidity = QRandomGenerator::global()->bounded(70, 81);
            break;
        case Month::August:
            temperature = QRandomGenerator::global()->bounded(20, 31);
            pressure = QRandomGenerator::global()->bounded(721, 731);
            humidity = QRandomGenerator::global()->bounded(62, 76);
            break;
        case Month::September:
            temperature = QRandomGenerator::global()->bounded(15, 21);
            pressure = QRandomGenerator::global()->bounded(736, 751);
            humidity = QRandomGenerator::global()->bounded(60, 71);
            break;
        case Month::October:
            temperature = QRandomGenerator::global()->bounded(10, 16);
            pressure = QRandomGenerator::global()->bounded(751, 766);
            humidity = QRandomGenerator::global()->bounded(50, 61);
            break;
        case Month::November:
            temperature = QRandomGenerator::global()->bounded(5, 11);
            pressure = QRandomGenerator::global()->bounded(760, 771);
            humidity = QRandomGenerator::global()->bounded(40, 51);
            break;
        default:
            break;
        }

        // Generating a random wind direction.
        windDirection = static_cast<WindDirection>(QRandomGenerator::global()->bounded(1, 9));

        // Adding the forecasted weather data to the weatherArr vector.
        weatherArr.push_back(weatherData(year, month, day, temperature, pressure, humidity, windDirection));
    }
}


// Used to get the number of days for which weather data was added.
int CWather::getWeatherSize()
{
    return weatherArr.size();
}


// Used to get the temperature for a certain day.
int CWather::getTemperature(int index)
{
    return weatherArr[index].m_temperature;
}


// Used to get the pressure for a certain day.
unsigned int CWather::getPressure(int index)
{
    return weatherArr[index].m_pressure;
}


// Used to get the humidity for a certain day.
int CWather::getHumidity(int index)
{
    return weatherArr[index].m_humidity;
}


// Overriding the >> operation for reading data from a file using QTextStream.
QTextStream& operator>>(QTextStream &inFile, CWather &weather)
{
    weather.weatherArr.clear();

    // Reading data until the end of the file is reached.
    while (!inFile.atEnd())
    {
        // Variables to store intermediate data before constructing the weatherData object.
        int month;
        QString windDirection;

        // Creating a weatherData object to store the read data.
        CWather::weatherData wData;

        // Reading data from the QTextStream.
        inFile >> wData.m_year >> month >> wData.m_day
            >> wData.m_temperature >> wData.m_pressure >> wData.m_humidity >> windDirection;

        // Converting the read month and wind direction to the corresponding enum types.
        wData.m_month = static_cast<Month>(month);
        wData.m_windDirection = convertTextToWindDir(windDirection);

        // Adding the constructed weatherData object to the weatherArr vector.
        weather.weatherArr.push_back(wData);
    }

    return inFile;
}


// Adds data about one day of weather.
void CWather::pushWeatherDataEnd(const weatherData &wData)
{
    weatherArr.push_back(wData);
}


// Overriding the << operation for writing data to a file using QTextStream.
QTextStream& operator<<(QTextStream& out, const CWather& weather)
{
    for (int i = 0; i < weather.weatherArr.size(); ++i)
    {
        // Writing weather data to the QTextStream.
        out << weather.weatherArr[i].m_year << " " << static_cast<int>(weather.weatherArr[i].m_month) << " " << weather.weatherArr[i].m_day
            << " " <<  weather.weatherArr[i].m_temperature << " " << weather.weatherArr[i].m_pressure << " " << weather.weatherArr[i].m_humidity
            << " " << convertWindDirToText(weather.weatherArr[i].m_windDirection);

        // Adding a newline character unless it's the last record.
        if(i != weather.weatherArr.size() - 1){
            out << "\n";
        }
    }

    return out;
}


// Returns the season based on the given month.
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


// Checks if the season has changed between two given dates.
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


// Converts a text representation to a wind direction.
WindDirection convertTextToWindDir(QString text)
{
    // Check the input text and return the corresponding wind direction enum.
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
        return Undefined; // Default case for undefined wind direction.
    }
}


// Converts a wind direction enum to its text representation.
QString convertWindDirToText(WindDirection direction)
{
    // Check the wind direction and return the corresponding text representation.
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
        return "Undefined"; // Default case for undefined wind direction.
    }
}


// Calculates the percentage of a given number.
double getPercentageOf(double digit)
{
    return digit / 100;
}


// Returns the number of days in a specific month of a given year.
int getNumDaysInMonth(Month month, int year)
{
    // Switch case to determine the number of days based on the month.
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
        // Check for leap year to determine the number of days in February.
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
            return 29;
        } else {
            return 28;
        }

    default:
        return 0; // Unknown month, return 0.
    }
}


// Returns the next month based on the current month.
Month getNextMonth(Month currentMonth) {
    // Check if the current month is December, return January; otherwise, return current month + 1.
    if (currentMonth == Month::December) {
        return Month::January;
    } else {
        return static_cast<Month>(static_cast<int>(currentMonth) + 1);
    }
}
