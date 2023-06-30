#include "BZ_log.h"
using namespace BZ_LOG;

int main()
{
    Logger::instance()->open("./log.log");

    Logger::instance()->log(Logger::DEBUG, "bz-in", 8, "hello world");
    Logger::instance()->log(Logger::DEBUG, "hello bz", 8, "age=%d , name=%s", 18, "bit-zero");
    Logger::instance()->log(Logger::INFO, "bz-in", 20, "hello");

    return 0;
}