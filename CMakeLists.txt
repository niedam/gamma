cmake_minimum_required(VERSION 3.0)
project(Gamma C)


if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif ()

# Nie chcemy oglądać komunikatów o procentowym postępie kompilacji.
set_property(GLOBAL PROPERTY RULE_MESSAGES OFF)

set(CMAKE_C_COMPILER_VERSION 11)

# Ustawiamy wspólne opcje kompilowania dla wszystkich wariantów projektu.
set(CMAKE_C_FLAGS "-std=c11 -Wall -Wextra")
# Domyślne opcje dla wariantów Release i Debug są sensowne.
# Jeśli to konieczne, ustawiamy tu inne.
# set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
# set(CMAKE_C_FLAGS_DEBUG "-g")

# Wskazujemy pliki źródłowe.
set(SOURCE_FILES
        src/gamma.c
        src/gamma.h
        src/field.c
        src/field.h
        src/stringology.c
        src/stringology.h
        src/batchmode.h
        src/batchmode.c src/input_interface.c src/input_interface.h src/interactivemode.c src/interactivemode.h src/gamma_model.c src/gamma_model.h)

# Wskazujemy plik zawierający funkcję main.
set(TEST_FILE src/gamma_test.c)

add_executable(gamma src/gamma_main.c ${SOURCE_FILES})

# Dodajemy obsługę Doxygena: sprawdzamy, czy jest zainstalowany i jeśli tak to:
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # Wskazujemy lokalizacją pliku konfiguracyjnego i podajemy jego docelową lokalizację w folderze, gdzie następuje kompilacja.
    configure_file(Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    # Dodajemy cel doc: użycie make doc spowoduje wywołanie doxygena z odpowiednim plikiem konfiguracyjnym w folderze kompilacji.
    # Na wyjście zostanie wypisany odpowiedni komentarz.
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
    )
endif (DOXYGEN_FOUND)


find_library(CMOCKA cmocka)
if (CMOCKA)
    message("Find cmocka")
    add_executable(gamma_test ${SOURCE_FILES} src/gamma_test.c)
    target_link_libraries(gamma_test ${CMOCKA})
    add_test(official_part1 gamma_test)
endif (CMOCKA)

file(COPY tests/test.sh DESTINATION .)

add_test(official_part2 ./test.sh ./gamma ${CMAKE_SOURCE_DIR}/tests/official_part2/)
add_test(unofficial_part2 ./test.sh ./gamma ${CMAKE_SOURCE_DIR}/tests/unofficial_part2/)

enable_testing()
