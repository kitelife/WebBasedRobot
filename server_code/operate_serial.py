# -*- coding: utf-8 *-*
import serial

CMD_CODE = {
    'info': '1',
    'run': '2',
    'forward': '3',
    'backward': '4',
    'turnleft': '5',
    'turnright': '6',
    'stop': '7',
    'count': '9'
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

'''
class user_serial(object):
    
    def __init__(self, baudrate=9600, port=1):
        self.ser = serial.Serial()
        self.ser.setBaudrate(baudrate)
        self.ser.setPort(port)

    def send_data(self, data):
        self.ser.write(data)

    def close(self):
        self.ser.close()
'''

def send_cmd(cmd, target):
    '''
    cmd: 包含两个部分---指令本身以及其参数，以空格分隔
    target: 小车的编号
    '''
    result_value = 'true'
    
    data_list = ['0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 
                    '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0']

    cmd_list = cmd.split(' ')

    if len(cmd_list) and (cmd_list[0] in CMD_CODE.keys()):
        cmd_code = CMD_CODE[cmd_list[0]]
        '''指令编码使用1,2两个字节存储，1字节存编码的十位，2字节存编码的个位'''
        if len(cmd_code) == 1:
            data_list[2] = cmd_code
        elif len(cmd_code) == 2:
            data_list[1] = cmd_code[0]
            data_list[2] = cmd_code[1]

    if len(cmd_list) == 2:
        cmd_arg = cmd_list[1].strip()
        '''指令参数编码使用3，4两个字节存储，3字节存储编码的十位，4字节存编码的个位'''
        if len(cmd_arg) == 1:
            data_list[4] = cmd_arg
        elif len(cmd_arg) == 2:
            data_list[3] = cmd_arg[0]
            data_list[4] = cmd_arg[1]
    '''使用"x"来指代所有小车'''
    if target == 'all':
        target_code = 'x'
    else:
        target_code = target
    '''32字节数据的0字节存储小车编号'''
    data_list[0] = target_code

    data_str = ('').join(data_list)
    print '%s' % data_str
    try:
        serial_handler = serial.Serial(2,
                                        baudrate=345600,
                                        bytesize=serial.EIGHTBITS,
                                        parity=serial.PARITY_NONE,
                                        stopbits=serial.STOPBITS_ONE,
                                        timeout=3,
                                        xonxoff=0,
                                        rtscts=0
                                        )
        serial_handler.setRTS(1)
        serial_handler.setDTR(1)
        serial_handler.flushInput()
        serial_handler.flushOutput()
        serial_handler.write(data_str)
        if cmd_list[0] == "info":
            response = serial_handler.read(32)
            print response
            result_value = response[0] + "号小车 --- " + "电量：" + response[1] + "." + response[2] + "(伏)；"
            result_value += "左电机: " + response[3] + response[4] + response[5] + response[6] + "，"
            result_value += "右电机：" + response[7] + response[8] + response[9] + response[10]

            print result_value
    except Exception, e:
        result_value = e.message.decode('gbk').encode('utf-8')

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