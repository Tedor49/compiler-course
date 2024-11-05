var x := [true, 2, "3"]
print x[1], x[2], x[3], "\n"
x := {boolianElem := true, 3, classMethod := func(x) is print "Your input was ", x end}
print x.boolianElem , x.2, x.3("Welcome"), "\n"
x := func(a)=>a+1
print x, x(2), "\n"