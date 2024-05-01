#include <iostream>
#include <cstdlib>
#include <ctime>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include "hlt.hpp"
#include "networking.hpp"

bool isNearEnemy(hlt::GameMap& map, const hlt::Location& loc, unsigned char myID) {
    for (const auto& dir : CARDINALS) {
        hlt::Location nextLoc = map.getLocation(loc, dir);
        if (map.getSite(nextLoc).owner != 0 && map.getSite(nextLoc).owner != myID) {
            return true; // Enemy is nearby
        }
    }
    return false; // No enemies nearby
}

hlt::Move selectBestMove(hlt::GameMap& map, hlt::Location loc, std::vector<hlt::Move>& possibleMoves) {
    return *std::max_element(possibleMoves.begin(), possibleMoves.end(), [&](const hlt::Move &a, const hlt::Move &b) {
        auto siteA = map.getSite(a.loc);
        auto siteB = map.getSite(b.loc);
        int overkillA = std::max(0, siteA.strength - siteB.strength);
        int overkillB = std::max(0, siteB.strength - siteA.strength);
        return overkillA + siteA.production < overkillB + siteB.production;
    });
}

int main() {
    srand(time(NULL));
    std::cout.sync_with_stdio(false);

    unsigned char myID;
    hlt::GameMap presentMap;
    getInit(myID, presentMap);
    sendInit("Papi");

    while (true) {
        std::set<hlt::Move> moves;
        getFrame(presentMap);

        std::map<hlt::Location, int> strengthMap; // Map to keep track of cumulative strength at each location

        for (unsigned short y = 0; y < presentMap.height; y++) {
            for (unsigned short x = 0; x < presentMap.width; x++) {
                hlt::Location loc = {x, y};
                const auto& site = presentMap.getSite(loc);

                if (site.owner != myID)
                    continue;

                bool nearEnemy = isNearEnemy(presentMap, loc, myID);
                int strengthThreshold = nearEnemy ? 3 : 5;

                if (site.strength < site.production * strengthThreshold || site.strength < 15) {
                    moves.insert(hlt::Move{loc, STILL});
                    strengthMap[loc] += site.strength; // Update map with static move
                    continue;
                }

                std::vector<hlt::Move> possibleMoves;
                for (unsigned char dir : CARDINALS) {
                    hlt::Location nextLoc = presentMap.getLocation(loc, dir);
                    if (presentMap.getSite(nextLoc).owner != myID && presentMap.getSite(nextLoc).strength < site.strength) {
                        possibleMoves.push_back(hlt::Move{loc, dir});
                    }
                }

                if (!possibleMoves.empty()) {
                    hlt::Move bestMove = selectBestMove(presentMap, loc, possibleMoves);
                    hlt::Location targetLoc = presentMap.getLocation(loc, bestMove.dir);
                    if (strengthMap[targetLoc] + site.strength <= 255) {
                        moves.insert(bestMove);
                        strengthMap[targetLoc] += site.strength; // Update map with planned move strength
                    } else {
                        moves.insert(hlt::Move{loc, STILL}); // Stay still to avoid overflow
                        strengthMap[loc] += site.strength;
                    }
                } else {
                    // If surrounded by own territory, move towards the nearest border
                    std::vector<std::pair<hlt::Move, int>> borderDistances;
                    for (unsigned char dir : CARDINALS) {
                        hlt::Location edgeLoc = loc;
                        int edgeDistance = 0;
                        do {
                            edgeLoc = presentMap.getLocation(edgeLoc, dir);
                            edgeDistance++;
                        } while (presentMap.getSite(edgeLoc).owner == myID && edgeDistance < presentMap.width);

                        if (presentMap.getSite(edgeLoc).owner != myID) {
                            borderDistances.push_back({hlt::Move{loc, dir}, edgeDistance});
                        }
                    }

                    if (!borderDistances.empty()) {
                        auto bestBorderMove = *std::min_element(borderDistances.begin(), borderDistances.end(),
                                [](const std::pair<hlt::Move, int> &a, const std::pair<hlt::Move, int> &b) {
                                    return a.second < b.second;
                                });
                        moves.insert(bestBorderMove.first);
                    }
                }
            }
        }

        sendFrame(moves);
    }

    return 0;
}
