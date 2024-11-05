var fibonacci := func(n) is
	if n <= 1 then
		return n;
	end
	return fibonacci(n-1) + fibonacci(n-2)
end

print fibonacci(readInt)
