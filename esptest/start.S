.section .init
.global _start

_start:
    li a0, 0x42010000
    jr a0
