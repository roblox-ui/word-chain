#include <array>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_set>

std::vector<std::string> readDictionary(std::string file_path)
{
	std::unordered_set<char> annotations{':', '&', '#', '=', '<', '^', '~', '+'};

	std::vector<std::string> dictionaryVector{};

	std::ifstream dictIn(file_path, std::ios_base::in);
	if (!dictIn) {
		std::cerr << "Error: Could not open the file at " << file_path << std::endl;
		return dictionaryVector;
	}

	for (std::string word; std::getline(dictIn, word);) {
		if (!word.empty() && annotations.find(word.back()) != annotations.end())
			word.pop_back();
		dictionaryVector.push_back(std::move(word));
	}

	return dictionaryVector;
};
