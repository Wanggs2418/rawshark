//
// Created by dyunu on 2025/2/20.
//

#include "ip2region_util.h"
#include <string>
#include <vector>
#include <sstream>

std::shared_ptr<xdb_search_t> IP2RegionUtil::xdbPtr;

bool IP2RegionUtil::init(const std::string &xdbFilePath) {
    xdbPtr = std::make_shared<xdb_search_t>(xdbFilePath);
    xdbPtr->init_content();
    return true;
}

std::string IP2RegionUtil::getIPLocation(const std::string &ip) {
    if (!xdbPtr) {
        init("D:/CLionProjects/lesson05/thirdparty/ip2region/ip2region.xdb");
    }
    // if ip is ipv6, return empty string
    if (ip.size() > 15 || ip.empty()) {
        return "";
    }
    std::string location = xdbPtr->search(ip);
    if (!location.empty() && location.find("invalid") == std::string::npos) {
        return parseLocation(location);
    } else {
        return "";
    }
}

std::string IP2RegionUtil::parseLocation(const std::string &input) {
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(input);

    if (input.find("内网") != std::string::npos) {
        return "内网";
    }

    while (std::getline(ss, token, '|')) {
        tokens.push_back(token);
    }
    if (tokens.size() > 4) {
        std::string result;
        if (tokens[0].compare("0") != 0) {
            result.append(tokens[0]);
        }
        if (tokens[2].compare("0") != 0) {
            result.append("-" + tokens[2]);
        }
        if (tokens[3].compare("0") != 0) {
            result.append("-" + tokens[3]);
        }
        return result;
    } else {
        return input;
    }
}

