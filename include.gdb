target remote localhost:2000
file aes.elf

set print address off
set print symbol-filename off

break main
continue

set $bos = $r1

set $diff = 0
set $max = 0

while ($diff >= 0)

      printf "step "

      set $tos = $r1
      set $diff = $bos - $tos 
      if ($diff > $max)
	 printf "%d -> %d\n", $max, $diff
      	 set $max = $diff
      end
      n 1
end

print $max

quit

