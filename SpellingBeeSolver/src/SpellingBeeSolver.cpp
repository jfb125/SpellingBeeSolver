//============================================================================
// Name        : SpellingBeeSolver.cpp
// Author      : Joe Baker
// Version     :
// Copyright   : Joe Baker, 2025
// Description : Program to search for words that solve NYT Games Spelling Bee
//============================================================================

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <filesystem>
#include <memory>
#include <ctype.h>

#include "FileDictionary.h"
#include "MemoryDictionary.h"

#include "SpellingBeeSolver.h"

/*    **********************************************************************    */
/*    **********************************************************************    */
/*                                macro definitions                             */
/*    **********************************************************************    */
/*    **********************************************************************    */

/*    revision 1.0 used an array of strings as the dictionary
 *     that required loading the whole external text dictionary (if supplied)
 *     into an array of strings
 *
 *     revision 2.0 uses a base abstract class 'Dictionary', which allows
 *     the derived class FileDictionary to load words one at a time from the disk
 */

#define    REVSION_STRING "Rev 2.00"

//using namespace std;

#define NUMBER_OF_WORDS_PRINTED_FROM_START_OF_DICTIONARY 4
#define NUMBER_OF_WORDS_PRINTED_FROM_END_OF_DICTIONARY 4
//    this sets the width of the index in 'printDictionary'
//      i.e.  if this is 5, "     3: aardvark\n"
#define WORD_NUMBER_PRINTED_WIDTH 5

#define MAX_NUMBER_OF_LETTERS 7
#define MINIMUM_MAX_NUMBER_OF_LETTERS_TO_SEARCH_FOR 5
#define GETLINE_TERMINATING_CHAR '\0'
#define GET_LETTERS_FROM_CONSOLE_QUIT_CHAR '!'
#define DEFAULT_DICTIONARY_NAME                             "dictionary"
#define DEFAULT_DICTIONARY_FILENAME_EXTENSION               ".txt"
#define DEFAULT_DICTIONARY_INCLUDE_FILENAME_EXTENSION       ".h"
#define DEFAULT_WORD                                        "emptydictionary"

#define HELP_STRING \
"SpellingBeeSolver:\n"\
"  Finds all words containing all characters and only\n"\
"    the characters from a user entered string\n"\
"    of at least " << MINIMUM_MAX_NUMBER_OF_LETTERS_TO_SEARCH_FOR\
                 << " but not more than " << MAX_NUMBER_OF_LETTERS << " letters\n"\
"    by searching a user specified dictionary file.\n"\
"    If no dictionary file is specified, an internal default dictionary is used\n"\
"\n"\
"  Usage: SpellingBeeSolver [-h] [-l \"abcdefg\"] [-f \"path/filename.ext\"]\n"\
"    -h prints this help string\n"\
"    -l uses a quote enclosed list of letters as search terms\n"\
"    -f specifies the path and filename to the dictionary\n"

#define HELP_TOKEN    'h'
#define LETTERS_TOKEN 'l'
#define FILENAME_TOKEN 'f'

#define USE_CONSOLE_FOR_LETTERS    -1
#define USE_DEFAULT_DICTIONARY    -1

#define FILE_INPUT_CHAR_ARRAY_LENGTH    128

#define COMPILE_DEFAULT_DICTIONARY 0


/*    **********************************************************************    */
/*    **********************************************************************    */
/*                                data types                                    */
/*    **********************************************************************    */
/*    **********************************************************************    */


/*    **********************************************************************    */
/*    **********************************************************************    */
/*                                internal dictionary                            */
/*    **********************************************************************    */
/*    **********************************************************************    */

#if COMPILE_DEFAULT_DICTIONARY == 1
#include "dictionary_array.h"
#else
static word_t default_word_list[] = {
        "DEFAULT", "DICTIONARY", "NOT",
        "COMPILED", "IN", "THIS", "BUILD" };
#endif

struct DictionaryStruct {
    word_t *word_list;
    dictionary_size_t size;
};


/*    **********************************************************************    */
/*    **********************************************************************    */
/*                                function prototypes                            */
/*    **********************************************************************    */
/*    **********************************************************************    */

