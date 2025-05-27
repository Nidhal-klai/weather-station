#include "mainwindow.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QStandardItemModel>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include<QSqlDatabase>
#include<QSqlQuery>
#include<QStringList>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
using namespace QtCharts;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{    ui->setupUi(this);


    connect(&_time, &QTimer::timeout, this, &MainWindow::updateDisplay);
   _time.start(1000);

   serial = new QSerialPort(this);
     connect(serial, &QSerialPort::readyRead, this, &MainWindow::readSerialData);
     //find available ports.
     foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
         ui->comboBox->addItem(serialPortInfo.portName());
     }
     connect(ui->b1,&QPushButton::clicked,this,&MainWindow::on_pushButton_connect_clicked);
     connect(ui->b2,&QPushButton::clicked,this,&MainWindow::on_pushButton_disconnect_clicked);
      connect(ui->b4,&QPushButton::clicked,this,&MainWindow::delet);
       connect(ui->b3,&QPushButton::clicked,[=](){
           if (OpenStorage(id())) { loadStoredData();
                }
       });
      ui->tableView->setModel(&_tempRecord);

       chart = new QChart();
       seriesT = new QLineSeries();
       seriesH = new QLineSeries();
       chart->addSeries(seriesT);
       chart->addSeries(seriesH);
         // Chart setup

        QDateTimeAxis *axisX = new QDateTimeAxis;
        axisX->setFormat("hh:mm:ss");
        axisX->setTitleText("Time");
        chart->addAxis(axisX, Qt::AlignBottom);
        seriesT->attachAxis(axisX);
        seriesH->attachAxis(axisX);
        QValueAxis *axisY = new QValueAxis;
        axisY->setTitleText("Value");
        chart->addAxis(axisY, Qt::AlignLeft);
        seriesT->attachAxis(axisY);
        seriesH->attachAxis(axisY);

         chartView = new QChartView(chart);
         chartView->setRenderHint(QPainter::Antialiasing);

         // Add chart view to the layout
         QVBoxLayout *layout = new QVBoxLayout(ui->widget);
         layout->addWidget(chartView);
         ui->widget->setLayout(layout);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateDisplay()
{
    QDateTime   now = QDateTime::currentDateTime();
     ui->label->setText(now.toString());
}
void MainWindow::on_pushButton_connect_clicked()
{
    serial->setPortName(ui->comboBox->currentText());
    serial->setBaudRate(QSerialPort::Baud115200);
    if (serial->open(QIODevice::ReadOnly)) {
        qDebug() << "Serial port opened successfully.";
   } else {
        qDebug() << "Failed to open serial port:" << serial->errorString();}
}

void MainWindow::readSerialData()
{data = serial->readAll();
 QDateTime now = QDateTime::currentDateTime();
 temperatureUpdate(now, data);}

void MainWindow::temperatureUpdate(QDateTime timestamp, QByteArray data)
{ if (!OpenStorage(id())) { OpenStorage();
    }
    _tempRecord.setColumnCount(5);
  _tempRecord.setHorizontalHeaderLabels(QStringList() << "Timestamp" << "Temperature"<<"humidite"<<"luminisote"<<"pression");


   QString msg = QString(data).trimmed();
   QStringList parts = msg.split(";");

   QString temperatureStr = parts[0];
   QString humidityStr = parts[1];
   QString lumStr=parts[2];
   QString pressionStr=parts[3];

   ui->lcd_H->display(humidityStr);
   ui->lcd_T->display(temperatureStr);
   ui->lcd_lum->display(lumStr);
   ui->lcd_pres->display(pressionStr);

   QSqlQuery query(m_db);
   query.prepare("INSERT INTO temperature_data (timestamp, temperature,humidity,luminosite,pression) VALUES (:timestamp, :temperature,:humidity,:luminosite,:pression)");
   query.bindValue(":timestamp", timestamp.toString(Qt::ISODate));
   query.bindValue(":temperature",temperatureStr );
   query.bindValue(":humidity", humidityStr);
   query.bindValue(":luminosite", lumStr);
   query.bindValue(":pression", pressionStr);


       if (!query.exec()) {
           qDebug() << " Failed to insert data:" ;
       } else {
           qDebug() << "Data inserted:" << timestamp.toString() <<  temperatureStr << humidityStr;
       }



// create a row with the timestamp and temperature.
  auto time = new QStandardItem;
   auto temp = new QStandardItem;
   auto hum = new QStandardItem;
   auto lum = new QStandardItem;
   auto pres = new QStandardItem;


// set the displayable data (Qt::DisplayRole)
    time->setData(timestamp, Qt::DisplayRole);
    temp->setData(temperatureStr, Qt::DisplayRole);
    hum->setData(humidityStr, Qt::DisplayRole);
    lum->setData(lumStr, Qt::DisplayRole);
    pres->setData(pressionStr, Qt::DisplayRole);
    // create the row
    QList<QStandardItem *> row;
    row << time << temp<<hum<<lum<<pres;
// add the row to the model
    _tempRecord.appendRow(row);


   }


