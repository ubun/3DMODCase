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

#endif // SanDZHIMENGPACKAGE_H
