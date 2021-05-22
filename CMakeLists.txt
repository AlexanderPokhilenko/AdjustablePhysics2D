cmake_minimum_required(VERSION 3.17)
project(AdjustablePhysics2D)

set(CMAKE_CXX_STANDARD 14)

add_library(AdjustablePhysics2D common/Material.h common/Shape.h common/MassInfo.h base/primitives.h math/Vector2.cpp math/Vector2.h common/Transform.h common/AABB.h common/Collision.h common/Polygon.h ecs/Context.cpp ecs/Context.h ecs/components/Component.h ecs/components/ComponentType.h ecs/systems/System.h ecs/components/Components.h PhysicsEngine.cpp PhysicsEngine.h ecs/systems/System.cpp ecs/components/BaseComponentContainer.h ecs/components/ComponentContainer.cpp ecs/components/ComponentContainer.h ecs/systems/MoveSystem.cpp ecs/systems/MoveSystem.h common/Transform.cpp ecs/systems/SystemType.h ecs/systems/GlobalGravitationSystem.cpp ecs/systems/GlobalGravitationSystem.h ecs/systems/GravitationalForceSystem.cpp ecs/systems/GravitationalForceSystem.h common/ShapeType.h ecs/systems/ShapeTranslationSystem.cpp ecs/systems/ShapeTranslationSystem.h common/Shape.cpp common/AABB.cpp common/Polygon.cpp)

add_compile_definitions(USE_INERTIA USE_ROTATION USE_LEAPFROG USE_GRAVITATIONAL_FORCE)
