#include "logging.h"

#include "threadUtils.h"
#include "timestamp.h"
#include "timeZone.h"
#include <cerrno>
#include <cstdio>
#include <cstring>

#include <sstream>
#include <sys/syslog.h>

namespace ybase
{

thread_local char tls_errnoString[512];
thread_local char tls_timeString[64];
thread_local time_t tls_lastSecond;

const char* strerror_tl(int savedErrno)
{
  return strerror_r(savedErrno, tls_errnoString, sizeof tls_errnoString);
}

Logger::LogLevel g_logLevel = Logger::INFO;

const char* LogLevelName[Logger::NUM_LOG_LEVELS] =
{
  "TRACE ",
  "DEBUG ",
  "INFO  ",
  "WARN  ",
  "ERROR ",
  "FATAL ",
};

// helper class for known string length at compile time
class T
{
 public:
  T(const char* str, unsigned len)
    :str_(str),
     len_(len)
  {
    assert(strlen(str) == len_);
  }

  const char* str_;
  const unsigned len_;
};

inline LogStream& operator<<(LogStream& s, T v) //output content
{
  s.append(v.str_, v.len_);
  return s;
}

inline LogStream& operator<<(LogStream& s, const Logger::SourceFile& v) //output filename
{
  s.append(v.m_data, v.m_size);
  return s;
}

void defaultOutput(const char* msg, int len)
{
  size_t n = fwrite(msg, 1, len, stdout); //write msg with length len to stdout
  //FIXME check n
  (void)n;
}

void sysLogOutput(const char* msg, int len)
{
    syslog(LOG_INFO, msg);
}

void defaultFlush()
{
  fflush(stdout);
}

#ifdef LOGTO_STDOUT
Logger::OutputFunc g_output = defaultOutput;
#else
Logger::OutputFunc g_output = sysLogOutput;
#endif
Logger::FlushFunc g_flush = defaultFlush;
TimeZone g_logTimeZone;

Logger::Impl::Impl(LogLevel level, int savedErrno, const SourceFile& file, int line)
  : m_time(Timestamp::now()),
    m_stream(),
    m_level(level),
    m_line(line),
    m_basename(file)
{
  formatTime();
  ThreadUtils::getTid();
  m_stream << T(ThreadUtils::getTidString(), ThreadUtils::getTidStringLength());
  m_stream << T(LogLevelName[level], 6);
  if (savedErrno != 0) {
    m_stream << strerror_tl(savedErrno) << " (errno=" << savedErrno << ") ";
  }
}

void Logger::Impl::formatTime()
{
  int64_t microSecondsSinceEpoch = m_time.microSecondsSinceEpoch();
  time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / Timestamp::kMicroSecondsPerSecond);
  int microseconds = static_cast<int>(microSecondsSinceEpoch % Timestamp::kMicroSecondsPerSecond);
  if (seconds != tls_lastSecond)
  {
    tls_lastSecond = seconds;
    struct tm tm_time;
    if (g_logTimeZone.valid())
    {
      tm_time = g_logTimeZone.toLocalTime(seconds);
    }
    else
    {
      ::gmtime_r(&seconds, &tm_time); // FIXME TimeZone::fromUtcTime
    }

    int len = snprintf(tls_timeString, sizeof(tls_timeString), "%4d%02d%02d %02d:%02d:%02d",
        tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                       tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    assert(len == 17); (void)len;
  }

  if (g_logTimeZone.valid())
  {
    Fmt us(".%06d ", microseconds);
    assert(us.length() == 8);
    m_stream << T(tls_timeString, 17) << T(us.data(), 8);
  }
  else
  {
    Fmt us(".%06dZ ", microseconds);
    assert(us.length() == 9);
    m_stream << T(tls_timeString, 17) << T(us.data(), 9);
  }
}

void Logger::Impl::finish()
{
  m_stream << " - " << m_basename << ':' << m_line << '\n';
}

Logger::Logger(SourceFile file, int line)
  : impl_(INFO, 0, file, line)
{
}

Logger::Logger(SourceFile file, int line, LogLevel level, const char* func)
  : impl_(level, 0, file, line)
{
  impl_.m_stream << func << ' ';
}

Logger::Logger(SourceFile file, int line, LogLevel level)
  : impl_(level, 0, file, line)
{
}

Logger::Logger(SourceFile file, int line, bool toAbort)
  : impl_(toAbort?FATAL:ERROR, errno, file, line)
{
}

Logger::~Logger()
{
  impl_.finish();
  const LogStream::Buffer& buf(stream().buffer());
  g_output(buf.data(), buf.length());
  if (impl_.m_level == FATAL)
  {
    g_flush();
    abort();
  }
}

void Logger::setLogLevel(Logger::LogLevel level)
{
  g_logLevel = level;
}

void Logger::setOutput(OutputFunc out)
{
  g_output = out;
}

void Logger::setFlush(FlushFunc flush)
{
  g_flush = flush;
}

void Logger::setTimeZone(const TimeZone& tz)
{
  g_logTimeZone = tz;
}

}  // namespace ybase