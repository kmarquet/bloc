target remote localhost:2000
file main.elf

set print address off
set print symbol-filename off

break main
continue

set $bos = $r1

b end_expe
c
p/x *((unsigned int *) $bos - 2000) @2000

quit

