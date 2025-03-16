#ifndef TSHARK_DATATYPE_H
#define TSHARK_DATATYPE_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

struct Packet {
	int frame_number; // 帧序号
	std::string time; // 时间戳
	std::string src_mac; // 源MAC地址
	std::string dst_mac; // 目的MAC地址
	uint32_t cap_len;   // 捕获的数据包长度
	uint32_t len;	   // 数据包长度
    std::string src_ip;
    std::string src_location;
    uint16_t src_port;
    std::string dst_ip;
    std::string dst_location;
    uint16_t dst_port;
    std::string protocol;
    std::string info;
	uint32_t file_offset;   // 文件偏移
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

#endif // TSHARK_DATATYPE_H

