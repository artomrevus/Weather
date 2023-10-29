#include "../Header Files/mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->weatherTable, &QTableWidget::itemChanged, this, [this](){
        statusBar()->showMessage("Not all changes are saved )=");
    });

    ui->weatherTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->weatherTable, &QTableWidget::customContextMenuRequested, this, &MainWindow::createTableContextMenu);
}


MainWindow::~MainWindow()
{
    delete ui;
}


bool MainWindow::isTableCompletelyFilled()
{
    for(int i = 0; i < ui->weatherTable->rowCount(); ++i){
        for(int j = 0; j < ui->weatherTable->columnCount(); ++j){
            if(ui->weatherTable->item(i, j) == nullptr)
            {
                QMessageBox msgBox;
                msgBox.setText("We cannot perform this action. The table is not completely filled in.");
                msgBox.setIconPixmap(QPixmap(":/error_icon.png"));
                msgBox.exec();
                return false;
            }
        }
    }
    return true;
}


void MainWindow::createTableContextMenu(const QPoint& pos)
{
    QModelIndex index = ui->weatherTable->indexAt(pos);
    int rowIndex = index.row();

    QMenu* tableContextMenu = new QMenu(tr("Set data"), this);

    QAction* setDate = tableContextMenu->addAction("Set date");
    QAction* setTemperature = tableContextMenu->addAction("Set temperature");
    QAction* setPressure = tableContextMenu->addAction("Set pressure");
    QAction* setHumidity = tableContextMenu->addAction("Set humidity");
    QAction* setWindDirection = tableContextMenu->addAction("Set wind direction");
    QAction* removeRow = tableContextMenu->addAction("Delete row");

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

    tableContextMenu->exec(ui->weatherTable->viewport()->mapToGlobal(pos));
}


