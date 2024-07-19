declare i32 @getint()
declare void @putint(i32)
define dso_local i32 @main() {
    %1 = alloca i32
    %2 = call i32 @getint()
    store i32 %2, ptr %1
    %3 = load i32, ptr %1
    %4 = add i32 %3, 4
    call void @putint(i32 %4)
    ret i32 0
}
