define dso_local i32 @main() {
    %1 = alloca i32
    store i32 456, ptr %1
    %2 = alloca i32
    store i32 8456, ptr %2
    %3 = alloca i32
    %4 = load i32, ptr %2
    %5 = load i32, ptr %1
    %6 = sub i32 %4, %5
    %7 = sdiv i32 %6, 1000
    %8 = sub i32 %7, 8
    store i32 %8, ptr %3
    %9 = alloca i32
    store i32 2, ptr %9
    %10 = load i32, ptr %3
    %11 = load i32, ptr %9
    %12 = add i32 %10, %11
    store i32 %12, ptr %3
    %13 = load i32, ptr %3
    %14 = load i32, ptr %9
    %15 = sub i32 %13, %14
    %16 = add i32 %15, 0
    ret i32 %16
}
