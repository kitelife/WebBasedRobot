# -*- coding: utf-8 *-*
import serial
import time

CMD_CODE = {
    'info': '1',
    'run': '2',
    'forward': '3',
    'backward': '4',
    'turnleft': '5',
    'turnright': '6',
    'stop': '7',
    'count': '8'
}

version_dict = {'1': 0, '2': 0, '3': 0, '4': 0, '5': 0}

def restore_version_dict():
    for key in version_dict:
        version_dict[key] = 0

def scan():
    '''串口扫描查找'''

    available = []
    for i in range(256):
        try:
            s = serial.Serial(i)
            available.append((i, s.portstr))
            s.close()
        except serial.SerialException:
            pass
    return available

def encode_cmd(cmd, target):
    '''功能：指令编码'''
    '''
    cmd: 包含两个部分---指令本身以及其参数，以空格分隔
    target: 小车的编号
    '''
    encoded_cmd_list = list()
    
    data_list = ['0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 
                    '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0']

    cmd_list = cmd.split(' ')

    if len(cmd_list) and (cmd_list[0] in CMD_CODE.keys()):
        cmd_code = CMD_CODE[cmd_list[0]]

        '''指令编码使用2,3两个字节存储，2字节存编码的十位，3字节存编码的个位'''
        if len(cmd_code) == 1:
            data_list[3] = cmd_code
        elif len(cmd_code) == 2:
            data_list[2] = cmd_code[0]
            data_list[3] = cmd_code[1]

    if len(cmd_list) == 2:
        cmd_arg = cmd_list[1].strip()
        '''指令参数编码使用4，5两个字节存储，4字节存储编码的十位，5字节存编码的个位'''
        if len(cmd_arg) == 1:
            data_list[5] = cmd_arg
        elif len(cmd_arg) == 2:
            data_list[4] = cmd_arg[0]
            data_list[5] = cmd_arg[1]
    '''使用"x"来指代所有小车'''
    if target == 'all':
        target_code = 'x'
    else:
        target_code = target

    ''' 更新版本号
        设置小车编号
        设置版本号
    '''
    if target_code == 'x':
        for key in version_dict.keys():
            version_dict[key] += 1
            one_target_data_list = data_list
            one_target_data_list[1] = key
            version = version_dict[key]
            if version >= 10000:
                version = 1
            index = 8
            while version > 0:
                remainder = version % 10
                one_target_data_list[index] = str(remainder)
                version = version / 10
                index -= 1
            one_target_data_str = ''.join(one_target_data_list)
            encoded_cmd_list.append(one_target_data_str)
    else:
        version_dict[target_code] = version_dict.get(target_code, 0) + 1
        data_list[1] = target_code
        version = version_dict[target_code]
        if version >= 10000:
            version = 1
        index = 8
        while version > 0:
            remainder = version % 10
            data_list[index] = str(remainder)
            version = version / 10
            index -= 1
        data_str = ''.join(data_list)
        encoded_cmd_list.append(data_str)        
    cmd_type = cmd_list[0]
    return (encoded_cmd_list, cmd_type)

def send_cmd(encoded_cmd_list, cmd_type, arg_of_cmd=0):
    '''功能：指令发送'''

    result_value = ''
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
        for encoded_cmd in encoded_cmd_list:
            serial_handler.write(encoded_cmd)
            if cmd_type == "info":
                response = serial_handler.read(32)
                print response
                result_value += response[0] + "号小车 --- " + "电量：" + response[1] + "." + response[2] + "(伏)；"
                result_value += "左电机: " + response[3] + response[4] + response[5] + response[6] + "，"
                result_value += "右电机：" + response[7] + response[8] + response[9] + response[10]
    except Exception, e:
        result_value = e.message.decode('gbk').encode('utf-8')
    if result_value == '':
        result_value = 'true'
    
    # 在服务器端实现控制指令的参数功能
    time.sleep(arg_of_cmd)

    return result_value


if __name__ == '__main__':
    '''单元测试'''
    
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
