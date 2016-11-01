#include "libvawt.h"
#include<fstream>

/**
 * @brief isFileExisiting: Checks if file is existing (good).
 * @param fileName
 * @return
 */

bool isFileExisiting(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}
