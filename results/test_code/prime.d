var prime := func(n) is
	if n <= 1 then
		return false
	end
	
	var k:=2
	while k*k <= n loop
		if ((n / k) * k) = n then
			return false
		end
		k := k + 1
	end
	return true
end

var x := 244

print prime(readInt)
