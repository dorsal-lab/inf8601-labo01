cmake_minimum_required(VERSION 3.13.5)
project(tp1 LANGUAGES C CXX)


include_directories(pipeline include)
add_executable(pipeline)
target_link_libraries(pipeline -lm -pthread -lpng -ltbb)
target_sources(pipeline PUBLIC
    source/filter.c
    source/image.c
    source/main.c
    source/pipeline-pthread.c
    source/pipeline-serial.c
    source/pipeline-tbb.cpp
    source/queue.c
)
# For macros with __FILE__
target_compile_options(pipeline PUBLIC "-fmacro-prefix-map=${CMAKE_SOURCE_DIR}/=")

add_custom_target(format
    COMMAND clang-format -i `find source -type f -iname '*.c'` `find include -type f -iname '*.h'`
    COMMAND clang-format -i `find source -type f -iname '*.cpp'` `find include -type f -iname '*.hpp'`
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_custom_target(remise
    COMMAND tar -zcvf remise.tar.gz source/pipeline-pthread.c source/pipeline-tbb.cpp
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_custom_target(run-serial
    COMMAND time ${CMAKE_CURRENT_BINARY_DIR}/pipeline --directory ${PROJECT_SOURCE_DIR}/data --pipeline serial
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
add_dependencies(run-serial pipeline)

add_custom_target(run-pthread
    COMMAND time ${CMAKE_CURRENT_BINARY_DIR}/pipeline --directory ${PROJECT_SOURCE_DIR}/data --pipeline pthread
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
add_dependencies(run-pthread pipeline)

add_custom_target(run-tbb
    COMMAND time ${CMAKE_CURRENT_BINARY_DIR}/pipeline --directory ${PROJECT_SOURCE_DIR}/data --pipeline tbb
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
add_dependencies(run-tbb pipeline)

add_custom_target(run-all
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
add_dependencies(run-all run-serial run-pthread run-tbb)

add_custom_target(generate-check
    COMMAND ./data/generate-random ./data/0000.png
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_custom_target(check
    COMMAND ./data/check.sh
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
add_dependencies(check generate-check run-pthread run-tbb run-serial)

install(TARGETS pipeline)
