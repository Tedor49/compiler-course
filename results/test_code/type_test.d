var types := [1, 1.1, false, "hello", empty, [1, 2, 3], {a:="lol", "kek"}, func(a, b) => a+b, readInt, readReal, readString, 1 + 1, 1+2.0, 3.0-4, 0.1 * 0.1]
var names := ["int", "real", "bool", "str", "emp", "arr", "tup", "func", "rInt", "rRl", "rStr", "i+i", "i+r", "r-i", "r*r"]

for i in 1 .. 8 loop
	print "///\t", names[i]
end
print "\n"

for i in 1 .. 15 loop
	print names[i], "\t"
	if types[i] is int then
		print "V", "\t"
	else
		print "X", "\t"	
	end
	if types[i] is real then
		print "V", "\t"
	else
		print "X", "\t"	
	end
	if types[i] is bool then
		print "V", "\t"
	else
		print "X", "\t"	
	end
	if types[i] is string then
		print "V", "\t"
	else
		print "X", "\t"	
	end
	if types[i] is empty then
		print "V", "\t"
	else
		print "X", "\t"	
	end
	if types[i] is [      ] then
		print "V", "\t"
	else
		print "X", "\t"	
	end
	if types[i] is {} then
		print "V", "\t"
	else
		print "X", "\t"	
	end
	if types[i] is func then
		print "V", "\t"
	else
		print "X", "\t"	
	end
	print "\n"
end