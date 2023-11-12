#include "../Header Files/mainwindow.h"
#include "./ui_mainwindow.h"


// Constructor.
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Connect the itemChanged signal of the weatherTable to a lambda function displaying a status message.
    connect(ui->weatherTable, &QTableWidget::itemChanged, this, [this](){
        statusBar()->showMessage("Not all changes are saved )=");
    });

    // Set up a custom context menu for the weatherTable.
    ui->weatherTable->setContextMenuPolicy(Qt::CustomContextMenu);

    // Connect the customContextMenuRequested signal to the createTableContextMenu slot.
    connect(ui->weatherTable, &QTableWidget::customContextMenuRequested, this, &MainWindow::createTableContextMenu);
}


// Default destructor
MainWindow::~MainWindow()
{
    delete ui;
}


// Display a warning message to the user.
bool MainWindow::showWarningMessage(const QString& warningMessage)
{
    QMessageBox msgBox;
    msgBox.setText(warningMessage);
    msgBox.setIconPixmap(QPixmap(":/Resource Files/Resource Files/warning.png"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);

    // Return true if the user clicks Ok, and false if the user clicks Cancel.
    return (msgBox.exec() == QMessageBox::Ok);
}


// Display a error message to the user.
void MainWindow::showErrorMessage(const QString &errorMessage)
{
    QMessageBox msgBox;
    msgBox.setText(errorMessage);
    msgBox.setIconPixmap(QPixmap(":/Resource Files/Resource Files/error_icon.png"));
    msgBox.exec();
}


// Display a data message (information) to the user.
void MainWindow::showOutputDataMessage(const QString &dataMessage)
{
    QMessageBox msgBox;
    msgBox.setText(dataMessage);
    msgBox.setIconPixmap(QPixmap(":/Resource Files/Resource Files/data_analysis.png"));
    msgBox.exec();
}


// Check if weather table completely filled.
bool MainWindow::isTableCompletelyFilled()
{
    for(int i = 0; i < ui->weatherTable->rowCount(); ++i){
        for(int j = 0; j < ui->weatherTable->columnCount(); ++j){
            if(ui->weatherTable->item(i, j) == nullptr)
            {
                showErrorMessage("We cannot perform this action. The table is not completely filled in.");
                return false;
            }
        }
    }

    // All cells are filled, return true.
    return true;
}


// Creates a context menu for the weather table to change the row data.
void MainWindow::createTableContextMenu(const QPoint& pos)
{
    // Get the index of the item at the given position.
    QModelIndex index = ui->weatherTable->indexAt(pos);
    int rowIndex = index.row();

    // Create a context menu.
    QMenu* tableContextMenu = new QMenu(tr("Set data"), this);

    // Create actions for various data modifications.
    QAction* setDate = tableContextMenu->addAction("Set date");
    QAction* setTemperature = tableContextMenu->addAction("Set temperature");
    QAction* setPressure = tableContextMenu->addAction("Set pressure");
    QAction* setHumidity = tableContextMenu->addAction("Set humidity");
    QAction* setWindDirection = tableContextMenu->addAction("Set wind direction");
    QAction* removeRow = tableContextMenu->addAction("Delete row");

    // Connect each action to its corresponding slot function.
    connect(setDate, &QAction::triggered, this, [this, rowIndex](){
        setDateToTable(rowIndex);
    });

    connect(setTemperature, &QAction::triggered, this, [this, rowIndex](){
        setTemperatureToTable(rowIndex);
    });

    connect(setPressure, &QAction::triggered, this, [this, rowIndex](){
        setPressureToTable(rowIndex);
    });

    connect(setHumidity, &QAction::triggered, this, [this, rowIndex](){
        setHumidityToTable(rowIndex);
    });

    connect(setWindDirection, &QAction::triggered, this, [this, rowIndex](){
        setWindDirectionToTable(rowIndex);
    });

    connect(removeRow, &QAction::triggered, this, [this, rowIndex](){
        deleteRow(rowIndex);
    });

    // Display the context menu at the given position.
    tableContextMenu->exec(ui->weatherTable->viewport()->mapToGlobal(pos));
}


// This function allows the user to set a new date for a specific row in the weather table.
void MainWindow::setDateToTable(int rowIndex)
{
    // Check if the given rowIndex is within the valid range.
    if(rowIndex < ui->weatherTable->rowCount() && rowIndex >= 0)
    {
        // Create a dialog for choosing a date with a date picker and a "Set" button.
        QDialog* dateDialog = createDialog("Choose date", 170, 100);
        QDateEdit* dateEdit = new QDateEdit(dateDialog);
        dateEdit->setCalendarPopup(true);
        QPushButton* setDateButton = new QPushButton("Set", dateDialog);

        // Set up the layout of the dialog.
        QVBoxLayout* layout = new QVBoxLayout(dateDialog);
        layout->addWidget(dateEdit);
        layout->addWidget(setDateButton);

        connect(setDateButton, &QPushButton::clicked, dateDialog, &QDialog::accept);

        // If the user accepts the dialog, update the corresponding row in the table with the selected date.
        if(dateDialog->exec() == QDialog::Accepted)
        {
            QDate selectedDate = dateEdit->date();

            setCellText(ui->weatherTable, rowIndex, 0, QString::number(selectedDate.year()));
            setCellText(ui->weatherTable, rowIndex, 1, QString::number(selectedDate.month()));
            setCellText(ui->weatherTable, rowIndex, 2, QString::number(selectedDate.day()));
        }

        statusBar()->showMessage("Not all changes are saved )=");
    }
    else
    {
        showErrorMessage("Error. Click on the row whose data you want to change.");
    }
}


// This function allows the user to set a new temperature for a specific row in the weather table.
void MainWindow::setTemperatureToTable(int rowIndex)
{
    // Check if the given rowIndex is within the valid range.
    if(rowIndex < ui->weatherTable->rowCount() && rowIndex >= 0)
    {
        // Create a dialog for choosing temperature with a horizontal slider, a label, and a "Set" button.
        QDialog* temperatureDialog = createDialog("Choose temperature", 230, 140);
        QSlider* temperatureSlider = createSlider(Qt::Horizontal, temperatureDialog, -100, 100, 0, 1);
        QLabel* temperatureLabel = new QLabel(temperatureDialog);
        temperatureLabel->setAlignment(Qt::AlignCenter);
        temperatureLabel->setText(QString("%1 °C").arg(temperatureSlider->value()));
        QPushButton* setTemperatureButton = new QPushButton("Set", temperatureDialog);

        // Set up the layout of the dialog.
        QVBoxLayout* layout = new QVBoxLayout(temperatureDialog);
        layout->addWidget(temperatureSlider);
        layout->addWidget(temperatureLabel);
        layout->addWidget(setTemperatureButton);

        connect(setTemperatureButton, &QPushButton::clicked, temperatureDialog, &QDialog::accept);

        // Connect the valueChanged signal of the slider to update the label with the selected temperature.
        connect(temperatureSlider, &QSlider::valueChanged, this, [temperatureLabel](int value) {
            temperatureLabel->setText(QString("%1 °C").arg(value));
        });

        // If the user accepts the dialog, update the corresponding row in the table with the selected temperature.
        if(temperatureDialog->exec() == QDialog::Accepted){
            setCellText(ui->weatherTable, rowIndex, 3, QString::number(temperatureSlider->value()));
        }

        statusBar()->showMessage("Not all changes are saved )=");
    }
    else
    {
        showErrorMessage("Error. Click on the row whose data you want to change.");
    }
}


// This function allows the user to set a new pressure for a specific row in the weather table.
void MainWindow::setPressureToTable(int rowIndex)
{
    // Check if the given rowIndex is within the valid range.
    if(rowIndex < ui->weatherTable->rowCount() && rowIndex >= 0)
    {
        // Create a dialog for choosing pressure with a horizontal slider, a label, and a "Set" button.
        QDialog* pressureDialog = createDialog("Choose pressure", 230, 140);
        QSlider* pressureSlider = createSlider(Qt::Horizontal, pressureDialog, 650, 850, 760, 1);
        QLabel* pressureLabel = new QLabel(pressureDialog);
        pressureLabel->setAlignment(Qt::AlignCenter);
        pressureLabel->setText(QString("%1 мм рт. ст.").arg(pressureSlider->value()));
        QPushButton* setPressureButton = new QPushButton("Set", pressureDialog);

        // Set up the layout of the dialog.
        QVBoxLayout* layout = new QVBoxLayout(pressureDialog);
        layout->addWidget(pressureSlider);
        layout->addWidget(pressureLabel);
        layout->addWidget(setPressureButton);

        connect(setPressureButton, &QPushButton::clicked, pressureDialog, &QDialog::accept);

        // Connect the valueChanged signal of the slider to update the label with the selected pressure.
        connect(pressureSlider, &QSlider::valueChanged, this, [pressureLabel](int value) {
            pressureLabel->setText(QString("%1 мм рт. ст.").arg(value));
        });

        // If the user accepts the dialog, update the corresponding row in the table with the selected pressure.
        if(pressureDialog->exec() == QDialog::Accepted){
            setCellText(ui->weatherTable, rowIndex, 4, QString::number(pressureSlider->value()));
        }

        statusBar()->showMessage("Not all changes are saved )=");
    }
    else
    {
        showErrorMessage("Error. Click on the row whose data you want to change.");
    }
}


// This function allows the user to set a new humidity for a specific row in the weather table.
void MainWindow::setHumidityToTable(int rowIndex)
{
    // Check if the given rowIndex is within the valid range.
    if(rowIndex < ui->weatherTable->rowCount() && rowIndex >= 0)
    {
        // Create a dialog for choosing humidity with a horizontal slider, a label, and a "Set" button.
        QDialog* humidityDialog = createDialog("Choose humidity", 230, 140);
        QSlider* humiditySlider = createSlider(Qt::Horizontal, humidityDialog, 1, 100, 40, 1);
        QLabel* humidityLabel = new QLabel(humidityDialog);
        humidityLabel->setAlignment(Qt::AlignCenter);
        humidityLabel->setText(QString("%1 %").arg(humiditySlider->value()));
        QPushButton* setHumidityButton = new QPushButton("Set", humidityDialog);

        // Set up the layout of the dialog.
        QVBoxLayout* layout = new QVBoxLayout(humidityDialog);
        layout->addWidget(humiditySlider);
        layout->addWidget(humidityLabel);
        layout->addWidget(setHumidityButton);

        connect(setHumidityButton, &QPushButton::clicked, humidityDialog, &QDialog::accept);

        // Connect the valueChanged signal of the slider to update the label with the selected humidity.
        connect(humiditySlider, &QSlider::valueChanged, this, [humidityLabel](int value) {
            humidityLabel->setText(QString("%1 %").arg(value));
        });

        // If the user accepts the dialog, update the corresponding row in the table with the selected humidity.
        if(humidityDialog->exec() == QDialog::Accepted){
            setCellText(ui->weatherTable, rowIndex, 5, QString::number(humiditySlider->value()));
        }

        statusBar()->showMessage("Not all changes are saved )=");
    }
    else
    {
        showErrorMessage("Error. Click on the row whose data you want to change.");
    }
}


// This function allows the user to set a new wind direction for a specific row in the weather table.
void MainWindow::setWindDirectionToTable(int rowIndex)
{
    // Check if the given rowIndex is within the valid range.
    if(rowIndex < ui->weatherTable->rowCount() && rowIndex >= 0)
    {
        // Create a dialog for choosing wind direction with a combo box.
        QDialog* windDirectionDialog = createDialog("Choose wind drection", 230, 140);
        QComboBox* directionComboBox = new QComboBox(windDirectionDialog);

        // Add directional options to the combo box.
        directionComboBox->addItem("North", QVariant("N"));
        directionComboBox->addItem("South", QVariant("S"));
        directionComboBox->addItem("East", QVariant("E"));
        directionComboBox->addItem("West", QVariant("W"));
        directionComboBox->addItem("North-East", QVariant("NE"));
        directionComboBox->addItem("North-West", QVariant("NW"));
        directionComboBox->addItem("South-East", QVariant("SE"));
        directionComboBox->addItem("South-West", QVariant("SW"));

        QPushButton* setWindDirectionButton = new QPushButton("Set", windDirectionDialog);

        // Set up the layout of the dialog.
        QVBoxLayout* layout = new QVBoxLayout(windDirectionDialog);
        layout->addWidget(directionComboBox);
        layout->addWidget(setWindDirectionButton);

        connect(setWindDirectionButton, &QPushButton::clicked, windDirectionDialog, &QDialog::accept);

        // If the user accepts the dialog, update the corresponding row in the table with the selected wind direction.
        if(windDirectionDialog->exec() == QDialog::Accepted){
            setCellText(ui->weatherTable, rowIndex, 6, directionComboBox->currentData().toString());
        }

        statusBar()->showMessage("Not all changes are saved )=");
    }
    else
    {
        showErrorMessage("Error. Click on the row whose data you want to change.");
    }
}


// This function allows the user to delete a specific row from the weather table.
void MainWindow::deleteRow(int rowIndex)
{
    // Check if the given rowIndex is within the valid range.
    if(rowIndex < ui->weatherTable->rowCount() && rowIndex >= 0)
    {
        ui->weatherTable->removeRow(rowIndex);
        statusBar()->showMessage("Not all changes are saved )=");
    }
    else
    {
        showErrorMessage("Error. Click on the row which want to delete.");
    }
}


// From the available weather information, it determines the weather for a certain period selected by the user.
CWather MainWindow::getWeatherPeriod(bool& isSuccess)
{
    // Initialize the CWather object to store the chosen weather period.
    CWather choosedPeriod;

    // Create a dialog for choosing the date period.
    QDialog* dialog = createDialog("Choose date", 250, 200);

    // Create date edit widgets for selecting start and end dates.
    QDateEdit* startDateEdit = new QDateEdit(dialog);
    startDateEdit->setCalendarPopup(true);
    QDateEdit* endDateEdit = new QDateEdit(dialog);
    endDateEdit->setCalendarPopup(true);

    // Create a button to confirm the selected period.
    QPushButton* setPeriodButton = new QPushButton("Choose period", dialog);

    // Set up the layout for the dialog.
    QVBoxLayout* layout = new QVBoxLayout(dialog);
    layout->addWidget(startDateEdit);
    layout->addWidget(endDateEdit);
    layout->addWidget(setPeriodButton);

    connect(setPeriodButton, &QPushButton::clicked, dialog, &QDialog::accept);

    // Execute the dialog and check if the user confirmed the period selection.
    if(dialog->exec() == QDialog::Accepted)
    {
        // Retrieve the selected start and end dates.
        QDate selectedStartDate = startDateEdit->date();
        QDate selectedEndDate = endDateEdit->date();

        // Check if the start date is greater than the end date and handle the error.
        try {
            if(selectedStartDate > selectedEndDate)
            {
                throw QString("The start date cannot be greater than the end date!");
            }
        } catch (const QString& exception) {
            showErrorMessage(exception);
            isSuccess = false;
            return choosedPeriod;
        }

        // Get the weather data for the chosen period.
        choosedPeriod = mainWeather.getWeatherByPeriod(selectedStartDate, selectedEndDate);

        // Check if there is no information available for the chosen period and handle the error.
        try {
            if(choosedPeriod.getWeatherSize() == 0)
            {
                throw QString("We have no information about this period. Try adding data for this period and refreshing the table.");
            }
        } catch (const QString& exception) {
            showErrorMessage(exception);
            isSuccess = false;
            return choosedPeriod;
        }
    }

    isSuccess = true;
    return choosedPeriod;
}


// Within a season (3 months), sort the records by the Pressure using a simple selection method and update the main weather table.
void MainWindow::on_actionSort_by_pressure_within_seasons_triggered()
{
    // Display a warning if there are unsaved changes.
    QString warningMessage = "You did not save all the changes you made. Do you want to continue?";
    if(statusBar()->currentMessage() == "Not all changes are saved )=" && !showWarningMessage(warningMessage)){
        return;
    }

    // Display a warning about the potential impact on the table.
    warningMessage = "This function can «ruin» a table because it changes the order of its elements. Do you really want to continue?";
    if (!showWarningMessage(warningMessage)){
        return;
    }

    mainWeather.sortPressureBySeason();
    mainWeather.completeTable(ui->weatherTable);
    statusBar()->showMessage("All changes have been saved (=");
}


// Adds a blank row to the end of the main weather table.
void MainWindow::on_actionAdd_row_triggered()
{
    ui->weatherTable->insertRow(ui->weatherTable->rowCount());
    statusBar()->showMessage("Not all changes are saved )=");
}


// Saves all changes made to the table and writes to the main weather object.
void MainWindow::on_actionSave_changes_triggered()
{
    if(isTableCompletelyFilled())
    {
        CWather newWeather(ui->weatherTable, ui->weatherTable->rowCount());

        try {
            // Validate the weather data.
            if(!newWeather.isWeatherDataValid())
            {
                throw QString("Incorrectly entered data. Below are the values that the data should take on.\n"
                              "\n----- Month: 1-12. -----\n------ Day: 1-31. ------\n---- Pressure: >0. ----\n"
                              "--- Humidity: 0-100. ---\nWind direction: N, S, E, W, NE, NW, SE or SW.");
            }
        } catch (const QString& exception) {
            showErrorMessage(exception);
            return;
        }

        // Update the main weather object with the new data.
        mainWeather = newWeather;

        statusBar()->showMessage("All changes have been saved (=");
    }
}


// Opens a file and reads data from it into a table and main weather object.
void MainWindow::on_actionOpen_triggered()
{ 
    QString warningMessage = "When you click the OK button, all previous data will be erased and the table will be filled from the file.";
    if(!showWarningMessage(warningMessage)){
        return;
    }

    // Prompt the user to select a text file.
    QString fileName = QFileDialog::getOpenFileName(this, "Select a file", "/Users/artomrevus/Desktop", "Text file (*.txt)");

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // Read the file using QTextStream.
        QTextStream in(&file);

        ui->weatherTable->setRowCount(0);

        // Read weather data from the file into the mainWeather object.
        in >> mainWeather;
        mainWeather.completeTable(ui->weatherTable);

        statusBar()->showMessage("All changes have been saved (=");
        file.close();
    }
    else
    {
        showErrorMessage("File could not be opened.");
    }
}


