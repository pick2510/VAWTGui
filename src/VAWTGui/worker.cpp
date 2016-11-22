#include "worker.h"

static volatile long int counter; //RPM Counter
static uint16_t reg[160]; //Maynuo DC Load Register
static modbus_t *ctx; //Modbus Connection Handler
static struct timeval start, diff, end;
const static double MAXONMULTI=500;

/**
 * @brief Worker::Worker: Constructor. Initalizes several parameters.
 * @param f
 * @param dela
 * @param mayumopath
 * @param isTorqueEnabled
 * @param fd
 * @param spi
 * @param parent
 */

Worker::Worker(std::ofstream &f, int dela, QString mayumopath,
               bool isTorqueEnabled, int fd, struct SPICONF spi, QObject *parent)  : file(f),
    torqueEnabled(isTorqueEnabled), mayumoPath(mayumopath), QObject(parent)
{
    struct SPICONF MYSPICONF = spi;
    del=float(dela);
    file.flush();
    this->fd=fd;
    piSetup();
    tx[0] = 6+((4& MYSPICONF.SPI_CHANNEL)>>2);
    tx[1] = (3 & MYSPICONF.SPI_CHANNEL)<<6;
    tx[2] = 0;
    struct spi_ioc_transfer spi_trans;
}

/**
 * @brief Worker::startWork: Selects appropriate measurement loop and starts it.
 */

void Worker::startWork()
{

    if ((mayumoPath.isEmpty())
            && (torqueEnabled==false)) measureLoopWithoutLoadWithoutTorque();
    if ((!mayumoPath.isEmpty())
            && (torqueEnabled==false)) measureLoopWithLoadWithoutTorque();
    if ((mayumoPath.isEmpty())
            && (torqueEnabled==true)) measureLoopWithoutLoadWithTorque();
    if ((!mayumoPath.isEmpty())
            && (torqueEnabled==true)) measureLoopWithLoadWithTorque();
    emit(execFinished());
}

/**
 * @brief Worker::piSetup: Sets pin for measurement. Defines interrupthandler
 */

void Worker::piSetup()
{
    pinMode(2, INPUT);
    pullUpDnControl(2, PUD_UP);
    wiringPiISR(2,INT_EDGE_FALLING, &countMaxonInterrupts);
}

/**
 * @brief Worker::rtsched: Activates realtime Scheduling.
 */

void Worker::rtsched()
{
    param.sched_priority = MY_PRIORITY;
    if (sched_setscheduler(0, SCHED_FIFO, &param)== -1) {
        exit(-1);
    }
}


/**
 * @brief Worker::openLoad: Opens ttyUSB for reading values.
 * @param c
 * @return
 */

int Worker::openLoad(const char *c)
{
    ctx=modbus_new_rtu(c,115200,'N',8,1);
    if (modbus_connect(ctx) == -1) {
        QMessageBox msg;
        QString dev(c);
        msg.setWindowTitle("Error!");
        msg.setText("Couldn't open Mayumo Load at " + dev);
        msg.exec();
        modbus_free(ctx);
        return -1;
    } else return 1;
}


/**
 * @brief Worker::readLoadRegister: Reads Modbus register at addr to reg
 * @param addr
 * @return
 */

int Worker::readLoadRegister(int addr)
{
    int i;
    i=modbus_read_registers(ctx,addr,31,reg);
    if (i == -1) {
        QMessageBox msg;
        QString err(modbus_strerror(errno));
        msg.setWindowTitle("Error!");
        msg.setText("Couldn't read :" + err);
        msg.exec();
        exit(-1);
    }
    return i;
}

/**
 * @brief Worker::countMaxonInterrupts: Count encoder interrupts
 */

void Worker::countMaxonInterrupts()
{
    counter++;
}

/**
 * @brief Worker::querySPI: Reads adc values with tx command to rx array.
 */

inline void Worker::querySPI()
{
    int ret=0;
    memset (&spi_trans, 0, sizeof (spi_trans));
    spi_trans.tx_buf = (unsigned long) &tx;
    spi_trans.rx_buf = (unsigned long) &rx;
    spi_trans.len = ARRAY_SIZE(rx);
    spi_trans.delay_usecs = MYSPICONF.delay;
    spi_trans.speed_hz = MYSPICONF.speed;
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &spi_trans);
    if (ret < 0) {
        QMessageBox msg;
        msg.setWindowTitle("ERROR!");
        msg.setText("Couldn't read SPI Values");
        msg.exec();
        exit(-1);
    }

}

