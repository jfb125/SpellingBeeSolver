/*
 * MemoryDictionary.h
 *
 *  Created on: Jul 17, 2025
 *      Author: joe
 */

#ifndef MEMORYDICTIONARY_H_
#define MEMORYDICTIONARY_H_

#include "Dictionary.h"

class MemoryDictionary: public Dictionary {
private:
	std::string *m_array;
	int m_next_word;
	int m_num_words;
	bool m_is_open;

public:
	MemoryDictionary() :
		m_array(nullptr),
		m_next_word(0),
		m_num_words(0),
		m_is_open(false) {}

	virtual ~MemoryDictionary() {}

	MemoryDictionary(const MemoryDictionary &other) {
		if (&other != this) {
			m_array = other.m_array;
			m_num_words = other.m_num_words;
			m_is_open = other.m_is_open;
			m_next_word = other.m_next_word;
		}
	}

	MemoryDictionary(MemoryDictionary &&other) {
		if (&other != this) {
			m_array = other.m_array;
			other.m_array = nullptr;
			m_num_words = other.m_num_words;
			other.m_num_words = 0;
			m_is_open = other.m_is_open;
			other.m_is_open = false;
			m_next_word = other.m_next_word;
			other.m_next_word = 0;
		}
	}

	MemoryDictionary& operator=(const MemoryDictionary &other) {
		if (&other != this) {
			m_array = other.m_array;
			m_num_words = other.m_num_words;
			m_is_open = other.m_is_open;
			m_next_word = other.m_next_word;
		}
		return *this;
	}

	MemoryDictionary& operator=(MemoryDictionary &&other) {
		if (&other != this) {
			m_array = other.m_array;
			other.m_array = nullptr;
			m_num_words = other.m_num_words;
			other.m_num_words = 0;
			m_is_open = other.m_is_open;
			other.m_is_open = false;
			m_next_word = other.m_next_word;
			other.m_next_word = 0;
		}
		return *this;
	}

	MemoryDictionary(std::string *array, int num_words) :
			m_array(array),
			m_next_word(0),
			m_num_words(num_words),
			m_is_open(false)
			{}

	bool	open(void) {
		if (m_array == nullptr)
			return false;
		m_next_word = 0;
		m_is_open = true;
		return true;
	}

	bool	close(void) {
		m_is_open = false;
		return true;
	}

	bool	begining(void) {
		m_next_word = 0;
		return true;
	}

	std::string nextWord(void) {
		if (!m_is_open ||
			m_next_word >= m_num_words ||
			m_array == nullptr) {
			return std::string("");
		} else {
			return m_array[m_next_word++];
		}
	}

	bool	isError(void) const {
		if (m_array == nullptr) {
			return true;
		} else {
			return false;
		}
	}

	bool	isOpen(void) const {
		return m_is_open;
	}
	bool	isNext(void) const {
		return m_next_word < m_num_words;
	}
};

#endif /* MEMORYDICTIONARY_H_ */
