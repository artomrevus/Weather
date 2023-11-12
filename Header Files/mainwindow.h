#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cwather.h"
#include "WeatherEnums.h"
#include <QMessageBox>
#include <QDateEdit>
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


/// Number of columns in the weather table.
static int COLUMN_CNT = 7;


// -------------------------------------------------------------------------------------------------------------------------


/// A class for configuring and adding features to the main application window.
class MainWindow : public QMainWindow
{
    Q_OBJECT

// -------------------------------------------------------------------------------------------------------------------------


public:

// (Public) Constructors & destructor section:


    /** @brief Constructor.
     *
     * Initializes the main window, sets up the user interface, and connects signals for handling changes and context menu requests.
     *
     * @param parent - A pointer to the parent widget (default is nullptr).
     */
    MainWindow(QWidget *parent = nullptr);


    /// Default destructor
    ~MainWindow();


// -------------------------------------------------------------------------------------------------------------------------


// (Public) Methods section:


    /** @brief Display a warning message to the user.
     *
     * This method shows a warning message dialog to the user with the specified 'warningMessage'.
     *
     * @param warningMessage - a QString containing the warning message to be displayed.
     *
     * @return False if the user press Cancel, True otherwise.
     */
    bool showWarningMessage(const QString& warningMessage);


    /** @brief Display a error message to the user.
     *
     * This method shows a error message dialog to the user with the specified 'errorMessage'.
     *
     * @param errorMessage - a QString containing the error message to be displayed.
     */
    void showErrorMessage(const QString& errorMessage);


    /** @brief Display a data message (information) to the user.
     *
     * This method shows a data message dialog to the user with the specified 'dataMessage'.
     *
     * @param dataMessage - a QString containing the data message to be displayed.
     */
    void showOutputDataMessage(const QString& dataMessage);


    /** @brief Check if weather table completely filled.
     *
     * This method checks if all cells in table are not empty.
     *
     * @return False if weather table has empty cells, True otherwise.
     */
    bool isTableCompletelyFilled();


    /** This method creates a context menu for the weather table to change the row data such as date, t, pressure,
    humidity, wind direction.
     *
     * @param pos - Indicates the position where this context menu was called. This position is used to determine for
    which table row the context menu is called.
     */
    void createTableContextMenu(const QPoint& pos);


    /** Creates a dialogue box where you can set the year, month, and day. This data is then set to the weather table.
     *
     * @param rowIndex - Determines in which row of the weather table the date data is added.
     */
    void setDateToTable(int rowIndex);


    /** Creates a dialogue box where you can set the temperature. This data is then set to the weather table.
     *
     * @param rowIndex - Determines in which row of the weather table the temperature data is added.
     */
    void setTemperatureToTable(int rowIndex);


    /** Creates a dialogue box where you can set the pressure. This data is then set to the weather table.
     *
     * @param rowIndex - Determines in which row of the weather table the pressure data is added.
     */
    void setPressureToTable(int rowIndex);


    /** Creates a dialogue box where you can set the humidity. This data is then set to the weather table.
     *
     * @param rowIndex - Determines in which row of the weather table the humidity data is added.
     */
    void setHumidityToTable(int rowIndex);


    /** Creates a dialogue box where you can set the wind direction. This data is then set to the weather table.
     *
     * @param rowIndex - Determines in which row of the weather table the wind direction data is added.
     */
    void setWindDirectionToTable(int rowIndex);


    /** Deletes a row from the table.
     *
     * @param rowIndex - The index of the row to be deleted.
     */
    void deleteRow(int rowIndex);


    /** From the available weather information, it determines the weather for a certain period selected by the user.
     *
     * @param isSuccess - The variable to which the method is written if it was successful.
     *
     * @return Weather (CWather) for the period selected by the user.
     */
    CWather getWeatherPeriod(bool& isSuccess);


    /** For each period (passed as a method parameter), it creates a weather table and displays its data.
     *
     * @param - A vector of periods to be displayed.
     */
    void displayWeatherPeriods(std::vector<CWather> periodsArr);


