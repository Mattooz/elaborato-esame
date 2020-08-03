#include <string>
#include "map.h"

#ifndef ELABORATO_ESAME_MAP_FACTORY_H
#define ELABORATO_ESAME_MAP_FACTORY_H

using namespace std;

namespace quarantine_game {
    /**
     * Fetches the maps used in the game from .json files.
     */
    class MapFactory {
    private:
        static const string map_folder;
        static vector<quarantine_game::Map> cached;
    public:

        /**
         * Gets a map from a given name. If no map is found it returns a "not-found" map.
         *
         * @param name the name of the map.
         * @return a map.
         */
        static quarantine_game::Map from_name(string name);

        /**
         * Gets a map from a given id. If no map is found it returns a "not-found" map.
         *
         * @param id the id of the map
         * @return a map.
         */
        static quarantine_game::Map from_id(string id);

        /**
         * Loads all the maps into memory if the cached map vector is empty, otherwise it will return the
         * cached vector.
         *
         * @return a map vector.
         */
        static vector<quarantine_game::Map> map_list();
    };
}


#endif //ELABORATO_ESAME_MAP_FACTORY_H
