//
// Created by ralph on 4/10/20.
//

#include "convertFormatUtils.h"
#include <cinttypes>

namespace ybase{
const char* zero = digits + 9;

std::string convertFormatUtils::formatSI(int64_t s)
{
    double n = static_cast<double>(s);
    char buf[64];
    if (s < 1000)
        snprintf(buf, sizeof(buf), "%" PRId64, s);
    else if (s < 9995)
        snprintf(buf, sizeof(buf), "%.2fk", n/1e3);
    else if (s < 99950)
        snprintf(buf, sizeof(buf), "%.1fk", n/1e3);
    else if (s < 999500)
        snprintf(buf, sizeof(buf), "%.0fk", n/1e3);
    else if (s < 9995000)
        snprintf(buf, sizeof(buf), "%.2fM", n/1e6);
    else if (s < 99950000)
        snprintf(buf, sizeof(buf), "%.1fM", n/1e6);
    else if (s < 999500000)
        snprintf(buf, sizeof(buf), "%.0fM", n/1e6);
    else if (s < 9995000000)
        snprintf(buf, sizeof(buf), "%.2fG", n/1e9);
    else if (s < 99950000000)
        snprintf(buf, sizeof(buf), "%.1fG", n/1e9);
    else if (s < 999500000000)
        snprintf(buf, sizeof(buf), "%.0fG", n/1e9);
    else if (s < 9995000000000)
        snprintf(buf, sizeof(buf), "%.2fT", n/1e12);
    else if (s < 99950000000000)
        snprintf(buf, sizeof(buf), "%.1fT", n/1e12);
    else if (s < 999500000000000)
        snprintf(buf, sizeof(buf), "%.0fT", n/1e12);
    else if (s < 9995000000000000)
        snprintf(buf, sizeof(buf), "%.2fP", n/1e15);
    else if (s < 99950000000000000)
        snprintf(buf, sizeof(buf), "%.1fP", n/1e15);
    else if (s < 999500000000000000)
        snprintf(buf, sizeof(buf), "%.0fP", n/1e15);
    else
        snprintf(buf, sizeof(buf), "%.2fE", n/1e18);
    return buf;
}

std::string convertFormatUtils::formatIEC(int64_t s)
{
    double n = static_cast<double>(s);
    char buf[64];
    const double Ki = 1024.0;
    const double Mi = Ki * 1024.0;
    const double Gi = Mi * 1024.0;
    const double Ti = Gi * 1024.0;
    const double Pi = Ti * 1024.0;
    const double Ei = Pi * 1024.0;

    if (n < Ki)
        snprintf(buf, sizeof buf, "%" PRId64, s);
    else if (n < Ki*9.995)
        snprintf(buf, sizeof buf, "%.2fKi", n / Ki);
    else if (n < Ki*99.95)
        snprintf(buf, sizeof buf, "%.1fKi", n / Ki);
    else if (n < Ki*1023.5)
        snprintf(buf, sizeof buf, "%.0fKi", n / Ki);

    else if (n < Mi*9.995)
        snprintf(buf, sizeof buf, "%.2fMi", n / Mi);
    else if (n < Mi*99.95)
        snprintf(buf, sizeof buf, "%.1fMi", n / Mi);
    else if (n < Mi*1023.5)
        snprintf(buf, sizeof buf, "%.0fMi", n / Mi);

    else if (n < Gi*9.995)
        snprintf(buf, sizeof buf, "%.2fGi", n / Gi);
    else if (n < Gi*99.95)
        snprintf(buf, sizeof buf, "%.1fGi", n / Gi);
    else if (n < Gi*1023.5)
        snprintf(buf, sizeof buf, "%.0fGi", n / Gi);

    else if (n < Ti*9.995)
        snprintf(buf, sizeof buf, "%.2fTi", n / Ti);
    else if (n < Ti*99.95)
        snprintf(buf, sizeof buf, "%.1fTi", n / Ti);
    else if (n < Ti*1023.5)
        snprintf(buf, sizeof buf, "%.0fTi", n / Ti);

    else if (n < Pi*9.995)
        snprintf(buf, sizeof buf, "%.2fPi", n / Pi);
    else if (n < Pi*99.95)
        snprintf(buf, sizeof buf, "%.1fPi", n / Pi);
    else if (n < Pi*1023.5)
        snprintf(buf, sizeof buf, "%.0fPi", n / Pi);

    else if (n < Ei*9.995)
        snprintf(buf, sizeof buf, "%.2fEi", n / Ei );
    else
        snprintf(buf, sizeof buf, "%.1fEi", n / Ei );
    return buf;
}

} //namespace ybase
