#ifndef ELABORATO_ESAME_MAP_H
#define ELABORATO_ESAME_MAP_H

#include <string>
#include <vector>
#include <memory>
#include "player.h"

using namespace std;

namespace QuarantineGame {
    /**
     * Base class for all the boxes on the board.
     */
    class Box {
    protected:
        uint8_t position;

        /**
         * Constructor for the box. Only requires a position param.
         *
         * @param position the position on the map.
         */
        explicit Box(uint8_t position);

    public:
        //TODO Render abstract

        /*
         * Required in order to make Box polymorphic and add safety while casting.
         */
        virtual const uint8_t &_position() const;

        virtual ~Box();
    };




    /**
     * Derived class from box. Used to represent the "glitch" and "prison" boxes.
     */
    class FunctionalBox : public Box {
    public:
        enum BoxType {
            GOTO_PRISON, PRISON, START, OTHER, GLITCH
        };
    private:
        BoxType type;
    public:
        /**
         * Constructor for functional_box.
         *
         * @param position the position of the box.
         * @param type the type of the functional box. Can either be "glitch", "goto-prison", "prison" or "start".
         */
        FunctionalBox(uint8_t position, string type);

        /**
         * @return the type of the box.
         */
        BoxType _type();

        const uint8_t &_position() const override;

        ~FunctionalBox() override;


    };

    /**
     * Derived class from box. Used to represent the property boxes on the map.
     */
    class PropertyBox : public Box {
    private:
        uint8_t id;
        int32_t cost;
        string name;
        uint8_t owner;
        uint8_t houses;
    public:

        /**
         * Constructor for property_box.
         *
         * @param position the position of the box on the map.
         * @param id the of the box. Each property box is numbered clockwise.
         * @param cost the cost of the property.
         * @param name the name of the property.
         */
        PropertyBox(uint8_t position, uint8_t id, int32_t cost, string name);

        /**
         * @return the id of the box.
         */
        const uint8_t &_id() const;

        /**
         * @return the cost of the property.
         */
        const int32_t &_cost() const;

        /**
         * @return the name of the property
         */
        const string &_name() const;

        /**
         * Returns a reference to the owner variable of the class.
         *
         * @return the owner of the property.
         */
        uint8_t &_owner();

        /**
         * Returns a reference to the houses variable of the class.
         *
         * @return the number of houses on the property.
         */
        uint8_t &_houses();

        const uint8_t &_position() const override;

        virtual ~PropertyBox() override;
    };

    /**
     * Represents the map of the game.
     *
     * All the methods in this class are able to handle an unordered vector of boxes.
     */
    class Map {
    private:
        string map_name;
        string map_id;
        vector<shared_ptr<Box>> boxes;
    public:
        const static uint8_t not_found;

        /**
         * Constructor for the map class.
         *
         * @param boxes a vector containing the boxes of the map.
         */
        Map(vector<shared_ptr<Box>> boxes, string map_name, string map_id);

        const string &_map_name() const;

        const string &_map_id() const;

        const uint32_t box_count() const;

        /**
         * Gets the cost of a specific property_box.
         *
         * @param box the id of the property.
         * @return the cost of the property. If no property is found at the given id it returns -1.
         */
        int32_t cost(uint8_t box) const;

        /**
         * Gets the name of a specific property_box.
         *
         * @param box the id of the property.
         * @return the name of the property. If no property is found at the given id it returns "not found".
         */
        string name(uint8_t box) const;

        /**
         * Gets the id of a specific property_box.
         *
         * @param position the position of the box on the map.
         * @return the id of the property. If no property is found at the given position it returns 0xFF.
         */
        uint8_t id(uint8_t position) const;

        /**
        * Gets the pos of a specific property_box.
        *
        * @param id the id of the property.
        * @return the id of the property. If no property is found at the given id it returns 0xFF.
        */
        uint8_t pos(uint8_t id) const;

        /**
         * Calculates the distance to the nearest glitch box. This distance must be > 1 in order to be usable.
         *
         * @param pos the position on the map.
         * @return the distance to the nearest glitch box. If no usable glitch box is found then it returns 0xFF.
         */
        uint8_t distance_to_next_glitch(uint8_t pos) const;

        /**
         * Checks if the box at the given position is a glitch box.
         *
         * @param pos the position that will be checked.
         * @return true or false.
         */
        bool is_glitch(uint8_t pos) const;

        /**
         * Calculates the distance to the nearest goto-prison box. This distance must be > 1 in order to be usable.
         *
         * @param pos the position on the map.
         * @return the distance to the goto-prison box.
         */
        uint8_t distance_to_prison(uint8_t pos) const;

        /**
         * Checks if the box at the given position is a goto-prison box.
         *
         * @param pos the position that will be checked.
         * @return true or false.
         */
        bool is_prison(uint8_t pos) const;

        /**
         * Gets all the properties that a player has.
         *
         * @param player the id of the player
         * @return the properties of the player. If the player has no properties it will return an empty vector.
         */
        vector<shared_ptr<PropertyBox>> get_player_properties(uint8_t player) const;

        /**
         * Deletes all the properties of a player
         *
         * @param player the id of the player
         */
        void delete_player_properties(uint8_t player) const;

        /**
         * @param pos the position of the box
         * @return a box. If no box is found it returns nullptr.
         */
        shared_ptr<Box> operator[](uint8_t pos) const;

        /**
         * @param name the name of the property.
         * @return a property box. If no box is found at the given name it returns nullptr.
         */
        shared_ptr<PropertyBox> operator[](const string& name) const;

        /**
         * @param id the id of the property.
         * @return a property box. If no box is found at the given id it returns nullptr.
         */
        shared_ptr<PropertyBox> from_id(uint8_t id) const;

        /**
         *
         */
        void reset();

        virtual ~Map();
    };


}


#endif //ELABORATO_ESAME_MAP_H
