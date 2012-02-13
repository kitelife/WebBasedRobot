import tornado.ioloop
import tornado.web

class mainHandler(tornado.web.RequestHandler):
	def get(self):
		self.render("index.html", title="formpost")
	
	def post(self):
		#print self.request.arguments
		#if 'firstname' in self.request.arguments.keys():
		#	print self.get_argument("firstname")
		#if 'lastname' in self.request.arguments.keys():
		#	print self.get_argument("lastname")
		if 'cmd' in self.request.arguments.keys():
			print self.get_argument("cmd")
		self.render("index.html", title="formpost")

application = tornado.web.Application([
	(r"/", mainHandler),
])

if __name__=="__main__":
	application.listen(8888)
	tornado.ioloop.IOLoop.instance().start()
