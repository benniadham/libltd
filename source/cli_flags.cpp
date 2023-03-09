#include <iostream>
#include <cstring>
#include <algorithm>

#include "../include/ltd/cli_flags.hpp"
#include "../include/ltd/fmt.hpp"

namespace ltd
{
    cli_flags::cli_flags()
    {}

    err::code cli_flags::bind(int* value, char short_opt, const std::string& long_opt, const std::string& help)
    {
        option opt;

        opt.value     = value;
        opt.short_opt = short_opt;
        opt.long_opt  = long_opt;
        opt.help      = help;

        options.push_back(opt);
        return err::no_error;
    }

    err::code cli_flags::bind(std::string* value, char short_opt, const std::string& long_opt, const std::string& help)
    {
        option opt;

        opt.value     = value;
        opt.short_opt = short_opt;
        opt.long_opt  = long_opt;
        opt.help      = help;

        options.push_back(opt);
        return err::no_error;
    }

    err::code cli_flags::bind(std::vector<const char*>* value, char short_opt, const std::string& long_opt, const std::string& help)
    {
        option opt;

        opt.value     = value;
        opt.short_opt = short_opt;
        opt.long_opt  = long_opt;
        opt.help      = help;

        options.push_back(opt);
        return err::no_error;
    }

    void cli_flags::iterate_options(void (func)(std::variant<int*, std::string*, std::vector<const char*>*>, char, const std::string&, const std::string&))
    {
        for (auto opt : options) {
            func(opt.value, opt.short_opt, opt.long_opt, opt.help);
        }
    }

    ret<char, err::code> cli_flags::get_short_opt(const std::string& long_opt) const
    {
        auto it = std::find_if(options.begin(), options.end(), [&long_opt](const option& opt) -> bool {
            return opt.long_opt == long_opt;
        });

        if (it == options.end())
            return {' ', err::not_found};

        return {(*it).short_opt, err::no_error};
    }

    ret<const std::string&, err::code> cli_flags::get_long_opt(char short_opt) const
    {
        auto it = std::find_if(options.begin(), options.end(), [short_opt](const option& opt) -> bool {
            return opt.short_opt == short_opt;
        });

        if (it == options.end())
            return {"", err::not_found};

        return {(*it).long_opt, err::no_error};
    }

    ret<int, err::code> cli_flags::get_index(char short_opt) const
    {
        int index = 0;

        for (auto opt : options) {
            if (opt.short_opt == short_opt)
                return {index, err::no_error};

            index++;
        }

        return {-1, err::not_found};
    }

    ret<const char*, err::code> cli_flags::at(size_t index) const
    {
        if (index < argc)
            return {argv[index], err::no_error};

        return {nullptr, err::index_out_of_bound};
    }

    ret<int, err::code> cli_flags::get_argument(char short_opt)
    {
        int index = 0;

        for (auto arg : arguments) {
            if (arg.short_opt == short_opt)
                return {index, err::no_error};

            index++;
        }

        return {-1, err::not_found};
    }

    ret<int, err::code> cli_flags::add_argument(char short_opt)
    {
        auto [index, err] = get_argument(short_opt);

        if (err == err::not_found) {
            argument arg;
            arg.short_opt = short_opt;
            arg.occurence = 0;

            arguments.push_back(arg);

            return {arguments.size() - 1, err::no_error};
        }

        return {index, err};
    }

    err::code cli_flags::parse_argv()
    {
        for (int i = 1; i < argc; i++) {
            int index = -1;

            if (argv[i][0] == '-') {
                if (argv[i][1] == '-') {
                    auto [short_opt, err] = get_short_opt(&argv[i][2]);

                    if (err != err::no_error)
                        return err;

                    catch_ret(index, err) = add_argument(short_opt);

                    if (err != err::no_error)
                        return err;

                    arguments[index].occurence++;
                } else {
                    for (int j = 1; argv[i][j] != '\0'; j++) {
                        err::code err = err::no_error;

                        auto short_opt = argv[i][j];
                        catch_ret(index, err) = add_argument(short_opt);

                        if (err != err::no_error)
                            return err;

                        arguments[index].occurence++;
                    }
                }

                // at this point our index should point to an option that might expect one or more value
                while (i+1 < argc && argv[i+1][0] != '-')
                    arguments[index].values.push_back(argv[++i]);
            }
        }

        return err::no_error;
    }

    err::code cli_flags::bind_values()
    {
        for (auto arg : arguments) {
            auto [index, err] = get_index(arg.short_opt);

            if (err != err::no_error)
                return err;

            if (std::holds_alternative<int*>(options[index].value)) {
                auto val_ptr = std::get<int*>(options[index].value);

                if (arg.values.size()==0) {
                    *val_ptr = arg.occurence;
                } else {
                    std::string value = arg.values[0];
                    bool isnumber = std::all_of(value.begin(), value.end(), ::isdigit);
                    // TODO: Why isnumber is not tested?
                    *val_ptr = std::stol(value);
                }
            } else if (std::holds_alternative<std::string*>(options[index].value)) {
                auto val_ptr = std::get<std::string*>(options[index].value);

                if (arg.values.size()!=0)
                    *val_ptr = arg.values[0];
                else
                    return err::not_found;
            } else if (std::holds_alternative<std::vector<const char*>*>(options[index].value)) {
                auto val_ptr = std::get<std::vector<const char*>*>(options[index].value);

                for (auto str_val : arg.values)
                    val_ptr->push_back(str_val);
            } else {
                // TODO: Fix this error status with proper one
                return err::type_conversion_failure;
            }
        }

        return err::no_error;
    }

    err::code cli_flags::parse(int argc, char** argv)
    {
        this->argc = argc;
        this->argv = argv;

        parse_argv();
        bind_values();

        return err::no_error;
    }

    size_t cli_flags::size() const
    {
        return argc;
    }

    void cli_flags::print_help(size_t indent) const
    {
        std::string indentation;

        for (int i=0; i<indent; i++)
            indentation += " ";

        for (auto opt : options) {
            std::string line;
            line += indentation;
            line += "-";
            line += opt.short_opt;
            line += ", --";
            line += opt.long_opt;

            if (line.length()-indent < 12)
                line += "\t\t";
            else
                line += "\t";

            line += opt.help;
            std::cout << line << std::endl;
        }
    }
} // namepsace ltd