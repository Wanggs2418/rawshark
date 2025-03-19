#include <iostream>
#include <windows.h>

#include "tshark_manager.h"
#include "third_library/loguru/loguru.hpp" // 引入日志系统

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#endif

// 初始化日志系统
void initLoguru(int argc, char* argv[]) {
	loguru::init(argc, argv);
	loguru::add_file("midshark.log", loguru::Append, loguru::Verbosity_MAX);
}


int main(int argc, char* argv[]) {

	//设置控制台输出编码
	SetConsoleOutputCP(CP_UTF8);
	//system("chcp 65001");

	// 初始化日志系统
	initLoguru(argc, argv);


    TsharkManager tsharkManager("E:/03CS_learning/04CS_Reverse_Engineering/wireshark/rawshark/code/midshark/midshark");
    tsharkManager.analysisFile("C:/Users/Wanggs/demo1.pcap");
    tsharkManager.printAllPackets();

    return 0;
}