#include <filesystem>
#include <fstream>
#include <iostream>
#include <functional>

#include "globals.hpp"

namespace globals
{
	bool display_config = false;

	namespace redis
	{
		std::string address = "127.0.0.1";
		size_t port = 6379;
		std::string password = "";
	}

	std::string path = "/path/to/folder/";
	bool cache_state = true;
	std::unordered_map<std::string, std::pair<oatpp::String, int64_t>> cache = {};

	static std::unordered_map<std::string, std::function<void(const std::string&)>> kv_map =
	{
		{ "display_config", [&](const std::string& value) { globals::display_config = string_to_bool(value); } },
		{ "redis_address",  [&](const std::string& value) { globals::redis::address = value; } },
		{ "redis_port",     [&](const std::string& value) { globals::redis::port = std::stoll(value); } },
		{ "redis_password", [&](const std::string& value) { globals::redis::password = value; } },
		{ "path",           [&](const std::string& value) { globals::path = value; } },
		{ "disable_cache",  [&](const std::string& value) { globals::cache_state = !string_to_bool(value); } }
	};

	void parse()
	{
		std::fstream file("config.txt");
		if (!file)
		{
			std::cout << "Config: config.json don't exist in this folder -> " << std::filesystem::current_path() << "\n";
			std::cout << "Config: Please create new config.json and restart program.\n";
			std::cout << "Press any button to leave...\n";
			std::ignore = getchar();
			exit(-1);
		}
		
		std::string line;
		while (!file.eof())
		{
			std::getline(file, line);
			if (line.size() == 0 || line[0] == '#') // Skip comments and emply lines
				continue;

			size_t splitter = line.find(" = ");
			if (splitter == std::string::npos) // Invalid strings
				continue;

			std::string key = line.substr(0, splitter);
			std::string value = line.substr(splitter + 3, line.size() - 1);

			auto& func = kv_map.find(key);
			if (func != kv_map.end())
			{
				if (display_config)
					std::cout << "Config: [ " << key << " == " << value << " ]\n";
				
				(*func).second(value);
			}
		}

		if (globals::path == "/path/to/avatars")
		{
			std::cout << "Config: You didn't initialize avatar path folder.\n";
			std::cout << "Config: Please select 'avatars' folder to make site work properly.\n";
			std::cout << "Press any button to leave...\n";
			std::ignore = getchar();
			exit(-10);
		}

		std::cout << "Config: Successfully parsed.\n";
	}

	int64_t get_time_epoch()
	{
		return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}
}