// rawsahrk.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//注意将开源库复制到 `include` 文件夹下： `D:\Visual Studio2022\VC\Tools\MSVC\14.43.34808\include`
#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <sstream>
#include <windows.h>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

//定义数据包结构体
struct Packet {
    int frame_number = -1;   //数据包编号
    std::string time;   //数据包时间戳
    std::string src_ip; //源IP地址
    std::string dst_ip; //目的IP
    std::string protocol;   //协议
    std::string info;   //数据包概要信息
};

//编写解析函数，针对输出的每一行，解析为一个 packet 结构体
void parseLine(std::string line, Packet& packet);

//将Packet对象转换为JSON打印输出
void printPacket(const Packet& packet);

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    //创建管道，r-从子进程读取数据
    const char* command = "E:/Wireshark4.2.6/Wireshark/tshark -r C:/Users/Wanggs/capture.pcap -T fields -e frame.number -e frame.time -e ip.src -e ip.dst -e _ws.col.Protocol -e _ws.col.Info";
    FILE* pipe = _popen(command, "r");
    if (!pipe) {
        std::cerr << "tshark failed" << std::endl;
        return 1;
    }
    
	std::vector<Packet> packets;
    char buffer[1024];
    
    //fgets标准库函数，定义在stdio.h中，从文件流中读取一行文本数据
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        //std::cout << buffer;
		Packet packet;
		parseLine(buffer, packet);
		packets.push_back(packet);
    }

	// C++11引入的新式for循环，通过auto关键子自动推断类型
    for (auto& p : packets) {
		printPacket(p);
        printf("frame_number: %d time: %s src_ip: %s dst_ip: %s protocol: %s info: %s\n",
            p.frame_number,
            p.time.c_str(),
            p.src_ip.c_str(),
            p.dst_ip.c_str(),
            p.protocol.c_str(),
            p.info.c_str());
    }
    //关闭管道，获取子进程的退出状态
    _pclose(pipe);
    return 0;
}

void parseLine(std::string line, Packet& packet) {
    //将行末尾的换行符去掉
    if (line.back() == '\n') {
        line.pop_back();
    }
    //stringstream是C++标准库中的一个类，允许把字符串当作输入输出流来处理
    std::stringstream ss(line);
    std::string field;
    std::vector<std::string> fields;

    // getline是C++提供的函数，用于从输入流中读取一行数据
    while (std::getline(ss, field, '\t')) {
        fields.push_back(field);
    }

    // 拆分字段保存到vector容器中，最后按照顺序赋值Packet 各个字段
    if (fields.size() >= 6) {
        packet.frame_number = std::stoi(fields[0]);
        packet.time = fields[1];
        packet.src_ip = fields[2];
        packet.dst_ip = fields[3];
        packet.protocol = fields[4];
        packet.info = fields[5];
    }

}

void printPacket(const Packet& packet) {
    //构建JSON对象
    rapidjson::Document pktObj;
    rapidjson::Document::AllocatorType& allocator = pktObj.GetAllocator();

    //设置JSON为Object对象类型
	pktObj.SetObject();

    //添加JSON字段
	pktObj.AddMember("frame_number", packet.frame_number, allocator);
	pktObj.AddMember("timestamp", rapidjson::Value(packet.time.c_str(), allocator), allocator);
	pktObj.AddMember("src_ip", rapidjson::Value(packet.src_ip.c_str(), allocator), allocator);
	pktObj.AddMember("dst_ip", rapidjson::Value(packet.dst_ip.c_str(), allocator), allocator);
	pktObj.AddMember("protocol", rapidjson::Value(packet.protocol.c_str(), allocator), allocator);
	pktObj.AddMember("info", rapidjson::Value(packet.info.c_str(), allocator), allocator);
	
    //序列化为 JSON 字符串
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	pktObj.Accept(writer);

    //打印JSON输出
	std::cout << buffer.GetString() << std::endl;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单
// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
