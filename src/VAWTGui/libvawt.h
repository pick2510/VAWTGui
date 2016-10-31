#ifndef LIBVAWT_H
#define LIBVAWT_H

#include <time.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

struct SPICONF {
    const char *device = "/dev/spidev0.0";
    const int SPI_CHANNEL=0;
    const uint8_t mode = SPI_MODE_3;
    const uint16_t delay = 0;
    const uint8_t bits = 8;
    const uint32_t speed = 2000000;
};


bool isFileExisiting(const char *fileName);


#endif // LIBVAWT_H