void MainWindow::setDateToTable(int rowIndex)
{
    if(rowIndex < ui->weatherTable->rowCount() && rowIndex >= 0)
    {
        QDialog* dateDialog = createDialog("Choose date", 170, 100);
        QDateEdit* dateEdit = new QDateEdit(dateDialog);
        dateEdit->setCalendarPopup(true);
        QPushButton* setDateButton = new QPushButton("Set", dateDialog);

        QVBoxLayout* layout = new QVBoxLayout(dateDialog);
        layout->addWidget(dateEdit);
        layout->addWidget(setDateButton);

        connect(setDateButton, &QPushButton::clicked, dateDialog, &QDialog::accept);

        if(dateDialog->exec() == QDialog::Accepted)
        {
            QDate selectedDate = dateEdit->date();

            setCellText(ui->weatherTable, rowIndex, 0, QString::number(selectedDate.year()));
            setCellText(ui->weatherTable, rowIndex, 1, QString::number(selectedDate.month()));
            setCellText(ui->weatherTable, rowIndex, 2, QString::number(selectedDate.day()));
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Error. Click on the row whose data you want to change.");
        msgBox.setIconPixmap(QPixmap(":/error_icon.png"));
        msgBox.exec();
    }
}


void MainWindow::setTemperatureToTable(int rowIndex)
{
    if(rowIndex < ui->weatherTable->rowCount() && rowIndex >= 0)
    {
        QDialog* temperatureDialog = createDialog("Choose temperature", 230, 140);

        QSlider* temperatureSlider = createSlider(Qt::Horizontal, temperatureDialog, -100, 100, 0, 1);

        QLabel* temperatureLabel = new QLabel(temperatureDialog);
        temperatureLabel->setAlignment(Qt::AlignCenter);
        temperatureLabel->setText(QString("%1 °C").arg(temperatureSlider->value()));

        QPushButton* setTemperatureButton = new QPushButton("Set", temperatureDialog);

        QVBoxLayout* layout = new QVBoxLayout(temperatureDialog);
        layout->addWidget(temperatureSlider);
        layout->addWidget(temperatureLabel);
        layout->addWidget(setTemperatureButton);

        connect(setTemperatureButton, &QPushButton::clicked, temperatureDialog, &QDialog::accept);

        connect(temperatureSlider, &QSlider::valueChanged, this, [temperatureLabel](int value) {
            temperatureLabel->setText(QString("%1 °C").arg(value));
        });

        if(temperatureDialog->exec() == QDialog::Accepted){
            setCellText(ui->weatherTable, rowIndex, 3, QString::number(temperatureSlider->value()));
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Error. Click on the row whose data you want to change.");
        msgBox.setIconPixmap(QPixmap(":/error_icon.png"));
        msgBox.exec();
    }
}


void MainWindow::setPressureToTable(int rowIndex)
{
    if(rowIndex < ui->weatherTable->rowCount() && rowIndex >= 0)
    {
        QDialog* pressureDialog = createDialog("Choose pressure", 230, 140);

        QSlider* pressureSlider = createSlider(Qt::Horizontal, pressureDialog, 650, 850, 760, 1);

        QLabel* pressureLabel = new QLabel(pressureDialog);
        pressureLabel->setAlignment(Qt::AlignCenter);
        pressureLabel->setText(QString("%1 мм рт. ст.").arg(pressureSlider->value()));

        QPushButton* setPressureButton = new QPushButton("Set", pressureDialog);

        QVBoxLayout* layout = new QVBoxLayout(pressureDialog);
        layout->addWidget(pressureSlider);
        layout->addWidget(pressureLabel);
        layout->addWidget(setPressureButton);

        connect(setPressureButton, &QPushButton::clicked, pressureDialog, &QDialog::accept);

        connect(pressureSlider, &QSlider::valueChanged, this, [pressureLabel](int value) {
            pressureLabel->setText(QString("%1 мм рт. ст.").arg(value));
        });

        if(pressureDialog->exec() == QDialog::Accepted){
            setCellText(ui->weatherTable, rowIndex, 4, QString::number(pressureSlider->value()));
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Error. Click on the row whose data you want to change.");
        msgBox.setIconPixmap(QPixmap(":/error_icon.png"));
        msgBox.exec();
    }
}


void MainWindow::setHumidityToTable(int rowIndex)
{
    if(rowIndex < ui->weatherTable->rowCount() && rowIndex >= 0)
    {
        QDialog* humidityDialog = createDialog("Choose humidity", 230, 140);

        QSlider* humiditySlider = createSlider(Qt::Horizontal, humidityDialog, 1, 100, 40, 1);

        QLabel* humidityLabel = new QLabel(humidityDialog);
        humidityLabel->setAlignment(Qt::AlignCenter);
        humidityLabel->setText(QString("%1 %").arg(humiditySlider->value()));

        QPushButton* setPressureButton = new QPushButton("Set", humidityDialog);

        QVBoxLayout* layout = new QVBoxLayout(humidityDialog);
        layout->addWidget(humiditySlider);
        layout->addWidget(humidityLabel);
        layout->addWidget(setPressureButton);

        connect(setPressureButton, &QPushButton::clicked, humidityDialog, &QDialog::accept);

        connect(humiditySlider, &QSlider::valueChanged, this, [humidityLabel](int value) {
            humidityLabel->setText(QString("%1 %").arg(value));
        });

        if(humidityDialog->exec() == QDialog::Accepted){
            setCellText(ui->weatherTable, rowIndex, 5, QString::number(humiditySlider->value()));
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Error. Click on the row whose data you want to change.");
        msgBox.setIconPixmap(QPixmap(":/error_icon.png"));
        msgBox.exec();
    }
}


void MainWindow::setWindDirectionToTable(int rowIndex)
{
    if(rowIndex < ui->weatherTable->rowCount() && rowIndex >= 0)
    {
        QDialog* windDirectionDialog = createDialog("Choose wind drection", 230, 140);

        QComboBox* directionComboBox = new QComboBox(windDirectionDialog);

        directionComboBox->addItem("North", QVariant("N"));
        directionComboBox->addItem("South", QVariant("S"));
        directionComboBox->addItem("East", QVariant("E"));
        directionComboBox->addItem("West", QVariant("W"));
        directionComboBox->addItem("North-East", QVariant("NE"));
        directionComboBox->addItem("North-West", QVariant("NW"));
        directionComboBox->addItem("South-East", QVariant("SE"));
        directionComboBox->addItem("South-West", QVariant("SW"));

        QPushButton* setWindDirectionButton = new QPushButton("Set", windDirectionDialog);

        QVBoxLayout* layout = new QVBoxLayout(windDirectionDialog);
        layout->addWidget(directionComboBox);
        layout->addWidget(setWindDirectionButton);

        connect(setWindDirectionButton, &QPushButton::clicked, windDirectionDialog, &QDialog::accept);

        if(windDirectionDialog->exec() == QDialog::Accepted){
            setCellText(ui->weatherTable, rowIndex, 6, directionComboBox->currentData().toString());
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Error. Click on the row whose data you want to change.");
        msgBox.setIconPixmap(QPixmap(":/error_icon.png"));
        msgBox.exec();
    }
}


void MainWindow::deleteRow(int rowIndex)
{
    ui->weatherTable->removeRow(rowIndex);
    statusBar()->showMessage("Not all changes are saved )=");
}


void MainWindow::readWeatherLineFromFile(QTextStream &in)
{
    CWather weather;
    in >> weather;

    int rowIndex = ui->weatherTable->rowCount();
    ui->weatherTable->insertRow(rowIndex);

    setCellText(ui->weatherTable, rowIndex, 0, QString::number(weather.year()));
    setCellText(ui->weatherTable, rowIndex, 1, QString::number(static_cast<int>(weather.month())));
    setCellText(ui->weatherTable, rowIndex, 2, QString::number(weather.day()));
    setCellText(ui->weatherTable, rowIndex, 3, QString::number(weather.temperature()));
    setCellText(ui->weatherTable, rowIndex, 4, QString::number(weather.pressure()));
    setCellText(ui->weatherTable, rowIndex, 5, QString::number(weather.humidity()));
    setCellText(ui->weatherTable, rowIndex, 6, convertWindDirToText(weather.windDirection()));
}


void MainWindow::writeWeatherTableToFile(QTextStream &out)
{
    for (int i = 0; i < weatherArr.size(); ++i)
    {
        out << weatherArr[i];
        if(i != weatherArr.size() - 1){
            out << "\n";
        }
    }
}


std::vector<CWather> MainWindow::getWeatherArrByPeriod(bool& isSuccess)
{
    std::vector<CWather> choosedPeriodArr;
    QDialog* dialog = createDialog("Choose date", 250, 200);

    QDateEdit* startDateEdit = new QDateEdit(dialog);
    startDateEdit->setCalendarPopup(true);
    QDateEdit* endDateEdit = new QDateEdit(dialog);
    endDateEdit->setCalendarPopup(true);

    QPushButton* setPeriodButton = new QPushButton("Choose period", dialog);

    QVBoxLayout* layout = new QVBoxLayout(dialog);
    layout->addWidget(startDateEdit);
    layout->addWidget(endDateEdit);
    layout->addWidget(setPeriodButton);

    connect(setPeriodButton, &QPushButton::clicked, dialog, &QDialog::accept);

    if(dialog->exec() == QDialog::Accepted)
    {
        QDate selectedStartDate = startDateEdit->date();
        QDate selectedEndDate = endDateEdit->date();

        if(selectedStartDate > selectedEndDate)
        {
            QMessageBox msgBox;
            msgBox.setText("The start date cannot be greater than the end date!");
            msgBox.setIconPixmap(QPixmap(":/error_icon.png"));
            msgBox.exec();

            isSuccess = false;
            return choosedPeriodArr;
        }

        for (int i = 0; i < weatherArr.size(); ++i) {
            QDate currentTableDate = QDate(weatherArr[i].year(), weatherArr[i].month(), weatherArr[i].day());
            if(currentTableDate >= selectedStartDate && currentTableDate <= selectedEndDate)
            {
                choosedPeriodArr.push_back(weatherArr[i]);
            }
        }

        if(choosedPeriodArr.size() == 0)
        {
            QMessageBox msgBox;
            msgBox.setText("We have no information about this period. Try adding data for this period and refreshing the table.");
            msgBox.setIconPixmap(QPixmap(":/error_icon.png"));
            msgBox.exec();

            isSuccess = false;
            return choosedPeriodArr;
        }
    }

    isSuccess = true;
    return choosedPeriodArr;
}


void MainWindow::on_actionSort_by_pressure_within_seasons_triggered()
{
    if(statusBar()->currentMessage() == "Not all changes are saved )=")
    {
        QMessageBox msgBox;
        msgBox.setText("You did not save all the changes you made. Do you want to continue?");
        msgBox.setIconPixmap(QPixmap(":/warning.png"));
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);

        if (msgBox.exec() == QMessageBox::Cancel){
            return;
        }
    }

    QMessageBox msgBox;
    msgBox.setText("This function can «ruin» a table because it changes the order of its elements. Do you really want to continue?");
    msgBox.setIconPixmap(QPixmap(":/warning.png"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);

    if (msgBox.exec() == QMessageBox::Cancel){
        return;
    }

    sortPressureBySeason(weatherArr);
    completeTable(weatherArr, ui->weatherTable);
}


void MainWindow::on_actionAdd_row_triggered()
{
    ui->weatherTable->insertRow(ui->weatherTable->rowCount());
}


void MainWindow::on_actionSave_changes_triggered()
{
    if(isTableCompletelyFilled()){
        weatherArr.clear();
        for(int i = 0; i < ui->weatherTable->rowCount(); ++i)
        {
            int year = ui->weatherTable->item(i, 0)->text().toInt();
            Month month = static_cast<Month>(ui->weatherTable->item(i, 1)->text().toInt());
            unsigned day = ui->weatherTable->item(i, 2)->text().toUInt();
            int t = ui->weatherTable->item(i, 3)->text().toInt();
            unsigned pressure = ui->weatherTable->item(i, 4)->text().toUInt();
            int humidity = ui->weatherTable->item(i, 5)->text().toInt();
            WindDirection windDirection = convertTextToWindDir(ui->weatherTable->item(i, 6)->text());

            weatherArr.push_back(CWather(year, month, day, t, pressure, humidity, windDirection));
        }

        statusBar()->showMessage("All changes have been saved (=");
    }
}


void MainWindow::on_actionOpen_triggered()
{
    QMessageBox msgBox;
    msgBox.setText("When you click the OK button, all previous data will be erased and the table will be filled from the file.");
    msgBox.setIconPixmap(QPixmap(":/warning.png"));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);

    if (msgBox.exec() == QMessageBox::Cancel){
        return;
    }

    ui->weatherTable->setRowCount(0);
    weatherArr.clear();

    QString fileName = QFileDialog::getOpenFileName(this, "Select a file", "/Users/artomrevus/Desktop", "Text file (*.txt)");

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        while (!in.atEnd())
        {
            readWeatherLineFromFile(in);
        }
        file.close(); // Закрити файл після зчитування
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("File could not be opened.");
        msgBox.setIconPixmap(QPixmap(":/error_icon.png"));
        msgBox.exec();
    }
}


void MainWindow::on_actionSave_triggered()
{
    if(statusBar()->currentMessage() == "Not all changes are saved )=")
    {
        QMessageBox msgBox;
        msgBox.setText("You did not save all the changes you made. Do you really want to save the last changes you made to a file?");
        msgBox.setIconPixmap(QPixmap(":/warning.png"));
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);

        if (msgBox.exec() == QMessageBox::Cancel){
            return;
        }
    }

    QString fileName = QFileDialog::getOpenFileName(this, "Select a file", "/Users/artomrevus/Desktop", "Text file (*.txt)");

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        writeWeatherTableToFile(out);
        file.close();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("The data has not been recorded!");
        msgBox.setIconPixmap(QPixmap(":/error_icon.png"));
        msgBox.exec();
    }
}


void MainWindow::setCellText(QTableWidget *table, const int rowIndex, const int columnIndex, const QString& text)
{
    table->setItem(rowIndex, columnIndex, new QTableWidgetItem(text));
}


QDialog *MainWindow::createDialog(const QString& windowTitle, const int& width, const int& height)
{
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle(windowTitle);
    dialog->setFixedSize(width, height);
    return dialog;
}


QSlider *MainWindow::createSlider(const Qt::Orientation& orientation, QWidget* parent, const int& min, const int& max,
                                  const int& initValue, const int& interval)
{
    QSlider* slider = new QSlider(orientation, parent);
    slider->setRange(min, max);
    slider->setValue(initValue);
    slider->setTickInterval(interval);
    return slider;
}


void MainWindow::on_actionBuild_graph_of_t_2_triggered()
{
    buildWeatherGraph(weatherArr, [this](int i){
        return weatherArr[i].temperature();
    }, "Temperature graph");
}


void MainWindow::on_actionBuild_graph_of_pressure_2_triggered()
{
    buildWeatherGraph(weatherArr, [this](int i){
            return weatherArr[i].pressure();
        }, "Pressure graph");
}


void MainWindow::on_actionBuild_graph_of_humidity_2_triggered()
{
    buildWeatherGraph(weatherArr, [this](int i){
            return weatherArr[i].humidity();
        }, "Humidity graph");
}


void MainWindow::on_actionFind_days_while_triggered()
{
    if(statusBar()->currentMessage() == "Not all changes are saved )=")
    {
        QMessageBox msgBox;
        msgBox.setText("Save your table before performing this action.");
        msgBox.setIconPixmap(QPixmap(":/error_icon.png"));
        msgBox.exec();
        return;
    }

    std::vector<std::vector<unsigned>> windNotChangeArr = findDaysWindNotChange(weatherArr);

    for (int i = 0; i < windNotChangeArr.size(); ++i) {
        QColor randomColor(QRandomGenerator::global()->bounded(256), QRandomGenerator::global()->bounded(256), QRandomGenerator::global()->bounded(256));
        for (int j = 0; j < windNotChangeArr[i].size(); ++j) {
            for (int colIndex = 0; colIndex < COLUMN_CNT; ++colIndex) {
                ui->weatherTable->item(windNotChangeArr[i][j], colIndex)->setBackground(randomColor);
            }
        }
    }

    QMessageBox msgBox;
    msgBox.setText("The days during which the wind direction did not change are marked with a common colour (different from white).");
    msgBox.setIconPixmap(QPixmap(":/data_analysis.png"));
    msgBox.exec();
}


void MainWindow::on_actionDetermine_the_avg_temperature_triggered()
{
    if(statusBar()->currentMessage() == "Not all changes are saved )=")
    {
        QMessageBox msgBox;
        msgBox.setText("You did not save all the changes you made. Do you want to continue searching for the average temperature?");
        msgBox.setIconPixmap(QPixmap(":/warning.png"));
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);

        if (msgBox.exec() == QMessageBox::Cancel){
            return;
        }
    }

    bool isSuccess;
    std::vector<CWather> findAvgTArr = getWeatherArrByPeriod(isSuccess);

    if(isSuccess)
    {
        double avgTemperature = getAvgTemperature(findAvgTArr);
        QMessageBox msgBox;
        msgBox.setText("Avg temperature = " + QString::number(avgTemperature) + "°C");
        msgBox.setIconPixmap(QPixmap(":/data_analysis.png"));
        msgBox.exec();
    }
}


