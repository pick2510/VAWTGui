#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QThread>
#include <QMessageBox>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include "wiringPi.h"
#include "wiringPiSPI.h"
#include <time.h>
#include <sys/time.h>
#include "modbus/modbus.h"
#include <sched.h>
#define MY_PRIORITY (35)



class Worker : public QObject {

    Q_OBJECT
public:
    explicit Worker(std::ofstream &f, int dela, QString mayumopath,
                    bool isTorqueEnabled,
                    QObject *parent=0);

public slots:
    void rtsched();
    float convfl(uint16_t *tab, int idx);
    int openLoad(const char *c);
    void piSetup();
    int readLoadRegister(int addr);
    void startWork();

signals:
    void resultReady(const QString &result);
    void execFinished();
    void sigFailure();

private:
    QString mayumoPath;
    std::ofstream &file;
    bool torqueEnabled;
    float del;
    static void countMaxonInterrupts ();
    struct sched_param param;
    int measureLoopWithoutLoadWithoutTorque();
    int measureLoopWithLoadWithoutTorque();
    int measureLoopWithoutLoadWithTorque();
    int measureLoopWithLoadWithTorque();
};

#endif // Worker_H
