add_executable(locknone locknone.c lockmain.c)
add_executable(lockwholefile lockfcntl.c lockmain.c)
add_executable(locktest1
  locktest1.c
  lock_reg.c
)
add_executable(locktest2
  locktest2.c
  lock_reg.c
)
add_executable(onedaemon
  onedaemon.c
  lock_reg.c
)
