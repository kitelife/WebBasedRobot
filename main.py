# -*- coding: utf-8 *-*
import os
import re

import tornado.ioloop
import tornado.web

import operate_serial


class mainHandler(tornado.web.RequestHandler):

    def get(self):
        self.render("index.html", title="console")
    
    def post(self):
        cmd = self.get_argument("command").strip().lower()
        target = self.get_argument("towho").strip().lower()
        print cmd, target
        status = operate_serial.send_cmd(cmd, target)
        self.write(status)

class detail_cmd_handler(tornado.web.RequestHandler):

    def post(self):
        cmd = self.get_argument("command").strip().lower()
        args = self.get_argument("args").strip().lower()
        target = self.get_argument("target").strip().lower()
        if args != "":
            cmd += " " + args
        print cmd, target
        status = operate_serial.send_cmd(cmd, target)
        self.write(status)

class handle_multi_cmds(tornado.web.RequestHandler):

    def post(self):
        syntax_err_msg = "第%s条代码有错<br />"
        multi_cmds = self.get_argument("multicmds").strip().lower()
        cmd_list = multi_cmds.split(";")
        for index, cmd in enumerate(cmd_list):
            cmd = cmd.strip("\n")
            cmd_parts = re.split(r'\s+', cmd)
            cmd = cmd_parts[0]
            cmd_len = len(cmd_parts)
            var_len_count = 0
            var_car_count = 0
            true_code = True
            if cmd_len == 2 or cmd_len == 3:
                second_part_list = cmd_parts[1].split("=")
                if second_part_list[0] == "len":
                    cmd += " " + second_part_list[1]
                    var_len_count += 1
                elif second_part_list[0] == "car":
                    target = second_part_list[1]
                    var_car_count += 1
                else:
                    true_code = False
                    self.write(syntax_err_msg % (str(index+1), ))
            if cmd_len == 3:
                third_part_list = cmd_parts[2].split("=")
                if third_part_list[0] == "len":
                    if var_len_count == 1:
                        true_code = False
                        self.write(syntax_err_msg % (str(index+1), ));
                    else:
                        cmd += " " + third_part_list[1]
                        var_len_count += 1
                elif third_part_list[0] == "car":
                    if var_car_count == 1:
                        true_code = False
                        self.write(syntax_err_msg % (str(index+1), ));
                    else:
                        target = third_part_list[1]
                        var_car_count += 1
                else:
                    true_code = False
                    self.write(syntax_err_msg % (str(index+1), ));
            
            if true_code:
                print cmd, target
                status = operate_serial.send_cmd(cmd, target)
                self.write(status + "<br />")
            else:
                return

settings = {
    "static_path": os.path.join(os.path.dirname(__file__), "static"),
    "debug": True
}

application = tornado.web.Application([
    (r"/", mainHandler),
    (r"/handlemulticmds", handle_multi_cmds),
    (r"/handledetailcmd", detail_cmd_handler),
    (r"/(favicon\.ico)", tornado.web.StaticFileHandler, dict(path=settings['static_path']))
], **settings)


if __name__ == "__main__":
    application.listen(8888)
    tornado.ioloop.IOLoop.instance().start()
