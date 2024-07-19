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
    %10 = load i32, ptr %3
    %11 = add i32 %9, %10
    %12 = load i32, ptr %4
    %13 = add i32 %11, %12
    %14 = icmp eq i32 %13, 10
    br i1 %14, label %15, label %18

15:
    %16 = load i32, ptr %5
    %17 = add i32 %16, 1
    store i32 %17, ptr %5
    br label %34

18:
    %19 = load i32, ptr %1
    %20 = load i32, ptr %2
    %21 = add i32 %19, %20
    %22 = load i32, ptr %3
    %23 = add i32 %21, %22
    %24 = load i32, ptr %4
    %25 = add i32 %23, %24
    %26 = icmp eq i32 %25, 8
    br i1 %26, label %27, label %30

27:
    %28 = load i32, ptr %5
    %29 = add i32 %28, 2
    store i32 %29, ptr %5
    br label %33

30:
    %31 = load i32, ptr %5
    %32 = add i32 %31, 4
    store i32 %32, ptr %5
    br label %33

33:
    br label %34

34:
    %35 = load i32, ptr %5
    call void @putint(i32 %35)
    ret i32 0
}
