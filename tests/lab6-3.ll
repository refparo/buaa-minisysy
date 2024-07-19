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
    br i1 %8, label %9, label %24

9:
    %10 = load i32, ptr %3
    %11 = srem i32 %10, 2
    %12 = icmp eq i32 %11, 0
    br i1 %12, label %13, label %17

13:
    %14 = load i32, ptr %3
    %15 = add i32 %14, 1
    store i32 %15, ptr %3
    br label %5

16:
    br label %17

17:
    %18 = load i32, ptr %3
    %19 = add i32 %18, 1
    store i32 %19, ptr %3
    %20 = load i32, ptr %4
    %21 = load i32, ptr %3
    %22 = add i32 %20, %21
    store i32 %22, ptr %4
    %23 = load i32, ptr %4
    call void @putint(i32 %23)
    call void @putch(i32 10)
    br label %5

24:
    ret i32 0
}
