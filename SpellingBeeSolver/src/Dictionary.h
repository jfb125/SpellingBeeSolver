/*
 * Dictionary.h
 *
 *  Created on: Jul 17, 2025
 *      Author: joe
 */

#ifndef DICTIONARY_H_
#define DICTIONARY_H_

#include <iostream>
#include <string>

class Dictionary {
public:
	Dictionary();
	virtual ~Dictionary();

	virtual bool	open(void) = 0;
	virtual bool	close(void) = 0;
	virtual bool	begining(void) = 0;
	virtual std::string nextWord(void) = 0;
	virtual	bool	isError(void) const = 0;
	virtual	bool	isOpen(void) const = 0;
	virtual bool	isNext(void) const = 0;
};

#endif /* DICTIONARY_H_ */