/**
 * @brief Worker::convfl:Decodes Mayumo Modbus values
 * @param tab
 * @param idx
 * @return
 */

float Worker::convfl(uint16_t *tab, int idx)
{
    uint32_t a;
    float f;
    a = (((uint32_t)tab[idx]) << 16) + tab[idx+1];
    memcpy(&f, &a, sizeof(float));
    return f;
}

/**
 * @brief Worker::measureLoopWithoutLoadWithoutTorque: Measureloop
 * @return
 */

int Worker::measureLoopWithoutLoadWithoutTorque()
{
    int idel = static_cast<int>(del);
    rtsched();
    int slen;
    gettimeofday(&start,0);
    QString qdisp;
    float rps=0,rpm=0;
    char *row=nullptr;
    char *disp=nullptr;
    time_t t;
    struct tm *tnow;
    while(!QThread::currentThread()->isInterruptionRequested()) {
        rps=(counter/MAXONMULTI)*(1000/del);
        rpm=rps*60;
        gettimeofday(&end,0);
        timersub(&end,&start,&diff);
        asprintf(&disp,"RPM: %f\nRPS: %f\n",rpm, rps);
        qdisp =  QString::fromUtf8(disp);
        emit resultReady(qdisp);
        counter = 0;
        time(&t);
        tnow=localtime(&t);
        slen = asprintf(&row,"%02d.%02d.%02d %02d:%02d:%02d;%li.%06li;%f;%f;0;0;0;0;0\n",
                 tnow->tm_mday, tnow->tm_mon+1, tnow->tm_year+1900, tnow->tm_hour, tnow->tm_min,
                 tnow->tm_sec, diff.tv_sec, diff.tv_usec, rpm,rps);
        file.write(row, slen);
        file.flush();
        free(disp);
        free(row);
        delay(idel);
    }

}

/**
 * @brief Worker::measureLoopWithLoadWithoutTorque: Measureloop
 * @return
 */

int Worker::measureLoopWithLoadWithoutTorque()
{
    QString qdisp;
    int slen;
    int idel = static_cast<int>(del);
    char *disp = nullptr;
    char *row = nullptr;
    rtsched();
    if (openLoad(mayumoPath.toStdString().c_str())==-1) {
        QMessageBox msg;
        msg.setWindowTitle("Error!");
        msg.setText("Exit");
        msg.exec();
        emit(sigFailure());
    }
    modbus_set_slave(ctx,1);
    modbus_set_response_timeout(ctx, 1, 0);
    gettimeofday(&start,0);
    double rps=0,rpm=0,i=0,u=0,p=0,r=0;
    time_t t;
    struct tm *tnow;
    int rc=0;
    while(!QThread::currentThread()->isInterruptionRequested()) {
        rc=readLoadRegister(0x0B00);
        rps=(counter/MAXONMULTI)*(1000/del);
        rpm=rps*60;
        i=convfl(&reg[2],0);
        u=convfl(&reg[0],0);
        //p=convfl(&reg[5],0);
        //r=convfl(&reg[7],0);
        p=i*u;
        gettimeofday(&end,0);
        timersub(&end,&start,&diff);
        asprintf(&disp,"RPM: %f\nRPS: %f\nI: %f A \nU: %f V \nP: %f W \nR: %f Ohm \n",
                 rpm, rps, i,u,p,r);
        qdisp = QString::fromUtf8(disp);
        emit resultReady(qdisp);
        counter = 0;
        time(&t);
        tnow=localtime(&t);
        slen = asprintf(&row,
                 "%02d.%02d.%02d %02d:%02d:%02d;%li.%0li;%f;%f;%f;%f;%f;%f;0\n",
                 tnow->tm_mday, tnow->tm_mon+1, tnow->tm_year+1900, tnow->tm_hour, tnow->tm_min,
                 tnow->tm_sec, diff.tv_sec, diff.tv_usec, rpm, rps, i,u,p,r);
        file.write(row, slen);
        file.flush();
        free(row);
        free(disp);
        delay(idel);
    }
}


/**
 * @brief Worker::measureLoopWithoutLoadWithTorque: Measure loop
 * @return
 */
