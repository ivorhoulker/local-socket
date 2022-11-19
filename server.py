# run this in the terminal to start server
# >>>>>> gunicorn --threads 50 server:app
# ----------------------------------
# the code about uses gunicorn to start the server
# it uses 50 threads
# nameOfFile : the app variable
import socketio


sio = socketio.Server()
app = socketio.WSGIApp(sio)


players = []


@sio.event
def connect(sid, environ):
    print("A New Player Connected: ", sid)
    if len(players) == 0:
        sio.emit("position", "Player1", room=sid)
    if len(players) == 1:
        sio.emit("position", "Player2", room=sid)

    players.append(sid)
    sio.enter_room(sid, "game room")
    print("A new player entered game room")


@sio.event
def move(sid, data):
    print("MOVE from client: ", data)
    sio.emit("receive", data, room="game room", skip_sid=sid)


@sio.event
def disconnect(sid):
    print("Disconnected SID >> ", sid)
    players.remove(sid)
    print("This is now the list: ", players)