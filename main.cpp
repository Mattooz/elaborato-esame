#include <iostream>
#include "utils.h"
#include "shortid.h"
#include "glitch_handler.h"
#include <vector>
#include <string>
#include <locale>
#include <nlohmann/json.hpp>
#include <server_http.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/locale.hpp>
#include <spdlog/spdlog.h>
#include "game.h"

#define REQUEST_LAMBDA_DECL [](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request)
#define SETUP_HEADERS request->header.insert({"Access-Control-Allow-Origin", "*"}); request->header.insert({"Access-Control-Request-Method", "*"}); request->header.insert({"Access-Control-Allow-Headers", "*"});
#define SET(path, method) server.resource[path][method] = REQUEST_LAMBDA_DECL
#define IFELSE_LADDER if(!game->started()) response->write(SimpleWeb::StatusCode::server_error_internal_server_error); else if((game->_turns() % game->player_count()) != game->get_player_turn(name)) response->write(SimpleWeb::StatusCode::server_error_internal_server_error);
#define TRY_CATCH(code) try { code } catch(std::exception &e) {spdlog::error(e.what()); response->write(SimpleWeb::StatusCode::server_error_internal_server_error); return;}

const string web_pages = "/Users/niccolomattei/CLionProjects/elaborato-esame/resources/web_pages";

using namespace std;
using namespace quarantine_game;
using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
using json = nlohmann::json;
using namespace boost;
using namespace boost::locale;

HttpServer server;
vector <std::shared_ptr<Game>> games;

std::map<string, string> parse_cookies(SimpleWeb::CaseInsensitiveMultimap &header) {
    auto it = header.find("Cookie");

    std::map<string, string> res;

    string cookie = (*it).second;
    boost::algorithm::trim(cookie);

    for (auto &a : Utils::split(cookie, ";")) {
        auto b = Utils::split(a, "=");
        string c = b[0], d = b[1];

        boost::algorithm::trim(c);
        boost::algorithm::trim(d);

        pair <string, string> entry{c, d};

        res.insert(entry);
    }

    return res;
}

std::shared_ptr<Game> get_game(const string &id) {
    for (auto &it : games) {
        if (it->_id() == id) return it;
    }
    return nullptr;
}


