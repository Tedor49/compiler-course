var slice := func(arr, l, r) is
	var newarr := []
	for i in l .. r loop
		newarr := newarr + [arr[i]]
	end
	return newarr
end

var merge := func(h1, l1, h2, l2) is
	//print l1, h1, " ", l2, h2, "\n";
	
	var merged := [], y1 := 1
	for y2 in 1 .. l2 loop
		while y1 <= l1 loop
			if h1[y1] >= h2[y2] then break end
			merged := merged + [h1[y1]]
			y1 := y1 + 1
		end
		merged := merged + [h2[y2]]
	end
	while y1 <= l1 loop
		merged := merged + [h1[y1]]
		y1 := y1 + 1
	end	
	return merged
end

var merge_sort := func(arrlen, arr) is
	if arrlen = 1 then
		return arr
	end
	
	var h1 := merge_sort(arrlen / 2, slice(arr, 1, arrlen / 2))
	var h2 := merge_sort(arrlen - arrlen / 2, slice(arr, arrlen / 2 + 1, arrlen))
	
	return merge(h1, arrlen / 2, h2, arrlen - arrlen / 2)
end

var arrlen := readInt
var arr := []

for i in 1 .. arrlen loop
	arr := arr + [readInt]
end

print merge_sort(arrlen, arr)
