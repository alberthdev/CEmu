/*
 * Part of tivars_lib_cpp
 * (C) 2015-2016 Adrien "Adriweb" Bertrand
 * https://github.com/adriweb/tivars_lib_cpp
 * License: MIT
 */

#include "../autoloader.h"

using namespace std;

namespace tivars
{

    data_t TH_0x21::makeDataFromString(const string& str, const options_t& options)
    {
        (void)options;

        std::cerr << "Unimplemented" << std::endl;
        return data_t();

        if (str == "" || !is_numeric(str))
        {
            std::cerr << "Invalid input string. Needs to be a valid Exact Real Pi Fraction" << std::endl;
        }
    }

    string TH_0x21::makeStringFromData(const data_t& data, const options_t& options)
    {
        (void)options;

        if (data.size() != TH_0x21::dataByteCount)
        {
            std::cerr << ("Empty data array. Needs to contain " + to_string(TH_0x21::dataByteCount) + " bytes") << std::endl;
            return "";
        }

        string coeff = TH_0x00::makeStringFromData(data);

        string str = ((coeff != "0") ? (dec2frac(atof(coeff.c_str())) + "*π") : "");

        // Improve final display
        str = regex_replace(str, regex("\\+1\\*"), "+");  str = regex_replace(str, regex("\\(1\\*"),  "(");
        str = regex_replace(str, regex("-1\\*"),   "-");  str = regex_replace(str, regex("\\(-1\\*"), "(-");
        str = regex_replace(str, regex("\\+-"),    "-");

        // Shouldn't happen - I don't believe the calc generate such files.
        if (str == "")
        {
            str = "0";
        }

        return str;
    }

}
