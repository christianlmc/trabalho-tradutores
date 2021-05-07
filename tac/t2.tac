.table
char str0[] = "asdasd"
char str1[] = "\n"

.code
write:
    mov $0, 0
    __write_loop:
    mov $1, #0
    mov $1, $1[$0]
    seq $2, $1, '\0'
    brnz __write_loop_end, $2
    print $1
    add $0, $0, 1
    jump __write_loop
    __write_loop_end:
    return

func:
    add $0, #0, #1
    return $0
    return
main:
    mov $1, 1
    mov $2, 2
    param $2
    mov $3, 3
    param $3
    call func, 2
    pop $4
    add $5, $1, $4
    mov $0, $5
    mov $6, &str0
    param $6
    call write, 1
    mov $7, &str1
    param $7
    call write, 1
    println
