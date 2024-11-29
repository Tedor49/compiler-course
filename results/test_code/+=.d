var x := 0
print x, "\n"
x += 1
print x, "\n"
x -= 2.5
print x, "\n"

var y := x
y += 2
print x, "\n"

var y := +x
y += 2
print x, "\n"

x := [1, 2, 3]
x[1] += 1
x[1] += 1
x[1] += 1
x[2] += 2
x[3] += 3 + x[1]
print x, "\n"
