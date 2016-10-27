#include "mainwindow.h"
#include "wiringPiSPI.h"
#include "wiringPi.h"
#include "libvawt.h"
#include "ui_mainwindow.h"

const static int SPI_CHANNEL = 0;
const static int SPI_DEV=0;
const static long int SPI_SPEED=1000000;
const static char *HEADER="Date;Timer;RPM;RPS;I;U;P;R;Nm(V)\n";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    if (!isFileExisiting(ui->txtMayumi->text().toStdString().c_str())) {
        ui->txtMayumi->setText("");
    }
    Path = ".";
    this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                                          this->size(),
                                          qApp->desktop()->availableGeometry()));
    fd=0;
    this->setFixedSize(this->size());
    piInitialize();

}

MainWindow::~MainWindow()
{
    delete ui;
    workerThread.quit();
    workerThread.wait();
}

bool MainWindow::checkParams()
{
    if (!ui->txtMayumi->text().toStdString().empty()) {
        if (!isFileExisiting(ui->txtMayumi->text().toStdString().c_str())) {
            QMessageBox msg;
            msg.setWindowTitle("Error!");
            msg.setText("Mayumi DC Load Devicefile doesn't exist: " +
                        ui->txtMayumi->text());
            msg.exec();
            return false;

        } else {
            isMayumoEnabled = true;
        }
    }
    return true;
}

void MainWindow::Start()
{
    openFile(f);
    writeHeader(f);
    int dela = ui->spbDelay->value();
    Worker *worker = new Worker(f, dela, ui->txtMayumi->text(),
                                ui->chkTorque->isChecked(), fd);
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(&workerThread, &QThread::started, worker, &Worker::startWork);
    connect(worker, &Worker::resultReady, this, &MainWindow::handleResults);
    connect(worker, &Worker::execFinished, this, &MainWindow::handleFinish);
    connect(worker, &Worker::sigFailure, this, &MainWindow::handleFailure);
    workerThread.start();
}

void MainWindow::on_btnClose_clicked()
{
    exit(0);
}

QString MainWindow::getPath() const
{
    return Path;
}

void MainWindow::setPath(const QString &value)
{
    Path = value;
}

void MainWindow::handleResults(const QString &res)
{
    ui->txtDisp->setText(res);
}

void MainWindow::handleFinish()
{
    ui->btnStart->setEnabled(true);
    ui->btnCancel->setEnabled(false);
    f.close();
}

void MainWindow::handleFailure()
{
    workerThread.terminate();
    exit(1);
}

void MainWindow::on_chkLogging_toggled(bool checked)
{
    if (checked == true) {
        ui->btnPath->setEnabled(true);
    } else {
        ui->btnPath->setEnabled(false);
    }
}

void MainWindow::on_btnPath_clicked()
{
    this->setPath(QFileDialog::getExistingDirectory(
                      this, tr("Open Directory"), ".",
                      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
}

void MainWindow::on_btnStart_clicked()
{
    isLoggingEnabled = ui->chkLogging->isChecked();
    isMayumoEnabled = false;
    if (checkParams()) {
        Start();
        ui->btnStart->setEnabled(false);
        ui->btnCancel->setEnabled(true);
    }
}

void MainWindow::openFile(std::ofstream &f)
{
    char *filename=nullptr;
    time_t t;
    struct tm *tnow=nullptr;
    std::stringstream file;
    time(&t);
    tnow = localtime(&t);
    asprintf(&filename, "%02d-%02d-%02d_%02d-%02d-%02d.csv" ,tnow->tm_mday,
             tnow->tm_mon, tnow->tm_year+1900, tnow->tm_hour, tnow->tm_min, tnow->tm_sec);
    file << Path.toStdString() << "/" << filename;
    f.open(file.str().c_str());
    if (f.fail()) {
        QMessageBox msg;
        msg.setWindowTitle("ERROR!");
        msg.setText("Couldn't open logfile: " + Path);
        msg.exec();
        exit(-1);
    }
}

void MainWindow::writeHeader(std::ofstream &f)
{
    f << HEADER;
    f.flush();
}

void MainWindow::on_btnCancel_clicked()
{
    workerThread.requestInterruption();
    workerThread.terminate();
}

void MainWindow::piInitialize()
{
    wiringPiSetup();
    fd=wiringPiSPISetup(SPI_DEV, SPI_SPEED);
}
