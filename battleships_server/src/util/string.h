#pragma once

namespace std
{
inline std::vector<std::string> split(std::string s, const std::string& delimiter)
{
    std::vector<std::string> tokens;

    size_t pos = s.find(delimiter);
    while (pos != std::string::npos)
    {
        tokens.push_back(s.substr(0, pos));
        s.erase(0, pos + delimiter.length());
        
        pos = s.find(delimiter);
    }

    if (!s.empty())
        tokens.push_back(s);

    return tokens;
}
}