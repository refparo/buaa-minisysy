declare void @putint(i32)
define dso_local i32 @main() {
    %1 = alloca i32
    store i32 10, ptr %1
    %2 = load i32, ptr %1
    %3 = icmp eq i32 %2, 0
    %4 = icmp eq i1 %3, 0
    %5 = icmp eq i1 %4, 0
    %6 = zext i1 %5 to i32
    %7 = sub i32 0, %6
    %8 = icmp ne i32 %7, 0
    br i1 %8, label %9, label %13

9:
    %10 = sub i32 0, 1
    %11 = sub i32 0, %10
    %12 = sub i32 0, %11
    store i32 %12, ptr %1
    br label %14

13:
    store i32 0, ptr %1
    br label %14

14:
    %15 = load i32, ptr %1
    call void @putint(i32 %15)
    ret i32 0
}
