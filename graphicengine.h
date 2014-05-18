#ifndef GRAPHICENGINE_H
#define GRAPHICENGINE_H

#include <vector>
#include <SFML/Graphics.hpp>
#include <graphicobject.h>
#include <worldsimulator.h>

namespace rbw
{

namespace Graphic
{
typedef struct
{
    sf::Texture texture;
    sf::Sprite sprite;
} modelData;

typedef struct
{
    sf::Texture texture;

    unsigned int tileWidth;
    unsigned int tileHeight;

    std::vector< sf::Rect< int > > rect;
    std::vector< sf::Sprite > tiles;
} AnimationData;

typedef struct
{
    int x;
    int y;
    AnimationData * data;
    int count;
} Animation;

typedef struct __AnimationChainElement
{
    __AnimationChainElement * prev = NULL;
    __AnimationChainElement * next = NULL;
    Animation * animation = NULL;
} AnimationChainElement;

typedef struct __AnimationChain
{
    AnimationChainElement * First = NULL;
    AnimationChainElement * Last = NULL;
} AnimationChain;

typedef struct
{
    rbw::Graphic::modelData background;

} Widgets;

}; // end of namespace Graphic
class GraphicEngine
{
public:
    GraphicEngine();
    bool initModels();
    bool initAnimations();
    bool initOutputWindow(sf::RenderWindow * window);
    bool Render(std::vector< rbw::GraphicObject > graphicObjects);    
    bool ShowScoreTable(std::vector< rbw::PlayerExportInformation > peiVector);
    bool ShowEventList(std::vector< std::string > eventVector);
    ~GraphicEngine();
private:
    sf::RenderWindow * outputWindow;

    std::vector< rbw::GraphicObject > * graphicObjects;

    rbw::Graphic::modelData BLACK_PlayerData;
    rbw::Graphic::modelData WHITE_PlayerData;

    rbw::Graphic::modelData HomingMissileData;
    rbw::Graphic::modelData BouncingBombData;
    rbw::Graphic::modelData GrenadeData;

    rbw::Graphic::AnimationData HM_ExplosionData;
    rbw::Graphic::AnimationData BB_ExplosionData;
    rbw::Graphic::AnimationData P_ExplosionData;
    rbw::Graphic::AnimationData G_ExplosionData;

    rbw::Graphic::AnimationChain Explosions;       

    int count = 0;

    sf::Font UbuntuMono;

    sf::Texture deskTexture;
    sf::Sprite deskSprite;
    std::vector< std::string > eventList;
};

};

#endif // GRAPHICENGINE_H
