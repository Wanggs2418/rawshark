//
// Created by dyunu on 2025/2/22.
//

#ifndef LESSON05_TSHARKMANAGER_H
#define LESSON05_TSHARKMANAGER_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <ctime>
#include <iomanip>

#include "tshark_datatype.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "ip2region/xdb_search.h"
#include "loguru/loguru.hpp"

using std::string;
using std::vector;
using std::unordered_map;
using std::shared_ptr;
using std::stringstream;

class TSharkManager {
public:
    explicit TSharkManager(const string& tsharkExecutablePath);
    ~TSharkManager();

    // 分析数据包文件
    bool analysisFile(const string& filePath);

    // 打印所有数据包信息
    void printAllPackets();

    // 获取指定编号的数据包的十六进制数据
    bool getPacketHexData(uint32_t frameNumber, vector<unsigned char> &data);

private:
    // 解析每一行，存入Packet对象中
    bool parseLine(string line, shared_ptr<Packet> packet);

    string tsharkPath;

    // 当前分析的文件路径
    string currentFilePath;

    // 分析得到的所有数据包信息，key是数据包id，value是数据包信息指针，方便根据编号获取指定数据包信息
    unordered_map<uint32_t, shared_ptr<Packet>> allPackets;
};


#endif //LESSON05_TSHARKMANAGER_H