int Worker::measureLoopWithoutLoadWithTorque()
{
    rtsched();
    int slen;
    int idel = static_cast<int>(del);
    gettimeofday(&start,0);
    QString qdisp;
    int out;
    double val;
    if (fd == -1) {
        QMessageBox msg;
        msg.setWindowTitle("Error Opening SPI Dev. Maybe not enabled?");
        msg.setText("Exit");
        msg.exec();
        emit(sigFailure());
    }
    double rps=0,rpm=0;
    char *row=nullptr;
    char *disp=nullptr;
    time_t t;
    struct tm *tnow;
    while(!QThread::currentThread()->isInterruptionRequested()) {
        rps=(counter/MAXONMULTI)*(1000/del);
        rpm=rps*60;
        querySPI();
        out = ((rx[1] & 15) << 8) | rx[2];
        val = out/4095.0 * 5.0;
        gettimeofday(&end,0);
        timersub(&end,&start,&diff);
        asprintf(&disp,"RPM: %f\nRPS: %f\nTorque: %f\n",rpm, rps, val);
        qdisp =  QString::fromUtf8(disp);
        emit resultReady(qdisp);
        counter = 0;
        time(&t);
        tnow=localtime(&t);
        slen = asprintf(&row,"%02d.%02d.%02d %02d:%02d:%02d;%li.%06li;%f;%f;0;0;0;0;%f\n",
                 tnow->tm_mday, tnow->tm_mon+1, tnow->tm_year+1900, tnow->tm_hour, tnow->tm_min,
                 tnow->tm_sec, diff.tv_sec, diff.tv_usec, rpm,rps, val);
        file.write(row, slen);
        file.flush();
        free(disp);
        free(row);
        delay(idel);
    }
}


/**
 * @brief Worker::measureLoopWithLoadWithTorque: Measure loop
 * @return
 */
int Worker::measureLoopWithLoadWithTorque()
{
    QString qdisp;
    std::string sres;
    int slen;
    int idel = static_cast<int>(del);
    char *disp = nullptr;
    char *row = nullptr;
    rtsched();
    if (openLoad(mayumoPath.toStdString().c_str())==-1) {
        QMessageBox msg;
        msg.setWindowTitle("Error!");
        msg.setText("Exit");
        msg.exec();
        return -1;
        //emit(sigFailure());
    }
    modbus_set_slave(ctx,1);
    modbus_set_response_timeout(ctx, 1, 0);
    int out;
    double val;
    if (fd == -1) {
        QMessageBox msg;
        msg.setWindowTitle("Error Opening SPI Dev. Maybe not enabled?");
        msg.setText("Exit");
        msg.exec();
        emit(sigFailure());
    }
    gettimeofday(&start,0);
    double rps=0,rpm=0,i=0,u=0,p=0,r=0;
    time_t t;
    struct tm *tnow;
    int rc=0;
    rc=readLoadRegister(0x0B00);
    if (rc == -1) {
        return -1;
    }
    while(!QThread::currentThread()->isInterruptionRequested()) {
        rc=readLoadRegister(0x0B00);
        rps=(counter/MAXONMULTI)*(1000/del);
        rpm=rps*60;
        i=convfl(&reg[2],0);
        u=convfl(&reg[0],0);
        querySPI();
        out = ((rx[1] & 15) << 8) | rx[2];
        val = out/4095.0 * 5.0;
        p=i*u;
        gettimeofday(&end,0);
        timersub(&end,&start,&diff);
        asprintf(&disp,
                 "RPM: %f\nRPS: %f\nI: %f A \nU: %f V \nP: %f W \nR: %f Ohm\nTorque %f Nm",
                 rpm, rps, i,u,p,r, val);
        qdisp = QString::fromUtf8(disp);
        emit resultReady(qdisp);
        counter = 0;
        time(&t);
        tnow=localtime(&t);
        slen = asprintf(&row,
                 "%02d.%02d.%02d %02d:%02d:%02d;%li.%0li;%f;%f;%f;%f;%f;%f;%f\n",
                 tnow->tm_mday, tnow->tm_mon+1, tnow->tm_year+1900, tnow->tm_hour, tnow->tm_min,
                 tnow->tm_sec, diff.tv_sec, diff.tv_usec, rpm, rps, i,u,p,r,val);
        file.write(row, slen);
        file.flush();
        free(row);
        free(disp);
        delay(idel);
    }

}
