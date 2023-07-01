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

#define debug(format, ...) Logger::instance()->log(Logger::DEBUG, __FILE__, __LINE__, format, ##__VA_ARGS__);

#define info(format, ...) Logger::instance()->log(Logger::INFO, __FILE__, __LINE__, format, ##__VA_ARGS__);

#define warn(format, ...) Logger::instance()->log(Logger::WARN, __FILE__, __LINE__, format, ##__VA_ARGS__);

#define error(format, ...) Logger::instance()->log(Logger::ERROR, __FILE__, __LINE__, format, ##__VA_ARGS__);

#define fatal(format, ...) Logger::instance()->log(Logger::FATAL, __FILE__, __LINE__, format, ##__VA_ARGS__);

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
        void max(int kb);
        void level(Level level);
        void log(Level level, const string &file, int line, const string &format, ...);

        static Logger *instance();

    private:
        Logger();
        ~Logger();

        void close();

        void rotate();

        const static vector<string> log_level;

        Level def_level;     // 默认日志级别
        string log_filename; // 存储日志的文件
        ofstream f_out;      // 控制日志走向的输出流
        int log_max;         // 日志默认大小
        int log_len;         // 记录日志文件的长度, 方便后续日志文件转存.

        static Logger *log_instance;
    };
}

using namespace BZ_LOG;

Logger *Logger::log_instance = nullptr;

Logger::Logger()
    : def_level(DEBUG), log_max(10240), log_len(0) // 默认每个日志大小为10kb
{
}

Logger::~Logger()
{
    close();
}

void Logger::max(int kb)
{
    log_max = kb * 1024;
}

void Logger::level(Level level)
{
    def_level = level;
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

    // seekp 和 tellp 函数
    // 输出文件流保留指向下一次写入数据的位置的内部指针。
    // seekp 成员函数将设置此指针，从而提供随机访问磁盘文件输出。
    // tellp 成员函数将返回文件位置。
    f_out.seekp(0, ios::end); // 将文件指针移到文件末尾
    log_len = f_out.tellp();
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

    if (level < def_level)
    {
        return;
    }

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
        log_len += len;
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
        log_len += len;
        delete content;
    }

    f_out << "\n";
    f_out.flush();

    if (log_max > 0 && log_len >= log_max)
    {
        rotate();
    }
}

void Logger::rotate()
{
    close();
    time_t timestamp = time(nullptr);
    tm *tm = localtime(&timestamp);

    char time[32];
    memset(time, 0, sizeof(time));
    strftime(time, sizeof(time), "%Y-%m-%d %H:%M:%S", tm);
    string filename = log_filename + time;

    // rename函数介绍
    // int rename ( const char * oldname, const char * newname );
    // 将oldname的文件名改为newname的文件名, 成功返回 0 , 不成功返回非0
    if (rename(log_filename.c_str(), filename.c_str()) != 0)
    {
        logic_error("rename log file failed " + string(strerror(errno)));
    }
    open(log_filename);
}