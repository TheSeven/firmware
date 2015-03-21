#define ASM_FILE
#include "global.h"

.syntax unified


#ifdef CPU_ARM_CORTEX_M0

.section .text.__clzsi2,"ax",%progbits
.global __clzsi2
.type __clzsi2, %function
__clzsi2:
    movs r1, #28
    movs r3, #1
    lsls r3, r3, #16
    cmp r0, r3
    bcc 2f
    lsrs r0, r0, #16
    subs r1, r1, #16
2:  lsrs r3, r3, #8
    cmp r0, r3
    bcc 2f
    lsrs r0, r0, #8
    subs r1, r1, #8
2:  lsrs r3, r3, #4
    cmp r0, r3
    bcc 2f
    lsrs r0, r0, #4
    subs r1, r1, #4
2:  adr r2, 1f
    ldrb r0, [r2, r0]
    add r0, r0, r1
    bx lr
.align 2
1:  .byte 4, 3, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0
.size __clzsi2, . - __clzsi2

.section .text.__aeabi_lmul,"ax",%progbits
.global __aeabi_lmul
.type __aeabi_lmul, %function
__aeabi_lmul:
    push {r4, lr}
    muls r1, r2
    muls r3, r0
    adds r1, r3
    lsrs r3, r0, #16
    lsrs r4, r2, #16
    muls r3, r4
    adds r1, r3
    lsrs r3, r0, #16
    uxth r0, r0
    uxth r2, r2
    muls r3, r2
    muls r4, r0
    muls r0, r2
    movs r2, #0
    adds r3, r4
    adcs r2, r2
    lsls r2, #16
    adds r1, r2
    lsls r2, r3, #16
    lsrs r3, #16
    adds r0, r2
    adcs r1, r3
    pop {r4, pc}
.size __aeabi_lmul, . - __aeabi_lmul

.section .text.__aeabi_uidivmod,"ax",%progbits
.global __aeabi_uidivmod
.global __aeabi_uidiv
.type __aeabi_uidivmod, %function
.type __aeabi_uidiv, %function
__aeabi_uidivmod:
__aeabi_uidiv:
    cmp r1, #0
    bne 1f
    ldr r3, =__aeabi_idiv0
    mov pc, r3
1:
    movs r2, #1
    movs r3, #0
    cmp r0, r1
    bls 2f
    adds r1, #0
    bmi 2f
1:
    lsls r2, #1
    lsls r1, #1
    bmi 2f
    cmp r0, r1
    bhi 1b
2:
    cmp r0, r1
    bcc 1f
    subs r0, r1
    orrs r3, r2
1:
    lsrs r1, #1
    lsrs r2, #1
    bne 2b
    mov r1, r0
    mov r0, r3
    bx lr
.size __aeabi_uidivmod, . - __aeabi_uidivmod

.section .text.__aeabi_idivmod,"ax",%progbits
.global __aeabi_idivmod
.type __aeabi_idivmod, %function
__aeabi_idivmod:
    cmp r0, #0
    bge 2f
    rsbs r0, r0, #0
    cmp r1, #0
    bge 1f
    rsbs r1, r1, #0
    push {lr}
    bl __aeabi_uidivmod
    rsbs r1, r1, #0
    pop {pc}
1:
    push {lr}
    bl __aeabi_uidivmod
    rsbs r0, r0, #0
    rsbs r1, r1, #0
    pop {pc}
2:
    cmp r1, #0
    bge __aeabi_uidivmod
    rsbs r1, r1, #0
    push {lr}
    bl __aeabi_uidivmod
    rsbs r0, r0, #0
    pop {pc}
.size __aeabi_idivmod, . - __aeabi_idivmod

.section .text.__aeabi_idiv,"ax",%progbits
.global __aeabi_idiv
.type __aeabi_idiv, %function
__aeabi_idiv:
    cmp r0, #0
    bge 1f
    rsbs r0, r0, #0
    cmp r1, #0
    bge 2f
    rsbs r1, r1, #0
    b __aeabi_uidivmod
1:
    cmp r1, #0
    bge __aeabi_uidivmod
    rsbs r1, r1, #0
2:
    push {lr}
    bl __aeabi_uidivmod
    rsbs r0, r0, #0
    pop {pc}
.size __aeabi_idiv, . - __aeabi_idiv

.section .text.__aeabi_uldivmod,"ax",%progbits
.global __aeabi_uldivmod
.type __aeabi_uldivmod, %function
__aeabi_uldivmod:
    cmp r3, #0
    bne 4f
    cmp r2, #0
    beq __aeabi_ldiv0
    cmp r1, #0
    beq 3f
    push {r4, r5, r6, r7, lr}
    movs r5, #1
    adds r7, r2, #0
    bmi 2f
    cmp r1, r7
    blo 2f
1:
    lsls r5, #1
    lsls r7, #1
    bmi 2f
    cmp r1, r7
    bhs 1b
2:
    mov r3, r1
    mov r2, r0
    movs r1, #0
1:
    cmp r3, r7
    bcc 2f
    subs r3, r7
    orrs r1, r5
2:
    lsrs r7, #1
    lsrs r5, #1
    bne 1b
    movs r5, #1
    lsls r5, #31
    movs r6, #0
    b 5f
