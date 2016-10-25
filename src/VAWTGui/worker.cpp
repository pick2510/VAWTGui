#include "worker.h"

static volatile long int counter; //RPM Counter
static uint16_t reg[160]; //Maynuo DC Load Register
static modbus_t *ctx; //Modbus Connection Handler
static struct timeval start, diff, end;
const float maxonMulti=500;

Worker::Worker(std::ofstream &f, int dela, QString mayumopath, QObject *parent)  : file(f), mayumoPath(mayumopath), QObject(parent)
{
    del=float(dela);
    file.flush();
}

void Worker::startWork()
{

    if (mayumoPath.isEmpty()){
        measureLoopWithoutLoad();
    } else {
        measureLoopWithLoad();
    }


    emit(execFinished());
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

void Worker::piSetup()
{
    wiringPiSetup();
    pinMode(2, INPUT);
    pullUpDnControl(2, PUD_UP);
    wiringPiISR(2,INT_EDGE_FALLING, &countMaxonInterrupts);
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
        exit(-1);
    }
    return i;
}

void Worker::countMaxonInterrupts()
{
    counter++;
}

float Worker::convfl(uint16_t *tab, int idx){
  uint32_t a;
  float f;
  a = (((uint32_t)tab[idx]) << 16) + tab[idx+1];
  memcpy(&f, &a, sizeof(float));
  return f;
}

int Worker::measureLoopWithoutLoad()
{
    rtsched();
    piSetup();
    gettimeofday(&start,0);
    QString qdisp;
    float rps=0,rpm=0;
    std::stringstream row;
    std::stringstream disp;
    time_t t;
    struct tm *tnow;
    while(!QThread::currentThread()->isInterruptionRequested()) {
        rps=(counter/maxonMulti)*(1000/del);
        rpm=rps*60;
        gettimeofday(&end,0);
        timersub(&end,&start,&diff);
        disp << "RPM: " << rpm << "\n" << "RPS: " << rps << "\n";
        qdisp = QString::fromStdString(disp.str());
        emit resultReady(qdisp);
        counter = 0;
        time(&t);
        tnow=localtime(&t);
        row << tnow->tm_mday << "." << tnow->tm_mon << "." << tnow->tm_year+1900 << " " <<  tnow->tm_hour << ":" << tnow->tm_min << ":" <<  tnow->tm_sec << ";" << diff.tv_sec << "." << diff.tv_usec << ";" << rpm << ";" << rps << ";;;;\n";
        //fprintf(csv,"%02d.%02d.%02d %02d:%02d:%02d;%li.%06li;%fl;%fl;;;;\n", tnow->tm_mday, tnow->tm_mon, tnow->tm_year+1900, tnow->tm_hour, tnow->tm_min, tnow->tm_sec, diff.tv_sec, diff.tv_usec, rpm,rps);
        file << row.str();
        file.flush();
        row.str("");
        disp.str("");
        delay(static_cast<int>(del));
    }

}

int Worker::measureLoopWithLoad()
{
    QString qdisp;
    char **disp;
    char **row;
    rtsched();
    piSetup();
    openLoad(mayumoPath.toStdString().c_str());
    modbus_set_slave(ctx,1);
    modbus_set_response_timeout(ctx, 1, 0);
      gettimeofday(&start,0);
      float rps=0,rpm=0,i=0,u=0,p=0,r=0;
      time_t t;
      struct tm *tnow;
      int rc=0;
         while(!QThread::currentThread()->isInterruptionRequested()){
          rc=readLoadRegister(0x0B00);
          rps=(counter/maxonMulti)*(1000/del);
          rpm=rps*60;
          i=convfl(&reg[2],0);
          u=convfl(&reg[0],0);
          //p=convfl(&reg[5],0);
          //r=convfl(&reg[7],0);
          p=i*u;
          gettimeofday(&end,0);
          timersub(&end,&start,&diff);
          asprintf(disp,"RPM: %f\nRPS: %f\nI: %f A \nU: %f V \nP: %f W \n",rpm, rps, i,u,p,r);
          qdisp = QString::fromUtf8(*disp);
          emit resultReady(qdisp);
          //printf("I: %f oA \nU: %f V \nP: %f W \nR: %f Ohm \n", );
          counter = 0;
          time(&t);
          tnow=localtime(&t);
          asprintf(row, "%02d.%02d.%02d %02d:%02d:%02d;%li.%0li;%f;%f;%f;%f;%f\n", tnow->tm_mday, tnow->tm_mon, tnow->tm_year+1900, tnow->tm_hour, tnow->tm_min, tnow->tm_sec, diff.tv_sec, diff.tv_usec, rpm, rps, i,u,p,r);
          file << row;
          file.flush();
          delay(static_cast<int>(del));
        }
      }


