#include "../include/ltd/err.hpp"

namespace ltd
{
    namespace err
    {
        std::string last_error;

        const char* to_string(code err_code)
        {
            return std_message[err_code];
        }

        void set_last_error(const std::string& message)
        {
            last_error = message;
        }

        std::string get_last_error()
        {
            return last_error;
        }
    }

} // namespace ltd
