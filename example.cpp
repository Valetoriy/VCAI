#include "include/vcai.hpp"

constexpr auto prog{R"(
fn:
fn_loop:
    cmp r1 a0
    je _fn
    push r1
    inc r1
    jmp fn_loop
_fn:
ret

main:
    # Ставим на стек 0 1 2 3 4 5
    mov a0 6
    call fn

    # Снимаем со стека значения и добавляем к r0
    xor r1 r1
main_loop:
    cmp r1 a0
    je _main
    pop r2
    add r0 r2
    inc r1
    jmp main_loop
_main:
ret
)"};

auto main() -> int {
    constexpr auto res{vcai::exec_fn(prog)};

    return res;
}
