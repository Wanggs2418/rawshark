#include <iostream>
#include "tshark_manager.h"
#include "third_library/loguru/loguru.hpp"

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#endif

void InitLog(int argc, char* argv[]) {
    // 初始化 Loguru
    loguru::init(argc, argv);

    // 设置日志文件路径
    loguru::add_file("app.log", loguru::Append, loguru::Verbosity_MAX);
}


int main(int argc, char* argv[]) {

    InitLog(argc, argv);

    TsharkManager tsharkManager("D:/easytshark_lessons/lesson06/lesson06");
    //tsharkManager.analysisFile("/Users/xuanyuan/Downloads/packets.pcap");
    //tsharkManager.printAllPackets();

    std::vector<AdapterInfo> adaptors = tsharkManager.getNetworkAdapters();
    for (auto item : adaptors) {
        LOG_F(INFO, "网卡[%d]: name[%s] remark[%s]", item.id, item.name.c_str(), item.remark.c_str());
    }

    return 0;
}