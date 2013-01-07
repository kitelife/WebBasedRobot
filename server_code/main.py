# -*- coding: utf-8 *-*
import os
import re
import time
import random
import json

import tornado.ioloop
import tornado.web

import operate_serial


class basic_handler(tornado.web.RequestHandler):
    
    session_id = None


class main_handler(basic_handler):

    def get(self):
        if basic_handler.session_id != None:
            if self.get_secure_cookie("current_user") == basic_handler.session_id:
                self.render("index.html", robot_list = operate_serial.version_dict.keys())
            else:
                self.render("static.html")
        else:
            session_id = str(time.time()) + str(random.uniform(100, 999))
            self.set_secure_cookie("current_user", session_id)
            basic_handler.session_id = session_id
            self.render("index.html", robot_list = operate_serial.version_dict.keys())
    
    def post(self):
        '''处理基本控制'''
        cmd = self.get_argument("command").strip().lower()
        target = self.get_argument("towho").strip().lower()
        print cmd, target

        encoded_cmd_list, cmd_type = operate_serial.encode_cmd(cmd, target)
        results = operate_serial.send_cmd(encoded_cmd_list, cmd_type)
        if type(results) == type(list()):
            self.write(json.dumps(results))
        else:
            self.write(results)

class detail_cmd_handler(basic_handler):

    def post(self):
        '''处理精确控制'''
        true_code = True
        err_msg = ""
        arg_of_cmd = 0

        cmd = self.get_argument("command").strip().lower()
        args = self.get_argument("args").strip().lower()
        target = self.get_argument("target").strip().lower().encode('utf-8')
        if args != "":
            arg_of_cmd = int(args)
        if arg_of_cmd > 99:
            err_msg += "时间参数应小于100"
            true_code = False

        print arg_of_cmd
        if true_code:
            print cmd, target
            encoded_cmd_list, cmd_type = operate_serial.encode_cmd(cmd, target)
            print encoded_cmd_list
            results = operate_serial.send_cmd(encoded_cmd_list, cmd_type, arg_of_cmd)
            if type(results) == type(list()):
                 self.write(json.dumps(results))
            else:
                self.write(results)
        else:
            self.write(err_msg)

class handle_multi_cmds(basic_handler):

    def post(self):
        '''处理批量指令'''
        arg_name_err = "第%s条代码的第%d个参数名称有错"
        arg_value_err = "第%s条代码的第%d个参数值不符合要求"
        err_msg = ""
        arg_of_cmd = 0

        multi_cmds = self.get_argument("multicmds").strip().lower().encode('utf-8')
        cmd_list = multi_cmds.split(";")
        if multi_cmds.endswith(';'):
            cmd_list = cmd_list[:-1]

        # 检查语法与逻辑错误
        true_code = True
        for index, cmd in enumerate(cmd_list):
            target = None
            cmd = cmd.strip("\n")
            cmd_parts = re.split(r'\s+', cmd)
            cmd = cmd_parts[0]
            cmd_len = len(cmd_parts)
            var_time_count = 0
            var_car_count = 0
            if cmd_len == 2 or cmd_len == 3:
                second_part_list = cmd_parts[1].split("=")
                if second_part_list[0].strip() == "time":
                    arg_of_cmd = int(second_part_list[1].strip())
                    print arg_of_cmd
                    if arg_of_cmd > 99:
                        err_msg += "第%s条代码：时间参数应小于100"
                        true_code = False
                    var_time_count += 1
                elif second_part_list[0].strip() == "car":
                    target = second_part_list[1].strip()
                    var_car_count += 1
                else:
                    true_code = False
                    err_msg += arg_name_err % (str(index+1), 1) + "<br />"
            if cmd_len == 3:
                third_part_list = cmd_parts[2].split("=")
                if third_part_list[0].strip() == "time":
                    if var_time_count == 1:
                        true_code = False
                        err_msg += "第%s条代码重复出现参数time" % (str(index+1), ) + "<br />"
                    else:
                        arg_of_cmd = int(third_part_list[1].strip())
                        if arg_of_cmd > 99:
                            err_msg += "第%s条代码：时间参数应小于100"
                            true_code = False
                        print arg_of_cmd
                        var_time_count += 1
                elif third_part_list[0].strip() == "car":
                    if var_car_count == 1:
                        true_code = False
                        err_msg += "第%s条代码重复出现参数car" % (str(index+1), ) + "<br />"
                    else:
                        target = third_part_list[1].strip()
                        var_car_count += 1
                else:
                    true_code = False
                    err_msg += arg_name_err % (str(index+1), 2) + "<br />"
            if cmd not in operate_serial.CMD_CODE.keys() or target == None:
                err_msg += '第%s条代码：指令格式不对' %(str(index+1), ) + "<br />"
                true_code = False

        if true_code:
            # 如果没有语法与逻辑错误，则逐条指令解析并发送

            results = ''

            for ctr_cmd in cmd_list:
                ctr_cmd = ctr_cmd.strip("\n")
                cmd_parts = re.split(r'\s+', ctr_cmd)
                cmd = cmd_parts[0]
                cmd_len = len(cmd_parts)
                if cmd_len >= 2:
                    second_part_list = cmd_parts[1].split('=')
                    if second_part_list[0].strip() == 'time':
                        arg_of_cmd = second_part_list[1].strip()
                    elif second_part_list[0].strip() == 'car':
                        target = second_part_list[1].strip()
                if cmd_len == 3:
                    third_part_list = cmd_parts[2].split('=')
                    if third_part_list[0].strip() == 'time':
                        arg_of_cmd = third_part_list[1].strip()
                    elif third_part_list[0].strip() == 'car':
                        target = third_part_list[1].strip()
                encoded_cmd_list, cmd_type = operate_serial.encode_cmd(cmd, target)
                result = operate_serial.send_cmd(encoded_cmd_list, cmd_type, int(arg_of_cmd))
                if type(result) == type(list()):
                    results += json.dumps(result)
                else:
                    results += result + "<br />"
            self.write(results)
        else:
            self.write(err_msg)


class exit_handler(basic_handler):

    def get(self):
        self.render("static.html")

    def post(self):
        operate_serial.restore_version_dict()
        basic_handler.session_id = None
        self.render("static.html")


settings = {
    "static_path": os.path.join(os.path.dirname(__file__), "static"),
    "debug": True,
    "cookie_secret": "61oETzKXQAGaYdkL5gEmGeJJFuYh7EQnp2XdTP1o/Vo="
}

application = tornado.web.Application([
    (r"/", main_handler),
    (r"/handlemulticmds", handle_multi_cmds),
    (r"/handledetailcmd", detail_cmd_handler),
    (r"/exit", exit_handler),
    (r"/(favicon\.ico)", tornado.web.StaticFileHandler, dict(path=settings['static_path']))
], **settings)


if __name__ == "__main__":
    application.listen(8888)
    tornado.ioloop.IOLoop.instance().start()
