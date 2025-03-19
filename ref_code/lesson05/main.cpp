#include <iostream>
#include "TSharkManager.h"
#include "loguru/loguru.hpp"

int main(int argc, char* argv[]) {
    system("chcp 65001");
    loguru::g_stderr_verbosity = loguru::Verbosity_MAX;
    loguru::init(argc, argv);
    loguru::add_file("logs.txt", loguru::Append, loguru::Verbosity_MAX);

//    LOG_F(INFO, "这是一条 INFO 日志");
//    LOG_F(WARNING, "这是一条 WARNING 日志");
//    LOG_F(ERROR, "这是一条 ERROR 日志");

    string workDir = "tshark";
    TSharkManager tSharkManager(workDir);
    string pcapFilePath = "D:/CLionProjects/file/wechat_login.pcap";
    tSharkManager.analysisFile(pcapFilePath);
    tSharkManager.printAllPackets();

    return 0;
}