// Opens a file and writes data to it from main weather object.
void MainWindow::on_actionSave_triggered()
{
    QString warningMessage = "You did not save all the changes you made. Do you really want to save the last changes "
                             "you made to a file?";

    if(statusBar()->currentMessage() == "Not all changes are saved )=" && !showWarningMessage(warningMessage)){
        return;
    }

    // Prompt the user to select a file for saving.
    QString fileName = QFileDialog::getOpenFileName(this, "Select a file", "/Users/artomrevus/Desktop", "Text file (*.txt)");

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        // Write weather data to the file using QTextStream.
        QTextStream out(&file);
        out << mainWeather;
        file.close();
    }
    else
    {
        showErrorMessage("The data has not been recorded!");
    }
}


// Sets text to a table cell
void MainWindow::setCellText(QTableWidget* table, const int& rowIndex, const int& columnIndex, const QString& text)
{
    table->setItem(rowIndex, columnIndex, new QTableWidgetItem(text));
}


// This method is used to create and configure a QDialog (dialogue window).
QDialog *MainWindow::createDialog(const QString& windowTitle, const int& width, const int& height)
{
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle(windowTitle);
    dialog->setFixedSize(width, height);
    return dialog;
}


// This method is used to create and configure a QSlider (slider).
QSlider *MainWindow::createSlider(const Qt::Orientation& orientation, QWidget* parent, const int& min, const int& max,
                                  const int& initValue, const int& interval)
{
    QSlider* slider = new QSlider(orientation, parent);
    slider->setRange(min, max);
    slider->setValue(initValue);
    slider->setTickInterval(interval);
    return slider;
}


