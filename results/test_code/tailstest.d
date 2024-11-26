var nest := [0, [{b:= "not here", a:={c:=13, func(a, b) is return {a:=a, b:=[1, 2, b]} end}}, 2]]
print nest[2][1].a.2(5, 10).b[3]