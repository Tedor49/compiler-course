var a := [1, 2]
a[5] := 5

var b := [1, empty, empty, empty, 5]
b[2] := 2

print a = b, "\n"

a := {x:=1, 2, z:=[1]}
print a = {x:=1, y:=2, z:=[1]}, "\n"
print a = {x:=1, 2, z:=1}, "\n"
print a = {x:=1, 2, z:=[1]}, "\n"