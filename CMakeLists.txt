cmake_minimum_required(VERSION 3.17)
project(AdjustablePhysics2D)

set(CMAKE_CXX_STANDARD 14)

add_library(AdjustablePhysics2D common/Material.h common/Shape.h common/MassInfo.h base/primitives.h math/Vector2.cpp math/Vector2.h common/Transform.h common/AABB.h common/Collision.h common/Polygon.h)

add_compile_definitions("USE_INERTIA")
add_compile_definitions("USE_ROTATION")