// Plots a graph in a new dialogue window with temperature data on the y-axis and dates on the x-axis.
void MainWindow::on_actionBuild_graph_of_t_2_triggered()
{ 
    QString warningMessage = "You did not save all the changes you made. Do you really want to continue build temperature graph?";
    if(statusBar()->currentMessage() == "Not all changes are saved )=" && !showWarningMessage(warningMessage)){
        return;
    }

    mainWeather.buildWeatherGraph([this](int i){
        return mainWeather.getTemperature(i);
    }, "Temperature graph");
}


// Plots a graph in a new dialogue window with pressure data on the y-axis and dates on the x-axis.
void MainWindow::on_actionBuild_graph_of_pressure_2_triggered()
{
    QString warningMessage = "You did not save all the changes you made. Do you really want to continue build pressure graph?";
    if(statusBar()->currentMessage() == "Not all changes are saved )=" && !showWarningMessage(warningMessage)){
        return;
    }

    mainWeather.buildWeatherGraph([this](int i){
        return mainWeather.getPressure(i);
    }, "Pressure graph");
}


// Plots a graph in a new dialogue window with humidity data on the y-axis and dates on the x-axis.
void MainWindow::on_actionBuild_graph_of_humidity_2_triggered()
{
    QString warningMessage = "You did not save all the changes you made. Do you really want to continue build humidity graph?";
    if(statusBar()->currentMessage() == "Not all changes are saved )=" && !showWarningMessage(warningMessage)){
        return;
    }

    mainWeather.buildWeatherGraph([this](int i){
        return mainWeather.getHumidity(i);
    }, "Humidity graph");
}


