#include <iostream>
#include "utils.h"
#include "shortid.h"
#include "glitch_handler.h"
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include <server_http.hpp>
#include "game.h"

#define REQUEST_LAMBDA_DECL [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request)
#define SETUP_HEADERS request->header.insert({"Access-Control-Allow-Origin", "*"}); request->header.insert({"Access-Control-Request-Method", "*"}); request->header.insert({"Access-Control-Allow-Headers", "*"});
#define SET(path, method) server.resource[path][method] = REQUEST_LAMBDA_DECL
#define IFELSE_LADDER if(!game->started()) response->write(SimpleWeb::StatusCode::server_error_internal_server_error); else if((game->_turns() % game->player_count()) != game->get_player_turn(name)) response->write(SimpleWeb::StatusCode::server_error_internal_server_error);

const string web_pages = "/Users/niccolomattei/CLionProjects/elaborato-esame/resources/web_pages";

using namespace std;
using namespace quarantine_game;
using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
using json = nlohmann::json;

HttpServer server;
vector<game> *games = new vector<game>();

game *get_game(const string &id) {
    for (auto it : *games) {
        if (it._id() == id) return &it;
    }
    return nullptr;
}


void setup_web_server() {
    server.config.port = 80;

    server.default_resource["GET"] = REQUEST_LAMBDA_DECL {
        SETUP_HEADERS

        response->write(utils::read_file(web_pages + "/404.html"));
    };

    SET("^/$", "GET") {
        SETUP_HEADERS

        response->write(utils::read_file(web_pages + "/index.html"));
    };

    SET("^/crt$", "POST") {
        SETUP_HEADERS
        json form = json::parse(request->content);

        string host = form["creator"];
        game g{host, 2000, "default", "default"};

        g.get_player(host)->add_update(g.create_default_update());

        games->push_back(g);

        json res;

        res["creator"] = host;
        res["uuid"] = g._id();
        res["starting_money"] = 2000;

        response->write(SimpleWeb::StatusCode::success_ok, res.dump());
    };

    SET("^/upd$", "POST") {
        SETUP_HEADERS

        json form = json::parse(request->content);

        string player = form["player_name"];
        string id = form["uuid"];
        string turn = form["turn"];


        auto game = get_game(id);
        if (game != nullptr) {
            auto p = game->get_player(player);
            if (p != nullptr) {
                if (!turn.empty()) {
                    json obj;

                    obj["turn"] = game->get_player_turn(player);

                    response->write(obj.dump());
                } else response->write(game->get_player(player)->get_update().dump());
            } else response->write(SimpleWeb::StatusCode::client_error_not_found);
        } else response->write(SimpleWeb::StatusCode::client_error_not_found);

    };

    SET("^/snd_upd$", "POST") {
        SETUP_HEADERS

        json form = json::parse(request->content);

        string uuid = form["uuid"];
        string name = form["player_name"];
        string type = form["type"];

        auto game = get_game(uuid);

        if (game == nullptr) {
            response->write(SimpleWeb::StatusCode::server_error_internal_server_error);
        } else {
            if (type == "buy") {
                IFELSE_LADDER else {
                    auto player = game->get_player(name);
                    auto box = game->map()[player->_position()];
                    auto cast = dynamic_pointer_cast<property_box>(box.lock());

                    if (cast) {
                        if (cast->_owner() == 7) {
                            game->buy_property(cast->_id(), name);
                            response->write(game::ok_status.dump());
                        } else response->write(game::not_ok_status.dump());
                    } else response->write(game::not_ok_status.dump());
                }
            } else if (type == "next_player") {
                IFELSE_LADDER else {
                    game->next_player(game->get_player_turn(name));
                    response->write(game::ok_status.dump());
                }
            } else if (type == "roll_dice") {
                IFELSE_LADDER else {
                    game->roll_dice(game->get_player_turn(name));
                    response->write(game::ok_status.dump());
                }
            } else if (type == "house") {
                IFELSE_LADDER else {

                    string property = form["property"];
                    int count = form["house_count"];
                    auto box = game->map()[property];
                    auto cast = dynamic_pointer_cast<property_box>(box.lock());

                    if (cast->_owner() != game->get_player_turn(name))
                        response->write(game::not_ok_status.dump());
                    else {
                        game->buy_house(game->get_player_turn(name), cast->_id(), count);
                    }

                }
            } else if (type == "glitch_response") {
                IFELSE_LADDER else {
                    string resp = form["response"];
                    int option = stoi(resp);

                    game->send_to_all(game->create_glitch_response_update(option));
                }
            } else if (type == "start") {
                if (game->started())
                    response->write(SimpleWeb::StatusCode::server_error_internal_server_error);
                else if (game->get_player_turn(name) != 0)
                    response->write(SimpleWeb::StatusCode::server_error_internal_server_error);
                else {
                    game->start();
                }
            }
        }
    };

}

int main() {
    setup_web_server();
    server.start();

    return 0;
}



//TODO add functions