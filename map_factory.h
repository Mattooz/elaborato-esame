#include <string>
#include "map.h"

#ifndef ELABORATO_ESAME_MAP_FACTORY_H
#define ELABORATO_ESAME_MAP_FACTORY_H

using namespace std;

namespace QuarantineGame::MapFactory {
        extern const QuarantineGame::Map not_found;

        /**
         * Gets a map from a given name. If no map is found it returns a "not-found" map.
         *
         * @param name the name of the map.
         * @return a map.
         */
        QuarantineGame::Map from_name(string name);

        /**
         * Gets a map from a given id. If no map is found it returns a "not-found" map.
         *
         * @param id the id of the map
         * @return a map.
         */
        QuarantineGame::Map from_id(string id);

        /**
         * Loads all the maps into memory if the cached map vector is empty, otherwise it will return the
         * cached vector.
         *
         * @return a map vector.
         */
        vector<QuarantineGame::Map> map_list();
    }


#endif //ELABORATO_ESAME_MAP_FACTORY_H
