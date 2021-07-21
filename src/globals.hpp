#ifndef globals_hpp_included
#define globals_hpp_included

#include <string>
#include <unordered_map>
#include <oatpp/core/Types.hpp>

namespace globals
{
	namespace redis
	{
		extern std::string address;
		extern size_t port;
		extern std::string password;
	}

	extern std::string path;
	extern bool cache_state;
	extern std::unordered_map<std::string, std::pair<oatpp::String, int64_t>> cache;

	inline bool string_to_bool(std::string value) { return value == "true"; }
	void parse();
	int64_t get_time_epoch();
}

#endif