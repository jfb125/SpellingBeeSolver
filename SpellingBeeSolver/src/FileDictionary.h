/*
 * FileDictionary.h
 *
 *  Created on: Jul 17, 2025
 *      Author: joe
 */

#ifndef FILEDICTIONARY_H_
#define FILEDICTIONARY_H_

#include "Dictionary.h"

#include <iostream>
#include <fstream>

#include "SpellingBeeSolver.h"

constexpr int DefaultFileDictionaryLineSize = 64;

#undef FILE_DICTIONARY_STATE
#define FILE_DICTIONARY_STATES \
	FILE_DICTIONARY_STATE(FILENAME_NOT_SPECIFIED)\
	FILE_DICTIONARY_STATE(FILE_NOT_OPEN)\
	FILE_DICTIONARY_STATE(FILE_OPEN)\
	FILE_DICTIONARY_STATE(FILE_OPEN_ERROR)\
	FILE_DICTIONARY_STATE(FILE_READ_ERROR)\
	FILE_DICTIONARY_STATE(READ_PAST_END)

#define FILE_DICTIONARY_STATE(e)	e,

enum class FileDictionaryState {
	FILE_DICTIONARY_STATES
};

std::string toString(FileDictionaryState state);

class FileDictionary: public Dictionary {
private:
	// member variables
	std::string *m_filename;
	std::ifstream *m_file;
	bool m_filename_specified;
	// file state errors are not reported if
	//  no attempt has been made to open the file
	bool m_fileopen_attempted;
	// this indicates the file has been opened,
	//	but does not guarantee the ifstream
	//	is not in an error state
	bool m_fileopen;
	// this is only valid if m_fileopen is true
	FileDictionaryState m_file_state;
	// this is the OR of all the things that could go wrong
	bool m_error;
	// limit the number of chars read
	int m_linesize;

	FileDictionary();

public:
	// Rule of Five
	virtual ~FileDictionary();
	// copy constructor and copy assignment operator
	//	disallowed due to not wanting
	//	contention/race conditions in the open file
//	FileDictionary(const FileDictionary &other);
//	FileDictionary& operator=(const FileDictionary &other);
	FileDictionary(FileDictionary &&other);
	FileDictionary& operator=(FileDictionary &&other);

	//	Resource Acquisition is Initialization
	FileDictionary(const std::string filename);

	/*	**********************************************	*/
	/*	functions inherited from base class Dictionary  */
	/* 	**********************************************	*/

	// attempts to open the file '*m_filename'
	//   if m_fileopened == true, returns false
	//	 if m_filename == nullptr, returns false
	//	 if fstream(*m_filename) constructor fails, return false;
	//	 returns true otherwise
	bool open(void);
	// attempst to close the file '*m_file'
	//	if the m_file == nullptr, returns true
	bool close(void);
	bool begining(void);
	std::string nextWord(void);
	bool isOpen(void) const;
	bool isNext(void) const;
	bool isError(void) const;

	//	functions specific to this class
//	bool open(std::string &filename);
	std::string	filename(void) const;
	FileDictionaryState getState() const;
};

#endif /* FILEDICTIONARY_H_ */
