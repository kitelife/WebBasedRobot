import tornado.ioloop
import tornado.web

import ctrserial

class mainHandler(tornado.web.RequestHandler):
	def get(self):
		self.render("index.html", title="console")
	
	def post(self):
		#print self.request.arguments
		if 'command2all' in self.request.arguments.keys():
			ctrserial.senddata(self.get_argument("command2all"))
			print self.get_argument("command2all")
		elif 'command2one' in self.request.arguments.keys():
			pass
		elif 'who' in self.request.arguments.keys():
			#print self.request.arguments.get('who')[0]
			fileHandler = open(self.request.arguments.get('who')[0]+'.txt', 'r')
			self.write(fileHandler.read())
application = tornado.web.Application([
	(r"/", mainHandler),
])

if __name__=="__main__":
	application.listen(8888)
	tornado.ioloop.IOLoop.instance().start()
