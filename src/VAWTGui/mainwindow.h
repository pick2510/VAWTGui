#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <iostream>
#include <fstream>
#include <map>
#include <vector>

#include <QMainWindow>
#include <stdint.h>
#include <QDir>
#include <QTreeWidgetItem>
#include <QFileDialog>
#include <QStyle>
#include <QDesktopWidget>
#include <QThread>
#include <QDir>
#include <QFileInfoList>
#include <QStringList>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <sys/time.h>
#include "worker.h"
#include "libvawt.h"




namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
    std::ofstream f;
    struct SPICONF MYSPICONF;
    QThread workerThread;
    Worker *worker=nullptr;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool checkParams();
    void Start();
    QString getPath() const;
    void setPath(const QString &value);

public slots:
    void handleResults(const QString &res);
    void handleFinish();
    void handleFailure();

private slots:
    void on_btnClose_clicked();
    void on_chkLogging_toggled(bool checked);
    void on_btnPath_clicked();
    void on_btnStart_clicked();
    void on_btnCancel_clicked();

private:
    Ui::MainWindow *ui;
    QString Path;
    int fd;
    void piInitialize();
    void SPIInitialize();
    void openFile(std::ofstream &f);
    void writeHeader(std::ofstream &f);
    bool isMayumoEnabled;
    bool isLoggingEnabled;

signals:
    void operate(const QString &);
};

#endif // MAINWINDOW_H
