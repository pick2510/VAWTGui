
#include "mainwindow.h"
#include "wiringPiSPI.h"
#include "wiringPi.h"
#include "libvawt.h"
#include "ui_mainwindow.h"


const static char *HEADER="Date;Timer;RPM;RPS;I;U;P;R;Nm(V)\n";

/**
* @brief MainWindow::MainWindow Constructor.
* Constructs the MainWindow Object and initializes Default Values
* @param parent
*
*/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    if (!isFileExisiting("/dev/ttyUSB0")) {
        ui->chkMayumi->setChecked(false);
        ui->chkMayumi->setEnabled(false);
    }
    Path=QDir::currentPath();
    this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                                          this->size(),
                                          qApp->desktop()->availableGeometry()));
    fd=0;
    this->setFixedSize(this->size());
    ui->txtPath->setText(Path);
    piInitialize();

}

/**
 * @brief MainWindow::~MainWindow
 * Desctructor. Kills ui and workerThread.
 */

MainWindow::~MainWindow()
{
    delete ui;
    workerThread.quit();
    workerThread.wait();
}



/**
 * @brief MainWindow::Start
 * Creates Workerobject and moves it to Worker thread. Connects various Signals
 */
void MainWindow::Start()
{
    openFile(f);
    writeHeader(f);
    int dela = ui->spbDelay->value();
    Worker *worker = new Worker(f, dela, "/dev/ttyUSB0",
                                ui->chkTorque->isChecked(), fd, MYSPICONF);
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(&workerThread, &QThread::started, worker, &Worker::startWork);
    connect(worker, &Worker::resultReady, this, &MainWindow::handleResults);
    connect(worker, &Worker::execFinished, this, &MainWindow::handleFinish);
    connect(worker, &Worker::sigFailure, this, &MainWindow::handleFailure);
    workerThread.start();
}


/**
 * @brief MainWindow::on_btnClose_clicked: Close Button handler
 */
void MainWindow::on_btnClose_clicked()
{
    exit(0);
}

/**
 * @brief MainWindow::getPath Pathgetter
 * @return
 */
QString MainWindow::getPath() const
{
    return Path;
}

/**
 * @brief MainWindow::setPath Path setter
 * @param value
 */
void MainWindow::setPath(const QString &value)
{
    Path = value;
}

/**
 * @brief MainWindow::handleResults: Handles Results from Workerobject.
 * Displays results.
 * @param res
 */

void MainWindow::handleResults(const QString &res)
{
    ui->txtDisp->setText(res);
}

/**
 * @brief MainWindow::handleFinish: Handles finishing of Workerobject. Closes Filehandler.
 */

void MainWindow::handleFinish()
{
    ui->btnStart->setEnabled(true);
    ui->btnCancel->setEnabled(false);
    f.close();
}

/**
 * @brief MainWindow::handleFailure: Handles failures from Worker thread.
 */

void MainWindow::handleFailure()
{
    workerThread.terminate();
    exit(1);
}

/**
 * @brief MainWindow::on_chkLogging_toggled: Logging chkbox handler.
 * @param checked
 */

void MainWindow::on_chkLogging_toggled(bool checked)
{
    if (checked == true) {
        ui->btnPath->setEnabled(true);
    } else {
        ui->btnPath->setEnabled(false);
    }
}


/**
 * @brief MainWindow::on_btnPath_clicked: Pathbtn handler. Opens Directory chooser
 */

void MainWindow::on_btnPath_clicked()
{
    this->setPath(QFileDialog::getExistingDirectory(
                      this, tr("Open Directory"), ".",
                      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
    ui->txtPath->setText(Path);
}

/**
 * @brief MainWindow::on_btnStart_clicked: Startbtn handler.
 */

void MainWindow::on_btnStart_clicked()
{
    isLoggingEnabled = ui->chkLogging->isChecked();
    Start();
    ui->btnStart->setEnabled(false);
    ui->btnCancel->setEnabled(true);
}

/**
 * @brief MainWindow::openFile: Opens filehandler at chosen path.
 * @param f
 */

void MainWindow::openFile(std::ofstream &f)
{
    char *filename=nullptr;
    time_t t;
    struct tm *tnow=nullptr;
    std::stringstream file;
    time(&t);
    tnow = localtime(&t);
    asprintf(&filename, "%02d-%02d-%02d_%02d-%02d-%02d.csv" ,tnow->tm_mday,
             tnow->tm_mon+1, tnow->tm_year+1900, tnow->tm_hour, tnow->tm_min, tnow->tm_sec);
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

/**
 * @brief MainWindow::writeHeader: Writes Header at filehandler f
 * @param f
 */

void MainWindow::writeHeader(std::ofstream &f)
{
    f << HEADER;
    f.flush();
}

/**
 * @brief MainWindow::on_btnCancel_clicked: Cancelbtn handler. Terminates worker thread
 */

void MainWindow::on_btnCancel_clicked()
{
    workerThread.requestInterruption();
    workerThread.terminate();
    ui->txtDisp->setText("");
}

/**
 * @brief MainWindow::piInitialize: Global Initalizer for Raspberry Pi
 */

void MainWindow::piInitialize()
{
    wiringPiSetup();
    SPIInitialize();
}

/**
 * @brief MainWindow::SPIInitialize: Initalize spidev kernel driver. Set Modes.
 */

void MainWindow::SPIInitialize()
{
    int ret=0;
    fd = open(MYSPICONF.device, O_RDWR);
    if (fd <  0) {
        QMessageBox msg;
        msg.setWindowTitle("ERROR!");
        msg.setText("Couldn't open SPIDEV: " + QString::fromLocal8Bit(
                        MYSPICONF.device));
        msg.exec();
        exit(-1);
    }
    ret = ioctl(fd, SPI_IOC_WR_MODE, &MYSPICONF.mode);
    if(ret < 0) {
        QMessageBox msg;
        msg.setWindowTitle("ERROR!");
        msg.setText("Couldn't set SPI MODE");
        msg.exec();
        exit(-1);
    }
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &MYSPICONF.bits);
    if(ret < 0) {
        QMessageBox msg;
        msg.setWindowTitle("ERROR!");
        msg.setText("Couldn't set SPI BITS PER WORD");
        msg.exec();
        exit(-1);
    }
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &MYSPICONF.speed);
    if(ret < 0) {
        QMessageBox msg;
        msg.setWindowTitle("ERROR!");
        msg.setText("Couldn't set SPI MAX SPEED");
        msg.exec();
        exit(-1);
    }

}
