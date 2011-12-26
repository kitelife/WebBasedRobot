# web-based swarm-robots remote control system,
# the server end is based on tornado(www.tornadoweb.org)
# 
# author: xiayf
# email: youngsterxyf@gmail.com

import tornado.ioloop
import tornado.web

class MainHandler(tornado.web.RequestHandler):
	def get(self):
		self.write("Hello,World")

application = tornado.web.Application([
	(r"/", MainHandler),
])

if __name__ == '__main__':
	application.listen(8888)
	tornado.ioloop.IOLoop.instance().start()
