declare void @putint(i32)
define dso_local i32 @main() {
    %1 = alloca i32
    %2 = alloca i32
    %3 = alloca i32
    store i32 1, ptr %3
    %4 = alloca i32
    %5 = alloca i32
    store i32 5, ptr %1
    store i32 5, ptr %2
    %6 = sub i32 0, 2
    store i32 %6, ptr %4
    store i32 2, ptr %5
    %7 = load i32, ptr %1
    %8 = load i32, ptr %2
    %9 = add i32 %7, %8
    %10 = icmp eq i32 %9, 9
    %11 = load i32, ptr %1
    %12 = load i32, ptr %2
    %13 = sub i32 %11, %12
    %14 = icmp eq i32 %13, 0
    %15 = load i32, ptr %5
    %16 = icmp ne i32 %15, 4
    %17 = and i1 %14, %16
    %18 = or i1 %10, %17
    br i1 %18, label %19, label %22

19:
    %20 = load i32, ptr %5
    %21 = add i32 %20, 3
    store i32 %21, ptr %5
    br label %37

22:
    %23 = load i32, ptr %3
    %24 = load i32, ptr %4
    %25 = add i32 %23, %24
    %26 = sub i32 0, 1
    %27 = icmp ne i32 %25, %26
    %28 = load i32, ptr %5
    %29 = add i32 %28, 1
    %30 = srem i32 %29, 2
    %31 = icmp eq i32 %30, 1
    %32 = or i1 %27, %31
    br i1 %32, label %33, label %36

33:
    %34 = load i32, ptr %5
    %35 = add i32 %34, 4
    store i32 %35, ptr %5
    br label %36

36:
    br label %37

37:
    %38 = load i32, ptr %5
    call void @putint(i32 %38)
    ret i32 0
}
