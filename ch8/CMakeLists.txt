add_library(myrwlock SHARED my_pthread_rwlock.c)
target_include_directories(myrwlock
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(myrwlock
  pthread
)

add_executable(testcancel testcancel.c)
target_include_directories(testcancel 
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(testcancel 
  myrwlock
)