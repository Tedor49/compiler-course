var tribonacci := func(n) is
	if n <= 2 then
		return 0;
	end
	if n = 2 then
		return 1;
	end
	return tribonacci(n-1) + tribonacci(n-2) + tribonacci(n-3)
end

print tribonacci(readInt)
