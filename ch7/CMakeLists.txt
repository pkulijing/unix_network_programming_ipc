add_executable(prodcons2 prodcons2.c)
target_link_libraries(prodcons2
  pthread
)
add_executable(prodcons6 prodcons6.c)
target_link_libraries(prodcons6
  pthread
)