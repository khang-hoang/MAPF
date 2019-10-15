#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include <iostream>

class config {
private:
    static std::string m_executePath;
public:
    static std::string getExecutePath();
    static void setExecutePath(std::string);
};

#endif // __CONFIG_HPP__
