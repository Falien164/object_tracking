#pragma once

enum class StatusCode{
	OK,
	MissingFile,
	MissingFrame,
	CameraMissing,
	NotLoadedCocoFile,
};

static const char* StatusToString(StatusCode s) {
	switch (s) {
	case StatusCode::OK:
		return "OK";
	case StatusCode::MissingFile:
		return "MissingFile";
	case StatusCode::MissingFrame:
		return "MissingFrame";
	case StatusCode::CameraMissing:
		return "CameraMissing";	
	}
}