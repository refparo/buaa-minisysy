declare void @putch(i32)
declare i32 @getint()
declare void @putint(i32)
define dso_local i32 @main() {
    %1 = alloca i32
    %2 = call i32 @getint()
    store i32 %2, ptr %1
    %3 = alloca i32
    store i32 0, ptr %3
    %4 = alloca i32
    store i32 0, ptr %4
    br label %5

5:
    %6 = load i32, ptr %3
    %7 = load i32, ptr %1
    %8 = icmp slt i32 %6, %7
    br i1 %8, label %9, label %16

9:
    %10 = load i32, ptr %3
    %11 = add i32 %10, 1
    store i32 %11, ptr %3
    %12 = load i32, ptr %4
    %13 = load i32, ptr %3
    %14 = add i32 %12, %13
    store i32 %14, ptr %4
    %15 = load i32, ptr %4
    call void @putint(i32 %15)
    call void @putch(i32 10)
    br label %5

16:
    ret i32 0
}