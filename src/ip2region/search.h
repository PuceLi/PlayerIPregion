#ifndef SEARCH_H
#define SEARCH_H

#include "header.h"
#include "ip.h"

namespace xdb {

class search_t {
protected:
    FILE *db;
    header_t header;
    int policy;

    int m_version;
    int m_ip_size;
    int m_content_size;

    char *vector  = nullptr;
    char *content = nullptr;

    int io_count  = 0;
    int cost_time = 0;

public:
    search_t(const string &file, int version, int p);
    ~search_t();

    int get_io_count() const { return io_count; }
    int get_cost_time() const { return cost_time; }

    string search(const string &str);

protected:
    string search(const ip_t &ip);

    void get_content_index(const ip_t &ip, int &left, int &right);
    void get_content(int index, ip_t &ip_left, ip_t &ip_right, int &region_len, int &region_index);

    char const *get_content_index_help(int index, char* buf);
    char const *get_content_help(int index, char* buf);
    string get_region(int index, int len);
};

}  // namespace xdb

#endif