bool ensureAllLettersAreInWord(letters_t &letters, word_t &word);
bool ensureWordOnlyUsesLetters(letters_t &letters, word_t &word);
int getLettersFromToken(letters_t &letters, char *cmd_line_token, unsigned max_number_of_letters);
int getLettersFromConsole(letters_t &letters, unsigned max_number_of_letters);
//dictionary_size_t loadDictionaryFromFile(DictionaryStruct &dictionary, std::string &filename);
void parseCommandLine(int &letters_arg_position, int &filename_arg_position, int argc, char **argv);
void printDictionary(Dictionary *dictionary, int num_words_at_start);
void removeDuplicateLetters(letters_t &letters);


/*    **********************************************************************    */
/*    **********************************************************************    */
/*                                    main                                    */
/*    **********************************************************************    */
/*    **********************************************************************    */

int main(int argc, char **argv) {

    std::cout << "SpellingBeeSolver " << REVSION_STRING
              << " built on "
              << __DATE__ << " at " << __TIME__ << std::endl;

    std::string default_dictionary_filename(DEFAULT_DICTIONARY_NAME);
    std::string default_dictionary_filename_extension(DEFAULT_DICTIONARY_FILENAME_EXTENSION);
    std::string default_dictionary_include_filename_extension(DEFAULT_DICTIONARY_INCLUDE_FILENAME_EXTENSION);

    int num_words_printed_from_start_of_dictionary =
            NUMBER_OF_WORDS_PRINTED_FROM_START_OF_DICTIONARY;
    size_t minimum_number_of_letters_to_search_for =
            MINIMUM_MAX_NUMBER_OF_LETTERS_TO_SEARCH_FOR;

    letters_t letters;
    dictionary_size_t default_dictionary_size =
            static_cast<long>(sizeof(default_word_list)/sizeof(word_t));

    MemoryDictionary default_dictionary(default_word_list, default_dictionary_size);

    FileDictionary *imported_dictionary;

    Dictionary *dictionary;

    //    attempt to open a dictionary file if provided
    std::string filename("");
    std::string letters_str("");

    int letters_arg_position = USE_CONSOLE_FOR_LETTERS;
    int filename_arg_position = USE_DEFAULT_DICTIONARY;

    parseCommandLine(letters_arg_position, filename_arg_position, argc, argv);

    if (filename_arg_position != USE_DEFAULT_DICTIONARY) {
        filename.clear();
        filename += argv[filename_arg_position];
        imported_dictionary = new FileDictionary(filename);
        if (imported_dictionary->isError()) {
            std::cout << "Unable to load dictionary from file " << filename
                      << std::endl
                      << "Using default internal dictionary" << std::endl;
            dictionary = &default_dictionary;
        } else {
            std::cout << "Opened dictionary file " << filename << std::endl;
            dictionary = imported_dictionary;
        }
    } else {
        std::cout << "Using default internal dictionary" << std::endl;
        dictionary = &default_dictionary;
    }

    std::cout << std::endl;
    dictionary->open();
    printDictionary(dictionary, num_words_printed_from_start_of_dictionary);
    dictionary->begining();

    if (letters_arg_position == USE_CONSOLE_FOR_LETTERS) {
        getLettersFromConsole(letters, MAX_NUMBER_OF_LETTERS);
    } else {
        getLettersFromToken(letters, argv[letters_arg_position], MAX_NUMBER_OF_LETTERS);
    }

    removeDuplicateLetters(letters);
    letters_str += '"';
    for (size_t i = 0; i != letters.size(); i++) {
        letters_str += letters[i];
    }
    letters_str += '"';

    if (letters.size() < minimum_number_of_letters_to_search_for) {
        std::cout << "Error: Only " << letters.size()
                  << " letters will result in too many matches." << std::endl
                  << "       Not performing search" << std::endl;
        return EXIT_SUCCESS;
    }

    std::cout << "Searching for words that contain all of these and only these letters:    "
              << letters_str << std::endl << std::endl;
    int success_count = 0;

    while (dictionary->isNext()) {
        std::string word = dictionary->nextWord();
        if (ensureAllLettersAreInWord(letters, word) &&
            ensureWordOnlyUsesLetters(letters, word)) {

            std::cout << std::setw(WORD_NUMBER_PRINTED_WIDTH)
                      << ++success_count << ": "
                      << letters_str << "   found in word   " << word << std::endl;
        }
    }
    if (success_count == 0) {
        std::cout << "No qualifying words found the dictionary" << std::endl;
    }

//    if (imported_dictionary) {
//        delete imported_dictionary;
//    }

    std::cout << std::endl << "SpellingBeeSolver completed" << std::endl;
    return EXIT_SUCCESS;
}


