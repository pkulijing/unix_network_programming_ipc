link_libraries(rt)
add_executable(mqnotifythread1 mqnotifythread1.c)
target_link_libraries(mqnotifythread1
  pthread
)
add_executable(mqnotifysig5 mqnotifysig5.c)
add_executable(mqnotifysig4 mqnotifysig4.c)
add_executable(mqnotifysig3 mqnotifysig3.c)
add_executable(mqnotifysig2 mqnotifysig2.c)
add_executable(mqnotifysig1 mqnotifysig1.c)
add_executable(mqsend mqsend.c)
add_executable(mqreceive mqreceive.c)
add_executable(mqgetattr mqgetattr.c)
add_executable(mqcreate1 mqcreate1.c)
add_executable(mqcreate2 mqcreate2.c)
add_executable(mqunlink mqunlink.c)

