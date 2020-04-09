//
// Created by ralph on 4/4/20.
//

#ifndef LINUXSERVER_EXCEPTION_H
#define LINUXSERVER_EXCEPTION_H

namespace ybase {

    class Exception : public std::exception {
    public:
        explicit Exception(const char *message = "") noexcept;

        ~Exception() noexcept override = default;

        const char *what() const noexcept override {
            return m_message.c_str();
        }

        const char *stackTrace() const noexcept {
            return m_stackTrace.c_str();
        }

    protected:
        std::string m_message;
        std::string m_stackTrace;
    };

} //namespace ybase
#endif //LINUXSERVER_EXCEPTION_H
