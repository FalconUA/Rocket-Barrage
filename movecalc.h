#ifndef MOVECALC_H
#define MOVECALC_H

#include "level.h"
#include "gameobject.h"
#include <vector>

enum ActionType
{
    MOVE_ACTION

};

class MoveCalc
{
public:
    MoveCalc();
    void init(Level * level, std::vector< GameObject* > object);
    void Calculate();
};

#endif // MOVECALC_H
