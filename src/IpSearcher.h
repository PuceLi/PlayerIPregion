#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <memory>

#include "ip2region/search.h"

namespace ip_locator {

class IpSearcher {
public:
    static IpSearcher& getInstance();

    bool init(std::filesystem::path const& v4Path, std::filesystem::path const& v6Path, int policy);

    std::string search(std::string_view ip);

private:
    IpSearcher() = default;

    std::unique_ptr<xdb::search_t> mSearcherV4;
    std::unique_ptr<xdb::search_t> mSearcherV6;
    bool mInited = false;
};

} // namespace ip_locator