// Identify the days during which the wind direction did not change and mark them with a unique colour in the weather table.
void MainWindow::on_actionFind_days_while_triggered()
{
    try {
        // Check if all changes are saved.
        if(statusBar()->currentMessage() == "Not all changes are saved )=")
        {
            throw QString("Save your table before performing this action.");
        }
    } catch (const QString& exception) {
        showErrorMessage(exception);
        return;
    }

    // Find days when the wind direction did not change.
    std::vector<std::vector<unsigned>> windNotChangeArr = mainWeather.findDaysWindNotChange();

    // Highlight the corresponding rows in the weather table with random background colors.
    for (int i = 0; i < windNotChangeArr.size(); ++i) {
        QColor randomColor(QRandomGenerator::global()->bounded(256), QRandomGenerator::global()->bounded(256), QRandomGenerator::global()->bounded(256));
        for (int j = 0; j < windNotChangeArr[i].size(); ++j) {
            for (int colIndex = 0; colIndex < COLUMN_CNT; ++colIndex) {
                ui->weatherTable->item(windNotChangeArr[i][j], colIndex)->setBackground(randomColor);
            }
        }
    }

    showOutputDataMessage("The days during which the wind direction did not change are marked with a common colour (different from white).");
    statusBar()->showMessage("All changes have been saved (=");
}


