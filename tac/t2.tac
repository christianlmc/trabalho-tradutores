.code
func:
    println $1023
    return
main:
    add $1023, 10, 10
    println $1023
    call func, 0
