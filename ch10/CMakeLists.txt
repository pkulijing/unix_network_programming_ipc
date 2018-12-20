add_executable(prodcons1
  prodcons1.c
)
target_link_libraries(prodcons1 pthread)

add_executable(prodcons3
  prodcons3.c
)
target_link_libraries(prodcons3 pthread)

add_executable(prodcons4
  prodcons4.c
)
target_link_libraries(prodcons4 pthread)

add_library(sem_impl_fifo SHARED
  sem_impl_fifo.c
)