// According to the period specified by the user, it determines the average temperature during this period.
void MainWindow::on_actionDetermine_the_avg_temperature_triggered()
{   
    QString warningMessage = "You did not save all the changes you made. Do you want to continue searching "
                             "for the average temperature?";
    if(statusBar()->currentMessage() == "Not all changes are saved )=" && !showWarningMessage(warningMessage)){
        return;
    }

    // Get weather data for a specified period.
    bool isSuccess;
    CWather findAvgTArr = getWeatherPeriod(isSuccess);

    if(isSuccess)
    {
        // Calculate and display the average temperature.
        double avgTemperature = findAvgTArr.getAvgTemperature();
        showOutputDataMessage("Avg temperature = " + QString::number(avgTemperature) + "°C");
    }
}


// According to the period specified by the user, it determines the days with the highest humidity.
void MainWindow::on_actionDetermine_highest_humidity_days_triggered()
{   
    QString warningMessage = "You did not save all the changes you made. Do you want to continue searching "
                             "for highest humidity days?";
    if(statusBar()->currentMessage() == "Not all changes are saved )=" && !showWarningMessage(warningMessage)){
        return;
    }

    // Get weather data for a specified period.
    bool isSuccess;
    CWather findHighestHumArr = getWeatherPeriod(isSuccess);

    if(isSuccess)
    {
        // Find and display the highest humidity days.
        std::vector<QDate> highestHumDaysArr = findHighestHumArr.getHighestHumidityDays();
        QString HHDaysQStr = "Highest humidity days: \n";
        for (int i = 0; i < highestHumDaysArr.size(); ++i) {
            QDate date(highestHumDaysArr[i].year(), highestHumDaysArr[i].month(), highestHumDaysArr[i].day());
            HHDaysQStr += date.toString("dd.MM.yyyy");
            HHDaysQStr += "\n";
        }

        showOutputDataMessage(HHDaysQStr);
    }
}


