unset(CMAKE_CXX_STANDARD)
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(Tests ContextAndComponentsTests.cpp MoveSystemTests.cpp GravitationTests.cpp)
target_link_libraries(Tests gtest gmock_main PhysicsEngine)
#add_test(NAME example_test COMMAND example)
