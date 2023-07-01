#include "BZ_log.h"
using namespace BZ_LOG;

int main()
{
    Logger::instance()->open("./log.log");

    Logger::instance()->max(5); // 设置日志文件最大容量, 单位为kb, 默认为10kb
    // Logger::instance()->level(Logger::WARN); // 设置级别, 默认为DEBUG

    Logger::instance()->log(Logger::DEBUG, "bz-in", 8, "hello world");
    Logger::instance()->log(Logger::INFO, "bz-in", 20, "hello");
    Logger::instance()->log(Logger::WARN, "hello bz", 8, "age=%d , name=%s", 18, "bit-zero");
    Logger::instance()->log(Logger::ERROR, "hello bz", 8, "age=%d , name=%s", 18, "bit-zero");
    Logger::instance()->log(Logger::FATAL, "hello bz", 8, "age=%d , name=%s", 18, "bit-zero");

    debug("测试1: %d:%d", 13, 15);
    info("测试1: %d:%d", 13, 15);
    warn("测试1: %d:%d", 13, 15);
    error("测试1: %d:%d", 13, 15);
    fatal("测试1: %d:%d", 13, 15);

    return 0;
}