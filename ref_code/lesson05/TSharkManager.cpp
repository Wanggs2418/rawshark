//
// Created by dyunu on 2025/2/22.
//

#include "TSharkManager.h"
#include "ip2region_util.h"

using std::cout;
using std::cerr;
using std::endl;

TSharkManager::TSharkManager(const string &tsharkExecutablePath) {
    tsharkPath = tsharkExecutablePath;
}

TSharkManager::~TSharkManager() = default;

bool TSharkManager::analysisFile(const string &filePath) {
    vector<string> tsharkArgs{
            tsharkPath,
            "-r", filePath,
            "-T", "fields",
            "-e", "frame.number",
            "-e", "frame.time_epoch",
            "-e", "frame.len",
            "-e", "frame.cap_len",
            "-e", "eth.src",
            "-e", "eth.dst",
            "-e", "ip.src",
            "-e", "ipv6.src",
            "-e", "ip.dst",
            "-e", "ipv6.dst",
            "-e", "tcp.srcport",
            "-e", "udp.srcport",
            "-e", "tcp.dstport",
            "-e", "udp.srcport",
            "-e", "_ws.col.Protocol",
            "-e", "_ws.col.Info"
    };
    string command;
    for (auto &arg: tsharkArgs) {
        command += arg;
        command += " ";
    }
    cout << command << endl;
    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe) {
        cerr << "Failed to run tshark command!" << endl;
        return false;
    }
    char buf[4096];
    uint32_t file_offset = sizeof(PcapHeader);
    int count = 0;
    while (fgets(buf, sizeof(buf), pipe) != nullptr) {
        shared_ptr<Packet> packet = std::make_shared<Packet>();
        parseLine(buf, packet);
        count++;

        // 计算当前报文偏移（数据部分），然后记录在Packet对象中
        packet->file_offset = file_offset + sizeof(PacketHeader);

        // 更新偏移游标
        file_offset += packet->caplen + sizeof(PacketHeader);

        // 将packet id与对应的packet存入哈希表中
        allPackets.insert(std::make_pair(packet->frame_number, packet));
    }

    LOG_F(INFO, "分析完成，数据包总数：%d", count);
    // 记录当前分析的文件路径
    currentFilePath = filePath;

    pclose(pipe);
    return true;
}

string formatted_timestamp(string timestamp) {
    size_t dot_pos = timestamp.find('.');
    if (dot_pos == string::npos) {
        return "";
    }
    string seconds_part = timestamp.substr(0, dot_pos);
    string nanoseconds_part = timestamp.substr(dot_pos + 1);

    // 转换为整数
    time_t seconds = std::stoll(seconds_part);
    long nanoseconds = std::stoll(nanoseconds_part);
    int microseconds = nanoseconds / 1000;

    // 转换为 std::tm 结构
    std::tm tm_time;
#ifdef _WIN32
    // Windows下使用 gmtime_s(struct tm *_Tm, const time_t *_Time)
    gmtime_s(&tm_time, &seconds);
#else
    // Linux下使用 gmtime_r(const time_t *_Time, struct tm *_Tm)
    gmtime_r(&seconds, &tm_time);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm_time, "%Y-%m-%d %H:%M:%S")
        << "." << std::setw(6) << std::setfill('0') << microseconds;
    return oss.str();
}

