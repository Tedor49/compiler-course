for i in 1 .. 10 loop
	if i <= 6 then
		print i, "\n"
		continue
		print "unreachable\n"
	else
		print i, "\n"
		print "break\n"
		break
		print "unreachable\n"
	end
end