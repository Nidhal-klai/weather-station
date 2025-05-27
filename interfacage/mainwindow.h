#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDateTime>
#include <QByteArray>
#include <QStandardItemModel>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QSqlDatabase>

#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

namespace QtCharts {
    class QChart;
    class QLineSeries;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool OpenStorage(const QString &id = "readings");
    QString id() const;

private slots:
    void updateDisplay();
    void on_pushButton_connect_clicked();
    void on_pushButton_disconnect_clicked();
    void readSerialData();
    void temperatureUpdate(QDateTime timestamp, QByteArray data);
    void loadStoredData();
    void delet();


private:
    Ui::MainWindow *ui;
    QTimer _time;
    QSerialPort *serial;
    QByteArray data;

    QStandardItemModel _tempRecord;
    QSqlDatabase m_db;
    QtCharts::QChartView *chartView;
    QtCharts::QChartView *widget;

    QtCharts::QChart *chart;
    QtCharts::QLineSeries *seriesT;
    QtCharts::QLineSeries *seriesH;
};

#endif // MAINWINDOW_H
