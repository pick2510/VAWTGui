#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <iostream>
#include <fstream>
#include <map>
#include <vector>

#include <QMainWindow>
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
#include <sys/time.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
    std::ofstream f;
    QThread workerThread;

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

private slots:
    void on_btnClose_clicked();
    void on_chkLogging_toggled(bool checked);
    void on_btnPath_clicked();
    void on_btnStart_clicked();
    void on_btnCancel_clicked();

private:
    Ui::MainWindow *ui;
    QString Path;
    void openFile(std::ofstream &f);
    void writeHeader(const char *c, std::ofstream &f);
    bool isAnemoEnabled;
    bool isMayumoEnabled;
    bool isLoggingEnabled;

signals:
    void operate(const QString &);
};

#endif // MAINWINDOW_H
