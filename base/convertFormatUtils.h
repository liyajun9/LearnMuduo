//
// Created by ralph on 4/10/20.
//

#ifndef LINUXSERVER_CONVERTFORMATUTILS_H
#define LINUXSERVER_CONVERTFORMATUTILS_H

#include <algorithm>

// TODO: better itoa.
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wtautological-compare"
#else
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif

namespace ybase {

constexpr char digits[] = "9876543210123456789";
static_assert(sizeof(digits) == 20, "wrong number of digits");
constexpr char digitsHex[] = "0123456789ABCDEF";
static_assert(sizeof digitsHex == 17, "wrong number of digitsHex");
extern const char* zero;

class convertFormatUtils {

// Efficient Integer to String Conversions, by Matthew Wilson.
public:
    /*
     Format a number with 5 characters, including SI units.
     [0,     999]
     [1.00k, 999k]
     [1.00M, 999M]
     [1.00G, 999G]
     [1.00T, 999T]
     [1.00P, 999P]
     [1.00E, inf)
    */
    static std::string formatSI(int64_t s);

    /*
     [0, 1023]
     [1.00Ki, 9.99Ki]
     [10.0Ki, 99.9Ki]
     [ 100Ki, 1023Ki]
     [1.00Mi, 9.99Mi]
    */
    static std::string formatIEC(int64_t s);

    template<typename T>
    static size_t convert(char buf[], T value)
    {
        T i = value;
        char* p = buf;

        do
        {
            int lsd = static_cast<int>(i % 10);
            i /= 10;
            *p++ = zero[lsd];
        } while (i != 0);

        if (value < 0)
        {
            *p++ = '-';
        }
        *p = '\0';
        std::reverse(buf, p);

        return p - buf;
    }

    static size_t convertHex(char buf[], uintptr_t value)
    {
        uintptr_t i = value;
        char* p = buf;

        do
        {
            int lsd = static_cast<int>(i % 16);
            i /= 16;
            *p++ = digitsHex[lsd];
        } while (i != 0);

        *p = '\0';
        std::reverse(buf, p);

        return p - buf;
    }

};

};

#endif //LINUXSERVER_CONVERTFORMATUTILS_H
