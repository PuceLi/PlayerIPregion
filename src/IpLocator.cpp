#include "IpLocator.h"
#include "IpSearcher.h"

#include <ll/api/Config.h>
#include <ll/api/event/EventBus.h>
#include <ll/api/event/player/PlayerJoinEvent.h>
#include <ll/api/service/Bedrock.h>

#include <mc/world/actor/player/Player.h>
#include <mc/network/NetworkIdentifier.h>
#include <mc/world/level/Level.h>

#include <functional>

namespace ip_locator {

IpLocator& IpLocator::getInstance() {
    static IpLocator instance;
    return instance;
}

bool IpLocator::load() {
    auto& logger = mSelf.getLogger();
    auto dbDir = mSelf.getDataDir() / "db";
    auto v4Path = dbDir / "ip2region_v4.xdb";
    auto v6Path = dbDir / "ip2region_v6.xdb";

    auto checkFile = [&](const std::filesystem::path& p) {
        if (std::filesystem::exists(p)) {
            auto size = std::filesystem::file_size(p);
            if (size == 0) {
                logger.error("数据库文件为空: {}", p.filename().string());
                return false;
            }
            return true;
        }
        return false;
    };

    bool hasV4 = checkFile(v4Path);
    bool hasV6 = checkFile(v6Path);

    if (!hasV4 && !hasV6) {
        logger.error("未找到有效的数据库文件于: {}", dbDir.string());
        return false;
    }

    int policy = std::clamp(mConfig.loadPolicy, 0, 2);

    try {
        if (!IpSearcher::getInstance().init(v4Path, v6Path, policy)) {
            logger.error("IP 搜索器初始化失败，请检查数据库文件是否损坏。");
            return false;
        }
    } catch (const std::exception& e) {
        logger.error("加载数据库时发生致命错误: {}", e.what());
        return false;
    }

    logger.info("IP 数据库加载成功（策略 ID: {}）。", policy);
    logger.info("插件成功加载！");
    logger.info("Author: PuceLi");
    return true;
}

bool IpLocator::enable() {
    auto& eventBus = ll::event::EventBus::getInstance();

    mJoinListener = eventBus.emplaceListener<ll::event::PlayerJoinEvent>(
        [this](ll::event::PlayerJoinEvent& event) {
            auto& player = event.self();

            std::string address = player.getNetworkIdentifier().getIPAndPort();
            std::string ip = address.substr(0, address.find_last_of(':'));
            if (!ip.empty() && ip.front() == '[' && ip.back() == ']') {
                ip = ip.substr(1, ip.size() - 2);
            }

            std::string location = IpSearcher::getInstance().search(ip);
            if (location.empty()) location = "未知地区";

            std::string msg = "§e" + player.getRealName() + " §7进入了服务器，位置：§a" + location;

            if (auto level = ll::service::bedrock::getLevel()) {
                level->forEachPlayer([&msg](Player& p) {
                    p.sendMessage(msg);
                    return true;
                });
            }

            mSelf.getLogger().info("玩家 {} 加入，IP: {}, 位置: {}", player.getRealName(), ip, location);
        }
    );
    return true;
}

bool IpLocator::disable() {
    ll::event::EventBus::getInstance().removeListener(mJoinListener);
    return true;
}

} // namespace ip_locator