#include "3d_zhimeng-package.h"
#include "general.h"
#include "skill.h"
#include "room.h"
#include "carditem.h"
#include "maneuvering.h"
#include "clientplayer.h"
#include "client.h"
#include "engine.h"
#include "general.h"

DujiCard::DujiCard(){
    target_fixed = true;
}

void DujiCard::use(Room *, ServerPlayer *player, const QList<ServerPlayer *> &) const{
    player->addToPile("du", getSubcards().first());
}

class DujiViewAsSkill: public OneCardViewAsSkill{
public:
    DujiViewAsSkill():OneCardViewAsSkill("Duji"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return player->getPile("du").isEmpty();
    }

    virtual bool viewFilter(const CardItem *to_select) const{
        return to_select->getCard()->getSuit() == Card::Spade;
    }

    virtual const Card *viewAs(CardItem *card_item) const{
        Card *card = new DujiCard;
        card->addSubcard(card_item->getFilteredCard());
        return card;
    }
};

class Duji: public TriggerSkill{
public:
    Duji():TriggerSkill("duji"){
        view_as_skill = new DujiViewAsSkill;
        events << CardUsed;
    }

    virtual bool triggerable(const ServerPlayer *i) const{
        return !i->hasSkill(objectName());
    }

    virtual bool trigger(TriggerEvent , ServerPlayer *player, QVariant &data) const{
        Room *room = player->getRoom();
        if(player->getPhase() != Player::Play)
            return false;

        CardUseStruct use = data.value<CardUseStruct>();
        ServerPlayer *liru = room->findPlayerBySkillName(objectName());
        if(!liru || liru->getPile("du").isEmpty())
            return false;

        if(use.card->inherits("Slash") && liru->askForSkillInvoke(objectName())){
            room->obtainCard(player, liru->getPile("du").first());
            room->setPlayerFlag(player, "drank");
            if(room->askForChoice(player, objectName(), "turn+lp") == "turn")
                player->turnOver();
            else
                room->loseHp(player);
        }
        return false;
    }
};

class ShiPo: public ProhibitSkill{
public:
    ShiPo():ProhibitSkill("shiPo"){

    }

    virtual bool isProhibited(const Player *, const Player *, const Card *card) const{
        return (card->inherits("Slash") && card->getSuit() == Card::Spade) ||
                (card->inherits("TrickCard") && card->getSuit() == Card::Spade);
    }
};

SanDZhimengPackage::SanDZhimengPackage()
    :Package("sand_zhimeng")
{
    General *diyliru = new General(this, "diyliru", "qun", 3);
    diyliru->addSkill(new Duji);
    diyliru->addSkill(new ShiPo);

    addMetaObject<DujiCard>();
}

ADD_PACKAGE(SanDZhimeng)
