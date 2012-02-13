import serial

def senddata(message):
	ser = serial.Serial(2)
	print ser.portstr
	if len(message) < 32:
		message = message + ' ' * (32-len(message))
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
