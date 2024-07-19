declare void @putint(i32)
define dso_local i32 @func1() {
    ret i32 555
}
define dso_local i32 @func2() {
    ret i32 111
}
define dso_local i32 @main() {
    %1 = alloca i32
    %2 = call i32 @func1()
    store i32 %2, ptr %1
    %3 = load i32, ptr %1
    %4 = call i32 @func2()
    %5 = sub i32 %3, %4
    call void @putint(i32 %5)
    ret i32 0
}
