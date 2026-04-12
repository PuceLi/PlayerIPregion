#include "IpSearcher.h"

#include <ll/api/io/Logger.h>

namespace ip_locator {

IpSearcher& IpSearcher::getInstance() {
    static IpSearcher instance;
    return instance;
}

std::string formatLocation(const std::string& raw) {
    if (raw.empty()) return "未知";

    std::vector<std::string> parts;
    std::stringstream ss(raw);
    std::string item;

    while (std::getline(ss, item, '|')) {
        parts.push_back(item);
    }

    // 标准 xdb 格式：Country|Province|City|ISP|iso-alpha2-code
    // 代码实现：Country|Province|City|ISP

    std::string result;
    for (size_t i : {0, 1, 2, 3}) {
        if (i < parts.size()) {
            const std::string& segment = parts[i];
            if (segment != "0" && segment != "Reserved") {
                if (!result.empty()) result += "|";
                result += segment;
            }
        }
    }

    return result.empty() ? "局域网/内网" : result;
}

bool IpSearcher::init(std::filesystem::path const& v4Path, std::filesystem::path const& v6Path, int policy) {
    try {
        mSearcherV4.reset();
        mSearcherV6.reset();

        if (std::filesystem::exists(v4Path)) {
            mSearcherV4 = std::make_unique<xdb::search_t>(v4Path.string(), xdb::ipv4, policy);
        }

        if (std::filesystem::exists(v6Path)) {
            mSearcherV6 = std::make_unique<xdb::search_t>(v6Path.string(), xdb::ipv6, policy);
        }

        mInited = (mSearcherV4 != nullptr || mSearcherV6 != nullptr);
        return mInited;
    } catch (const std::exception& e) {
        printf("[PlayerIPregion] 初始化失败: %s\n", e.what());
        return false;
    }
}

std::string IpSearcher::search(std::string_view ip) {
    if (!mInited) return "数据库未就绪";

    std::string ipStr{ip};
    bool isV6 = ip.find(':') != std::string_view::npos;

    std::string rawLocation;
    try {
        if (isV6) {
            rawLocation = mSearcherV6 ? mSearcherV6->search(ipStr) : "IPv6库未加载";
        } else {
            rawLocation = mSearcherV4 ? mSearcherV4->search(ipStr) : "IPv4库未加载";
        }
    } catch (...) {
        return "查询异常";
    }
    return formatLocation(rawLocation);
}

} // namespace ip_locator