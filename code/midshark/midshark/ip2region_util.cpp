#include "ip2region_util.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

std::shared_ptr<xdb_search_t> IP2RegionUtil::xdbPtr;

// 初始化
bool IP2RegionUtil::init(const std::string& xdbFilePath) {

    xdbPtr = std::make_shared<xdb_search_t>(xdbFilePath);
    xdbPtr->init_content();
    return true;
}

// 获取IP地址的地理位置
std::string IP2RegionUtil::getIpLocation(const std::string& ip) {

    //if is IPv6, return empty string
    if (ip.size() > 15) {
        return "";
    }

	// 如果location不为空且不包含invalid，返回location
    std::string location = xdbPtr->search(ip);
    if (!location.empty() && location.find("invalid") == std::string::npos) {
        return parseLocation(location);
    }
    else {
        return "";
    }
}

// 解析地理位置
std::string IP2RegionUtil::parseLocation(const std::string& input) {
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(input);

    if (input.find("内网") != std::string::npos) {
        return "内网";
    }

    // 以"|"为分隔符，将input分割成多个字符串
    while (std::getline(ss, token, '|')) {
        tokens.push_back(token);
    }

    // 如果tokens[0]不等于"0"，将tokens[0]拼接到result中
    if (tokens.size() >= 4) {
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
    }
    else {
        return input;
    }
}