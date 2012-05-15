#ifndef SANDZHIMENGPACKAGE_H
#define SANDZHIMENGPACKAGE_H

#include "package.h"
#include "card.h"

class SanDZhimengPackage: public Package{
    Q_OBJECT

public:
    SanDZhimengPackage();
};

class DujiCard: public SkillCard{
    Q_OBJECT

public:
    Q_INVOKABLE DujiCard();

    virtual void use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const;
};

class PengriCard: public SkillCard{
    Q_OBJECT

public:
    Q_INVOKABLE PengriCard();

    virtual void use(Room *room, ServerPlayer *source, const QList<ServerPlayer *> &targets) const;
    virtual bool targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const;
};

#endif // SanDZHIMENGPACKAGE_H
