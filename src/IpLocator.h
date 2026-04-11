#pragma once

#include "Config.h"

#include <ll/api/mod/NativeMod.h>
#include <ll/api/event/Listener.h>

namespace ip_locator {

class IpLocator {
public:
    static IpLocator& getInstance();

    IpLocator() : mSelf(*ll::mod::NativeMod::current()) {}

    bool load();
    bool enable();
    bool disable();

private:
    ll::mod::NativeMod& mSelf;
    ll::event::ListenerPtr mJoinListener;
    Config mConfig;
};

} // namespace ip_locator