// For each period (passed as a method parameter), it creates a weather table and displays its data.
void MainWindow::displayWeatherPeriods(std::vector<CWather> periodsArr)
{
    // Create a dialog to display the weather data.
    QDialog dialog;
    dialog.setWindowTitle("Periods (3 and more days) when the pressure varied within ±2.5% and t ±3.6%.");
    dialog.setFixedSize(760, 400);

    QScrollArea scrollArea;

    // Create a tab widget to hold tables for each period.
    QTabWidget tabWidget;
    tabWidget.setMinimumWidth(720);
    tabWidget.setFixedHeight(350);

    // Iterate through each period and create a table for each one.
    for (int i = 0; i < periodsArr.size(); ++i) {
        QTableWidget* periodtable = createWeatherTable(periodsArr[i]);
        tabWidget.addTab(periodtable, "Period " + QString::number(i + 1));
    }

    // Set the tab bar properties.
    QTabBar* tabBar = tabWidget.findChild<QTabBar*>();
    tabBar->setElideMode(Qt::ElideNone);
    tabBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    scrollArea.setWidget(&tabWidget);

    // Set up the layout for the dialog.
    QVBoxLayout layout(&dialog);
    layout.addWidget(&scrollArea);

    dialog.exec();
}


// This method is used to create and configure a QTableWidget (table).
QTableWidget* MainWindow::createWeatherTable(CWather weather)
{
    QTableWidget* weatherTable = new QTableWidget();
    weatherTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    weatherTable->setRowCount(weather.getWeatherSize());

    weatherTable->setColumnCount(7);
    QStringList columnNames;
    columnNames << "Year" << "Month" << "Day" << "t" << "Pressure" << "Humidity" << "Wind direction";
    weatherTable->setHorizontalHeaderLabels(columnNames);

    weather.completeTable(weatherTable);

    return weatherTable;
}


