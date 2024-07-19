declare void @putint(i32)
define dso_local i32 @gcd(i32 %0, i32 %1) {
    %3 = icmp eq i32 %1, 0
    br i1 %3, label %4, label %6

4:
    ret i32 %0

5:
    br label %6

6:
    %7 = srem i32 %0, %1
    %8 = call i32 @gcd(i32 %1, i32 %7)
    ret i32 %8
}
define dso_local i32 @main() {
    %1 = alloca i32
    store i32 100, ptr %1
    %2 = alloca i32
    store i32 48, ptr %2
    %3 = load i32, ptr %1
    %4 = load i32, ptr %2
    %5 = call i32 @gcd(i32 %3, i32 %4)
    call void @putint(i32 %5)
    ret i32 0
}
