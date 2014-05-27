#include "graphicengine.h"
#include <sstream>
#include <string>
#include <stdlib.h>


#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

namespace rbw
{

GraphicEngine::GraphicEngine()
{
    this->Explosions.First = NULL;
    this->Explosions.Last = NULL;

    this->UbuntuMono.loadFromFile("Resources/fonts/UbuntuMono-R.ttf");
}

bool ImportModelData(rbw::Graphic::modelData * model, const std::string fileName, float hitboxRadius)
{
    if (!model->texture.loadFromFile(fileName)){
        std::cout << "Failed to load model sheet." << std::endl;
        return false;
    }
    model->sprite.setTexture(model->texture);
    sf::Vector2u size = model->texture.getSize();

    model->sprite.setOrigin(size.x/2, size.y/2);
    return true;
}
bool ImportAnimationData(rbw::Graphic::AnimationData * animation, const std::string filename, sf::Vector2u Size)
{
    sf::Rect< int > emptyRec;
    sf::Sprite emptySprite;
    animation->rect.resize(Size.x*Size.y, emptyRec);
    animation->tiles.resize(Size.x*Size.y, emptySprite);
    if (!animation->texture.loadFromFile(filename)){
        std::cout << "Failed to load animation tile sheet." << std::endl;
        return false;
    }

    unsigned int tileWidth = animation->texture.getSize().x / Size.x;
    unsigned int tileHeight = animation->texture.getSize().y / Size.y;

    for (unsigned int x=0; x<Size.x; x++){
        for (unsigned int y=0; y<Size.y; y++){
            sf::Rect< int > newRect;
            newRect.top = y * tileHeight + 1;
            newRect.left = x * tileWidth + 1;
            newRect.height = tileHeight;
            newRect.width = tileWidth;
            animation->rect[y*Size.x + x] = newRect;

            sf::Sprite newSprite;
            newSprite.setTexture(animation->texture);
            newSprite.setTextureRect(animation->rect[y*Size.x + x]);
            newSprite.setOrigin(tileWidth/2, tileHeight/2);

            animation->tiles[y*Size.x + x] = newSprite;
        }
    }

    std::cout << "animation loaded ! " << std::endl;
    return true;
}

bool GraphicEngine::initOutputWindow(sf::RenderWindow *window)
{
    this->outputWindow = window;
    if (window == NULL) return false;

    if (!this->deskTexture.loadFromFile("Resources/window/Texture.jpg")){
        std::cout << "Failed to load desk sheet." << std::endl;
    }
    this->originalWinSize = window->getSize();

    this->deskSprite.setTexture(this->deskTexture);
    this->deskSprite.setPosition(this->originalWinSize.x - 200, 0);


    return true;
}

bool GraphicEngine::initModels()
{
    ImportModelData(&(this->BLACK_PlayerData), rbw::Graphic::ModelPath + rbw::Graphic::BLACK_TANK_IMAGE_FILENAME,
                    rbw::GameParam::PLAYER_HITBOX_RADIUS);

    ImportModelData(&(this->WHITE_PlayerData), rbw::Graphic::ModelPath + rbw::Graphic::WHITE_TANK_IMAGE_FILENAME,
                    rbw::GameParam::PLAYER_HITBOX_RADIUS);

    ImportModelData(&(this->HomingMissileData), rbw::Graphic::ModelPath + rbw::Graphic::HOMING_MISSILE_IMAGE_FILENAME,
                    rbw::GameParam::HOMING_MISSILE_HITBOX_RADIUS);

    ImportModelData(&(this->BouncingBombData), rbw::Graphic::ModelPath + rbw::Graphic::BOUNCING_BOMB_IMAGE_FILENAME,
                    rbw::GameParam::BOUNCING_BOMB_HITBOX_RADIUS);

    ImportModelData(&(this->GrenadeData), rbw::Graphic::ModelPath + rbw::Graphic::GRENADE_IMAGE_FILENAME,
                    rbw::GameParam::GRENADE_HITBOX_RADIUS);
    return true;
}
bool GraphicEngine::initAnimations()
{
    ImportAnimationData(&(this->HM_ExplosionData), rbw::Graphic::AnimationPath + rbw::Graphic::HM_EXPLOSION_ANIMATION_FILENAME,
                        sf::Vector2u(5,5));
    ImportAnimationData(&(this->BB_ExplosionData), rbw::Graphic::AnimationPath + rbw::Graphic::BB_EXPLOSION_ANIMATION_FILENAME,
                        sf::Vector2u(4,4));
    ImportAnimationData(&(this->P_ExplosionData), rbw::Graphic::AnimationPath + rbw::Graphic::P_EXPLOSION_ANIMATION_FILENAME,
                        sf::Vector2u(5,3));
    ImportAnimationData(&(this->G_ExplosionData), rbw::Graphic::AnimationPath + rbw::Graphic::G_EXPLOSION_ANIMATION_FILENAME,
                        sf::Vector2u(5,4));

    return true;
}

bool GraphicEngine::Render(std::vector< GraphicObject > graphicObjects)
{
    for (int i=0; i<graphicObjects.size(); i++){
        GraphicObject * tmpObj = &graphicObjects[i];
        rbw::Graphic::modelData * data;

        if ((tmpObj->type == rbw::Graphic::PLAYER) && (tmpObj->team == rbw::TEAM_BLACK))
        {
            data = &(this->BLACK_PlayerData);

            float angleraw = atan2(tmpObj->velocity_y, tmpObj->velocity_x);
            float angle = atan2(tmpObj->velocity_y, tmpObj->velocity_x) * 180 / M_PI;

            data->sprite.setRotation(angle);
            data->sprite.setPosition(tmpObj->x, tmpObj->y);

            sf::RectangleShape hpBar;
            hpBar.setFillColor(sf::Color::Black);
            hpBar.setSize(sf::Vector2f(42.0f, 5.0f));
            hpBar.setPosition(tmpObj->x - 21, tmpObj->y - 25);

            sf::RectangleShape health;
            health.setFillColor(sf::Color::Yellow);
            health.setSize(sf::Vector2f(40.0f * (tmpObj->HealthPoint/100.0f), 3.0f));
            health.setPosition(tmpObj->x - 20, tmpObj->y - 24);

            sf::Text text;
            text.setFont(this->UbuntuMono);
            text.setCharacterSize(12);
            text.setStyle(sf::Text::Bold);

            text.setString(tmpObj->Name);
            text.setColor(sf::Color::Black);
            text.setPosition(sf::Vector2f(tmpObj->x - 25, tmpObj->y-40));


            this->outputWindow->draw(text);

            this->outputWindow->draw(data->sprite);
            this->outputWindow->draw(hpBar);
            this->outputWindow->draw(health);

            continue;
        }

        if ((tmpObj->type == rbw::Graphic::PLAYER) && (tmpObj->team == rbw::TEAM_WHITE))
        {
            data = &(this->WHITE_PlayerData);

            float angleraw = atan2(tmpObj->velocity_y, tmpObj->velocity_x);
            float angle = atan2(tmpObj->velocity_y, tmpObj->velocity_x) * 180 / M_PI;

            data->sprite.setRotation(angle);
            data->sprite.setPosition(tmpObj->x, tmpObj->y);

            sf::RectangleShape hpBar;
            hpBar.setFillColor(sf::Color::Black);
            hpBar.setSize(sf::Vector2f(42.0f, 5.0f));
            hpBar.setPosition(tmpObj->x - 21, tmpObj->y - 25);

            sf::RectangleShape health;
            health.setFillColor(sf::Color::Yellow);
            health.setSize(sf::Vector2f(40.0f * (tmpObj->HealthPoint/100.0f), 3.0f));
            health.setPosition(tmpObj->x - 20, tmpObj->y - 24);

            sf::Text text;
            text.setFont(this->UbuntuMono);
            text.setCharacterSize(12);
            text.setStyle(sf::Text::Bold);

            text.setString(tmpObj->Name);
            text.setColor(sf::Color::Black);
            text.setPosition(sf::Vector2f(tmpObj->x - (text.getLocalBounds().width/2), tmpObj->y-30-text.getLocalBounds().height));


            this->outputWindow->draw(text);


            this->outputWindow->draw(data->sprite);
            this->outputWindow->draw(hpBar);
            this->outputWindow->draw(health);

            continue;
        }
        if (tmpObj->type == rbw::Graphic::FOLLOW_ROCKET)
        {
            data = &(this->HomingMissileData);

            float angleraw = atan2(tmpObj->velocity_y, tmpObj->velocity_x);
            float angle = atan2(tmpObj->velocity_y, tmpObj->velocity_x) * 180 / M_PI;

            data->sprite.setRotation(angle);
            data->sprite.setPosition(tmpObj->x, tmpObj->y);

            this->outputWindow->draw(data->sprite);
            continue;
        }
        if (tmpObj->type == rbw::Graphic::BOUNCE_ROCKET)
        {
            data = &(this->BouncingBombData);
            float angleraw = atan2(tmpObj->velocity_y, tmpObj->velocity_x);
            float angle = atan2(tmpObj->velocity_y, tmpObj->velocity_x) * 180 / M_PI;

            data->sprite.setRotation(angle);
            data->sprite.setPosition(tmpObj->x, tmpObj->y);

            this->outputWindow->draw(data->sprite);
            continue;
        }
        if (tmpObj->type == rbw::Graphic::GRENADE)
        {
            data = &(this->GrenadeData);

            float angleraw = atan2(tmpObj->velocity_y, tmpObj->velocity_x);
            float angle = atan2(tmpObj->velocity_y, tmpObj->velocity_x) * 180 / M_PI;

            data->sprite.setRotation(angle);
            data->sprite.setPosition(tmpObj->x, tmpObj->y);

            data->sprite.setScale(tmpObj->zoom_coefficient, tmpObj->zoom_coefficient);
            this->outputWindow->draw(data->sprite);
            continue;
        }

        if (tmpObj->type == rbw::Graphic::HOMINGMISSILE_EXPLOSION){
            rbw::Graphic::Animation * newExplosion = new rbw::Graphic::Animation;
            rbw::Graphic::AnimationChainElement * newElement = new rbw::Graphic::AnimationChainElement;

            newExplosion->data = &(this->HM_ExplosionData);
            newExplosion->count = 0;
            newExplosion->x = tmpObj->x;
            newExplosion->y = tmpObj->y;

            // new chain element
            newElement->animation = newExplosion;

            if (this->Explosions.Last == NULL){
                //std::cout << "LAST is NULL" << std::endl;it's new
                this->Explosions.First = newElement;
                this->Explosions.Last = newElement;
                newElement->next = NULL;
                newElement->prev = NULL;
            }
            else {
                //std::cout << "LAST is not NULL" << std::endl;it's new
                newElement->prev = this->Explosions.Last;
                newElement->next = NULL;
                this->Explosions.Last->next = newElement;
                this->Explosions.Last = newElement;
            }
            continue;
        }
        if (tmpObj->type == rbw::Graphic::BOUNCINGBOMB_EXPLOSION){
            rbw::Graphic::Animation * newExplosion = new rbw::Graphic::Animation;
            rbw::Graphic::AnimationChainElement * newElement = new rbw::Graphic::AnimationChainElement;

            newExplosion->data = &(this->BB_ExplosionData);
            newExplosion->count = 0;
            newExplosion->x = tmpObj->x;
            newExplosion->y = tmpObj->y;

            // new chain element
            newElement->animation = newExplosion;

            if (this->Explosions.Last == NULL){
                //std::cout << "LAST is NULL" << std::endl;it's new
                this->Explosions.First = newElement;
                this->Explosions.Last = newElement;
                newElement->next = NULL;
                newElement->prev = NULL;
            }
            else {
                //std::cout << "LAST is not NULL" << std::endl;it's new
                newElement->prev = this->Explosions.Last;
                newElement->next = NULL;
                this->Explosions.Last->next = newElement;
                this->Explosions.Last = newElement;
            }
            continue;
        }
        if (tmpObj->type == rbw::Graphic::PLAYER_EXPLOSION){
            rbw::Graphic::Animation * newExplosion = new rbw::Graphic::Animation;
            rbw::Graphic::AnimationChainElement * newElement = new rbw::Graphic::AnimationChainElement;

            newExplosion->data = &(this->P_ExplosionData);
            newExplosion->count = 0;
            newExplosion->x = tmpObj->x;
            newExplosion->y = tmpObj->y;

            // new chain element
            newElement->animation = newExplosion;

            if (this->Explosions.Last == NULL){
                //std::cout << "LAST is NULL" << std::endl;it's new
                this->Explosions.First = newElement;
                this->Explosions.Last = newElement;
                newElement->next = NULL;
                newElement->prev = NULL;
            }
            else {
                //std::cout << "LAST is not NULL" << std::endl;it's new
                newElement->prev = this->Explosions.Last;
                newElement->next = NULL;
                this->Explosions.Last->next = newElement;
                this->Explosions.Last = newElement;
            }
            continue;
        }
        if (tmpObj->type == rbw::Graphic::GRENADE_EXPLOSION){
            rbw::Graphic::Animation * newExplosion = new rbw::Graphic::Animation;
            rbw::Graphic::AnimationChainElement * newElement = new rbw::Graphic::AnimationChainElement;

            newExplosion->data = &(this->G_ExplosionData);
            newExplosion->count = 0;
            newExplosion->x = tmpObj->x;
            newExplosion->y = tmpObj->y;

            // new chain element
            newElement->animation = newExplosion;

            if (this->Explosions.Last == NULL){
                //std::cout << "LAST is NULL" << std::endl;it's new
                this->Explosions.First = newElement;
                this->Explosions.Last = newElement;
                newElement->next = NULL;
                newElement->prev = NULL;
            }
            else {
                //std::cout << "LAST is not NULL" << std::endl;it's new
                newElement->prev = this->Explosions.Last;
                newElement->next = NULL;
                this->Explosions.Last->next = newElement;
                this->Explosions.Last = newElement;
            }
            continue;
        }
        DrawGraphicObject(this->outputWindow, tmpObj, false);
    }

    rbw::Graphic::AnimationChainElement * tmpElement = this->Explosions.First;
    while (tmpElement != NULL){
        rbw::Graphic::Animation * explosion = tmpElement->animation;
        rbw::Graphic::AnimationChainElement * currElement = tmpElement;
        tmpElement = tmpElement->next;


        sf::Sprite sprite = explosion->data->tiles[explosion->count];
        sprite.setPosition(explosion->x, explosion->y);

        this->outputWindow->draw(sprite);

        explosion->count += 1;

        if (explosion->count >= explosion->data->tiles.size()){
            rbw::Graphic::AnimationChainElement * previousEl = currElement->prev;
            rbw::Graphic::AnimationChainElement * nextEl = currElement->next;
            if (previousEl != NULL)
                previousEl->next = nextEl;
            if (nextEl != NULL)
                nextEl->prev = previousEl;
            // if current element is the first rocket in chain
            if (this->Explosions.First == currElement){
                this->Explosions.First = currElement->next;
            }
            // if current element is the last rocket in chain
            if (this->Explosions.Last == currElement){
                this->Explosions.Last = currElement->prev;
            }
            delete currElement;
        }
    }

    this->outputWindow->draw(this->deskSprite);
}

bool GraphicEngine::ShowEventList(std::vector<std::string> eventVector)
{
    for (int i=0; i<eventVector.size(); i++){
        this->eventList.push_back(eventVector[i]);
        if (this->eventList.size() > 35){
            this->eventList.erase(eventList.begin());
        }
    }
    std::string outputStr;
    for (int i=0; i<this->eventList.size(); i++)
        outputStr += this->eventList[i]+"\n";

    sf::Text tableText;
    tableText.setFont(this->UbuntuMono);
    tableText.setPosition(sf::Vector2f(this->originalWinSize.x - 195, 100));
    tableText.setString(outputStr);
    tableText.setCharacterSize(14);
    tableText.setColor(sf::Color::Yellow);

    this->outputWindow->draw(tableText);
}

bool GraphicEngine::ShowScoreTable(std::vector< rbw::PlayerExportInformation > peiVector)
{
    sf::Color bgcolor;
    bgcolor.g = 0;
    bgcolor.r = 0;
    bgcolor.b = 0;
    bgcolor.a = 162;

    sf::RectangleShape background;
    background.setPosition(40.0f, 40.0f);
    background.setSize(sf::Vector2f(430, 330));
    background.setFillColor(bgcolor);
    background.setOutlineThickness(3);
    background.setOutlineColor(sf::Color::Black);

    //std::sort(peiVector.begin(), peiVector.end());

    std::string tableStr;
    tableStr += " Team Black:                                                         \n";
    tableStr += "|--------------------------|------|-------|--------------|----------|\n";
    tableStr += "| Nickname                 | Kill | Death | Damage dealt | HM/BB/G  |\n";
    tableStr += "|--------------------------|------|-------|--------------|----------|\n";

    //            12345678901234567890123456 123456 1234567 12345678901234 1234567890
    //              26                          6     7        14              10

    for (int i=0; (int) i<peiVector.size(); i++){
        rbw::PlayerExportInformation tmpInfo = peiVector[i];
        if (tmpInfo.team != rbw::TEAM_BLACK) continue;
        std::string nickName = tmpInfo.PlayerName;
        std::string kill = std::to_string(tmpInfo.Kill);
        std::string death = std::to_string(tmpInfo.Death);
        std::string damageDealt = std::to_string(tmpInfo.DamageDealt);
        std::string hm_bb_g = std::to_string(tmpInfo.HomingMissilesLeft) + "/" +
                std::to_string(tmpInfo.BouncingBombsLeft) + "/" +
                std::to_string(tmpInfo.GrenadesLeft);

        nickName.resize(25,' ');
        kill.resize(5,' ');
        death.resize(6,' ');
        damageDealt.resize(13,' ');
        hm_bb_g.resize(9,' ');

        std::string newLine("| " + nickName + "| " + kill + "| " + death + "| " + damageDealt + "| " + hm_bb_g + "|\n");
        tableStr += newLine;
    }
    tableStr += "|--------------------------|------|-------|--------------|----------|\n";
    tableStr += "                                                                     \n";
    tableStr += " Team White:                                                         \n";
    tableStr += "|--------------------------|------|-------|--------------|----------|\n";
    tableStr += "| Nickname                 | Kill | Death | Damage dealt | HM/BB/G  |\n";
    tableStr += "|--------------------------|------|-------|--------------|----------|\n";

    for (int i=0; (int) i<peiVector.size(); i++){
        rbw::PlayerExportInformation tmpInfo = peiVector[i];
        if (tmpInfo.team != rbw::TEAM_WHITE) continue;
        std::string nickName = tmpInfo.PlayerName;
        std::string kill = std::to_string(tmpInfo.Kill);
        std::string death = std::to_string(tmpInfo.Death);
        std::string damageDealt = std::to_string(tmpInfo.DamageDealt);
        std::string hm_bb_g = std::to_string(tmpInfo.HomingMissilesLeft) + "/" +
                std::to_string(tmpInfo.BouncingBombsLeft) + "/" +
                std::to_string(tmpInfo.GrenadesLeft);

        nickName.resize(25,' ');
        kill.resize(5,' ');
        death.resize(6,' ');
        damageDealt.resize(13,' ');
        hm_bb_g.resize(9,' ');

        std::string newLine("| " + nickName + "| " + kill + "| " + death + "| " + damageDealt + "| " + hm_bb_g + "|\n");
        tableStr += newLine;
    }
    tableStr += "|--------------------------|------|-------|--------------|----------|\n";

    sf::Text tableText;
    tableText.setFont(this->UbuntuMono);
    tableText.setPosition(sf::Vector2f(background.getPosition().x+6, background.getPosition().y+2));
    tableText.setString(tableStr);
    tableText.setCharacterSize(14);
    tableText.setColor(sf::Color::Yellow);

    background.setSize(sf::Vector2f(tableText.getLocalBounds().width+12, tableText.getLocalBounds().height+5));

    this->outputWindow->draw(background);
    this->outputWindow->draw(tableText);

    return true;
}

}; // end of namespace rbw