void setup_web_server() {
    server.config.port = 80;

    /*
    server.default_resource["GET"] = REQUEST_LAMBDA_DECL {
        SETUP_HEADERS

        response->write(utils::read_file(web_pages + "/404.html"));
    };
     */

    SET("^/$", "GET") {
        SETUP_HEADERS

        response->write(Utils::read_file(web_pages + "/index.html"));
    };

    SET("^/crt$", "POST") {
        SETUP_HEADERS
        json form = json::parse(request->content.string());
        string host = form["creator"];
        try {
            Game g{host, 2000, "default-list", "default-Map"};
            g.get_player(host).lock()->add_update(g.create_default_update());
            games.push_back(make_shared<Game>(g));
            json res;

            res["creator"] = host;
            res["uuid"] = g._id();
            res["starting_money"] = 2000;
            response->write(res.dump());
        } catch (std::exception &e) {
            spdlog::error(e.what());
        }
    };

    SET("^/upd$", "POST") {
        SETUP_HEADERS

        json form = json::parse(request->content.string());

        string player = form["player_name"];
        string id = form["uuid"];

        auto game = get_game(id);
        if (game != nullptr) {
            auto p = game->get_player(player);
            if (!p.expired()) {
                if (form.find("turn") != form.end()) {
                    json obj;

                    obj["turn"] = game->get_player_turn(player);

                    response->write(obj.dump());
                } else response->write(game->get_player(player).lock()->get_update().dump());
            } else response->write(SimpleWeb::StatusCode::client_error_not_found);
        } else response->write(SimpleWeb::StatusCode::client_error_not_found);

    };

    SET("^/snd-upd$", "POST") {
        SETUP_HEADERS

        json form = json::parse(request->content.string());

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
                    if (player.expired()) {
                        response->write(Game::not_ok_status.dump());
                    }
                    auto box = game->map()[player.lock()->_position()];
                    auto cast = dynamic_pointer_cast<property_box>(box.lock());

                    if (cast) {
                        if (cast->_owner() == quarantine_game::Map::not_found) {
                            TRY_CATCH(game->buy_property(cast->_id(), name);)
                            response->write(Game::ok_status.dump());
                        } else response->write(Game::not_ok_status.dump());
                    } else response->write(Game::not_ok_status.dump());
                }
            } else if (type == "next_player") {
                IFELSE_LADDER else {
                    TRY_CATCH(game->next_player(game->get_player_turn(name));)
                    response->write(Game::ok_status.dump());
                }
            } else if (type == "roll_dice") {
                IFELSE_LADDER else {
                    TRY_CATCH(game->roll_dice(game->get_player_turn(name));)
                    response->write(Game::ok_status.dump());
                }
            } else if (type == "house") {
                cout << "chosen house" << "\"" << name << "\" " << (game == nullptr) << endl;
                IFELSE_LADDER else {
                    cout << "past ladder" << endl;


                    string property;
                    string cnt;

                    property = form["property"];
                    cnt = form["house_count"];

                    auto count = stoi(cnt);

                    cout << property << endl;

                    auto box = game->map()[property];

                    auto cast = dynamic_pointer_cast<property_box>(box.lock());

                    if (cast->_owner() != game->get_player_turn(name))
                        response->write(Game::not_ok_status.dump());
                    else {
                        TRY_CATCH(game->buy_house(game->get_player_turn(name), cast->_id(), count);)
                        response->write(Game::ok_status.dump());
                    }

                }
            } else if (type == "glitch_response") {
                IFELSE_LADDER else {
                    string resp = form["response"];
                    int option = stoi(resp);

                    game->send_to_all(game->create_glitch_response_update(option));
                    response->write(Game::ok_status.dump());
                }
            } else if (type == "start") {
                if (game->started())
                    response->write(SimpleWeb::StatusCode::server_error_internal_server_error);
                else if (game->get_player_turn(name) != 0)
                    response->write(SimpleWeb::StatusCode::server_error_internal_server_error);
                else {
                    game->start();
                    response->write(Game::ok_status.dump());
                }
            }
        }
    };

    SET("^/join/([\\s\\S]+)$", "GET") {
        string id = request->path_match[1];

        if (ShortId::is_id(id)) {
            auto game = get_game(id);

            if (game != nullptr) {
                string page = Utils::read_file(web_pages + "/join.html");

                replace_all(page, "%UUID%", id);

                response->write(page);

            } else response->write(Utils::read_file(web_pages + "/404.html"));
        } else response->write(Utils::read_file(web_pages + "/404.html"));
    };

    SET("^/join/([\\s\\S]+)$", "POST") {
        json form = json::parse(request->content.string());

        string name = form["name"];
        string id = form["id"];
        auto game = get_game(id);

        if (name.empty()) response->write(SimpleWeb::StatusCode::server_error_internal_server_error);
        else if (!ShortId::is_id(id)) response->write(SimpleWeb::StatusCode::server_error_internal_server_error);
        else if (game == nullptr) response->write(SimpleWeb::StatusCode::server_error_internal_server_error);
        else {
            json res;
            if (game->started()) {
                res["ok"] = false;
                res["error"] = u8"La partita è già iniziata";
            } else if (!game->get_player(name).expired()) {
                res["ok"] = false;
                res["error"] = u8"Questo nome è già stato usato nella partita";
            } else if (game->full()) {
                res["ok"] = false;
                res["error"] = u8"Questa partita è già piena";
            } else {
                res["ok"] = true;
                game->add_player(name);
                game->send_to_all(game->create_default_update());
            }

            response->write(res.dump());
        }
    };

    SET("^/([\\s\\S]+)$", "GET") {
        string s = request->path_match[0];
        s = s.substr(1, s.size() - 1);

        if (ShortId::is_id(s)) {
            auto game = get_game(s);
            if (game != nullptr) {
                auto cookies = parse_cookies(request->header);
                if (!cookies["player_joined"].empty() && !cookies["player_name"].empty()) {
                    response->write(Utils::read_file(web_pages + "/Game.html"));
                } else response->write(Utils::read_file(web_pages + "/404.html"));
            } else response->write(Utils::read_file(web_pages + "/404.html"));
        } else response->write(Utils::read_file(web_pages + "/404.html"));

    };

}

int main() {
    setup_web_server();
    server.start();

    return 0;
}