bool TSharkManager::parseLine(string line, shared_ptr<Packet> packet) {
    if (line.back() == '\n') {
        line.pop_back();
    }
    stringstream ss(line);
    string field;
    vector<string> fields;

    // 手动拆分字符串
    size_t start = 0, end;
    while ((end = line.find('\t', start)) != string::npos) {
        fields.push_back(line.substr(start, end - start));
        start = end + 1;
    }
    // 插入最后一个字段
    fields.push_back(line.substr(start));

    // 字段顺序：
    // 0: frame.number
    // 1: frame.time_epoch
    // 2: frame.len
    // 3: frame.caplen
    // 4: eth.src
    // 5: eth.dst
    // 6: ip.src
    // 7: ipv6.src
    // 8: ip.dst
    // 9: ipv6.dst
    // 10: tcp.srcport
    // 11: udp.srcport
    // 12: tcp.dstport
    // 13: udp.dstport
    // 14: _ws.col.Protocol
    // 15: _ws.col.Info
    if (fields.size() >= 16) {
        packet->frame_number = stoi(fields[0]);
        packet->time = formatted_timestamp(fields[1]);
        packet->len = std::stoi(fields[2]);
        packet->caplen = stoi(fields[3]);
        packet->src_mac = fields[4];
        packet->dst_mac = fields[5];
        packet->src_ip = fields[6].empty() ? fields[7] : fields[6];
        packet->src_location = IP2RegionUtil::getIPLocation(packet->src_ip);
        packet->dst_ip = fields[8].empty() ? fields[9] : fields[8];
        packet->dst_location = IP2RegionUtil::getIPLocation(packet->dst_ip);
        if (!fields[10].empty() || !fields[11].empty()) {
            packet->src_port = stoi(fields[10].empty() ? fields[11] : fields[12]);
        }
        if (!fields[12].empty() || !fields[13].empty()) {
            packet->dst_port = stoi(fields[12].empty() ? fields[13] : fields[12]);
        }
        packet->protocol = fields[14];
        packet->info = fields[15];

        return true;
    }
    return false;
}

void TSharkManager::printAllPackets() {
    for (auto &pair: allPackets) {
        auto packet = pair.second;

        // 构建JSON对象
        rapidjson::Document packetObject;
        rapidjson::Document::AllocatorType &allocator = packetObject.GetAllocator();

        // 设置JSON为Object对象类型
        packetObject.SetObject();

        // 添加JSON字段
        packetObject.AddMember("frame_number", packet->frame_number, allocator);
        packetObject.AddMember("timestamp", rapidjson::Value(packet->time.c_str(), allocator), allocator);
        packetObject.AddMember("src_mac", rapidjson::Value(packet->src_mac.c_str(), allocator), allocator);
        packetObject.AddMember("dst_mac", rapidjson::Value(packet->dst_mac.c_str(), allocator), allocator);
        packetObject.AddMember("src_ip", rapidjson::Value(packet->src_ip.c_str(), allocator), allocator);
        packetObject.AddMember("src_location", rapidjson::Value(packet->src_location.c_str(), allocator), allocator);
        packetObject.AddMember("src_port", packet->src_port, allocator);
        packetObject.AddMember("dst_ip", rapidjson::Value(packet->dst_ip.c_str(), allocator), allocator);
        packetObject.AddMember("dst_location", rapidjson::Value(packet->dst_location.c_str(), allocator), allocator);
        packetObject.AddMember("dst_port", packet->dst_port, allocator);
        packetObject.AddMember("protocol", rapidjson::Value(packet->protocol.c_str(), allocator), allocator);
        packetObject.AddMember("info", rapidjson::Value(packet->info.c_str(), allocator), allocator);
        packetObject.AddMember("file_offset", packet->file_offset, allocator);
        packetObject.AddMember("caplen", packet->caplen, allocator);
        packetObject.AddMember("len", packet->len, allocator);

        // 序列化为JSON字符串
        rapidjson::StringBuffer buffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        packetObject.Accept(writer);

        // 打印JSON输出
        cout << buffer.GetString() << endl;

        // 输出十六进制报文数据
        vector<unsigned char> data(packet->len);
        getPacketHexData(packet->frame_number, data);
        printf("Packet Hex: ");
        for (unsigned char byte: data) {
            printf("%02X ", byte);
        }
        printf("\n\n");
    }
}

bool TSharkManager::getPacketHexData(uint32_t frameNumber, vector<unsigned char> &data) {
    auto packet = allPackets[frameNumber];
    std::ifstream file(currentFilePath, std::ios::binary);
    if (!file) {
        cerr << "Failed to open file: " << currentFilePath << endl;
        return false;
    }
    file.seekg(packet->file_offset, std::ios::beg);
    file.read(reinterpret_cast<char*>(data.data()), packet->len);
    file.close();
    return false;
}
