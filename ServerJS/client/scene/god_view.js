import { io } from "/socket.io/socket.io.esm.min.js"

export function fix_anims(anim_data)
{
	for (let clip of anim_data.animations)
	{
		for (let track of clip.tracks)
		{
			track.name = "mixamorig"+track.name;
		}
	}
}

let map_filenames = {
	gold: "CyberbotGold.glb",
	green: "CyberbotGreen.glb",
	pink: "CyberbotPink.glb",
	red: "CyberbotRed.glb",
	silver: "CyberbotSilver.glb",
	yellow: "CyberbotYellow.glb"
}


export async function load(doc)
{
	let UI =  document.createElement("div");
	UI.style.cssText = "position:absolute; top:20px; left:60%; width:30%; height:15%; background:rgba(126, 126, 126, 0.5); border-radius:5%;";
	document.body.appendChild(UI);
	
	let GlobalStatusBar =  document.createElement("div");
	GlobalStatusBar.style.cssText = "position:relative;";
	UI.appendChild(GlobalStatusBar);
	
	let StatusBar =  document.createElement("div");
	StatusBar.style.cssText = "position:relative;";
	UI.appendChild(StatusBar);
	
	let ActionBar =  document.createElement("div");
	ActionBar.style.cssText = "position:relative;";
	UI.appendChild(ActionBar);
	
	ActionBar.innerHTML = "Go to Maze_";
	
	let input = document.createElement("input");
	input.style.cssText = "position:relative; width:50px;";
	ActionBar.appendChild(input);
	
	let button = document.createElement("input");
	button.style.cssText = "position:relative;";
	button.type = "button";
	button.value = "Go!";
	ActionBar.appendChild(button);
	
	let num_mazes = 1;
	let current_maze_id = 0;
	
	let socket = io();
	socket.emit('god', "");
	
	const users = {};
	
	const reset_users = ()=>
	{
		users.gold = null;
		users.green = null;
		users.pink = null;
		users.red = null;
		users.silver = null;
		users.yellow = null;
	};
	
	
	socket.on('num_mazes', (msg)=>{
		num_mazes = parseInt(msg);
		GlobalStatusBar.innerHTML = `There are ${num_mazes} mazes in total.`;
	});
	
	const view_maze = (maze_id)=>
	{
		socket.emit('view', maze_id.toString());
	};
	view_maze(current_maze_id);
	
	button.addEventListener('click', ()=>{
		let id = parseInt(input.value)-1;
		if (id<0) id= 0;
		else if (id>=num_mazes) id = num_mazes-1;
		view_maze(id);
	});
	
	socket.on('identity', async (identity) =>{
		reset_users();
		doc.reset();
		doc.load_xml("maze.xml");
		let maze = await doc.create("model", {src: `assets/models/${identity.maze}`});
		let balloon = await doc.create("model", {src: "assets/models/balloon.glb",  scale: "0.01,0.01,0.01", position: "30,1,-30"});
		current_maze_id = identity.maze_id;
	});
	
	const update_user_info = (user, info) =>{
		if (info.hasOwnProperty('state'))
		{
			if (user.state!=info.state)
			{
				doc.Tags.character.set_state(doc, user, info.state);
			}
		}
		if (info.hasOwnProperty('position'))
		{
			user.position.set(info.position.x, info.position.y, info.position.z);
		}
		if (info.hasOwnProperty('quaternion'))
		{
			user.quaternion.set(info.quaternion._x, info.quaternion._y, info.quaternion._z, info.quaternion._w);
		}
	};
	
	socket.on('full status', async (msg) =>{
		let count = 0;
		const info_users = JSON.parse(msg);
		for (let _user_id in users)
		{
			const user = users[_user_id];
			const info = info_users[_user_id];
			if (info!=null) count++;
			if (user!=null && info == null)
			{
				doc.remove(user);
				users[_user_id]=null;
			}
			else if (user!=null && info!=null)
			{
				update_user_info(user, info);
			}
			else if (user==null && info!=null)
			{					
				let fn_avatar = map_filenames[info.id];
				let pos = info.position;
				const options = {};
				options.src = `assets/models/${fn_avatar}`;
				options.fix_anims = "fix_anims";
				options.position = `${pos.x},${pos.y},${pos.z}`;
				options.rotation = "0,135,0";
				options.url_anim = "assets/models/Animations.glb";
				options.name_idle = "idle";
				options.name_forward = "walk_forward";
				options.name_backward = "walk_backward";				
				const user = await doc.create("character", options);
				users[_user_id] = user;
				update_user_info(user, info);
			}
		}
		
		StatusBar.innerHTML = `Your are viewing Maze_${current_maze_id+1} as God! There are ${count} player(s) in the maze.`;
	});
	
	socket.on('avatar status', (msg) =>{
		const info = JSON.parse(msg);
		let user = users[info.id];
		if (user!=null)
		{
			update_user_info(user, info);
		}
	});
	
	

}
