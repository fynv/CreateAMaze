# CreateAMaze

A maze-game server that generates one maze after another maze for you to play.

## Referenced work

* [GLM](https://github.com/g-truc/glm) is included as a submodule.
* [Node.js](https://nodejs.org/) for server architecturing.
* [Express](https://expressjs.com/) as server-side dependency.
* [Socket.io](https://socket.io/docs/v4/) for browser-server communication.
* [cmake-js](https://github.com/cmake-js/cmake-js) for Node.js addon building.

The following projects are included as plain source-code:

* [TinyGLTF](https://github.com/syoyo/tinygltf) for writing glb models.
* [three.js](https://threejs.org/) for WebGL rendering.
* [node-addon-api](https://github.com/nodejs/node-addon-api) for Node.js addon binding.

## Building

You need Node.js and cmake-js:

```
# npm install -g cmake-js
# cmake-js
```

## Running the server

```
# cd ServerJS
# npm install
# node server.js
```
You may need to use "nohup" to run it on a remote server as a background process.

