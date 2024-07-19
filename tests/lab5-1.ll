declare i32 @getint()
declare void @putint(i32)
define dso_local i32 @main() {
    %1 = alloca i32
    %2 = call i32 @getint()
    store i32 %2, ptr %1
    %3 = alloca i32
    store i32 2, ptr %3
    %4 = load i32, ptr %1
    %5 = load i32, ptr %3
    %6 = add i32 %4, %5
    call void @putint(i32 %6)
    %7 = alloca i32
    %8 = call i32 @getint()
    store i32 %8, ptr %7
    %9 = load i32, ptr %7
    %10 = load i32, ptr %3
    %11 = add i32 %9, %10
    call void @putint(i32 %11)
    %12 = alloca i32
    %13 = load i32, ptr %1
    %14 = add i32 %13, 2
    store i32 %14, ptr %12
    %15 = load i32, ptr %1
    %16 = load i32, ptr %12
    %17 = add i32 %15, %16
    call void @putint(i32 %17)
    ret i32 0
}
