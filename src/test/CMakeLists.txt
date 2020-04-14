include_directories(../..)

find_library(CMOCKA cmocka)
if(CMOCKA)
    enable_testing()

    add_executable(queue_test
            ../utilities/queue.h
            ../utilities/queue.c
            queue_test.c)

    add_executable(golden_move_test
            ../gamma.c
            ../gamma.h
            ../field.c
            ../field.h
            ../stringology.c
            ../stringology.h
            ../utilities/queue.c
            ../utilities/queue.h
            ../utilities/uset.h
            ../utilities/uset.c
            gamma_golden_move.c)


    target_link_libraries(queue_test ${CMOCKA})
    target_link_libraries(golden_move_test ${CMOCKA})
    add_test(queue_test queue_test)
    add_test(golden_move_test golden_move_test)
endif()