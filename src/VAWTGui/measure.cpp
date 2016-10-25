#include "measure.h"
#include "ui_mainwindow.h"
#include "libvawt.h"


Measure::Measure(Ui::MainWindow *mw, QString path)
{
    mainwindow = mw;
    counter = 0;
    isLoggingEnabled = mw->chkLogging->isChecked();
    isAnemoEnabled=false;
    isMayumoEnabled=false;
    this->Path=path;
    if (checkParams()){
        StartMeasurement();
    }


}

void Measure::StartMeasurement()
{
    std::ofstream f;
    openFile(f);
    writeHeader("Datum;Timer;RPM;RPS;I;U;P;R\n", f);
    piSetup();
    gettimeofday(&start,0);
}

bool Measure::checkParams()
{
    if (!mainwindow->txtAnemo->text().toStdString().empty()){
        if (!isFileExisiting(mainwindow->txtAnemo->text().toStdString().c_str())){
            QMessageBox msg;
            msg.setWindowTitle("Error!");
            msg.setText("Anemometer Devicefile doesn't exist:" + mainwindow->txtAnemo->text());
            msg.exec();
            return false;
        } else {
            isAnemoEnabled = true;
        }

    }
    if (!mainwindow->txtMayumi->text().toStdString().empty()){
        if (!isFileExisiting(mainwindow->txtMayumi->text().toStdString().c_str())){
            QMessageBox msg;
            msg.setWindowTitle("Error!");
            msg.setText("Mayumi DC Load Devicefile doesn't exist: " + mainwindow->txtMayumi->text());
            msg.exec();
            return false;

        } else {
            isMayumoEnabled = true;
        }
    }
    return true;
}

void Measure::rtsched()
{
    param.sched_priority = MY_PRIORITY;
    if (sched_setscheduler(0, SCHED_FIFO, &param)== -1)
    {
        exit(-1);
    }
}

int Measure::openLoad(const char * c)
{
    ctx=modbus_new_rtu(c,115200,'N',8,1);
    if (modbus_connect(ctx) == -1){
        QMessageBox msg;
        QString dev(c);
        msg.setWindowTitle("Error!");
        msg.setText("Couldn't open Mayumo Load at " + dev);
        msg.exec();
        modbus_free(ctx);
        return -1;
    }
    else return 1;
}

void Measure::piSetup()
{
    wiringPiSetup();
    pinMode(2, INPUT);
    pullUpDnControl(2, PUD_UP);
    wiringPiISR(2,INT_EDGE_FALLING, &countMaxonInterrupts);
}

int Measure::readLoadRegister(int addr)
{
    int i;
    i=modbus_read_registers(ctx,addr,31,reg);
    if (i == -1){
        QMessageBox msg;
        QString err(modbus_strerror(errno));
        msg.setWindowTitle("Error!");
        msg.setText("Couldn't read :" + err);
        msg.exec();
        exit(-1);
    }
    return i;
}

void Measure::countMaxonInterrupts()
{
    counter++;
}

void Measure::openFile(std::ofstream &f)
{
    time_t t;
    struct tm *tnow;
    std::stringstream file;
    time(&t);
    tnow=localtime(&t);
    file << Path.toStdString() << "/" << tnow->tm_mday << "-" <<
    tnow->tm_mon << "-" << tnow->tm_year+1900 << "_" << tnow->tm_hour << "_" <<
    tnow->tm_min << "_" << tnow->tm_sec << ".csv";
    QMessageBox msg;
    msg.setWindowTitle("Logpath");
    msg.setText(QString(file.str().c_str()));
    msg.exec();
    f.open(file.str().c_str());
    if (f.fail()){
        msg.setWindowTitle("ERROR!");
        msg.setText("Couldn't open logfile: " + Path);
        msg.exec();
        exit(-1);
    }
}

void Measure::writeHeader(const char *c, std::ofstream &f)
{
    f << c;
    f.flush();
}

int Measure::withoutLoad(Ui::MainWindow *mw, std::ofstream &f)
{
/*    float rps=0,rpm=0;
    std::stringstream row;
    std::stringstream disp;
    QString qdisp;
    time_t t;
    struct tm *tnow;
    float del =(float)mw->spbDelay->value();
      for(;;){
        rps=(counter/maxonMulti)*(1000/del);
        rpm=rps*60;
        gettimeofday(&end,0);
        timersub(&end,&start,&diff);
        disp << "RPM: " << rpm << "\n" << "RPS: " << rps << "\n";
        qdisp = QString::fromStdString(disp.str());
        mw->txtDisp->setText(qdisp);
        counter = 0;
        time(&t);
        tnow=localtime(&t);
        row << tnow->tm_mday << "." << tnow->tm_mon << "." << tnow->tm_year+1900 << " " <<  tnow->tm_hour << ":" << tnow->tm_min << ":" <<  tnow->tm_sec << ";" << diff.tv_sec << "." << diff.tv_usec << ";" << rpm << ";" << rps << ";;;;\n";
        //fprintf(csv,"%02d.%02d.%02d %02d:%02d:%02d;%li.%06li;%fl;%fl;;;;\n", tnow->tm_mday, tnow->tm_mon, tnow->tm_year+1900, tnow->tm_hour, tnow->tm_min, tnow->tm_sec, diff.tv_sec, diff.tv_usec, rpm,rps);
        f << row.str();
        f.flush();
        row.str("");
        delay(static_cast<int>(del));
      }
*/
}




