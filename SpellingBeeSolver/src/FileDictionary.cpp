/*
 * FileDictionary.cpp
 *
 *  Created on: Jul 17, 2025
 *      Author: joe
 */

#include "FileDictionary.h"

FileDictionary::FileDictionary() :
	m_filename(nullptr),
	m_file(nullptr),
	m_filename_specified(false),
	m_fileopen_attempted(false),
	m_fileopen(false),
	m_file_state(FileDictionaryState::FILE_NOT_OPEN),
	m_error(false),
	m_linesize(DefaultFileDictionaryLineSize) {}

FileDictionary::~FileDictionary() {

	if (m_file) {
		m_file->close();
		delete m_file;
		m_file = nullptr;
	}

	if (m_filename) {
		delete m_filename;
		m_filename = nullptr;
	}
}

// copy constructor / assignment operator not allowed to avoid
//	creating two owners of an open file
#if 0
FileDictionary::FileDictionary(const FileDictionary &other) {
}

FileDictionary& FileDictionary::operator=(const FileDictionary &other) {
}
#endif

FileDictionary::FileDictionary(FileDictionary &&other) {
	// move the dynamically allocated members of other
	m_filename = other.m_filename;
	other.m_filename = nullptr;
	m_file = other.m_file;
	other.m_file = nullptr;

	m_filename_specified = other.m_filename_specified;
	other.m_filename_specified = false;

	m_fileopen_attempted = other.m_fileopen_attempted;
	other.m_fileopen_attempted = false;

	m_fileopen = other.m_fileopen;
	other.m_fileopen = false;

	m_file_state = other.m_file_state;
	other.m_file_state = FileDictionaryState::FILE_NOT_OPEN;

	m_error = other.m_error;
	other.m_error = false;

	m_linesize = DefaultFileDictionaryLineSize;
	other.m_linesize = DefaultFileDictionaryLineSize;
}

FileDictionary& FileDictionary::operator=(FileDictionary &&other) {
	if (&other == this)
		return *this;

	// move the dynamically allocated members of the other
	m_filename = other.m_filename;
	other.m_filename = nullptr;

	m_file = other.m_file;
	other.m_file = nullptr;

	//	assign the other's variables, and initialize the other's variables
	m_filename_specified = other.m_filename_specified;
	other.m_filename_specified = false;

	m_fileopen_attempted = other.m_fileopen_attempted;
	other.m_fileopen_attempted = false;

	m_fileopen = other.m_fileopen;
	other.m_fileopen = false;

	m_file_state = other.m_file_state;
	other.m_file_state = FileDictionaryState::FILE_NOT_OPEN;

	m_error = other.m_error;
	other.m_error = false;

	m_linesize = DefaultFileDictionaryLineSize;
	other.m_linesize = DefaultFileDictionaryLineSize;

	return *this;
}

FileDictionary::FileDictionary(const std::string filename) {

	m_filename = new std::string(filename);
	m_filename_specified = true;

	m_fileopen_attempted = true;
	m_file = new std::ifstream(filename);

	if (m_file->good()) {
		m_fileopen = true;
		m_error = false;
		m_file_state = FileDictionaryState::FILE_OPEN;
	} else {
		m_fileopen = false;
		m_error = true;
		m_file_state = FileDictionaryState::FILE_OPEN_ERROR;
	}

	m_linesize = DefaultFileDictionaryLineSize;
}


