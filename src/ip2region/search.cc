#include "search.h"
#include <stdexcept>

namespace xdb {

search_t::search_t(const string &file, int version, int p)
    : db(fopen(file.data(), "rb")), header(db), policy(p), vector(nullptr), content(nullptr) {
    
    if (db == NULL) throw std::runtime_error("Failed to open xdb file: " + file);

    m_version = version;
    m_ip_size = (version == ipv4 ? 4 : 16);
    m_content_size = m_ip_size * 2 + 2 + 4;

    // init_xdb(version);

    if (header.ip_version() != version) {
        fclose(db);
        throw std::runtime_error("IP version mismatch in xdb file");
    }

    if (policy != policy_file) {
        vector = (char *)malloc(length_vector);
        if (!vector) throw std::bad_alloc();
        read_bin(length_header, vector, length_vector, db);

        if (policy == policy_content) {
            _fseeki64(db, 0, SEEK_END);
            long long full_size = _ftelli64(db);
            long long size = full_size - length_vector - length_header;

            if (size <= 0) {
                free(vector);
                fclose(db);
                throw std::runtime_error("Invalid xdb file size");
            }

            content = (char *)malloc(static_cast<size_t>(size));
            if (!content) {
                free(vector);
                throw std::bad_alloc();
            }
            read_bin(length_vector + length_header, content, static_cast<size_t>(size), db);
        }
    }
}

search_t::~search_t() {
    if (db) fclose(db);
    if (vector) free(vector);
    if (content) free(content);
}

char const *search_t::get_content_index_help(int index, char* buf) {
    if (policy != policy_file) return vector + index;
    ++io_count;
    read_bin(length_header + index, buf, 8, db);
    return buf;
}

void search_t::get_content_index(const ip_t &ip, int &left, int &right) {
    int index = ((unsigned char)ip.p[0] * 256 + (unsigned char)ip.p[1]) * 8;
    char buf[8];
    const char *p = get_content_index_help(index, buf);
    left          = (int)to_uint(p);
    right         = (int)to_uint(p + 4);
}

char const *search_t::get_content_help(int index, char* buf) {
    if (policy == policy_content) return content + index - length_header - length_vector;
    ++io_count;
    read_bin(index, buf, m_content_size, db);
    return buf;
}

string search_t::get_region(int index, int len) {
    if (policy == policy_content)
        return string(content + index - length_header - length_vector, len);
    ++io_count;
    std::vector<char> buf(len);
    read_bin(index, buf.data(), len, db);
    return string(buf.data(), len);
}

void search_t::get_content(int index, ip_t &ip_left, ip_t &ip_right, int &region_len, int &region_index) {
    char buf[64];
    const char *p = get_content_help(index, buf);

    ip_left.from_xdb(p, m_version, m_ip_size);
    ip_right.from_xdb(p + m_ip_size, m_version, m_ip_size);

    region_len   = (int)to_ushort(p + m_ip_size * 2);
    region_index = (int)to_uint(p + m_ip_size * 2 + 2);
}

string search_t::search(const ip_t &ip) {
    io_count = 0;
    int content_left, content_right;
    get_content_index(ip, content_left, content_right);

    if (content_left == 0 || content_right == 0) return "";

    ip_t ip_left, ip_right;
    int  region_len, region_index;
    int  left  = 0;
    int  right = (content_right - content_left) / m_content_size;

    while (left <= right) {
        int mid       = left + (right - left) / 2;
        int mid_index = content_left + mid * m_content_size;
        get_content(mid_index, ip_left, ip_right, region_len, region_index);

        if (ip < ip_left)
            right = mid - 1;
        else if (ip_right < ip)
            left = mid + 1;
        else
            return get_region(region_index, region_len);
    }
    return "";
}

string search_t::search(const string &str) {
    unsigned long long t1 = get_time();
    ip_t ip;
    if (ip.from_str(str, m_version) == false)
        return "invalid ip: " + str;

    string region = search(ip);
    unsigned long long t2 = get_time();
    cost_time = static_cast<int>(t2 - t1); 
    return region;
}

} // namespace xdb