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
#include "libvawt.h"
#include <fcntl.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define MY_PRIORITY (90)

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))



class Worker : public QObject {

    Q_OBJECT
public:
    explicit Worker(std::ofstream &f, int dela, QString mayumopath,
                    bool isTorqueEnabled, int fd, struct SPICONF spi,
                    QObject *parent=0);

public slots:
    void rtsched();
    float convfl(uint16_t *tab, int idx);
    int openLoad(const char *c);
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
    struct spi_ioc_transfer spi_trans;
    struct SPICONF MYSPICONF;
    char tx[3];
    char rx[3];
    void piSetup();
    float del;
    int fd;
    static void countMaxonInterrupts ();
    struct sched_param param;
    inline void querySPI();
    int measureLoopWithoutLoadWithoutTorque();
    int measureLoopWithLoadWithoutTorque();
    int measureLoopWithoutLoadWithTorque();
    int measureLoopWithLoadWithTorque();
};

#endif // Worker_H
