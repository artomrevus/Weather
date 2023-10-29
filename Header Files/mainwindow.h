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

static int COLUMN_CNT = 7;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool showWarningMessage(const QString& warningMessage);
    void showErrorMessage(const QString& errorMessage);
    void showOutputDataMessage(const QString& dataMessage);
    bool isTableCompletelyFilled();
    void createTableContextMenu(const QPoint& pos);
    void setDateToTable(int rowIndex);
    void setTemperatureToTable(int rowIndex);
    void setPressureToTable(int rowIndex);
    void setHumidityToTable(int rowIndex);
    void setWindDirectionToTable(int rowIndex);
    void deleteRow(int rowIndex);
    CWather getWeatherPeriod(bool& isSuccess);
    void displayWeatherPeriods(std::vector<CWather> periodsArr);
    QTableWidget* createWeatherTable(CWather weather);



private slots:
    void on_actionSort_by_pressure_within_seasons_triggered();

    void on_actionAdd_row_triggered();

    void on_actionSave_changes_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionBuild_graph_of_t_2_triggered();

    void on_actionBuild_graph_of_pressure_2_triggered();

    void on_actionBuild_graph_of_humidity_2_triggered();

    void on_actionFind_days_while_triggered();

    void on_actionDetermine_the_avg_temperature_triggered();

    void on_actionDetermine_highest_humidity_days_triggered();

    void on_actionFind_days_while_pressure_2_5_triggered();

    void on_actionForecast_weathe_for_next_month_triggered();

private:
    Ui::MainWindow *ui;
    CWather mainWeather;

    void setCellText(QTableWidget* table, const int rowIndex, const int columnIndex, const QString& text);
    QDialog* createDialog(const QString& windowTitle, const int& width, const int& height);
    QSlider* createSlider(const Qt::Orientation& orientation, QWidget* parent, const int& min, const int& max,
                          const int& initValue,  const int& interval);
};
#endif // MAINWINDOW_H
