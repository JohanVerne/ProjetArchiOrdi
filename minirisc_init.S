.section .minirisc_init,"ax"
.global _minirisc_init
.type   _minirisc_init, @function

_minirisc_init:
	/* Set stack pointer and call _start */
	la sp, __stack_top
	call _start
	/* Powerdown */
	li t3,0x220a0000
	li t2,0xdeadbeef
	sw t2,0(t3)
	