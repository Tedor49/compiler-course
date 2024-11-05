var map := func(apply, n, arr) is
	var ans := []
	for i in 1 .. n loop
		ans := ans + [apply(arr[i])];
	end
	return ans
end

var inc2by := func(x) => x+2;

var array := [1, 2, 3]
print map(inc2by, 3, array) 
