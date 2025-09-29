local scriptpath = core.get_builtin_path()
local clientpath = scriptpath.."client"..DIR_DELIM
local commonpath = scriptpath.."common"..DIR_DELIM

dofile(clientpath .. "register.lua")
dofile(commonpath .. "after.lua")
dofile(commonpath .. "mod_storage.lua")
dofile(commonpath .. "chatcommands.lua")
dofile(commonpath .. "information_formspecs.lua")
dofile(clientpath .. "chatcommands.lua")
dofile(clientpath .. "misc.lua")
dofile(clientpath .. "keystroker.lua")
assert(loadfile(commonpath .. "item_s.lua"))({}) -- Just for push/read node functions
function register_hits(pos, dir, texture, raycast, amount)
	if not minetest.localplayer then
		return false
	end
    if minetest.localplayer:get_control().dig and not lmbpress then
        lmbpress = true
        for pointed_thing in raycast do
            local new_pos = vector.add(pointed_thing.intersection_point, vector.new(-0.2, -0.4, -0.2))
            local other_pos = vector.add(pointed_thing.intersection_point, vector.new(0.2, 0.4, 0.2))
            if pointed_thing.type == "object" then
                minetest.add_particlespawner({
                    amount = amount,
                    time = 0.1,
                    minpos = new_pos, 
                    maxpos = other_pos,
                    minvel = {x = -1, y = -2, z = -1},
                    maxvel = {x = 1, y = 2, z = 1},
                    minacc = {x = -0.1, y = -0.1, z = -0.1},
                    maxacc = {x = 0.1, y = 0.1, z = 0.1},
                    minexptime = 0.5,
                    maxexptime = 0.7,
                    minsize = 1,
                    maxsize = 2,
                    collisiondetection = true,
                    collision_removal = false,
                    object_collision = false,
                    vertical = false,
                    texture = texture
                })
                break
            end
        end
    elseif not minetest.localplayer:get_control().dig then
        lmbpress = false
    end
end

core.register_globalstep(function(dtime)
	if core.camera then
		local pos = core.camera:get_pos()
		local dir = core.camera:get_look_dir()
		local raycast = core.raycast(pos, vector.add(pos, vector.multiply(dir, 4)), true, false)
		local texture = "hit.png"
		local amount = 120
		if core.settings:get_bool("particles") then
			register_hits(pos, dir, texture, raycast, amount)
		end
	end
end)