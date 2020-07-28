//
// Created by Niccolò Mattei on 17/07/2020.
//
#include <string>
#include <vector>

#ifndef ELABORATO_ESAME_UTILS_H
#define ELABORATO_ESAME_UTILS_H

using namespace std;

namespace quarantine_game {
    /**
     * Class containing miscellaneous static methods.
     */
    class utils {
    public:
        /**
        * Generates a pseudo-random 64-bit unsigned integer.
        *
        * @return a 64-bit uinsigned integer
        */
        static uint64_t get_random_long();

        /**
         * Reads a file from storage.
         *
         * @param path_to_file the path of the file.
         * @return the contents of the file. If it fails to find the file returns "not found".
         */
        static string read_file(const string &path_to_file) noexcept;

        static uint8_t get_random_dice();

        static uint32_t get_random_num(uint32_t max);

        static vector<string> split(const string &input, const string &regex);

        static bool is_number(string s);

        static bool is_integer(string s);
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