/*    **********************************************************************    */
/*    **********************************************************************    */
/*                            auxiliary functions                                */
/*    **********************************************************************    */
/*    **********************************************************************    */

bool ensureAllLettersAreInWord(letters_t &letters, word_t &word) {

    std::string lower_case("");

    for (size_t i = 0; i != word.size(); i++) {
        lower_case += tolower(word[i]);
    }

    for (char letter : letters) {
        if (lower_case.find(tolower(letter)) == std::string::npos) {
            return false;
        }
    }
    return true;
}


bool ensureWordOnlyUsesLetters(letters_t &letters, word_t &word) {
    for (char word_letter : word) {
        bool letter_in_word_is_not_in_input = true;
        for (char input_letter : letters) {
            if (tolower(word_letter) == tolower(input_letter)) {
                letter_in_word_is_not_in_input = false;
                break;
            }
        }
        if (letter_in_word_is_not_in_input)
            return false;
    }
    return true;
}


int getLettersFromToken(letters_t &letters, char *token, unsigned max_number_of_letters) {

    unsigned num_letters = 0;
    letters.clear();

    for (unsigned i = 0; token[i] != '\0'; i++)
    {
        char c = token[i];
        if (c == '"') {
            continue;
        } else {
            if (isalpha(c)) {
                if (num_letters >= max_number_of_letters) {
                    std::cout << "Parsed " << max_number_of_letters
                              << " from command line.  Discarding letters after \"";
                    for (unsigned j = 0; j != max_number_of_letters; j++) {
                        std::cout << token[j];
                    }
                    std::cout << "\"" << std::endl;
                    break;
                }
                letters.push_back(c);
                num_letters++;
            }
        }
    }

    return letters.size();
}


int getLettersFromConsole(letters_t &letters, unsigned max_number_of_letters) {

    char empty_letter = '~';
    unsigned size_of_line = max_number_of_letters+1;
    char line[size_of_line];

    letters.clear();
    for (unsigned i = 0; i < max_number_of_letters; i++) {
        line[i] = empty_letter;
    }
    line[max_number_of_letters] = '\0';

    std::cout << "Enter between " << MINIMUM_MAX_NUMBER_OF_LETTERS_TO_SEARCH_FOR
              << " and " << max_number_of_letters
              << " letters, then press return (enter ! to quit): ";
    std::cin.getline(line, size_of_line);
    int number_of_entered_letters = 0;
    for (unsigned i = 0; i != max_number_of_letters; i++) {
        switch(line[i]) {
        case GET_LETTERS_FROM_CONSOLE_QUIT_CHAR:
            letters.clear();
            std::cout << "Exiting due to '!'" << std::endl;
            goto GET_LETTERS_RETURN;
        case GETLINE_TERMINATING_CHAR:
            // '\0'
            goto GET_LETTERS_RETURN;
        default:
            if (isalpha(line[i])) {
                letters.push_back(line[i]);
                number_of_entered_letters++;
            }
        }
    }

GET_LETTERS_RETURN:
    return letters.size();
}

