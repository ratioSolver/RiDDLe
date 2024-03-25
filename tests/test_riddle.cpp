#include <fstream>
#include <cassert>
#include "parser.hpp"

int main(int argc, char const *argv[])
{
    // the problem files..
    std::vector<std::string> prob_names;
    for (int i = 1; i < argc - 1; i++)
        prob_names.push_back(argv[i]);

    for (const auto &f : prob_names)
        if (std::ifstream ifs(f); ifs)
        {
            riddle::parser prs(ifs);
            auto cu = prs.parse();
        }
    return 0;
}
