#include <stdint.h>

#include <reg24lu1.h>

// These are MDU optimized integer multiplication/division functions.
// At the moment only 16 bit multiplication, might add division later.

#pragma save
#pragma less_pedantic

#define xl r0
#define xh r1
#define yl r2
#define yh r3

#define negcnt r4

int32_t mul_16x16(int16_t x, int16_t y)
{
	x,y;		// silence the compiler

	__asm
	
	mov negcnt,#0x00; we count the number of negations in this
	
	mov xl,dpl		; load the arguments into registers
	mov xh,dph
	
	mov dptr,#_mul_16x16_PARM_2
	movx a,@dptr
	mov yl,a
	inc dptr
	movx a,@dptr
	mov yh,a
	
	mov a,xh		; check if operand x is negative
	anl a,#0x80
	jz check_y

	mov a,xl		; negate x (bitwise NOT + 1)
	cpl a
	add a,#0x01
	mov xl,a
	mov a,xh
	cpl a
	addc a,#0x00
	mov xh,a
	inc negcnt		; one negation done
	
check_y:

	mov a,yh		; check if operand y is negative
	anl a,#0x80
	jz do_calc

	mov a,yl		; negate x (bitwise NOT + 1)
	cpl a
	add a,#0x01
	mov yl,a
	mov a,yh
	cpl a
	addc a,#0x00
	mov yh,a
	inc negcnt		; another negation done

do_calc:

	mov _MD0,xl		; setup a multiplication on the MDU
	mov _MD4,yl
	mov _MD1,xh
	mov _MD5,yh
	
	nop				; wait 11 cycles for the multiplication to finish
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	
	mov	dpl,_MD0	; it should be done by now, load the result
	mov	dph,_MD1
	mov	b,_MD2
	mov	r1,_MD3

	mov a,#0xff		; do we need to negate the result?
	add a,negcnt
	jnz finished
	
	mov a,dpl		; yes, we need to negate
	cpl a
	add a,#0x01
	mov dpl,a
	
	mov a,dph
	cpl a
	addc a,#0x00
	mov dph,a
	
	mov a,b
	cpl a
	addc a,#0x00
	mov b,a

	mov a,r1
	cpl a
	addc a,#0x00
	mov r1,a
	
finished:

	mov a,r1

	__endasm;
}

#pragma restore
