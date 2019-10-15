#include "config.hpp"

std::string config::getExecutePath() {
    return m_executePath;
}

void config::setExecutePath(std::string path) {
    if (m_executePath.empty()) {
        m_executePath = path;
    }
}

std::string config::m_executePath;
