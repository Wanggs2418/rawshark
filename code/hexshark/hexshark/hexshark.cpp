// hexshark.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
// 读取PCAP文件中的十六进制文件

#include <iostream> // 标准输入输出流
#include <fstream>  // 文件输入输出类
#include <vector>   // 常用容器，动态大小调整、快速随机访问和高效的元素插入/删除（在尾部）等
#include <stdint.h> // 标准整数类型定义

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


int main()
{
	// 注意一定是pcap格式，必须在tshark 中添加 -F pcap 选项
	std::string pcap_path = "C:/Users/Wanggs/demo1.pcap";
	std::ifstream file(pcap_path, std::ios::binary);
	if (!file) {
		std::cerr << "Error: open pcap file failed!" << std::endl;
		return 0;
	}

	// 读取PCAP文件头
	PcapHeader pcapHeader;
	file.read(reinterpret_cast<char*>(&pcapHeader), sizeof(PcapHeader));

    // 循环读取每一个数据报文
	while (file) {
		// 读取报文头
		PacketHeader packetHeader;
		file.read(reinterpret_cast<char*>(&packetHeader), sizeof(PacketHeader));
		if (!file) {
			break;
		}

		// 读取报文内容
		std::vector<unsigned char> data(packetHeader.caplen);
		file.read(reinterpret_cast<char*>(data.data()), packetHeader.caplen);
		printf("数据包[时间：%d 长度：%d]：", packetHeader.ts_sec, packetHeader.caplen);
		// 以十六进制输出，%x-十六进制，02至少占 2 个字符宽度，不足用 0 补齐
		for (unsigned char byte : data) {
			printf("%02x ", byte);
		}
		std::cout << "\n";
	}
	file.close();
	return 0;
}