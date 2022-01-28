#include "CocoFiles.h"

#include <fstream>
#include <iostream>
#include <QDebug>

StatusCode CocoFiles::LoadCOCOFile(const std::string& fileName) {
	if (NamesCocoArray.size()!=0) {
		return StatusCode::OK;
	}
	qDebug() << "Loading file...";
	std::ifstream myfile(fileName);  //"x64/data/coco.names"
	if (!myfile.is_open()) {
		qDebug() << "Unable to open file.";
		return StatusCode::MissingFile;
	}

	std::string line;
	while (getline(myfile, line))
		NamesCocoArray.emplace_back(line);
	myfile.close();
	qDebug() << "Object names loaded";
	return StatusCode::OK;
}