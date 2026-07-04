#ifndef GAMECONSTANTS_H
#define GAMECONSTANTS_H

namespace GameConstants {
constexpr int StageWidth = 1280;
constexpr int StageHeight = 720;

constexpr int SeatPlayer = 1;
constexpr int SeatRight = 2;
constexpr int SeatLeft = 3;

enum ButtonGroup {
    ButtonsCallLandlord = 0,
    ButtonsFightLandlord = 1,
    ButtonsDiscard = 2,
    ButtonsGameOver = 3
};
}

#endif // GAMECONSTANTS_H
