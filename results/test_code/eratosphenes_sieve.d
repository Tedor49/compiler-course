var n := readInt, arr := [], ans := 0

for i in 2 .. n loop
	if arr[i] is empty then
		// print i, " "
		ans += 1
		for j in i .. n / i loop
			arr[j*i] := true
		end
	end
end

print ans