    /** This method is used to create and configure a QTableWidget (table).
     *
     * @param weather - Weather data from which the table will be filled in.
     *
     * @return A pointer to the created table.
     */
    QTableWidget* createWeatherTable(CWather weather);


// -------------------------------------------------------------------------------------------------------------------------


private slots:

// (Private) Slots section:


    /// Within a season (3 months), sort the records by the Pressure using a simple selection method and update the main weather table.
    void on_actionSort_by_pressure_within_seasons_triggered();

    /// Adds a blank row to the end of the main weather table.
    void on_actionAdd_row_triggered();

    /// Saves all changes made to the table and writes to the main weather object.
    void on_actionSave_changes_triggered();

    /// Opens a file and reads data from it into a table and main weather object.
    void on_actionOpen_triggered();

    /// Opens a file and writes data to it from main weather object.
    void on_actionSave_triggered();

    /// Plots a graph in a new dialogue window with temperature data on the y-axis and dates on the x-axis.
    void on_actionBuild_graph_of_t_2_triggered();

    /// Plots a graph in a new dialogue window with pressure data on the y-axis and dates on the x-axis.
    void on_actionBuild_graph_of_pressure_2_triggered();

    /// Plots a graph in a new dialogue window with humidity data on the y-axis and dates on the x-axis.
    void on_actionBuild_graph_of_humidity_2_triggered();

    /// Identify the days during which the wind direction did not change and mark them with a unique colour in the weather table.
    void on_actionFind_days_while_triggered();

    /// According to the period specified by the user, it determines the average temperature during this period.
    void on_actionDetermine_the_avg_temperature_triggered();

    /// According to the period specified by the user, it determines the days with the highest humidity.
    void on_actionDetermine_highest_humidity_days_triggered();

    /// Find the periods in which the pressure varied within ±2.5% and t - 3.6% and display them in the form of tables.
    void on_actionFind_days_while_pressure_2_5_triggered();

    /// Predict the weather for the next month, taking into account the change of season, and write it at the end of the table.
    void on_actionForecast_weathe_for_next_month_triggered();


// -------------------------------------------------------------------------------------------------------------------------


private:

// (Private) class field:


    /// Indicates the user interface for the main window of the application. Used to interact with the graphical user interface.
    Ui::MainWindow *ui;

    /// A weather class object for basic work with weather data.
    CWather mainWeather;


// -------------------------------------------------------------------------------------------------------------------------


// (Private) methods section:


    /** Sets text to a table cell
     *
     * @param table - A pointer to the table where you want to set the text in the cell.
     * @param rowIndex - The index of the table row where the cell where you want to change the text is located.
     * @param columnIndex - The index of the table column where the cell where you want to change the text is located.
     * @param text - The text you want to insert into the cell
     */
    void setCellText(QTableWidget* table, const int& rowIndex, const int& columnIndex, const QString& text);


    /** This method is used to create and configure a QDialog (dialogue window).
     *
     * @param windowTitle - The title of the dialog window that will be created.
     * @param width - The width of the dialog box that will be created.
     * @param height - The height of the dialog box that will be created.
     *
     * @return A pointer to the created dialog window.
     */
    QDialog* createDialog(const QString& windowTitle, const int& width, const int& height);


    /** This method is used to create and configure a QSlider (slider).
     *
     * @param orientation - The orientation of the slider. You can specify Qt::Horizontal (horizontal slider), or
    Qt::Vertical (vertical slider).
     * @param parent - The parent widget to which the created slider will be added.
     * @param min - The minimum value for the slider.
     * @param max - The maximum value for the slider.
     * @param initValue - The initial value of the slider, which is set when it is created.
     * @param interval - The interval (step) between slider values.
     *
     * @return A pointer to the created slider.
     */
    QSlider* createSlider(const Qt::Orientation& orientation, QWidget* parent, const int& min, const int& max,
                          const int& initValue,  const int& interval);


// -------------------------------------------------------------------------------------------------------------------------

};

// -------------------------------------------------------------------------------------------------------------------------

#endif // MAINWINDOW_H
