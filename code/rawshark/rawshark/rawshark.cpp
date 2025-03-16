// rawsahrk.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//注意将开源库复制到 `include` 文件夹下： `D:\Visual Studio2022\VC\Tools\MSVC\14.43.34808\include`
#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib") 
#endif
#include "ip2region_util.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <sstream>
#include <format>
#include <windows.h>
#include <fstream>
#include <stdint.h>

//定义数据包结构体
// 字段顺序：-e frame.number -e frame.time -e frame.cap_len -e ip.src -e ipv6.src -e ip.dst -e ipv6.dst
    // -e tcp.srcport -e udp.srcport -e tcp.dstport -e udp.dstport -e _ws.col.Protocol -e _ws.col.Info
    // 0: frame.number
    // 1: frame.time
    // 2: frame.cap_len
    // 3: ip.src
    // 4: ipv6.src
    // 5: ip.dst
    // 6: ipv6.dst
    // 7: tcp.srcport
    // 8: udp.srcport
    // 9: tcp.dstport
    // 10: udp.dstport
    // 11: _ws.col.Protocol
    // 12: _ws.col.Info

struct Packet {
    int frame_number;   //数据包编号
    std::string time;   //数据包时间戳
	uint32_t cap_len;   //捕获的数据包长度
    std::string src_ip; //源IP地址
    uint16_t src_port;
    std::string dst_ip; //目的IP
    uint16_t dst_port;
    std::string protocol;   //协议
    std::string info;   //数据包概要信息
	uint32_t file_offset;   //文件偏移
	std::string src_location;   //源IP地理位置
	std::string dst_location;   //目的IP地理位置
};

// PCAP Gloabal Header
struct PcapHeader {
    uint32_t magic_number;	// 标识文件格式，无符号32位整数,0xa1b2c3d4大端
    uint16_t version_major;	// PCAP文件版本号，一般为2.4
    uint16_t version_minor;
    int32_t thiszone;		// 时区偏移，一般为0，有符号32位整数
    uint32_t sigfigs;		// 时间戳精度，通常为0
    uint32_t snaplen;		// 捕获数据包最大长度，通常为65535
    uint32_t network;		// 链路层类型，0x01表示以太网
};

// PCAP Packet Header
struct PacketHeader {
    uint32_t ts_sec;	// 数据包捕获的时间戳(s)
    uint32_t ts_usec;	// 数据包捕获的时间戳(μs)微秒
    uint32_t caplen;	// 捕获的数据包长度
    uint32_t len;		// 数据包原始长度
};

//字符串转数字
int port2Int(std::string port);

//编写解析函数，针对输出的每一行，解析为一个 packet 结构体
bool parseLine(std::string line, Packet& packet);

//将Packet对象转换为JSON打印输出
void toJson(const Packet& packet);

//将Packet结构体转换为字符串
void toString(Packet& packet);

// 根据偏移和长度，指定文件的数据到vector中
bool readPacketHex(const std::string& filePath, uint32_t offset, uint32_t length, std::vector<unsigned char>& buffer);


