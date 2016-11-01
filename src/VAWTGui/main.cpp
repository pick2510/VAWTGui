
/*************************************************************************
 *                              VAWTGui                                  *
 *     --------------------------------------------------------          *
 *  This Program reads the values of a Maxon HEDS 5540 Motor Encoder     *
 *  and certain values of a Maynuo M9812 DC Load.                        *
 *  In addition, it also reads a ADC for a Torque Sensor.                *
 *  It's based on libWiringPi by Gordon Henderson and libmodbus by       *
 *  Stéphan Raimbault. It's free software according to GPL v.2           *
 *              by Dominik Strebel dstrebel@hsr.ch                       *
 ************************************************************************/


#include "mainwindow.h"
#include <QApplication>
/**
 * @brief Main Function
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
