# -*- coding: utf-8 *-*
import serial

CMD_CODE = {
    'info': 0x01,
    'run': 0x02,
    'forward': 0x03,
    'backward': 0x04,
    'turnLeft': 0x05,
    'turnRight': 0x06,
    'stop': 0x07,
}

CAR_CODE = {
    1: 0x01,
    2: 0x02,
    3: 0x03,
    4: 0x04,
    5: 0x05,
    6: 0x06
}

class user_serial(object):
    
    def __init__(self, baudrate=9600, port=1):
        self.ser = serial.Serial()
        self.ser.setBaudrate(baudrate)
        self.ser.setPort(port)

    def send_data(self, data):
        self.ser.write(data)

    def close(self):
        self.ser.close()


def send_cmd(cmd, target):

    result_value = 'OK'

    cmd_list = cmd.split(' ')

    if len(cmd_list) and (cmd_list[0] in CMD_CODE.keys()):
        cmd_code = CMD_CODE[cmd_list[0]]
    if target == 'all':
        target_code = 0xff
    elif target in CAR_CODE.keys():
        target_code = CAR_CODE[target]
    print target_code, cmd_code
    data = (target_code << 24) | (cmd_code << 16)
    print 'target_code: %x, cmd_code: %x' %(target_code, cmd_code)
    print '%x' % data
    try:
        serial_handler = user_serial()
        serial_handler.send_data(data)
    except Exception, e:
        result_value = e.message

    return result_value


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
