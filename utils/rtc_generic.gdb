define drtc
  set $count = *((unsigned long int*) 0x42)
end


target remote localhost:2000
file main.elf

break *START_DECRYPT_ADD
break *END_EXPE_ADD

continue

drtc
print "ENCRYPT_SMCLK:"
print $count
continue

drtc
print "DECRYPT_SMCLK:"
print $count

quit