dictionary_size_t loadDictionaryFromFile(DictionaryStruct &dictionary, std::string &filename) {

    //    set the dictionary to a valid state
    dictionary.size = 0;
    dictionary.word_list = new std::string(DEFAULT_WORD);
    dictionary_size_t num_words = 0;

    //     attempt to open the file

    std::ifstream dictionary_file(filename);
    if (!dictionary_file.is_open()) {
        std::cout << "file " << filename << " failed to open in path "
                  << std::filesystem::current_path() << std::endl;
        return dictionary.size;
    }

    //    count the number of words in the file
    //      a word is defined as a string of text that terminates with
    //      a ',', ' ', '\n'
    char *line = new char[FILE_INPUT_CHAR_ARRAY_LENGTH];
    char line_delimiter = '\n';

    //    TODO - parse delimiters other than '\n'
    while (!dictionary_file.eof()) {
        num_words++;
        dictionary_file.getline(line, FILE_INPUT_CHAR_ARRAY_LENGTH-1, line_delimiter);
        if (!dictionary_file.fail()) {
            num_words++;
        } else {
            break;
        }
    }

    //     if counting the number of words in the file failed (or it is empty)
    if (num_words == 0)
        return dictionary.size;

    // allocate the memory
    try {
        dictionary.word_list = new word_t[num_words];
    } catch (...) {
        std::cout << "allocation of dictionary containing " << num_words
                  << " from file " << filename
                  << " failed" << std::endl;
        return dictionary.size;

    }

    //     move the istream back to the start of the file
    dictionary_file.clear();
    dictionary_file.seekg(std::ios::beg);

    num_words = 0;
    while (!dictionary_file.eof()) {
        line[0] = '\0';
        dictionary_file.getline(line, FILE_INPUT_CHAR_ARRAY_LENGTH-1, line_delimiter);
        if (line[0] != '\0') {
            dictionary.word_list[num_words] = line;
            num_words++;
        } else {
            break;
        }
    }
    dictionary.size = num_words;

    dictionary_file.close();
    return dictionary.size; // dictionary.size();
}


void parseCommandLine(int &letters_arg_position, int &filename_arg_position, int argc, char **argv)
{
    letters_arg_position  = USE_CONSOLE_FOR_LETTERS;
    filename_arg_position = USE_DEFAULT_DICTIONARY;
    bool print_help_menu = false;

    // argv[0] is the program name

    for (int i = 1; i < argc; i++) {
        char *token = argv[i];
        if (token[0] == '-') {
            token[1] = tolower(token[1]);
            switch(token[1]) {
            case HELP_TOKEN:
                print_help_menu = true;
                break;
            case LETTERS_TOKEN:
                letters_arg_position = i+1;
                // move past the next token = letter list
                i++;
                break;
            case FILENAME_TOKEN:
                filename_arg_position = i+1;
                // move past the next token = filename
                i++;
                break;
            default:
                std::cout << "Unrecognized command line switch: "
                          << token << std::endl;
                print_help_menu = true;
                break;
            }
        }
    }

    if (print_help_menu) {
        std::cout << HELP_STRING << std::endl;
    }
}


void printDictionary(Dictionary *dictionary, int num_words_at_start) {

    // store iostream flags that will be modified using iomanip members
    std::ios_base::fmtflags _flags = std::cout.flags();

    // improves readability

//    int beginning_block_size = num_words_at_start;
    dictionary->open();
    dictionary->begining();

    if (dictionary->isError()) {
        std::cout << "dictionary in error state" << std::endl;
        return;
    }

    for (int i = 0; i != num_words_at_start; i++) {
        if (!dictionary->isNext())
            break;
        std::cout << std::setw(5) << std::right << i << ": "
                  << dictionary->nextWord() << std::endl;
    }
    dictionary->begining();

    // restore iostream flags that were modified using iomanip members
    std::cout.flags(_flags);
}


void removeDuplicateLetters(letters_t &letters) {

    int last_letter_position = letters.size()-1;

    for (int i = 0; i < last_letter_position; i++) {
        char c = tolower(letters[i]);
        for (int j = i+1; j <= last_letter_position; j++) {
            // if [j] duplicates [i],
            //   copy the rest of the vector onto [j:last_position-1]
            if (c == tolower(letters[j])) {
                while (j < last_letter_position) {
                    letters[j] = letters[j+1];
                    j++;
                }
                // remove the last element
                //  that has been copied into [last_position-1]
                letters.pop_back();
                // the array is now one element smaller
                last_letter_position--;
                break;
            }
        }
    }
}