// Find the periods in which the pressure varied within ±2.5% and t - 3.6% and display them in the form of tables.
void MainWindow::on_actionFind_days_while_pressure_2_5_triggered()
{
    QString warningMessage = "You did not save all the changes you made. Do you want to continue searching "
                             "for periods when the pressure varied within ±2.5% and t varied within 3.6%?";

    if(statusBar()->currentMessage() == "Not all changes are saved )=" && !showWarningMessage(warningMessage)){
            return;
    }

    // Find periods when pressure and temperature vary within specified ranges.
    std::vector<CWather> periodsArr = mainWeather.findPeriodTemperatureAndPressureChangeWithinRange(3.6, 2.5);

    // Display a message if no periods are found.
    if(periodsArr.size() == 0){
        showOutputDataMessage("In your table, there are no periods when the pressure varied within ±2.5% and t varied within 3.6%.");
    }

    displayWeatherPeriods(periodsArr);
}


// Predict the weather for the next month, taking into account the change of season, and write it at the end of the table.
void MainWindow::on_actionForecast_weathe_for_next_month_triggered()
{
    QString warningMessage = "You did not save all the changes you made. Do you want to continue forecasting weather?";
    if(statusBar()->currentMessage() == "Not all changes are saved )=" && !showWarningMessage(warningMessage)){
        return;
    }

    // Forecast weather for the next month.
    mainWeather.forecastWeatherForNextMonth();
    mainWeather.completeTable(ui->weatherTable);

    showOutputDataMessage("The weather for the next month has been successfully predicted and added to the table.");
    statusBar()->showMessage("All changes have been saved (=");
}
