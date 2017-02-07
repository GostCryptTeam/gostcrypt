import os
import sys
import re

#for windows "\\"
#for linux "/"

def recursive(dir,totalLines):
	for each in os.listdir(dir):
		print(dir + "/" + each)
		if os.path.isdir(dir + "/" + each):
			print ("going into " + dir + "/" + each)
			totalLines = recursive(dir + "/" + each,totalLines)
		if os.path.isfile(dir + "/" + each):
			if str(dir + "/" + each).endswith(".c") or str(dir + "\\" + each).endswith(".cpp"):
				print (dir + "/" + each + " commented")
				totalLines = commentFile(dir + "/" + each,totalLines)
	return totalLines
	
def commentFile(input,totalLines, output = 0):
	if (output == 0):
		output = input
		
	inputFile = open(input, 'r')
	outputBuffer = inputFile.readlines()
	totalLines += len(outputBuffer)
	inputFile.close()
	
	outputBuffer = commentHead(outputBuffer,input.replace("/","\\"))
	outputBuffer = commentFunctions(outputBuffer)
	
	outputFile = open(output, 'w')
	for line in outputBuffer:
		outputFile.write(line)
	outputFile.close()
	print("File copied in " + output)
	return totalLines
	
def commentFunctions(file):
	i=0
	regex = r"\n\t*(([^\s\t(\n]+\s[^\s\t(\n]+\s?[^)\n]*\s?[(][^)]*)[)]\n?)\s*\t*[{]"
	prog = re.compile(regex)	
	result = prog.findall(''.join(file))
	result = list(set(result))
	for line in result:
		for i in range(len(file)):	
			if line[1]+")" in file[i] and (not "else" in line[1]):
				file.insert(i," */\n")
				file.insert(i," *\n")
				file.insert(i," *	\\return\n")
				for param in line[0].split("(")[1].split(","):
					try :
						buff = param.split(" ")
						buff = list(filter(None, buff))
						if ("*" in buff[1]):
							nb = buff[1].count("*")
							if (len(buff) > 2):
								file.insert(i," *	\\param[in] "+ buff[0].replace("\n", "")+ " " + nb*"*" + " "+ buff[1].replace("*","").replace("\n","").replace(")","") + buff[2].replace("\n","").replace(")","") + "\n")
							else:
								file.insert(i," *	\\param[in] "+ buff[0].replace("\n", "")+ " " + nb*"*" + " "+ buff[1].replace("*","").replace("\n","").replace(")","") + "\n")

						else:
							file.insert(i," *	\\param[in] "+ buff[0].replace("\n", "") +" "+ buff[1].replace("\n", "").replace(")","") +"\n")
							
					except :
						pass
				file.insert(i," *	\\brief\n")
				file.insert(i," *	\\fn "+ line[0].replace("\n", "") + "\n")
				file.insert(i," *\n")
				file.insert(i,"/**\n")
				break
	return file
	
def commentHead(file,inputFileName):
	i=0
	while(1):
		if (file[i].startswith("#") or file[i].startswith("/*")):
			file.insert(i," */\n\n")
			file.insert(i," *\n")
			file.insert(i," *	\\date\n")
			file.insert(i," *	\\version\n")
			file.insert(i," *	\\brief\n")
			file.insert(i," *	\\file "+ str(inputFileName).split("\\")[len(str(inputFileName).split("\\"))-1] +"\n")
			file.insert(i," *\n")
			file.insert(i,"/**\n")
			return file
		i=i+1
	return file
	
if __name__ == "__main__":
	totalLines = 0
	if (len(sys.argv) < 2): #not enough arguments
		print("Arguments required")
		print("Usage : doxygenBase.py [option] inputFile.c [outputFile]\nFor options type doxygenBase.py help")
		sys.exit(0)
		
	if (len(sys.argv) > 4): #too many arguments
		print("Too many arguments")
		print("Usage : doxygenBase.py [option] inputFile.c [outputFile]\nFor options type doxygenBase.py help")
		sys.exit(0)
		
	if  (sys.argv[1] == "help"): #help 
		print("-r : recursive over the folder specified\nExample : 'doxygenBase.py [-r] PereTypus.c'")
		sys.exit(0)
		
	if (sys.argv[1] == "-r" and len(sys.argv) > 2): #recursive
		try:
			directories = os.listdir(sys.argv[2]) #check the existence of the directory
		except:
			print("Specify a folder that exists please")
			sys.exit(0)
		totalLines = recursive(sys.argv[2],totalLines) #call the function recursive that will manage the algorithm to fulfil the file
		print("lines to be commented " + str(totalLines))
		sys.exit(0)
		
	if (len(sys.argv) > 1): #use directly on a single file
		if (len(sys.argv) == 3 and sys.argv[1] != sys.argv[1]): #if the name is specified for the output (should be different than the inputfile name)
			totalLines = commentFile(sys.argv[1],totalLines,sys.argv[2])
			print("lines to be commented " + str(totalLines))
		else:
			totalLines = commentFile(sys.argv[1],totalLines)
			print("lines to be commented " + str(totalLines))
		sys.exit(0)
		
		
		
		
		
