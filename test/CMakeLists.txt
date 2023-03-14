add_executable(LockFreeQueueTest
               ../src/util/CompilerSpecifics.hpp
               ../src/util/LockFreeQueue.hpp
               ../src/util/FixedSizeLockFreeQueue.hpp
               LockFreeQueueTest/main.cpp)

add_executable(CircularDequeTest
               ../src/util/CircularDeque.hpp
               ../src/util/FixedSizeCircularDeque.hpp
               CircularDequeTest/main.cpp)