// 智能类型指针
std::shared_ptr<xdb_search_t> IP2RegionUtil::xdbPtr = nullptr;
int main()
{
    //设置控制台输出编码
    SetConsoleOutputCP(CP_UTF8);
    //创建管道，r-从子进程读取数据
	//注意tshark提取的字段数，>=8，否则输出空字段
	std::string tshark_loc = "E:/Wireshark4.2.6/Wireshark/tshark";
	std::string pcap_file = "C:/Users/Wanggs/demo1.pcap";
	std::string fields = "-T fields -e frame.number -e frame.time -e frame.cap_len -e ip.src -e ipv6.src -e ip.dst -e ipv6.dst -e tcp.srcport -e udp.srcport -e tcp.dstport -e udp.dstport -e _ws.col.Protocol -e _ws.col.Info";

	std::string command = tshark_loc + " -r " + pcap_file + " " + fields;

	//command.c_str()将command对象（一个std::string类型的字符串）转换为C风格的字符串（const char* 类型）
    FILE* pipe = _popen(command.c_str(), "r");
    //FILE* pipe = _popen(command, "r");
    if (!pipe) {
        std::cerr << "tshark failed" << std::endl;
        return 1;
    }
    
	std::vector<Packet> packets;
    char buffer[4096];
	uint32_t file_offset = sizeof(PcapHeader);  //全局文件头偏移
    IP2RegionUtil ip2RegionUtil; // IP地址解析初始化
    ip2RegionUtil.init("E:\\03CS_learning\\04CS_Reverse_Engineering\\wireshark\\rawshark\\third_library\\ip2region\\ip2region.xdb");

    //fgets标准库函数，定义在stdio.h中，从文件流中读取一行文本数据
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
		Packet packet;

        if (parseLine(buffer, packet)) {

			// 获取IP地址的地理位置信息
            packet.src_location = ip2RegionUtil.getIpLocation(packet.src_ip);
            packet.dst_location = ip2RegionUtil.getIpLocation(packet.dst_ip);

            // 计算文件偏移
            packet.file_offset = file_offset + sizeof(PacketHeader);
            file_offset += packet.cap_len + sizeof(PacketHeader);
            packets.push_back(packet);
        }
        else {
			assert(false);
        }
    }

	// C++11引入的新式for循环，通过auto关键子自动推断类型
	/* 16进制输出
    for (auto& p : packets) {
        toString(p);

        // 读取报文的十六进制
        std::vector<unsigned char> buffer;
		readPacketHex(pcap_file, p.file_offset, p.cap_len, buffer);

        // 打印读取到的数据
		printf("Packet Hex: ");
		for (unsigned char byte : buffer) {
			printf("%02X ", byte);
		}

		printf("\n");
    }
    */

    //转化为json输出
    for (auto& p : packets) {
        toJson(p);
    }
     
    //关闭管道，获取子进程的退出状态
    _pclose(pipe);
    return 0;
}

// -----------------------------------------------------

//静态方法init的实现。输入：字符类型参数xdbFilePath，表示文件路径
bool IP2RegionUtil::init(const std::string& xdbFilePath) {

    xdbPtr = std::make_shared<xdb_search_t>(xdbFilePath);
    xdbPtr->init_content(); // 调用xdbPtr的init_content方法
    return true;
}


// 获取IP地址的地理位置信息
std::string IP2RegionUtil::getIpLocation(const std::string& ip) {
    // 如果ip的长度大于15(IPv6)，返回空字符串
    if (ip.size() > 15) {
        return "";
    }
    std::string location = xdbPtr->search(ip);

    // 如果location不为空且location中不包含"invalid"，调用parseLocation方法
    if (!location.empty() && location.find("invalid") == std::string::npos) {
        return parseLocation(location);
    }
    else {
        return "";
    }
}


