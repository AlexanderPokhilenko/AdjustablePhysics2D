#ifndef ADJUSTABLEPHYSICS2D_SHAPETRANSLATIONSYSTEM_H
#define ADJUSTABLEPHYSICS2D_SHAPETRANSLATIONSYSTEM_H

#include "System.h"

class ShapeTranslationSystem : public System {
private:
    static ComponentsBitset createCurrentSystemBitset();
protected:
    void update(Context &context, EntityId id, real deltaTime) override;
public:
    ShapeTranslationSystem();
};

#endif //ADJUSTABLEPHYSICS2D_SHAPETRANSLATIONSYSTEM_H
