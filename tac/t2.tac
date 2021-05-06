.code
func:
    println $1023
    return
main:
    add $1023, -15, 10
    println $1023
    call func, 0
