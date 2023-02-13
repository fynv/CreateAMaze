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
	let start_maze_id = localStorage.getItem('current_maze') || 0;
	
	let socket = io();
	socket.emit('player', start_maze_id.toString());
	
	let avatar = null;
	let self = null;
	
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
	
	const init = async (identity) =>{
		self = identity;
		localStorage.setItem('current_maze', self.maze_id);
		reset_users();
		doc.reset();
		doc.load_xml("maze.xml");
		let maze = await doc.create("model", {src: `assets/models/${self.maze}`, is_building: "True"});
		let balloon = await doc.create("model", {src: "assets/models/balloon.glb",  scale: "0.01,0.01,0.01", position: "30,1,-30"});		
		let fn_avatar = map_filenames[self.id];
		let pos = self.position;
		avatar = await doc.create("avatar", { 
			src:`assets/models/${fn_avatar}`, 
			name_idle:"idle", 
			name_forward:"walk_forward", 
			name_backward:"walk_backward", 
			camera_position:"0.0, 3.0, -4.0", 
			lookat_height:"2.0",
			url_anim: "assets/models/Animations.glb",
			fix_anims: "fix_anims",
			position: `${pos.x},${pos.y},${pos.z}`,
			rotation: "0, 135,0"
			}
		);
		
		const update_avatar = (doc, avatar, delta) => {
			if (self.state!='idle' || avatar.state!='idle')
			{
				self.state = avatar.state;
				self.position = avatar.position;
				self.quaternion = avatar.quaternion;        
				socket.emit('avatar status', JSON.stringify(self));        
			}
		};
		
		doc.set_tick(avatar, update_avatar);   
	};
	socket.on('identity', init);	
	
	
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
		const info_users = JSON.parse(msg);
		for (let _user_id in users)
		{
			const user = users[_user_id];
			const info = info_users[_user_id];
			if (user!=null && info == null)
			{
				doc.remove(user);
				users[_user_id]=null;
			}
			else if (user!=null && info!=null)
			{
				update_user_info(user, info);
			}
			else if (user==null && info!=null && _user_id!=self.id)
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
	});
	
	socket.on('avatar status', (msg) =>{
		const info = JSON.parse(msg);
		console.log(info);
		let user = users[info.id];
		if (user!=null)
		{
			update_user_info(user, info);
		}
	});
	
}
