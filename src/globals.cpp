#include <filesystem>
#include <fstream>
#include <iostream>

#include "globals.hpp"

namespace globals
{
	namespace redis
	{
		std::string address = "127.0.0.1";
		size_t port = 6379;
		std::string password = "";
	}

	std::string path = "/path/to/folder/";
	bool cache_state = true;
	std::unordered_map<std::string, std::pair<oatpp::String, int64_t>> cache = {};

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

			std::cout << "Config: [ " << key << " == " << value << " ]\n";

			// I know this might be shitty but its works
			if (key == "redis_address")
				globals::redis::address = value;
			if (key == "redis_port")
				globals::redis::port = std::stoll(value);
			if (key == "redis_password")
				globals::redis::password = value;

			if (key == "path")
				globals::path = value;
			if (key == "disable_cache")
				globals::cache_state = !string_to_bool(value);
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