#include "libvawt.h"
#include<fstream>


bool isFileExisiting(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}
