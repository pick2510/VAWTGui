#include "mainwindow.h"
#include "libvawt.h"
#include "ui_mainwindow.h"
#include "worker.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    if (!isFileExisiting(ui->txtMayumi->text().toStdString().c_str())) {
        ui->txtMayumi->setText("");
    }
    if (!isFileExisiting(ui->txtAnemo->text().toStdString().c_str())) {
        ui->txtAnemo->setText("");
    }
    Path = ".";
    this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                                          this->size(),
                                          qApp->desktop()->availableGeometry()));
    this->setFixedSize(this->size());
}

MainWindow::~MainWindow()
{
    delete ui;
    workerThread.quit();
    workerThread.wait();
}

bool MainWindow::checkParams()
{

    if (!ui->txtAnemo->text().toStdString().empty()) {
        if (!isFileExisiting(ui->txtAnemo->text().toStdString().c_str())) {
            QMessageBox msg;
            msg.setWindowTitle("Error!");
            msg.setText("Anemometer Devicefile doesn't exist: " +
                        ui->txtAnemo->text());
            msg.exec();
            return false;
        } else {
            isAnemoEnabled = true;
        }
    }
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
    writeHeader("Datum;Timer;RPM;RPS;I;U;P;R\n", f);
    int dela = ui->spbDelay->value();
    Worker *worker = new Worker(0, f, dela);
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(&workerThread, &QThread::started, worker, &Worker::startWork);
    connect(worker, &Worker::resultReady, this, &MainWindow::handleResults);
    connect(worker, &Worker::execFinished, this, &MainWindow::handleFinish);
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
    isAnemoEnabled = false;
    isMayumoEnabled = false;
    if (checkParams()) {
        Start();
        ui->btnStart->setEnabled(false);
        ui->btnCancel->setEnabled(true);
    }
}

void MainWindow::openFile(std::ofstream &f)
{
    time_t t;
    struct tm *tnow;
    std::stringstream file;
    time(&t);
    tnow = localtime(&t);
    file << Path.toStdString() << "/" << tnow->tm_mday << "-" << tnow->tm_mon
         << "-" << tnow->tm_year + 1900 << "_" << tnow->tm_hour << "_"
         << tnow->tm_min << "_" << tnow->tm_sec << ".csv";
    QMessageBox msg;
    msg.setWindowTitle("Logpath");
    msg.setText(QString(file.str().c_str()));
    msg.exec();
    f.open(file.str().c_str());
    if (f.fail()) {
        msg.setWindowTitle("ERROR!");
        msg.setText("Couldn't open logfile: " + Path);
        msg.exec();
        exit(-1);
    }
}

void MainWindow::writeHeader(const char *c, std::ofstream &f)
{
    f << c;
    f.flush();
}

void MainWindow::on_btnCancel_clicked()
{
    workerThread.requestInterruption();
}
