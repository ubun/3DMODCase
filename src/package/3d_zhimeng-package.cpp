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

class Liefu: public TriggerSkill{
public:
    Liefu(): TriggerSkill("liefu"){
        events << SlashMissed;
    }

    virtual bool trigger(TriggerEvent, ServerPlayer *player, QVariant &data) const{
        SlashEffectStruct effect = data.value<SlashEffectStruct>();

        Room *room = player->getRoom();
        if(player->askForSkillInvoke(objectName())){
            LogMessage log;
            log.type = "#Liefu";
            log.from = player;
            log.to << effect.to;
            log.arg = objectName();
            room->sendLog(log);

            room->slashResult(effect, NULL);
            if(room->askForChoice(player, objectName(), "pan+feng") == "pan")
                room->askForDiscard(player, objectName(), qMin(player->getCardCount(true), effect.to->getLostHp()), false, true);
            else
                effect.to->drawCards(qMin(effect.to->getHp(), 5));
        }

        return false;
    }
};

PengriCard::PengriCard(){
}

bool PengriCard::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    return targets.isEmpty() && to_select != Self && !to_select->isNude() && to_select->canSlash(Self, false);
}

void PengriCard::use(Room *room, ServerPlayer *player, const QList<ServerPlayer *> &tar) const{
    int card_id = room->askForCardChosen(player, tar.first(), "he", "pengri");
    room->obtainCard(player, card_id, room->getCardPlace(card_id) != Player::Hand);

    Slash *slash = new Slash(Card::NoSuit, 0);
    slash->setSkillName("pengri");
    CardUseStruct use;
    use.card = slash;
    use.from = tar.first();
    use.to << player;
    room->useCard(use);
}

class Pengri:public ZeroCardViewAsSkill{
public:
    Pengri():ZeroCardViewAsSkill("pengri"){

    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return ! player->hasUsed("PengriCard");
    }

    virtual const Card *viewAs() const{
        return new PengriCard;
    }
};

class Gangli:public MasochismSkill{
public:
    Gangli():MasochismSkill("gangli"){
    }

    virtual void onDamaged(ServerPlayer *player, const DamageStruct &damage) const{
        Room *room = player->getRoom();
        if(damage.damage != 1 || !damage.from || damage.from == player)
            return;

        if(room->askForSkillInvoke(player, objectName(), QVariant::fromValue(damage))){
            ServerPlayer *target = room->askForPlayerChosen(player, room->getOtherPlayers(damage.from), objectName());
            Slash *slash = new Slash(Card::NoSuit, 0);
            slash->setSkillName(objectName());
            CardUseStruct use;
            use.card = slash;
            use.from = damage.from;
            use.to << target;
            room->useCard(use, false);
        }
    }
};

SanDZhimengPackage::SanDZhimengPackage()
    :Package("sand_zhimeng")
{
    General *diyliru = new General(this, "diyliru", "qun", 3);
    diyliru->addSkill(new Duji);
    diyliru->addSkill(new ShiPo);

    General *diypanfeng = new General(this, "diypanfeng", "qun");
    diypanfeng->addSkill(new Liefu);

    General *diychengyu = new General(this, "diychengyu", "wei", 3);
    diychengyu->addSkill(new Pengri);
    diychengyu->addSkill(new Gangli);

    addMetaObject<DujiCard>();
    addMetaObject<PengriCard>();
}

ADD_PACKAGE(SanDZhimeng)