3:
    mov r1, r2
    push {lr}
    bl __aeabi_uidivmod
    mov r2, r1
    movs r1, #0
    movs r3, #0
    pop {pc}
4:
    push {r4, r5, r6, r7, lr}
    mov r7, r3
    mov r6, r2
    mov r3, r1
    mov r2, r0
    movs r1, #0
    movs r5, #1
    adds r7, #0
    bmi 5f
    cmp r3, r7
    blo 5f
1:
    lsls r5, #1
    lsls r7, #1
    lsls r6, #1
    adcs r7, r1
    bmi 5f
    cmp r3, r7
    bhs 1b
5:
    movs r0, #0
1:
    mov r4, r3
    cmp r2, r6
    sbcs r4, r7
    bcc 2f
    subs r2, r6
    sbcs r3, r7
    orrs r0, r5
2:
    lsls r4, r7, #31
    lsrs r6, #1
    lsrs r7, #1
    orrs r6, r4
    lsrs r5, #1
    bne 1b
    pop {r4, r5, r6, r7, pc}
.size __aeabi_uldivmod, . - __aeabi_uldivmod

.section .text.__aeabi_llsl,"ax",%progbits
.global __aeabi_llsl
.type __aeabi_llsl, %function
__aeabi_llsl:
    cmp r2, #31
    bhi 1f
    movs r3, r1
    lsls r0, r2
    lsls r1, r2
    rsbs r2, r2, #0
    adds r2, #32
    lsrs r3, r2
    orrs r1, r3
    bx lr
1:
    subs r2, #32
    movs r1, r0
    lsls r1, r2
    movs r0, #0
    bx lr
.size __aeabi_llsl, . - __aeabi_llsl

.section .text.__aeabi_llsr,"ax",%progbits
.global __aeabi_llsr
.type __aeabi_llsr, %function
__aeabi_llsr:
    cmp r2, #31
    bhi 1f
    movs r3, r1
    lsrs r0, r2
    lsrs r1, r2
    rsbs r2, r2, #0
    adds r2, #32
    lsls r3, r2
    orrs r0, r3
    bx lr
1:
    subs r2, #32
    movs r0, r1
    lsrs r0, r2
    movs r1, #0
    bx lr
.size __aeabi_llsr, . - __aeabi_llsr

#else

.section .text.__aeabi_uldivmod,"ax",%progbits
.global __aeabi_uldivmod
.type __aeabi_uldivmod, %function
__aeabi_uldivmod:
    cmp r3, #0
    bne 3f
    cmp r1, #0
    bne 1f
    mov r1, r0
    udiv r0, r0, r2
    mul r2, r0, r2
    sub r2, r1, r2
    mov r1, #0
    bx lr
1:
    push {r4, r5, r6, r7, lr}
    movs r5, #1
    adds r7, r2, #0
    bmi 2f
    cmp r1, r7
    blo 2f
1:
    lsls r5, #1
    lsls r7, #1
    bmi 2f
    cmp r1, r7
    bhs 1b
2:
    mov r3, r1
    mov r2, r0
    movs r1, #0
1:
    cmp r3, r7
    bcc 2f
    subs r3, r7
    orrs r1, r5
2:
    lsrs r7, #1
    lsrs r5, #1
    bne 1b
    movs r5, #1
    lsls r5, #31
    movs r6, #0
    b 2f
3:
    push {r4, r5, r6, r7, lr}
    mov r7, r3
    mov r6, r2
    mov r3, r1
    mov r2, r0
    movs r1, #0
    movs r5, #1
    adds r7, #0
    bmi 2f
    cmp r3, r7
    blo 2f
1:
    lsls r5, #1
    lsls r7, #1
    lsls r6, #1
    adcs r7, r1
    bmi 2f
    cmp r3, r7
    bhs 1b
2:
    movs r0, #0
1:
    mov r4, r3
    cmp r2, r6
    sbcs r4, r7
    bcc 2f
    subs r2, r6
    sbcs r3, r7
    orrs r0, r5
2:
    lsls r4, r7, #31
    lsrs r6, #1
    lsrs r7, #1
    orrs r6, r4
    lsrs r5, #1
    bne 1b
    pop {r4, r5, r6, r7, pc}
.size __aeabi_uldivmod, . - __aeabi_uldivmod

#endif

.section .text.__aeabi_atexit,"ax",%progbits
.global __dso_handle
.global __aeabi_atexit
.type __aeabi_atexit, %function
__dso_handle:
__aeabi_atexit:
    movs r0, #0
    bx lr
.size __aeabi_atexit, . - __aeabi_atexit

.section .text.__gnu_thumb1_case_uqi,"ax",%progbits
.global __gnu_thumb1_case_uqi
.type __gnu_thumb1_case_uqi, %function
__gnu_thumb1_case_uqi:
    push {r1}
    mov r1, lr
    lsrs r1, r1, #1
    lsls r1, r1, #1
    ldrb r1, [r1, r0]
    lsls r1, r1, #1
    add lr, lr, r1
    pop {r1}
    bx lr
.size __gnu_thumb1_case_uqi, . - __gnu_thumb1_case_uqi
