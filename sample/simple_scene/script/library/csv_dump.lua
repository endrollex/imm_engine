-- csv_dump.lua

-- str_trim
function str_trim(s)
	return (s:gsub("^%s*(.-)%s*$", "%1"))
end

-- str_split
function str_split(inputstr, sep)
	if sep == nil then sep = "," end
	local t={}
	for str in string.gmatch(inputstr, "([^"..sep.."]+)") do
		t[#t+1] = str_trim(str)
	end
	return t
end

-- read_csv
function read_csv(fname)
	local file = io.open(fname, "r")
	local t = {}
	local i = 1
	while true do
		local temp = file:read("l")
		if temp == nil then break end
		t[i] = str_split(temp)
		i = i+1
	end
	file:close()
	return t
end

-- print_table2d
function print_table2d(t)
	for k, v in pairs(t) do
		local temp = ""
		for k2, v2 in pairs(v) do
			temp = temp .. " " .. v2
		end
		temp = string.sub(temp, 2)
		print(temp)
	end
end

--