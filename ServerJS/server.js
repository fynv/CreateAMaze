import { createRequire } from "module";
import { fileURLToPath } from 'url';
import path from 'path';

const require = createRequire(import.meta.url);

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const express = require('express');
const http = require('http');
const { Server } = require("socket.io");
const MazeNode = require('../build/Release/MazeNode');
const fs = require('fs');

////////////// Start ///////////////////////////////

const app = express();
const server = http.createServer(app);

app.use(express.static(path.join(__dirname, "client")));


let map_filenames = {
    gold: "CyberbotGold.glb",
    green: "CyberbotGreen.glb",
    pink: "CyberbotPink.glb",
    red: "CyberbotRed.glb",
    silver: "CyberbotSilver.glb",
    yellow: "CyberbotYellow.glb"
    
}

class Maze
{
    constructor(maze_id, start_points)
    {
        this.maze_id = maze_id;
        this.start_points = {};
        this.start_points.gold = start_points[0];
        this.start_points.green = start_points[1];
        this.start_points.pink = start_points[2];
        this.start_points.red = start_points[3];
        this.start_points.silver = start_points[4];
        this.start_points.yellow = start_points[5];
        this.users = {}
        this.users.gold = null;
        this.users.green = null;
        this.users.pink = null;
        this.users.red = null;
        this.users.silver = null;
        this.users.yellow = null;
        
    }
    
    add_user(user)
    {
        this.users[user.id] = user;
    }
    
    remove_user(user)
    {
        this.users[user.id] = null;
    }
    
    emit_info(io)
    {
        io.sockets.in(this.maze_id).emit('full status', JSON.stringify(this.users));
    }
}

const arr_mazes = [];
let arr_mazes_static = [];

fs.readFile("mazes.json", (err, data)=>{
    if (err)
    {
        console.log(err);
        return;
    }
    arr_mazes_static = JSON.parse(data);
    for (let record of arr_mazes_static)
    {
        let maze = new Maze(record.maze_id, record.start_points);
        arr_mazes.push(maze);
    }
});

const io = new Server(server);
io.on('connection', (socket) => {
    const ip = socket.handshake.headers['x-forwarded-for'] || socket.conn.remoteAddress.split(":")[3];
    console.log("a user connected: "+ip);
    
    let maze_id = -1;
    let maze = null;
    let user = null;  
    
    const join_maze_as = (i, id) =>{
        maze_id = i;
        maze = arr_mazes[maze_id];
        let start_point = maze.start_points[id];
        let position = {};
        position.x = start_point.x * 3 - 30;
        position.y = 0;
        position.z = 30 - start_point.y * 3;
        user = {
            maze_id: maze_id,
            maze: `maze_${maze_id}.glb`,
            id: id,
            state: "idle",
            position: position,
        };
        maze.add_user(user);        
        socket.emit("identity", user);
        socket.join(maze_id);
        maze.emit_info(io);
    }
    
    const join_maze = (i)=>{
        let maze = arr_mazes[i];
        if (maze.users.gold == null)
        {
            join_maze_as(i, "gold");
            return true;
        }
        if (maze.users.green == null)
        {
            join_maze_as(i, "green");
            return true;
        }
        if (maze.users.pink == null)
        {
            join_maze_as(i, "pink");
            return true;
        }
        if (maze.users.red == null)
        {
            join_maze_as(i, "red");
            return true;
        }
        if (maze.users.silver == null)
        {
            join_maze_as(i, "silver");
            return true;
        }
        if (maze.users.yellow == null)
        {
            join_maze_as(i, "yellow");
            return true;
        }
        return false;
    }
    
    const new_maze = ()=>{
        maze_id = arr_mazes.length;
        let arr = MazeNode.createAMaze(`maze_${maze_id}.glb`, 21,21);
        let maze = new Maze(maze_id, arr);
        arr_mazes.push(maze);
        join_maze(maze_id);
        
        let record = {
            maze_id: maze_id,
            start_points: arr
        };
        arr_mazes_static.push(record);
        fs.writeFile("mazes.json", JSON.stringify(arr_mazes_static), (err) => 
        {
            if (err)
            {
                console.log(err);
                return;
            }               
        });
    }
    
    for (let i=0; i<arr_mazes.length; i++)
    {
        if (join_maze(i)) break;        
    }
    
    if (maze_id == -1) new_maze();
    
    socket.on('avatar status', (msg) => {        
        if (user!=null && maze_id>=0)
        {
            let avatar_status = JSON.parse(msg);
            if (avatar_status.maze_id != maze_id) return;
            let position =  avatar_status.position;
            if (Math.floor((31.5 + position.x)/3)==20 && Math.floor((31.5 -position.z)/3)==20)
            {
                maze.remove_user(user);
                maze.emit_info(io);
                socket.leave(maze_id); 
                let last_maze_id = maze_id;
                maze_id = -1;
                for (let i=last_maze_id+1; i<arr_mazes.length; i++)
                {
                    if (join_maze(i)) break;
                }
                if (maze_id == -1) new_maze();
            }
            else
            {                
                user.state = avatar_status.state;
                user.position = avatar_status.position;
                user.quaternion = avatar_status.quaternion;
                socket.broadcast.to(maze_id).emit('avatar status', msg);
            }
        }
    });
    
    socket.on('disconnect', () => {
        setTimeout(()=>
        {
            console.log('user disconnected: ' + ip);        
             
            if (user!=null && maze!=null)
            {   
                maze.remove_user(user);
                maze.emit_info(io);
            }
        }, 0);   
    });
    
});


server.listen(8888, () => {
    console.log('listening on *:8888');
});



