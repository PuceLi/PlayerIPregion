# PlayerIPregion (WIP)

一款 **LeviLamina** 插件旨为服务器提供玩家 IP 归属地和 ISP 查询与全服广播功能

**该项目仍在开发中**

---

### 简介

**PlayerIPregion** 利用 **[ip2region](https://github.com/lionsoul2014/ip2region)** 离线数据库，可在玩家加入时获取其 IP 信息（如省份、城市及运营商），并向全服玩家发送提示

该插件完全不需要主动联网，所有查询均在本地完成，确保了高效性与隐私性

### 功能

* **协议支持**：支持 **IPv4** 地址查询，**IPv6** 地址查询还在开发中

* **离线数据**：用 **[ip2region](https://github.com/lionsoul2014/ip2region)** 的离线数据库，实现最快微秒级的本地解析

* **查询策略**：数据库加载策略包括“全部载入内存”、“仅加载索引”或“直接读取文件”，以平衡内存占用与查询速度

### 技术实现

针对 **Windows** 环境优化了 **[ip2region](https://github.com/lionsoul2014/ip2region)** 的 C++ 源码（原仓库的代码针对的是 **Linux** 系统），解决了跨平台兼容性及多线程环境下的全局变量冲突问题

### 安装（服务端）

1.  下载 `PlayerIPregion-server-windows-x64.zip` 并解压到 `plugins` 目录，然后启动一次服务端

2.  关闭服务端，在插件的数据目录 `plugins/PlayerIPregion/db/` 下手动放入 `ip2region_v4.xdb` 和 `ip2region_v6.xdb` 数据库文件（可从 [ip2region/data](https://github.com/lionsoul2014/ip2region/tree/master/data) 目录中获取）

3.  通过生成的 `config/config.json` 调整数据库加载策略

> 为了确保数据准确，建议每个月从 [ip2region/data](https://github.com/lionsoul2014/ip2region/tree/master/data) 获取一次数据库

### 配置

``` json5
{
    "version": 1,   // 勿动
    /**
     * 加载策略配置：
     * 0 - 不加载数据库（查询速度慢）
     * 1 - 只加载索引（平衡内存与速度）
     * 2 - 全部加载到内存（速度极快，高内存消耗）
     */
    "loadPolicy": 1
}
```