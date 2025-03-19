//
// Created by dyunu on 2025/2/20.
//

#ifndef IP2REGIONUTIL_H
#define IP2REGIONUTIL_H

#include <string>
#include <memory>
#include "thirdparty/ip2region/xdb_search.h"

class IP2RegionUtil {
public:
    static bool init(const std::string& xdbFilePath);
    static std::string getIPLocation(const std::string& ip);

private:
    static std::string parseLocation(const std::string& input);
    static std::shared_ptr<xdb_search_t> xdbPtr;
};

#endif //IP2REGIONUTIL_H
