# C++ Bot for Halite Game

## Overview
Papi is an AI bot designed to compete in the Halite game, employing a combination of strategic territorial management and aggressive expansion techniques. The bot strategically manages its territories, engages with enemies, and expands its reach across the game map.

## Key Strategies
The bot incorporates several strategies to optimize performance and maximize map control:

### 1. **Territory Management**
The bot prioritizes the accumulation of strength in territories before making offensive moves. This is handled by assessing if a site's strength is below a dynamic threshold based on the site's production capability or a minimum strength value.

```cpp
if (site.strength < site.production * strengthThreshold || site.strength < 15) {
    moves.insert(hlt::Move{loc, STILL});
    strengthMap[loc] += site.strength;
}
```
### 2. **Aggressive Expansion**
When a site has accumulated sufficient strength, the bot assesses potential moves against nearby enemy or neutral territories. The goal is to expand territory by taking over weaker sites.

```cpp
for (unsigned char dir : CARDINALS) {
    hlt::Location nextLoc = presentMap.getLocation(loc, dir);
    if (presentMap.getSite(nextLoc).owner != myID && presentMap.getSite(nextLoc).strength < site.strength) {
        possibleMoves.push_back(hlt::Move{loc, dir});
    }
}
if (!possibleMoves.empty()) {
    hlt::Move bestMove = selectBestMove(presentMap, loc, possibleMoves);
    moves.insert(bestMove);
}
```
### 3. **Border Patrol and Expansion Strategy**
To effectively expand, the bot identifies and moves towards the nearest border or enemy, which facilitates strategic growth and exploitation of less defended areas.

```cpp
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
```

### Key Data Structures ###
#### std::vector ####
    Stores potential moves for each site.
#### std::set: ####
    Keeps track of all planned moves to ensure no conflicts during a game frame.
#### std::map: ####
    Used to track and manage cumulative strength at each location, aiding in coordination of moves and strength management. 