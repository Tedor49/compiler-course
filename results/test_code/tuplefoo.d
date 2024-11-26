var bubble_sort := func(point) is
	return point.x*point.x+point.y*point.y
end

print bubble_sort({x := readReal, y := readReal})