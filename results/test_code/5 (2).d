var random := func(x, y) is
	if x > y * 5 then
		return x;
	end
	return random(x * 2, y)
end

print random(1, 300)
