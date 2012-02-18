import tornado.ioloop
import tornado.web

import ctrserial

class mainHandler(tornado.web.RequestHandler):
	def get(self):
		self.render("index.html", title="console")
	
	def post(self):
		#print self.request.arguments
		if 'cmd' in self.request.arguments.keys():
			ctrserial.senddata(self.get_argument("cmd"))
			print self.get_argument("cmd")
		#self.render("index.html", title="console")

application = tornado.web.Application([
	(r"/", mainHandler),
])

if __name__=="__main__":
	application.listen(8888)
	tornado.ioloop.IOLoop.instance().start()
