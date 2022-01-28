#pragma once

#include <vector>
#include <string>
#include "common.h"
class CocoFiles {
public:
	//Methods
	StatusCode LoadCOCOFile(const std::string& fileName);

	// Poles
	std::vector<std::string> NamesCocoArray;
	~CocoFiles() {
		NamesCocoArray.clear();
	}

};