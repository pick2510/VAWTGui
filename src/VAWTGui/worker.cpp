#include "worker.h"

static volatile long int counter; //RPM Counter
static uint16_t reg[160]; //Maynuo DC Load Register
static modbus_t *ctx; //Modbus Connection Handler
static struct timeval start, diff, end;
const static double MAXONMULTI=500;
const static int SPI_CHANNEL = 0;
const static double REFERENCE_VOLTAGE=3.3;

Worker::Worker(std::ofstream &f, int dela, QString mayumopath,
               bool isTorqueEnabled, int fd, QObject *parent)  : file(f),
    torqueEnabled(isTorqueEnabled), mayumoPath(mayumopath), QObject(parent)
{
    del=float(dela);
    file.flush();
    this->fd=fd;
    piSetup();
}

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

void Worker::piSetup()
{
    pinMode(2, INPUT);
    pullUpDnControl(2, PUD_UP);
    wiringPiISR(2,INT_EDGE_FALLING, &countMaxonInterrupts);
}

void Worker::rtsched()
{
    param.sched_priority = MY_PRIORITY;
    if (sched_setscheduler(0, SCHED_FIFO, &param)== -1) {
        exit(-1);
    }
}

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
        emit(sigFailure());
    }
    return i;
}

void Worker::countMaxonInterrupts()
{
    counter++;
}

float Worker::convfl(uint16_t *tab, int idx)
{
    uint32_t a;
    float f;
    a = (((uint32_t)tab[idx]) << 16) + tab[idx+1];
    memcpy(&f, &a, sizeof(float));
    return f;
}

int Worker::measureLoopWithoutLoadWithoutTorque()
{
    rtsched();
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
        asprintf(&row,"%02d.%02d.%02d %02d:%02d:%02d;%li.%06li;%f;%f;0;0;0;0;0\n",
                 tnow->tm_mday, tnow->tm_mon, tnow->tm_year+1900, tnow->tm_hour, tnow->tm_min,
                 tnow->tm_sec, diff.tv_sec, diff.tv_usec, rpm,rps);
        file << row;
        file.flush();
        free(disp);
        free(row);
        delay(static_cast<int>(del));
    }

}

int Worker::measureLoopWithLoadWithoutTorque()
{
    QString qdisp;
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
        //printf("I: %f oA \nU: %f V \nP: %f W \nR: %f Ohm \n", );
        counter = 0;
        time(&t);
        tnow=localtime(&t);
        asprintf(&row,
                 "%02d.%02d.%02d %02d:%02d:%02d;%li.%0li;%f;%f;%f;%f;%f;%f;0\n",
                 tnow->tm_mday, tnow->tm_mon, tnow->tm_year+1900, tnow->tm_hour, tnow->tm_min,
                 tnow->tm_sec, diff.tv_sec, diff.tv_usec, rpm, rps, i,u,p,r);
        file << *row;
        file.flush();
        free(row);
        free(disp);
        delay(static_cast<int>(del));
    }
}

int Worker::measureLoopWithoutLoadWithTorque()
{
    rtsched();
    gettimeofday(&start,0);
    QString qdisp;
    int out;
    double val;
    unsigned char data[3];
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
        data[0]= 1;
        data[1]= (8 + SPI_CHANNEL) << 4;
        data[2] = 0;
        wiringPiSPIDataRW(0, data,3);
        out = ((data[1] & 3 ) << 8) + data[2];
        val = out/1023.0 * REFERENCE_VOLTAGE;
        gettimeofday(&end,0);
        timersub(&end,&start,&diff);
        asprintf(&disp,"RPM: %f\nRPS: %f\nTorque: %f\n",rpm, rps, val);
        qdisp =  QString::fromUtf8(disp);
        emit resultReady(qdisp);
        counter = 0;
        time(&t);
        tnow=localtime(&t);
        asprintf(&row,"%02d.%02d.%02d %02d:%02d:%02d;%li.%06li;%f;%f;0;0;0;0;%f\n",
                 tnow->tm_mday, tnow->tm_mon, tnow->tm_year+1900, tnow->tm_hour, tnow->tm_min,
                 tnow->tm_sec, diff.tv_sec, diff.tv_usec, rpm,rps, val);
        file << row;
        file.flush();
        free(disp);
        free(row);
        delay(static_cast<int>(del));
    }
}

int Worker::measureLoopWithLoadWithTorque()
{
    QString qdisp;
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
    int out;
    double val;
    unsigned char data[3];
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
    while(!QThread::currentThread()->isInterruptionRequested()) {
        rc=readLoadRegister(0x0B00);
        rps=(counter/MAXONMULTI)*(1000/del);
        rpm=rps*60;
        i=convfl(&reg[2],0);
        u=convfl(&reg[0],0);
        data[0]= 1;
        data[1]= (8 + SPI_CHANNEL) << 4;
        data[2] = 0;
        wiringPiSPIDataRW(0, data,3);
        out = ((data[1] & 3 ) << 8) + data[2];
        val = out/1023.0 * REFERENCE_VOLTAGE;
        //p=convfl(&reg[5],0);
        //r=convfl(&reg[7],0);
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
        asprintf(&row,
                 "%02d.%02d.%02d %02d:%02d:%02d;%li.%0li;%f;%f;%f;%f;%f;%f;%f\n",
                 tnow->tm_mday, tnow->tm_mon, tnow->tm_year+1900, tnow->tm_hour, tnow->tm_min,
                 tnow->tm_sec, diff.tv_sec, diff.tv_usec, rpm, rps, i,u,p,r,val);
        file << *row;
        file.flush();
        free(row);
        free(disp);
        delay(static_cast<int>(del));
    }

}