void MainWindow::on_actionDetermine_highest_humidity_days_triggered()
{
    if(statusBar()->currentMessage() == "Not all changes are saved )=")
    {
        QMessageBox msgBox;
        msgBox.setText("You did not save all the changes you made. Do you want to continue searching for highest humidity days?");
        msgBox.setIconPixmap(QPixmap(":/warning.png"));
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);

        if (msgBox.exec() == QMessageBox::Cancel){
            return;
        }
    }

    bool isSuccess;
    std::vector<CWather> findHighestHumArr = getWeatherArrByPeriod(isSuccess);

    if(isSuccess)
    {
        std::vector<QDate> highestHumDaysArr = getHighestHumidityDays(findHighestHumArr);
        QString HHDaysQStr = "Highest humidity days: \n";
        for (int i = 0; i < highestHumDaysArr.size(); ++i) {
            QDate date(highestHumDaysArr[i].year(), highestHumDaysArr[i].month(), highestHumDaysArr[i].day());
            HHDaysQStr += date.toString("dd.MM.yyyy");
            HHDaysQStr += "\n";
        }

        QMessageBox msgBox;
        msgBox.setText(HHDaysQStr);
        msgBox.setIconPixmap(QPixmap(":/data_analysis.png"));
        msgBox.exec();
    }
}