//	functions inherited from base class Dictionary
bool FileDictionary::open(void) {

	if (isError())
		return false;

	if (isOpen()) {
		begining();
		return true;
	}

	if (!m_filename_specified)
		return false;

	if (m_file) {
		m_file->close();
		delete m_file;
		m_file = nullptr;
	}

	m_fileopen_attempted = true;

	m_file = new std::ifstream(*m_filename);

	if (m_file->is_open()) {
		m_fileopen = true;
		m_file_state = FileDictionaryState::FILE_OPEN;
		m_error = false;
	} else {
		m_fileopen = false;
		m_file_state = FileDictionaryState::FILE_OPEN_ERROR;
		m_error = true;
	}

	return !m_error;
}
#if 0
bool FileDictionary::open(std::string &filename) {

	m_filename = new std::string(filename);
	m_filename_specified = true;
	return open();
}
#endif
bool FileDictionary::close(void) {

	if (m_file && m_file->is_open()) {
		m_file->close();
	}
	delete m_file;
	m_file = nullptr;

	m_filename_specified = m_filename == nullptr;
	m_fileopen_attempted = false;
	m_fileopen = false;
	m_file_state = FileDictionaryState::FILE_NOT_OPEN;
	m_error = false;
	return true;
}

bool FileDictionary::begining() {

	bool result = false;

	// if the file is not open, it is not an internal error
	//  even though the user may have made an error in calling this
	if (m_file_state != FileDictionaryState::FILE_OPEN) {
		return result;
	}

	// Move back to the beginning of the file
	m_file->clear(); // Clear any error flags (e.g., eofbit if at end of file)
	m_file->seekg(0, std::ios::beg); // Set the read pointer to the beginning
	if (!m_file->bad()) {
		m_error = false;
		m_file_state = FileDictionaryState::FILE_OPEN;
		result = true;
	} else {
		m_error = true;
		m_file_state = FileDictionaryState::FILE_READ_ERROR;
		result = false;
	}

	return result;
}

bool FileDictionary::isError(void) const {

	// if no file operations have been attempted, there can be no error
	if (!m_fileopen_attempted)
		return false;

	// m_fileopen_attempted is true, if file is not open, fileopen failed
	if (!m_fileopen)
		return true;

	// the file was opened
	if (m_file_state == FileDictionaryState::FILE_OPEN_ERROR ||
		m_file_state == FileDictionaryState::FILE_READ_ERROR ||
		m_file_state == FileDictionaryState::READ_PAST_END)
		return true;

	if (m_error)
		return true;

	return false;
}

bool FileDictionary::isOpen(void) const {

	// the file is open, and it is in a non-errored state
	return m_fileopen && m_file_state == FileDictionaryState::FILE_OPEN;
}

bool FileDictionary::isNext(void) const {

	if (!isOpen())
		return false;

	return !m_file->eof();
}

std::string FileDictionary::nextWord() {

	std::string result("");
	if (m_file == nullptr || !isNext() || isError() || !isOpen())
		return result;

	if (m_file->eof()) {
		m_file_state = FileDictionaryState::READ_PAST_END;
		m_error = true;
		return result;
	}

	char *line = new char[m_linesize+1];
	m_file->getline(line, m_linesize);

	result = line;
	delete[] line;
	line = nullptr;
	return result;
}

std::string	FileDictionary::filename(void) const {
	return *m_filename;
}

FileDictionaryState FileDictionary::getState() const {
	return m_file_state;
}

/* ************************************************************	*/
/*					enum class FileDictioaryState				*/
/* ************************************************************	*/

//#define STRINGIFY(x) #x
#undef FILE_DICTIONARY_STATE
#define FILE_DICTIONARY_STATE(e) #e,
static std::string FileDictionaryStateStrings[] = {
		FILE_DICTIONARY_STATES
};

bool isValid(FileDictionaryState state) {
	switch(state) {
	case FileDictionaryState::FILENAME_NOT_SPECIFIED:
	case FileDictionaryState::FILE_NOT_OPEN:
	case FileDictionaryState::FILE_OPEN:
	case FileDictionaryState::FILE_OPEN_ERROR:
	case FileDictionaryState::FILE_READ_ERROR:
	case FileDictionaryState::READ_PAST_END:
		return true;
	default:
		return false;
	}
}

std::string toString(FileDictionaryState state) {
	if (isValid(state)) {
		int i = static_cast<int>(state);
		return FileDictionaryStateStrings[i];
	}
	return "INVALID FileDictionaryState";
}
