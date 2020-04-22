#ifndef LEARNMUDUO_BASE_LOGGING_H
#define LEARNMUDUO_BASE_LOGGING_H

#include "logStream.h"
#include "timestamp.h"

namespace ybase
{

class TimeZone;

class Logger
{
 public:
  enum LogLevel
  {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    NUM_LOG_LEVELS,
  };

  // compile time calculation of basename of source file
  class SourceFile
  {
   public:
    template<int N>
    SourceFile(const char (&arr)[N])
      : m_data(arr)
      , m_size(N - 1)
    {
      const char* slash = strrchr(m_data, '/');
      if (slash){ //extract filename and filename length from full path
          m_data = slash + 1;
          m_size -= static_cast<int>(m_data - arr);
      }
    }

    explicit SourceFile(const char* filename)
      : m_data(filename)
    {
      const char* slash = strrchr(filename, '/');
      if (slash){
          m_data = slash + 1;
      }
        m_size = static_cast<int>(strlen(m_data));
    }

    const char* m_data;
    int m_size;
  };

  Logger(SourceFile file, int line);
  Logger(SourceFile file, int line, LogLevel level);
  Logger(SourceFile file, int line, LogLevel level, const char* func);
  Logger(SourceFile file, int line, bool toAbort);
  ~Logger();

  LogStream& stream() { return impl_.m_stream; }

  static LogLevel logLevel();
  static void setLogLevel(LogLevel level);

  using OutputFunc = void(*)(const char* msg, int len);
  using FlushFunc = void(*)();

  static void setOutput(OutputFunc);
  static void setFlush(FlushFunc);
  static void setTimeZone(const TimeZone& tz);

 private:

    class Impl
    {
     public:
      typedef Logger::LogLevel LogLevel;
      Impl(LogLevel level, int old_errno, const SourceFile& file, int line);
      void formatTime();
      void finish();

      Timestamp m_time;
      LogStream m_stream;
      LogLevel m_level;
      int m_line;
      SourceFile m_basename;
    };

  Impl impl_;

};



extern Logger::LogLevel g_logLevel;

inline Logger::LogLevel Logger::logLevel()
{
  return g_logLevel;
}

//
// CAUTION: do not write:
//
// if (good)
//   LOG_INFO << "Good news";
// else
//   LOG_WARN << "Bad news";
//
// this expends to
//
// if (good)
//   if (logging_INFO)
//     logInfoStream << "Good news";
//   else
//     logWarnStream << "Bad news";
//
#define LOG_TRACE if (ybase::Logger::logLevel() <= ybase::Logger::TRACE) \
  ybase::Logger(__FILE__, __LINE__, ybase::Logger::TRACE, __func__).stream()
#define LOG_DEBUG if (ybase::Logger::logLevel() <= ybase::Logger::DEBUG) \
  ybase::Logger(__FILE__, __LINE__, ybase::Logger::DEBUG, __func__).stream()
#define LOG_INFO if (ybase::Logger::logLevel() <= ybase::Logger::INFO) \
  ybase::Logger(__FILE__, __LINE__).stream()
#define LOG_WARN ybase::Logger(__FILE__, __LINE__, ybase::Logger::WARN).stream()
#define LOG_ERROR ybase::Logger(__FILE__, __LINE__, ybase::Logger::ERROR).stream()
#define LOG_FATAL ybase::Logger(__FILE__, __LINE__, ybase::Logger::FATAL).stream()
#define LOG_SYSERR ybase::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL ybase::Logger(__FILE__, __LINE__, true).stream()

const char* strerror_tl(int savedErrno);

// Taken from glog/logging.h
//
// Check that the input is not NULL.  This is very useful in constructor initializer lists.

#define CHECK_NOTNULL(val) \
  ::ybase::CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non NULL", (val))

// A small helper for CHECK_NOTNULL().
template <typename T>
T* CheckNotNull(Logger::SourceFile file, int line, const char *names, T* ptr)
{
  if (ptr == NULL)
  {
   Logger(file, line, Logger::FATAL).stream() << names;
  }
  return ptr;
}

}  // namespace ybase

#endif  // LEARNMUDUO__BASE_LOGGING_H
