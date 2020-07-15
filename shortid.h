
#include <string>
#include <random>

#ifndef GIOCHINO_DELLA_QUARENTENA_SHORTID_H
#define GIOCHINO_DELLA_QUARENTENA_SHORTID_H


using namespace std;

namespace quarantine_game {

    /**
     * Class used for generating shorter pseudo unique ids then regular UUIDs.
     * This ids are 16 characters long generated from a single 64-bit unsigned number (uint64) using the Base64
     * character set. They are URL safe and are used to generate the game ids.
     */
    class shortid {

    private:
        /**
         * Base64 charset. '+' and '/' are replaced with '-' and '_' to make the ids URL safe.
         */
        const static char chars[64];

        /**
         * Checks if the given char is part of the charset
         * @param c a given char
         * @return returns the index of the char inside the char array or 65 if the char is not present
         */
        static uint8_t index_of(char c);

    public:
        /**
         * Generates a pseudo-random 64-bit unsigned integer.
         *
         * @return a 64-bit uinsigned integer
         */
        static uint64_t get_random_long();
        /**
        * Generates an id from a 64-bit unsigned number
        *
        * @param base long number used in generating the id
        * @return a string representing the id generated
        */
        static string id(uint64_t base);

        /**
         * Generates a pseudo-random id using a random generator.
         *
         * @return a pseudo-random id
         */
        static string get_new_id();

        /**
         * Generates an id from a string and time.
         * The first 32 bits of the long integer are generated using the first 4 characters of the string, while the
         * other 32 are generated using the last 32 bits of the current time in milliseconds (another 64 bit number)
         *
         * @param text the string used in the generation the id
         * @return the id generated. Note that this can return an empty string if the given string is shorter than 4
         *         characters
         */
        static string get_new_id(string text);


        /**
         * Checks if the given string is an id.
         *
         * @param id the id
         * @return true or false based on the outcome
         */
        static bool is_id(string id);
    };


}


#endif //GIOCHINO_DELLA_QUARENTENA_SHORTID_H