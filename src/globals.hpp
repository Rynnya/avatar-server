#ifndef globals_hpp_included
#define globals_hpp_included

#include <filesystem>
#include <string>
#include <oatpp/core/Types.hpp>

#include "concurrent_cache.hpp"

namespace globals {

    extern bool display_config;
    extern uint16_t port;

    namespace redis {
        extern std::string address;
        extern size_t port;
        extern std::string password;
    }

    extern std::filesystem::path path;

    extern bool cache_state;
    extern size_t cache_size;

    extern lib::lru_cache<std::string, oatpp::String> cache;
    extern oatpp::String default_image;

    void parse();

    int64_t get_time_epoch();
    bool string_to_bool(const std::string& value);
}

#endif