inputs = [1000, 10000, 100000, 150000, 200000, 250000, 300000]
naive = []
quick = []
graham = []

reg = Regexp.new("([a-zA-Z]*\s*)*(?<input>[0-9]+) -> (?<output>[0-9]+) in (?<time>[0-9\.]+)(\S*\s*)*")

inputs.each do |inval|
	x0 = `./ConvexHull2D --count #{inval} --atype 0`.split("\n")[1]
	x1 = `./ConvexHull2D --count #{inval} --atype 1`.split("\n")[1]
	x2 = `./ConvexHull2D --count #{inval} --atype 2`.split("\n")[1]
	ndata = reg.match(x0)
	qdata = reg.match(x1)
	gdata = reg.match(x2)
	templist = [ndata["input"], ndata["output"], ndata["time"]]
	naive.push(templist)	
	templist = [qdata["input"], qdata["output"], qdata["time"]]
	quick.push(templist)	
	templist = [gdata["input"], gdata["output"], gdata["time"]]
	graham.push(templist)	
end

naive.each do |tuple|
	puts tuple.inspect
end
puts "-------------------"
quick.each do |tuple|
	puts tuple.inspect
end
puts "-------------------"
graham.each do |tuple|
	puts tuple.inspect
end