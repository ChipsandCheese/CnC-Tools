.text

.global clockTest

clockTest:
    mov $1, %r8
    mov $50, $rcx
    xor %rdx, %rdx
clockTestLoop:
    .rept 50
    rep add %r8, %rdx
    .endr
    sub $rcx, %rbi
    jnz clockTestLoop
    ret