#include <filesystem>
#include <fstream>
#include <iostream>

#include "config.hpp"

namespace config
{
	std::string path = "/path/to/folder/";

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
		
		std::vector<std::string> config_lines;
		std::string line;
		while (!file.eof())
		{
			std::getline(file, line);
			if (line.size() == 0 || line[0] == '#') // Skip comments and emply lines
				continue;

			config_lines.push_back(line);
		}

		for (std::string cfg : config_lines)
		{
			size_t splitter = cfg.find(" = ");
			if (splitter == std::string::npos) // Invalid strings
				continue;

			std::string key = cfg.substr(0, splitter);
			std::string value = cfg.substr(splitter + 3, cfg.size() - 1);

			std::cout << "Config: [ " << key << " == " << value << " ]\n";
			if (key == "path")
				config::path = value;
		}

		std::cout << "Config: Successfully parsed.\n";
	}
}