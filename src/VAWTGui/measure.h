#ifndef MEASURE_H
#define MEASURE_H

#include <QMainWindow>
#include <QErrorMessage>
#include <QMessageBox>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "mainwindow.h"
#include "wiringPi.h"
#include <time.h>
#include <sys/time.h>
#include "modbus/modbus.h"
#include <signal.h>
#include <sched.h>
#include <errno.h>
#include <thread>
#define MY_PRIORITY (35)


static volatile long int counter; //RPM Counter
static uint16_t reg[160]; //Maynuo DC Load Register
static modbus_t *ctx; //Modbus Connection Handler
static struct timeval start, diff, end;


class Measure : public QObject
{
Q_OBJECT

public:
    Measure(Ui::MainWindow *mw, QString path);
    void StartMeasurement();
    bool checkParams();
public slots:
    int withoutLoad(Ui::MainWindow *mw, std::ofstream &f);
private:
    Ui::MainWindow *mainwindow;
    bool isAnemoEnabled;
    bool isMayumoEnabled;
    bool isLoggingEnabled;
    struct sched_param param;
    QString Path;
    void rtsched();
    int openLoad(const char *c);
    void piSetup();
    int readLoadRegister(int addr);
    static void countMaxonInterrupts ();
    void openFile(std::ofstream &f);
    void writeHeader(const char *c, std::ofstream &f);
};
#endif // MEASURE_H
