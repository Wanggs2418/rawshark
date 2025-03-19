//
// Created by dyunu on 2025/2/22.
//

#ifndef LESSON05_TSHARK_DATATYPE_H
#define LESSON05_TSHARK_DATATYPE_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cstdint>

using std::string;
using std::vector;

// PCAP全局文件头
struct PcapHeader {
    uint32_t magic_number;      // 魔数
    uint16_t version_major;     // 主版本号
    uint16_t version_minor;     // 副版本号
    int32_t thiszone;           // 时区偏移，通常为0
    uint32_t sigfigs;           // 时间戳精度，通常为0
    uint32_t snaplen;           // 捕获的数据包最大长度
    uint32_t network;           // 链路层类型，例如以太网(0x01)
};

// 单个数据报文头
struct PacketHeader {
    uint32_t ts_sec;        // 数据包捕获的时间戳（秒）
    uint32_t ts_usec;       // 数据包捕获的时间戳（微秒）
    uint32_t caplen;        // 实际捕获的数据包长度
    uint32_t len;           // 原始数据包长度
};

// 解析报文字段
struct Packet {
    int frame_number;		// 数据包编号
    string time;		    // 数据包时间戳
    string src_mac;         // 源mac地址
    string dst_mac;         // 目的mac地址
    uint32_t caplen;        // 捕获数据包长度
    uint32_t len;           // 实际数据包长度
    string src_ip;		    // 源ip
    string src_location;    // 源物理地址
    int src_port;           // 源端口
    string dst_ip;		    // 目的ip
    string dst_location;    // 目的物理地址
    int dst_port;           // 目的端口
    string protocol;	    // 协议
    string info;		    // 数据包概要信息
    uint32_t file_offset;   // 数据包在pcap文件中的偏移
};

#endif //LESSON05_TSHARK_DATATYPE_H
