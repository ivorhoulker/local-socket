import socketio

sio = socketio.Server()
app = socketio.WSGIApp(sio)

@sio.event
def connect(sid, environ):
    print("Got connection: ", sid)
    return sid
    

@sio.event
def handshake(sid, data):
    print("handshake from client: ", data)
    sio.emit("receive", data, room="game room", skip_sid=sid)
    return "hi from the python server"

@sio.event
def disconnect(sid):
    print("Disconnected SID >> ", sid)
