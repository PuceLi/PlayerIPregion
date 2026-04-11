#pragma once

namespace ip_locator {

struct Config {
    int version = 1;
    
    /**
     * 数据库载入方式：
     * 0 - 不加载数据库，查询速度慢
     * 1 - 只加载索引
     * 2 - 全部加载，速度极快，需要高内存
     */
    int loadPolicy = 2; 
};

} // namespace ip_locator