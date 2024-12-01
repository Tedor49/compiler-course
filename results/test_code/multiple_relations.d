var c2 := func () is
	print "side effect\n"
	return 2
end

print 1 < 2 < 3 < 4 <= 5, "\n"
print 2 <= 2 >= 2 > -2 > -10, "\n"
print 1 = 1 < 3, "\n"
print 1 /= 2 /= 3 /= 4, "\n"
print 1 > 2 > 1 > 0, "\n"

print 0 < c2() < 10 // should only print "side effect" once