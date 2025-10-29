cmake_minimum_required(VERSION 3.21)

project(ArrayModifier)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Настройка CRT для MSVC (устраняет конфликты LIBCMT / MSVCRT)
if (MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Подключаем GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        9706f75b8f91c52a3840cf5d878a7f37ea10ef00
)

if(MSVC)
    set(gtest_disable_pthreads ON CACHE BOOL "" FORCE)
     add_compile_definitions(_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)
endif()
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Основная программа
add_executable(ArrayModifier Lab_2.cpp)

# Тесты (убедись что test.cpp включает Lab_2_methods.cpp или добавь его здесь)
add_executable(Testing test.cpp Lab_2_methods.cpp)
target_link_libraries(Testing PRIVATE gtest gtest_main)  # БЕЗ GTest::

# Включаем тестирование
enable_testing()
include(GoogleTest)
gtest_discover_tests(Testing)

add_custom_command(TARGET Testing POST_BUILD
    COMMAND Testing
    COMMENT "Запуск тестов и вывод результатов..."
)
