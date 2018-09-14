target remote :1234
b *0x7c00
c
display/3i $pc
x /10i $pc
x /32xb $pc
