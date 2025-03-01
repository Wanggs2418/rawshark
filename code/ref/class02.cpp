#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <vector>
#include <sstream>
#include <format>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

struct Packet {
    int frame_number = - 1;
    std::string timestamp;
    std::string src_ip;
    int src_port = - 1 ;
    std::string dst_ip;
    int dst_port = - 1 ;
    std::string protocol;
    std::string info;
};
/*
* 将tshark输出的一行数据解析成Packet结构体
*/
void parseLine(std::string line, Packet& packet);

/*
* 将Packet结构体转换成字符串格式
*/
std::string toString(Packet& packet);

/*
* 把端口字段的字符串转换成int
*/
int portToInt(std::string port);

/*
* 将Packet结构体转换成JSON对象
*/
rapidjson::Document toJSONDocument(const Packet& packet);

/*
* 将JSON对象转换成JSON字符串
*/
std::string toJSONString(const rapidjson::Document& document);

int main() {
    SetConsoleOutputCP(CP_UTF8);  // 解决控制台输出tashrk返回的中文时出现的乱码问题
    const char* command = "D:/wireshark/tshark.exe -r D:/temp/temp.pcap -T fields -e frame.number -e frame.time -e ip.src -e tcp.srcport -e ip.dst -e tcp.dstport -e _ws.col.Protocol -e _ws.col.Info";
    FILE* pipe = _popen(command, "r");
    if (!pipe) {
    std::cerr << "运行tshark失败！" << std::endl;
    return 1 ;
    }
    std::vector<Packet> packets;
    char buffer[ 4096 ];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        Packet packet;
        parseLine(buffer, packet);
        packets.push_back(packet);
    }
    std::cout << "Print in normal format: " << std::endl;
    for (auto& p : packets) {
	    std::cout << toString(p) << std::endl;
	}
	std::cout << std::endl << "Print in JSON format: " << std::endl;
    for (auto& p : packets) {
        // 解析成JSON对象
        rapidjson::Document json = toJSONDocument(p);
        // 转换成字符串输出
        std::cout << toJSONString(json) << std::endl;
	}
	_pclose(pipe);
	// 打印 源IP 和 目的IP 为空的数据包
    std::cout << std::endl << "Packet that the value of src_ip and dst_ip are blank:" << std::endl;
    for (auto& p : packets) {
        if (p.src_ip.empty() && p.dst_ip.empty()) {
        printf("protocol: %s\tinfo: %s\n", p.protocol.c_str(),p.info.c_str());
        }
    }
 }
void parseLine(std::string line, Packet& packet) {
    if (line.back() == '\n') {
    	line.pop_back();
    }
    std::stringstream ss(line);
    std::string field;
    std::vector<std::string> fields;
    while (std::getline(ss, field, '\t')) {
    	fields.push_back(field);
    }
    if (fields.size()) {
        packet.frame_number = std::stoi(fields[0]);
        packet.timestamp = fields[1];
        packet.src_ip = fields[2];
        packet.src_port = portToInt(fields[3]);
        packet.dst_ip = fields[4];
        packet.dst_port = portToInt(fields[5]);
        packet.protocol = fields[6];
        packet.info = fields[7];
    }
}

std::string toString(Packet& packet) {
    std::string s;
    return std::format("frame_number: {0}\ttimestamp: {1}\tsrc_ip:{2}\tsrc_port: {3}\tdst_ip: {4}\tdst_port: {5}\tprotocol: {6}\tinfo: {7}",
    packet.frame_number,
    packet.timestamp,
    packet.src_ip,
    packet.src_port,
    packet.dst_ip,
    packet.dst_port,
    packet.protocol,
    packet.info
    );
} 

int portToInt(std::string port) {
    try {
    	return std::stoi(port);
    }
    catch (const std::exception e) {
        // 字符串转数字失败时，返回-1
        return -1;
    }
}

rapidjson::Document toJSONDocument(const Packet& packet) {
    rapidjson::Document pktObj;
    rapidjson::Document::AllocatorType& allocator = pktObj.GetAllocator();
    pktObj.SetObject();
    pktObj.AddMember("frame_numer", packet.frame_number, allocator);
    pktObj.AddMember("timestamp",
    rapidjson::Value(packet.timestamp.c_str(), allocator), allocator);
    pktObj.AddMember("src_ip", rapidjson::Value(packet.src_ip.c_str(),
    allocator), allocator);
    pktObj.AddMember("src_port", packet.src_port, allocator);
    pktObj.AddMember("dst_ip", rapidjson::Value(packet.dst_ip.c_str(),
    allocator), allocator);
    pktObj.AddMember("dst_port", packet.dst_port, allocator);
    pktObj.AddMember("protocol", rapidjson::Value(packet.protocol.c_str(),
    allocator), allocator);
    pktObj.AddMember("info", rapidjson::Value(packet.info.c_str(),
    allocator), allocator);
    return pktObj;
}

std::string toJSONString(const rapidjson::Document& document) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);
    return buffer.GetString();
}
