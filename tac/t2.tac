.code
func:
    add $0, #0, #1
    return $0
main:
    mov $1, 1
    minus $2, $1
    mov $3, 2
    param $3
    mov $4, 3
    param $4
    call func, 2
    pop $5
    add $6, $2, $5
    mov $0, $6
    println $0
