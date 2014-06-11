define drtc
  set $count = *((unsigned long int*) 0x42)
end


target remote localhost:2000
file main.elf
layout split
focus cmd

break START_DECRYPT_ADD
break setbreak END_EXPE_ADD

continue

drtc
print "ENCRYPT_SMCLK:"
print $count
continue

drtc
print "DECRYPT_SMCLK:"
print $count
continue
