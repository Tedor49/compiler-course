var arr := []
arr[10] := "hello"
arr[100] := {a:=10, arr[10]*2, c:=true xor false}
arr[1000] := func(a, b) is
		return a + b
		end
print arr, "\n", arr[10], arr[100], arr[1000], "\n"
print arr[1]