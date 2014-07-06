#!/usr/bin/python

import sys

if len(sys.argv) != 3:
	print 'Usage: python %s [input file] [output file]' % ( sys.argv[0] )
	quit()

with open(sys.argv[1], 'r') as input:
	sort_list = []
	output_list = []
	if_sorting = 0

	for line in input:
		if if_sorting:
			if line == '#@endsort\n':
				sort_list.sort()
				output_list.extend(sort_list)
				sort_list = []
				output_list.append('//#@endsort\n')
				if_sorting = 0
			else:
				sort_list.append(line)
		else:
			if line == '#@sort\n':
				output_list.append('//#@sort\n')
				if_sorting = 1
			else:
				output_list.append(line)

	with open(sys.argv[2], 'w') as output:
		for item in output_list:
			output.write(item)
