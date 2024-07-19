define dso_local i32 @main() {
    %1 = alloca i32
    %2 = sub i32 123, 122
    store i32 %2, ptr %1
    %3 = load i32, ptr %1
    ret i32 %3
}