void MainWindow::displayWeatherPeriods(std::vector<std::vector<CWather>> periodsArr)
{
    QDialog dialog;
    dialog.setWindowTitle("Periods (3 and more days) when the pressure varied within ±2.5% and t ±3.6%.");
    dialog.setFixedSize(760, 400);

    QScrollArea scrollArea;

    QTabWidget tabWidget;
    tabWidget.setMinimumWidth(720);
    tabWidget.setFixedHeight(350);

    for (int i = 0; i < periodsArr.size(); ++i) {
        QTableWidget* periodtable = createWeatherTable(periodsArr[i]);
        tabWidget.addTab(periodtable, "Period " + QString::number(i + 1));
    }

    QTabBar* tabBar = tabWidget.findChild<QTabBar*>();
    tabBar->setElideMode(Qt::ElideNone);
    tabBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    scrollArea.setWidget(&tabWidget); // Встановлюємо QTabWidget як вміст QScrollArea

    QVBoxLayout layout(&dialog);
    layout.addWidget(&scrollArea); // Додаємо QScrollArea до розміщення

    dialog.exec();
}


QTableWidget* MainWindow::createWeatherTable(std::vector<CWather> weatherArr)
{
    QTableWidget* weatherTable = new QTableWidget();
    weatherTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    weatherTable->setRowCount(weatherArr.size());

    weatherTable->setColumnCount(7);
    QStringList columnNames;
    columnNames << "Year" << "Month" << "Day" << "t" << "Pressure" << "Humidity" << "Wind direction";
    weatherTable->setHorizontalHeaderLabels(columnNames);

    completeTable(weatherArr, weatherTable);

    return weatherTable;
}


void MainWindow::on_actionFind_days_while_pressure_2_5_triggered()
{
    if(statusBar()->currentMessage() == "Not all changes are saved )=")
    {
        QMessageBox msgBox;
        msgBox.setText("You did not save all the changes you made. Do you want to continue searching for periods "
                       "when the pressure varied within ±2.5% and t varied within 3.6%?");
        msgBox.setIconPixmap(QPixmap(":/warning.png"));
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);

        if (msgBox.exec() == QMessageBox::Cancel){
            return;
        }
    }

    std::vector<std::vector<CWather>> periodsArr = findPeriodTemperatureAndPressureChangeWithinRange(weatherArr, 3.6, 2.5);

    if(periodsArr.size() == 0){
        QMessageBox msgBox;
        msgBox.setText("In your table, there are no periods when the pressure varied within ±2.5% and t varied within 3.6%.");
        msgBox.setIconPixmap(QPixmap(":/data_analysis.png"));
        msgBox.exec();
    }

    displayWeatherPeriods(periodsArr);
}

