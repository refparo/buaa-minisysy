declare void @putch(i32)
declare void @putint(i32)
define dso_local i32 @main() {
    %1 = icmp sgt i32 25, 24
    br i1 %1, label %2, label %7

2:
    %3 = alloca i32
    store i32 24, ptr %3
    %4 = load i32, ptr %3
    %5 = mul i32 %4, 50
    %6 = sub i32 12, %5
    call void @putint(i32 %6)
    call void @putch(i32 10)
    br label %7

7:
    %8 = alloca i32
    %9 = sdiv i32 25, 4
    store i32 %9, ptr %8
    %10 = load i32, ptr %8
    %11 = sdiv i32 50, %10
    call void @putint(i32 %11)
    %12 = alloca i32
    %13 = mul i32 25, 4
    store i32 %13, ptr %12
    %14 = load i32, ptr %12
    %15 = load i32, ptr %8
    %16 = sdiv i32 %14, %15
    call void @putint(i32 %16)
    call void @putch(i32 10)
    %17 = sdiv i32 50, 12
    call void @putint(i32 %17)
    ret i32 0
}