// 解析地理位置信息
std::string IP2RegionUtil::parseLocation(const std::string& input) {
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(input);
    // 如果input中包含"内网"，返回"内网"
    if (input.find("内网") != std::string::npos) {
        return "内网";
    }
    // 以"|"为分隔符，将input分割成多个字符串
    while (std::getline(ss, token, '|')) {
        tokens.push_back(token);
    }

    // 如果tokens的长度大于等于4，返回拼接后的字符串
    if (tokens.size() >= 4) {
        std::string result;
        // 如果tokens[0]不等于"0"，将tokens[0]拼接到result中
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
    // tokens长度小于4，返回input
    else {
        return input;
    }
}

// 根据偏移和长度，指定文件的数据到vector中
bool readPacketHex(const std::string& filePath, uint32_t offset, uint32_t length, std::vector<unsigned char> &buffer) {
	std::ifstream file(filePath, std::ios::binary);
	if (!file) {
		std::cerr << "open file failed" << std::endl;
		return false;
	}

	file.seekg(offset, std::ios::beg);
	if (!file) {
		std::cerr << "seek file failed" << std::endl;
		return false;
	}

	buffer.resize(length);
    file.read(reinterpret_cast<char*>(buffer.data()), length);
	if (!file) {
		std::cerr << "read file failed" << std::endl;
		return false;
	}

	return true;
}
 
 
//字符串转数字
int port2Int(std::string port) {
    try {
        return std::stoi(port);
    }
	catch (std::exception& e) {
		return -1;
	}
}


//将tshark输出解析成packet结构体
bool parseLine(std::string line, Packet& packet) {
    //将行末尾的换行符去掉
    if (line.back() == '\n') {
        line.pop_back();
    }
    //stringstream是C++标准库中的一个类，允许把字符串当作输入输出流来处理
    std::stringstream ss(line);
    std::string field;
    std::vector<std::string> fields;

    // getline是C++提供的函数，用于从输入流中读取一行数据
    /*while (std::getline(ss, field, '\t')) {
        fields.push_back(field);
    }*/

    // 实现字符串拆分,避免字段数量不足导致数据包无法对齐
    size_t start = 0, end;
    while ((end = line.find('\t', start)) != std::string::npos) {
        fields.push_back(line.substr(start, end - start));
        start = end + 1;
    }
    fields.push_back(line.substr(start)); // 添加最后一个子串

    // 拆分字段保存到vector容器中，最后按照顺序赋值Packet 各个字段
    // 如果<13，则会输出空的字段
    if (fields.size() >= 13) {
        packet.frame_number = port2Int(fields[0]);
        packet.time = fields[1];
        packet.cap_len = port2Int(fields[2]);
        packet.src_ip = fields[3].empty() ? fields[4] : fields[3];
        packet.dst_ip = fields[5].empty() ? fields[5] : fields[6];
        if (!fields[7].empty() || !fields[8].empty()) {
            packet.src_port = port2Int(fields[7].empty() ? fields[8] : fields[7]);
        }
        if (!fields[9].empty() || !fields[10].empty()) {
            packet.src_port = port2Int(fields[9].empty() ? fields[10] : fields[9]);
        }
        packet.protocol = fields[11];
        packet.info = fields[12];
		return true;
    }
    else
	{
		std::cerr << "parse line failed" << std::endl;
		return false;
    }
}

//将Packet结构体转换为字符串
//format是C++20的新特性，用于格式化字符串
 void toString(Packet & packet) {
    std::string s = std::format(
        "frame_number:{0}\t time:{1}\t cap_len:{8}\t src_ip:{2}\t scr_port:{3}\t src_location:{4}\t dst_ip:{5}\t dst_port:{6}\t dst_location:{7}\t protocol:{8}\t info:{9}",
        packet.frame_number,
        packet.time,
        packet.src_ip,
        packet.src_port,
        packet.src_location,
        packet.dst_ip,
		packet.dst_port,
		packet.dst_location,
        packet.protocol,
        packet.info,
        packet.cap_len
    );
    std::cout << s << std::endl;
}

//将Packet对象转换为JSON打印输出
void toJson(const Packet& packet) {
    //构建JSON对象
    rapidjson::Document pktObj;
    rapidjson::Document::AllocatorType& allocator = pktObj.GetAllocator();

    //设置JSON为Object对象类型
	pktObj.SetObject();

    //添加JSON字段
	pktObj.AddMember("frame_number", packet.frame_number, allocator);
	pktObj.AddMember("timestamp", rapidjson::Value(packet.time.c_str(), allocator), allocator);
	pktObj.AddMember("src_ip", rapidjson::Value(packet.src_ip.c_str(), allocator), allocator);
	pktObj.AddMember("src_port", packet.src_port, allocator);
	pktObj.AddMember("src_location", rapidjson::Value(packet.src_location.c_str(), allocator), allocator);
	pktObj.AddMember("dst_ip", rapidjson::Value(packet.dst_ip.c_str(), allocator), allocator);
	pktObj.AddMember("dst_port", packet.dst_port, allocator);
	pktObj.AddMember("dst_location", rapidjson::Value(packet.dst_location.c_str(), allocator), allocator);
	pktObj.AddMember("protocol", rapidjson::Value(packet.protocol.c_str(), allocator), allocator);
	pktObj.AddMember("info", rapidjson::Value(packet.info.c_str(
    ), allocator), allocator);
	pktObj.AddMember("file_offset", packet.file_offset, allocator);
    pktObj.AddMember("cap_len", packet.cap_len, allocator);

    //序列化为 JSON 字符串
	rapidjson::StringBuffer buffer;
	//rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	pktObj.Accept(writer);

    //打印JSON输出
    std::cout << buffer.GetString() << std::endl;
    std::cout << "\n" << std::endl;
}