void MainWindow::on_pushButton_disconnect_clicked()
{
    if (serial->isOpen()) {
        serial->close();
        qDebug() << "Serial port closed.";
    }}

bool MainWindow::OpenStorage(const QString &id)
{
    QString dbName = QString("ReadingStore_%1").arg(id);
    QString dbFileName = QString("%1.sqlite").arg(dbName);

    // Check if a connection with this name already exists
    if (QSqlDatabase::contains(dbName)) {
        m_db = QSqlDatabase::database(dbName);
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE", dbName);
        m_db.setDatabaseName(dbFileName);
    }

    if (!m_db.open()) {
        qDebug() << "Failed to open database:" ;
        return false;
    }

    // Create table if it doesn't exist
    QSqlQuery query(m_db);
    QString createTable = R"(
        CREATE TABLE IF NOT EXISTS temperature_data (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp TEXT NOT NULL,
            temperature TEXT NOT NULL,
            humidity TEXT NOT NULL,
            luminosite TEXT NOT NULL,
            pression  TEXT NOT NULL
        )
    )";

    if (!query.exec(createTable)) {
        qDebug() << "Failed to create table:" ;
        return false;
    }

    qDebug() << "Database opened and table ready:" << dbFileName;
}

QString MainWindow::id() const
{
    return("readings");
}
void MainWindow::loadStoredData()
{
    QSqlQuery query("SELECT timestamp, temperature, humidity,luminosite,pression FROM temperature_data ORDER BY timestamp ASC", m_db);
    seriesT->clear();
    seriesH->clear();
    _tempRecord.clear(); // Clear the existing data in the table view
    _tempRecord.setColumnCount(5);
    _tempRecord.setHorizontalHeaderLabels(QStringList() << "Timestamp" << "Temperature"<<"humidite"<<"luminisote"<<"pression");

    while (query.next()) {
        QString timestampStr = query.value(0).toString();
        QString temperatureStr = query.value(1).toString();
        QString humidityStr = query.value(2).toString();
         QString lumStr = query.value(3).toString();
         QString pressionStr = query.value(4).toString();
        QDateTime timestamp = QDateTime::fromString(timestampStr, Qt::ISODate);

        double temperature = temperatureStr.toDouble();
        double humidity = humidityStr.toDouble();


         seriesT->append(timestamp.toMSecsSinceEpoch(), temperature);
         seriesH->append(timestamp.toMSecsSinceEpoch(), humidity);

        auto timeItem = new QStandardItem(timestampStr);
        auto tempItem = new QStandardItem(temperatureStr);
        auto humItem  = new QStandardItem(humidityStr);
        auto lumItem  = new QStandardItem(lumStr);
        auto pressItem  = new QStandardItem(pressionStr);

        QList<QStandardItem*> row;
        row << timeItem << tempItem << humItem<<lumItem<<pressItem;
        _tempRecord.appendRow(row);
    }

    // Update chart axes after loading data


        QDateTime firstTimestamp = QDateTime::fromMSecsSinceEpoch(seriesT->at(0).x());
        QDateTime lastTimestamp = QDateTime::fromMSecsSinceEpoch(seriesT->at(seriesT->count() - 1).x());
        chart->axes(Qt::Horizontal).first()->setRange(firstTimestamp, lastTimestamp);



        double minT = std::numeric_limits<double>::max();
        double maxT = std::numeric_limits<double>::lowest();
        double minH = std::numeric_limits<double>::max();
        double maxH = std::numeric_limits<double>::lowest();
        auto hAxes = chart->axes(Qt::Horizontal);
        auto vAxes = chart->axes(Qt::Vertical);


        for (const QPointF& point : seriesT->points()) {
            minT = qMin(minT, point.y());
            maxT = qMax(maxT, point.y());
        }
        for (const QPointF& point : seriesH->points()) {
            minH = qMin(minH, point.y());
            maxH = qMax(maxH, point.y());
        }

        chart->axes(Qt::Vertical).first()->setRange(qMin(minT, minH), qMax(maxT, maxH));


    qDebug() << " Loaded data from database into the table view and chart.";
}
void MainWindow::delet(){
    QSqlQuery query(m_db);
        query.exec("DELETE FROM temperature_data");

            // Also clear the table view model and the chart
            _tempRecord.clear();
            seriesT->clear();
            seriesH->clear();
            chart->axes(Qt::Horizontal).first()->setRange(0, 1);
            chart->axes(Qt::Vertical).first()->setRange(0, 1);
        }
