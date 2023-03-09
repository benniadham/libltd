#ifndef _LTD_INCLUDE_CLI_FLAGS_HPP_
#define _LTD_INCLUDE_CLI_FLAGS_HPP_

#include <variant>
#include <vector>
#include <string>
#include <map>

#include "err.hpp"
#include "std_alias.hpp"

namespace ltd
{
     /**
     * @brief
     * Provides helper functions for handling with command-line arguments.
     * 
     * @details
     * Class cli_flags implements command line arguments helper tools.
     * 
     * This class provides command line arguments parser and simple help system.
     * To use this class, initialize it with `argc` and `argv` taken from `main()`.
     * 
     * Bind local variable with an option by defining short option ('-'), the
     * corresponding long option ('--') and description for help text.
     * 
     * ```C++
     *          int main(int argc, char *argv[])
     *          {
     *              cli_flags args;
     *              int debug     = 0;
     *              int verbosity = 0;
     * 
     *              args.bind(&debug, 'g', "debug", "Build with debug information.");
     *              args.bind(&verbosity, 'v', "verbose", "Verbose logging.");
     *              args.parse(argc, argv);
     * 
     *              if (arguments.size() < 2)
     *              {
     *                  print_help();
     *                  arguments.print_help(4);
     *                  return 1;
     *              }
     * 
     *              return 0;
     *          }
     * ```
     */
    class cli_flags
    {
    private:
        struct option
        {
            using value_t = std::variant<int*, std::string*, std::vector<const char*>*>;

            std::string long_opt;
            std::string help;
            char        short_opt;
            value_t     value;
        };

        struct argument
        {
            std::vector<const char*> values;
            size_t occurence;
            char   short_opt;
        };

    private:
        int    argc;
        char** argv;

        std::vector<option>   options;
        std::vector<argument> arguments;

    private:
        ret<int, err::code> get_argument(char short_opt);
        ret<int, err::code> add_argument(char short_opt);

        err::code parse_argv();
        err::code bind_values();

    public:
        /**
         * @brief
         * Construct a new cli args object
         */
        cli_flags();

        /**
         * @brief
         * Binds a string to an option.
         * 
         * @details
         * Call this function to bind a string variable to an argument. If
         * the argument is provided in the cli command the variable will be
         * assigned with that value. If the option is not provided, then a
         * default value will be assigned to the variable.
         */
        err::code bind(int* value, char short_opt, const std::string& long_opt, const std::string& help);

        /**
         * @brief
         * Binds a string to an option.
         * 
         * @details
         * Call this function to bind a string variable to an argument. If
         * the argument is provided in the cli command the variable will be
         * assigned with that value. If the option is not provided, then a
         * default value will be assigned to the variable.
         */
        err::code bind(std::string* value, char short_opt, const std::string& long_opt, const std::string& help);

        /**
         * @brief
         * Binds a string to an option.
         * 
         * @details
         * Call this function to bind a string variable to an argument. If
         * the argument is provided in the cli command the variable will be
         * assigned with that value. If the option is not provided, then a
         * default value will be assigned to the variable.
         */
        err::code bind(std::vector<const char*>* value, char short_opt, const std::string& long_opt, const std::string& help);

        /**
         * @brief
         * Iterates all registered options
         * 
         * @param func Iterator callback
         */
        void  iterate_options(void (func)(std::variant<int*, std::string*, std::vector<const char*>*>, char, const std::string&, const std::string&));

        /**
         * @brief
         * Get the short opt object from a long option
         * 
         * @param long_opt  The long option
         * @return ret<char, error>
         */
        ret<char, err::code> get_short_opt(const std::string& long_opt) const;

        /**
         * @brief
         * Get the long opt object from short option
         * 
         * @param short_opt The short option
         * @return ret<const std::string&, error>
         */
        ret<const std::string&, err::code> get_long_opt(char short_opt) const;

        /**
         * @brief
         * Get the index object by short option
         * 
         * @param short_opt The short option
         * @return ret<int, error>
         */
        ret<int, err::code> get_index(char short_opt) const;

        /**
         * @brief
         * Retrieve argv value in the given index
         * 
         * @param index     The index of argv to retrieve
         * @return ret<const char*, err::code>
         */
        ret<const char*, err::code> at(size_t index) const;

        /**
         * @brief
         * Parse argc and argv for bindings
         * 
         * @param argc
         * @param argv
         * @return err::code
         */
        err::code parse(int argc, char** argv);

        /**
         * @brief
         * Get the `argc` value.
         * 
         * @return size_t The value of argc.
         */
        size_t size() const;

        /**
         * @brief
         * Prints options switches to terminal.
         * 
         * @details
         * Prints all short and long options into the terminal. This can be used
         * with help display.
         * 
         * @param indent Size of the indentation.
         */
        void print_help(size_t indent) const;

    }; // class cli_flags
} // namespace ltd

#endif // _LTD_INCLUDE_CLI_FLAGS_HPP_