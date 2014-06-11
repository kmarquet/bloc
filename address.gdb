target remote localhost:2000
file main.elf

#Set logging on
#set logging file log.txt
#set logging redirect on
#
set print address off
set print symbol-filename off

break main
continue

set $bos = $r1

#p/x *((unsigned int *) $bos - 4000) @4000

b end_expe
c
p/x *((unsigned int *) $bos - 4000) @4000

#Set $diff = 0
#set $max = 0

#set $mc_address = &memcpy
#set $mc_address_end = $mc_address + 40

#printf "adress of memcpy = %x\n", $mc_address

#while ($diff >= 0)
      
#       set $cma = $r0
#       printf "%d => %d\n", $cma, $mc_address
#       if ($cma >= $mc_address && $cma <= $mc_address_end)
#       	  printf "========= SKIP =========\n"
#       	  finish
#       else	
#       		printf "### step ###"
#		s
#       end 
       
#       set $tos = $r1
#       set $diff = $bos - $tos 
#       if ($diff > $max)
#	  printf "********** %d -> %d **********\n", $max, $diff
#      	  set $max = $diff
#       end
#
#       printf "=> %d %d %d %d\n", $bos, $tos, $max, $diff
#	printf "loop\n"
#end

quit

