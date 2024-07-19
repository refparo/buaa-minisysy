declare void @putch(i32)
define dso_local i32 @main() {
    %1 = alloca i32
    store i32 1, ptr %1
    br label %2

2:
    %3 = load i32, ptr %1
    %4 = icmp slt i32 %3, 12
    br i1 %4, label %5, label %31

5:
    %6 = alloca i32
    store i32 0, ptr %6
    br label %7

7:
    %8 = icmp eq i32 1, 1
    br i1 %8, label %9, label %27

9:
    %10 = load i32, ptr %6
    %11 = srem i32 %10, 3
    %12 = icmp eq i32 %11, 1
    br i1 %12, label %13, label %15

13:
    %14 = add i32 48, 1
    call void @putch(i32 %14)
    br label %16

15:
    call void @putch(i32 48)
    br label %16

16:
    %17 = load i32, ptr %6
    %18 = add i32 %17, 1
    store i32 %18, ptr %6
    %19 = load i32, ptr %6
    %20 = load i32, ptr %1
    %21 = mul i32 2, %20
    %22 = sub i32 %21, 1
    %23 = icmp sge i32 %19, %22
    br i1 %23, label %24, label %26

24:
    br label %27

25:
    br label %26

26:
    br label %7

27:
    call void @putch(i32 10)
    %28 = load i32, ptr %1
    %29 = add i32 %28, 1
    store i32 %29, ptr %1
    br label %2

30:
    br label %2

31:
    ret i32 0
}
