declare void @putch(i32)
define dso_local i32 @main() {
    %1 = alloca i32
    store i32 1, ptr %1
    br label %2

2:
    %3 = load i32, ptr %1
    %4 = icmp slt i32 %3, 12
    br i1 %4, label %5, label %26

5:
    %6 = alloca i32
    store i32 0, ptr %6
    br label %7

7:
    %8 = load i32, ptr %6
    %9 = load i32, ptr %1
    %10 = mul i32 2, %9
    %11 = sub i32 %10, 1
    %12 = icmp slt i32 %8, %11
    br i1 %12, label %13, label %23

13:
    %14 = load i32, ptr %6
    %15 = srem i32 %14, 3
    %16 = icmp eq i32 %15, 1
    br i1 %16, label %17, label %19

17:
    %18 = add i32 48, 1
    call void @putch(i32 %18)
    br label %20

19:
    call void @putch(i32 48)
    br label %20

20:
    %21 = load i32, ptr %6
    %22 = add i32 %21, 1
    store i32 %22, ptr %6
    br label %7

23:
    call void @putch(i32 10)
    %24 = load i32, ptr %1
    %25 = add i32 %24, 1
    store i32 %25, ptr %1
    br label %2

26:
    ret i32 0
}
