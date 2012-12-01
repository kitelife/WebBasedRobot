# -*- coding: utf-8 *-*
import serial

CMD_CODE = {
    'info': '1',
    'run': '2',
    'forward': '3',
    'backward': '4',
    'turnLeft': '5',
    'turnRight': '6',
    'stop': '7',
}


def scan():
    '''scan for available ports. return a list of tuples (num, name)'''
    available = []
    for i in range(256):
        try:
            s = serial.Serial(i)
            available.append((i, s.portstr))
            s.close()
        except serial.SerialException:
            pass
    return available


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
    
    data_list = ['0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 
                    '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0']

    cmd_list = cmd.split(' ')

    if len(cmd_list) and (cmd_list[0] in CMD_CODE.keys()):
        cmd_code = CMD_CODE[cmd_list[0]]
        if len(cmd_code) == 1:
            data_list[2] = cmd_code
        elif len(cmd_code) == 2:
            data_list[1] = cmd_code[0]
            data_list[2] = cmd_code[1]

    if len(cmd_list) == 2:
        cmd_arg = cmd_list[1].strip()
        if len(cmd_arg) == 1:
            data_list[3] = cmd_arg
        elif len(cmd_arg) == 2:
            data_list[3] = cmd_arg[0]
            data_list[4] = cmd_arg[1]

    if target == 'all':
        target_code = 'x'
    else:
        target_code = target
    data_list[0] = target_code

    data_str = ('').join(data_list)
    print '%s' % data_str
    try:
        serial_handler = user_serial()
        #serial_port = scan()[0][0]
        #serial_handler.ser.setPort(serial_port)
        serial_handler.send_data(data_str)
    except Exception, e:
        result_value = e.message

    return result_value


if __name__ == '__main__':
    import time
    send_cmd('info', 'all')
    time.sleep(1)
    send_cmd('run', '1')
    time.sleep(2)
    send_cmd('backward', '1')
    time.sleep(2)
    send_cmd('forward', '1')
    time.sleep(2)
    send_cmd('turnLeft', '1')
    time.sleep(2)
    send_cmd('turnRight', '1')
    time.sleep(2)
    send_cmd('stop', '1')