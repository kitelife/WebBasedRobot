# -*- coding: utf-8 *-*
import os

import tornado.ioloop
import tornado.web

#import ctrserial
import operate_file


class mainHandler(tornado.web.RequestHandler):

    def get(self):
        self.render("index.html", title="console")

    def post(self):
        if 'id' in self.request.arguments.keys():
            print self.request.arguments.get('id')[0]
            fileHandler = open('test/' + self.request.arguments.get('id')[0] +
            '.xml', 'r')
            self.write(fileHandler.read())
            fileHandler.close()


class parseCmd(tornado.web.RequestHandler):

    def post(self):
        if self.get_argument("command") == 'query':
            print self.get_argument("command"), self.get_argument("towho")
            #ctrserial.senddata(self.get_argument("command"))
            fileHandler = open('test/robotlist.xml', 'r')
            self.write(fileHandler.read())
            fileHandler.close()


class processFile(tornado.web.RequestHandler):

    def post(self):
        if self.request.files:
            print self.get_argument('writewho')
            for f in self.request.files['postfile']:
                operate_file.saveFile(f)


settings = {
    "static_path": os.path.join(os.path.dirname(__file__), "static")
}

application = tornado.web.Application([
    (r"/", mainHandler),
    (r"/sendcmd", parseCmd),
    (r"/uploadfile", processFile),
], **settings)


if __name__ == "__main__":
    application.listen(8888)
    tornado.ioloop.IOLoop.instance().start()
