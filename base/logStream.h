#ifndef LEARNMUDUO_BASE_LOGSTREAM_H
#define LEARNMUDUO_BASE_LOGSTREAM_H

#include "stringPiece.h"
#include "typeUtils.h"
#include <cassert>
#include <cstring>

namespace ybase
{

constexpr int kSmallBuffer = 4000;
constexpr int kLargeBuffer = 4000*1000;

template<int SIZE>
class FixedBuffer
{
 public:
  FixedBuffer()
    : m_curPos(m_data)
  {
    setCookie(cookieStart);
  }

  ~FixedBuffer()
  {
    setCookie(cookieEnd);
  }

  void append(const char* buf, size_t len)
  {
    // FIXME: append partially
    if (implicit_cast<size_t>(avail()) > len)
    {
      memcpy(m_curPos, buf, len);
        m_curPos += len;
    }
  }

  const char* data() const { return m_data; }
  int length() const { return static_cast<int>(m_curPos - m_data); } //current data length

  // write to m_data directly
  char* current() { return m_curPos; }
  int avail() const { return static_cast<int>(end() - m_curPos); }
  void add(size_t len) { m_curPos += len; }

  void reset() { m_curPos = m_data; }
  void bzero() { memZero(m_data, sizeof m_data); }

  // for used by GDB
  const char* debugString();
  void setCookie(void (*cookie)()) { m_cookie = cookie; }

  // for used by unit test
  std::string toString() const { return string(m_data, length()); }
  StringPiece toStringPiece() const { return StringPiece(m_data, length()); }

 private:
  const char* end() const { return m_data + sizeof m_data; } //sentinel
  // Must be outline function for cookies.
  static void cookieStart();
  static void cookieEnd();

  void (*m_cookie)(){};
  char m_data[SIZE]{};
  char* m_curPos;
};

/* a FixedLengthBuffer of size kSmallBuffer
 *
 */
class LogStream
{
  typedef LogStream self;

 public:
  typedef FixedBuffer<kSmallBuffer> Buffer;

  self& operator<<(bool v)
  {
    m_buffer.append(v ? "1" : "0", 1);
    return *this;
  }

  self& operator<<(short);
  self& operator<<(unsigned short);
  self& operator<<(int);
  self& operator<<(unsigned int);
  self& operator<<(long);
  self& operator<<(unsigned long);
  self& operator<<(long long);
  self& operator<<(unsigned long long);

  self& operator<<(const void*);

  self& operator<<(float v)
  {
    *this << static_cast<double>(v);
    return *this;
  }
  self& operator<<(double);
  // self& operator<<(long double);

  self& operator<<(char v)
  {
    m_buffer.append(&v, 1);
    return *this;
  }

  // self& operator<<(signed char);
  // self& operator<<(unsigned char);

  self& operator<<(const char* str)
  {
    if (str)
    {
      m_buffer.append(str, strlen(str));
    }
    else
    {
      m_buffer.append("(null)", 6);
    }
    return *this;
  }

  self& operator<<(const unsigned char* str)
  {
    return operator<<(reinterpret_cast<const char*>(str));
  }

  self& operator<<(const std::string& v)
  {
    m_buffer.append(v.c_str(), v.size());
    return *this;
  }

  self& operator<<(const StringPiece& v)
  {
    m_buffer.append(v.data(), v.size());
    return *this;
  }

  self& operator<<(const Buffer& v)
  {
    *this << v.toStringPiece();
    return *this;
  }

  void append(const char* data, int len) { m_buffer.append(data, len); }
  const Buffer& buffer() const { return m_buffer; }
  void resetBuffer() { m_buffer.reset(); }

 private:
  void staticCheck(); //check kMaxNumericSize can contain string form of (double, long, long double, long long)

  template<typename T>
  void formatInteger(T);

  Buffer m_buffer;

  static const int kMaxNumericSize = 32;
};

/* encapsulate a format string (can only accept one value)
 *
 */
class Fmt
{
NonCopyable(Fmt)
 public:
  template<typename T>
  Fmt(const char* fmt, T val);

  const char* data() const { return m_buf; }
  int length() const { return m_length; }

 private:
  char m_buf[32];
  int m_length;
};

inline LogStream& operator<<(LogStream& s, const Fmt& fmt)
{
  s.append(fmt.data(), fmt.length());
  return s;
}


}  // namespace ybase

#endif  // LEARNMUDUO_BASE_LOGSTREAM_H
