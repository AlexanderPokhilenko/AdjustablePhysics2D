#include "System.h"

System::System(ComponentsBitset bitset) : componentsBitset(bitset) { }

void System::update(Context &context, real deltaTime) {
    auto size = context.inUse.size();
    for (EntityId i = 0; i < size; i++) {
        if(context.checkEntity(i, componentsBitset))
        {
            update(context, i, deltaTime);
        }
    }
}
