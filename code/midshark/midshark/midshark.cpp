#include <iostream>
#include <windows.h>

#include "tshark_manager.h"

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#endif


int main(int argc, char* argv[]) {

    //设置控制台输出编码
    SetConsoleOutputCP(CP_UTF8);
    TsharkManager tsharkManager("E:/03CS_learning/04CS_Reverse_Engineering/wireshark/rawshark/code/midshark/midshark");
    tsharkManager.analysisFile("C:/Users/Wanggs/demo1.pcap");
    tsharkManager.printAllPackets();

    return 0;
}