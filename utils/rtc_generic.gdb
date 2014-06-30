define drtc
  set $count = *((unsigned long int*) 0x42)
end


target remote localhost:2000
file main.elf

break *START_DECRYPT_ADD
break *END_EXPE_ADD

continue

drtc
printf "ENCRYPT_SMCLK: %u\n", $count
continue

drtc
printf "DECRYPT_SMCLK: %u\n", $count

quit
