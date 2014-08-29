with open('tokens.txt','r') as f:
	data = f.read()
	data = data.replace(' ','')
	dataArr = data.split(',')
	for i in dataArr:
		print ('"'+i+'",'),
	
