import os

def saveFile(fileInfo):
	currentDir = os.getcwd()
	print currentDir
	targetDir = currentDir +"/files/"
	if not os.path.isdir(targetDir):
		os.mkdir(targetDir)
	rawname = fileInfo['filename']
	print rawname
	absolute = targetDir + rawname
	tf = file(absolute, 'wb+')
	tf.write(fileInfo['body'])
	tf.seek(0)
	tf.close()