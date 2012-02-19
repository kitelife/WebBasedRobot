import tornado.ioloop
import tornado.web

import ctrserial
import operateFile

class mainHandler(tornado.web.RequestHandler):
	def get(self):
		self.render("index.html", title="console")
	
	def post(self):
		print self.request.arguments
		if self.request.files:
			#print self.request.files
			for f in self.request.files['postfile']:
				operateFile.saveFile(f)
		if 'command' in self.request.arguments.keys():
			if self.get_argument("command") == 'query':
				print self.get_argument("command"), self.get_argument("towho")
				ctrserial.senddata(self.get_argument("command"))
				fileHandler = open('robotlist.xml', 'r')
				self.write(fileHandler.read())
				fileHandler.close()
		elif 'id' in self.request.arguments.keys():
			print self.request.arguments.get('id')[0]
			fileHandler = open(self.request.arguments.get('id')[0]+'.xml', 'r')
			self.write(fileHandler.read())
			fileHandler.close()
application = tornado.web.Application([
	(r"/", mainHandler),
])

if __name__=="__main__":
	application.listen(8888)
	tornado.ioloop.IOLoop.instance().start()
