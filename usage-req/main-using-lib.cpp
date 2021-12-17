#include "simplestrutils/simplestrutils.hpp"
#include <iostream>
#include <string>

int main(int argc, char * argv[])
{
    {
        std::string s("abc123ABC");
        std::cout << s << ": " << std::boolalpha
            << simplestrutils::is_all_alphanumeric(s) << std::endl;
    }

    {
        std::string s("a1+-A");
        std::cout << s << ": " << std::boolalpha
            << simplestrutils::is_all_alphanumeric(s) << std::endl;
    }

    return 0;
}
