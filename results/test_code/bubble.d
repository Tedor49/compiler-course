var bubble_sort := func(arrlen, arr) is
	for i in 1 .. arrlen - 1 loop
        for j in 1 .. arrlen - i loop
            if arr[j] > arr[j + 1] then
                var c := arr[j+1]
				arr[j+1] := arr[j]
				arr[j] := c
            end
        end
    end
	return arr
end

var arrlen := readInt
var arr := []

for i in 1 .. arrlen loop
	arr := arr + [readInt]
end

print bubble_sort(arrlen, arr)