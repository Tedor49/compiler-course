var _

var getarr := func(l, elem) is
	var arr := []
	var n := 0
	for _ in 1 .. l loop
		arr := arr + [elem]
		n := n + 1
	end
	return arr
end

var min := func(a, b) is
	if a < b then return a end
	return b
end

var max := func(a, b) is
	if a > b then return a;end
	return b
end

var maxnum := 50; var n := readInt, arr := []

for i in 1 .. n loop
	arr := arr + [readInt]
	maxnum := max(maxnum, arr[i] + 1)
end

var tree := getarr(n*4, 0)
var lazy := getarr(n*4, 1)

var build := func(cx, cl, cr) is
	if cr - cl = 1 then
		tree[cx] := arr[cl]
		return empty
	end
	_ := build(cx*2, cl, (cl + cr) / 2)
	_ := build(cx*2 + 1, (cl + cr) / 2, cr)
	tree[cx] := min(tree[cx*2], tree[cx*2+1])
end 

var propagate := func(cx, cl, cr) is
	if cr - cl /= 1 then
		lazy[cx*2] := lazy[cx*2] * lazy[cx]
		lazy[cx*2+1] := lazy[cx*2+1] * lazy[cx]
	end
	tree[cx] := tree[cx] * lazy[cx]
	lazy[cx] := 1
	maxnum := max(maxnum, tree[cx] + 1)
end

var getmin := func(cx, cl, cr, tl, tr) is
	_ := propagate(cx, cl, cr)
	if tl >= cr or cl >= tr then
		return maxnum
	end
	
	if tl <= cr and cl <= tr then
		return tree[cx]
	end
	
	return min(getmin(cx*2, cl, (cl+cr)/2, tl, tr), getmin(cx*2+1, (cl+cr)/2, cr, tl, tr))
end

var mult_segment := func(cx, cl, cr, tl, tr, op) is
	_ := propagate(cx, cl, cr)
	if tl >= cr or cl >= tr then
		return empty
	end
	
	if tl <= cr and cl <= tr then
		lazy[cx] := lazy[cx]* op
		_ := propagate(cx, cl, cr)
		return empty
	end
	_ := mult_segment(cx*2, cl, (cl+cr)/2, tl, tr, op)
	_ := mult_segment(cx*2+1, (cl+cr)/2, cr, tl, tr, op)
	tree[cx] := min(tree[cx*2], tree[cx*2+1])
	return empty
end

_ := build(1, 1, n+1)

var c, l, r, a

while true loop
	c := readString
	if c = "getmin" then
		l := readInt
		r := readInt
		print getmin(1, 1, n+1, l, r+1), "\n"
	else if c = "mult" then
		l := readInt
		r := readInt
		a := readInt
		_ := mult_segment(1, 1, n+1, l, r+1, a)
	else 
		print "Unknown command\n"
	end end
end