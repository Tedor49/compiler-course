var field := [[" ", " ", " "], [" ", " ", " "], [" ", " ", " "]]

var players := ["X", "O"]

var turn := 1

var print_field := func is
	for i in 1 .. 3 loop
		print "-------\n|"
		for j in 1 .. 3 loop
			if field[i][j] = " " then
				print (i - 1) * 3 + j, "|"
			else 
				print field[i][j], "|"
			end
		end
		print "\n"
	end
	print "-------\n"
	return
end	

var check_winner := func is
	for i in 1 .. 3 loop
		if field[i][1] = field[i][2] and field[i][2] = field[i][3] and field[i][1] /= " " then
			return field[i][1]
		end
		
		if field[1][i] = field[2][i] and field[2][i] = field[3][i] and field[1][i] /= " " then
			return field[1][i]
		end
	end
	
	if field[1][1] = field[2][2] and field[2][2] = field[3][3] and field[1][1] /= " " then
		return field[1][1]
	end
	
	if field[1][3] = field[2][2] and field[2][2] = field[3][1] and field[2][2] /= " " then
		return field[3][1]
	end
	
	return " "
end

var convert := func (idx) is
	var x := 1
	
	while idx > 3 loop
		x += 1
		idx -= 3
	end
	
	return {x:=x, y:=idx}
end

var finished := true

for move in 1 .. 9 loop
	print_field()
	print "\n", players[turn], " to move: "
	var move := convert(readInt)
	
	if field[move.x][move.y] /= " " then
		print("Invalid move!\n")
		continue
	end
	
	field[move.x][move.y] := players[turn]
	
	var winner := check_winner()
	
	if winner /= " " then
		print_field()
		print "\n", players[turn], " won!\n"
		finished := true
		break
	end
	
	turn += 1
	if turn = 3 then
		turn := 1
	end
end

if not finished then
	print_field()
	print "\nIt's a draw!"
end
