declare i32 @getint()
declare void @putint(i32)
define dso_local i32 @main() {
    %1 = alloca i32
    %2 = call i32 @getint()
    store i32 %2, ptr %1
    %3 = alloca i32
    %4 = call i32 @getint()
    store i32 %4, ptr %3
    %5 = load i32, ptr %1
    %6 = load i32, ptr %3
    %7 = icmp sle i32 %5, %6
    br i1 %7, label %8, label %9

8:
    call void @putint(i32 1)
    br label %10

9:
    call void @putint(i32 0)
    br label %10

10:
    ret i32 0
}
