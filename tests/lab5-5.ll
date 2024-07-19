declare void @putint(i32)
@b = dso_local global i32 7
define dso_local i32 @main() {
    %1 = alloca i32
    %2 = load i32, ptr @b
    store i32 %2, ptr %1
    %3 = alloca i32
    store i32 8, ptr %3
    %4 = load i32, ptr %3
    %5 = load i32, ptr %1
    %6 = add i32 %4, %5
    call void @putint(i32 %6)
    ret i32 0
}
