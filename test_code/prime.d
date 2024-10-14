var prime := func(n) is
	var k:=2
	while k*k < n loop
		if ((n / k) * k) = n then
			return false
		end
		k := k + 1
	end
	return true
end

x := 244

print prime(readInt)
