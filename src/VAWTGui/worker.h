#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QThread>
#include <QMessageBox>
#include <fstream>
#include <sstream>
#include <iostream>
#include "wiringPi.h"
#include <time.h>
#include <sys/time.h>
#include "modbus/modbus.h"
#include <sched.h>
#define MY_PRIORITY (35)



class Worker : public QObject {

    Q_OBJECT
public:
    explicit Worker(QObject *parent, std::ofstream &f, int dela);

public slots:
    void startWork();

signals:
    void resultReady(const QString &result);
    void execFinished();

private:
    std::ofstream &file;
    float del;
    void rtsched();
    int openLoad(const char *c);
    void piSetup();
    int readLoadRegister(int addr);
    static void countMaxonInterrupts ();
    struct sched_param param;
    int measureLoop();
};

#endif // Worker_H
