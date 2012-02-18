import serial

codeCmd = {'run':'0', 'left':'1', 'right':'2', 'back':'3', 'stop':'4'}

def senddata(message):
	ser = serial.Serial(3)
	ser.baudrate = 345600
	print ser.portstr
	if message in codeCmd.keys():
		message = codeCmd[message]
	message = message + ' ' * 31
	print message
	print len(message)
	ser.write(message)
	ser.close()

if __name__ == '__main__':
	message = 'left'
	num = 5
	while num > 0:
		senddata(message)
		num = num - 1
		sleeptimes = 10000
		while sleeptimes > 0:
			print sleeptimes
			sleeptimes -= 5 
