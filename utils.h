//
// Created by Niccolò Mattei on 17/07/2020.
//
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <codecvt>

#ifndef ELABORATO_ESAME_UTILS_H
#define ELABORATO_ESAME_UTILS_H

using namespace std;

namespace QuarantineGame {
    /**
     * Namespace containing miscellaneous static methods.
     */
    namespace Utils {
        /**
        * Generates a pseudo-random 64-bit unsigned integer.
        *
        * @return a 64-bit uinsigned integer
        */
        uint64_t get_random_long();

        /**
         * Reads a file from storage.
         *
         * @param path_to_file the path of the file.
         * @return the contents of the file. If it fails to find the file returns "not found".
         */
        string read_file(const string &path_to_file) noexcept;

        /**
         * Reads an utf-8 file from storage.
         *
         * @param path_to_file the path of the file
         * @return a wstring with the contents of the file.
         */
        wstring read_utf8_file(const string &path_to_file) noexcept;

        uint8_t get_random_dice();

        uint32_t get_random_num(uint32_t max);

        vector<string> split(const string &input, const string &regex);

        bool is_number(string s);

        bool is_integer(string s);
    };

    class game_error : public exception {
    private:
        string _what;
    public:
        explicit game_error(const string &what);

        const char *what() const noexcept override;
    };
}

#endif //ELABORATO_ESAME_UTILS_H
