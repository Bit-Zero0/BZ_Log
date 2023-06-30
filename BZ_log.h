#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

using namespace std;
namespace BZ_LOG
{

    class Logger
    {
    public:
        enum Level
        {
            DEBUG = 0,
            INFO,
            WARN,
            ERROR,
            FATAL,
            LEVER_NUM,
        };

        void open(const string &filename);
        void max(int byte);
        void log(Level level, const string &file, int line, const string &format, ...);

        static Logger *instance();

    private:
        Logger();
        ~Logger();

        void close();

        const static vector<string> log_level;

        Level def_level;     // 默认日志级别
        string log_filename; // 存储日志的文件
        ofstream f_out;      // 控制日志走向的输出流

        static Logger *log_instance;
    };
}

using namespace BZ_LOG;

Logger *Logger::log_instance = nullptr;

Logger::Logger()
    : def_level(DEBUG)
{
}

Logger *Logger::instance()
{
    if (log_instance == nullptr)
        log_instance = new Logger();
    return log_instance;
}

void Logger::open(const string &filename)
{
    log_filename = filename;
    f_out.open(filename, ios::app);
    if (!f_out.is_open())
    {
        throw std::logic_error("open log file failed" + filename);
    }
}

void Logger::close()
{
    f_out.close();
}

const vector<string> Logger::log_level = {
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL"};

void Logger::log(Level level, const string &file, int line, const string &format, ...)
{
    if (!f_out.is_open())
    {
        throw logic_error("open log file failed" + log_filename);
    }

    time_t timestamp = time(nullptr); // 获取当前时间的时间戳

    tm *ptm = localtime(&timestamp); // 将时间戳存入 tm 结构体

    char time[32];
    memset(time, 0, sizeof(time));
    strftime(time, sizeof(time), "%Y-%m-%d %H:%M:%S", ptm);

    int len = 0;
    const char *fmt = "%s [%s] <%s:%d> "; // 需要格式化的内容

    // 获得此语句长度
    len = snprintf(nullptr, 0, fmt, time, log_level[level].c_str(), file.c_str(), line);

    if (len > 0)
    {
        char *buffer = new char[len + 1];
        snprintf(buffer, len + 1, fmt, time, log_level[level].c_str(), file.c_str(), line);
        buffer[len] = 0;
        f_out << buffer;
        delete buffer;
    }

    va_list arg;
    va_start(arg, format);
    len = vsnprintf(nullptr, 0, format.c_str(), arg);
    va_end(arg);

    if (len > 0)
    {
        char *content = new char[len + 1];
        va_start(arg, format);
        vsnprintf(content, len + 1, format.c_str(), arg);
        va_end(arg);
        content[len] = 0;
        f_out << content;
        delete content;
    }

    f_out << "\n";
    f_out.flush();
}