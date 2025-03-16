// getip.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
// 测试开源库ip2region获取IP地址的地理位置信息
#include "ip2region_util.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream> // 引入字符串流（string stream）
#include <fstream>
#include <windows.h>
#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib") 
#endif


// 智能类型指针
std::shared_ptr<xdb_search_t> IP2RegionUtil::xdbPtr = nullptr;

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


int main() {
	
	SetConsoleOutputCP(CP_UTF8);
	IP2RegionUtil ip2RegionUtil;
	//ip2RegionUtil.init("ip2region.xdb");
	ip2RegionUtil.init("E:\\03CS_learning\\04CS_Reverse_Engineering\\wireshark\\rawshark\\third_library\\ip2region\\ip2region.xdb");

	std::string ip = "201.21.92.124";
	std::string location = ip2RegionUtil.getIpLocation(ip);
	
	std::cout << ip << ":" << location << std::endl;
	return 0;
}