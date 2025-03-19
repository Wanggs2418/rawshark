//
// Created by xuanyuan on 2025/2/16.
//

#ifndef TSHARK_DATATYPE_H
#define TSHARK_DATATYPE_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

struct Packet {
    int frame_number;
    std::string time;
    std::string src_mac;
    std::string dst_mac;
    uint32_t cap_len;
    uint32_t len;
    std::string src_ip;
    std::string src_location;
    uint16_t src_port;
    std::string dst_ip;
    std::string dst_location;
    uint16_t dst_port;
    std::string protocol;
    std::string info;
    uint32_t file_offset;
};


// PCAP全局文件头
struct PcapHeader {
    uint32_t magic_number;
    uint16_t version_major;
    uint16_t version_minor;
    int32_t thiszone;
    uint32_t sigfigs;
    uint32_t snaplen;
    uint32_t network;
};

// 每一个数据报文前面的头
struct PacketHeader {
    uint32_t ts_sec;
    uint32_t ts_usec;
    uint32_t caplen;
    uint32_t len;
};

// 网卡信息
struct AdapterInfo {
    int id;
    std::string name;
    std::string remark;
};


#endif // TSHARK_DATATYPE_H
