var arr := []
for i in 1 .. 10 loop
    var l := +i
    arr[i] := func(x) => x * l * l
end

var a := readInt

for i in 1 .. 10 loop
    print arr[i](a), "\n"
end
