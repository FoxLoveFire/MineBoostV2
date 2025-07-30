local screenW = minetest.settings:get("screen_width") or 1920
local screenH = minetest.settings:get("screen_height") or 1080 
local hudpos = {x = 0.925, y = 0.725}

local rmb_sprite = core.create_sprite({x = 0.897, y = 0.931, width = 118, height = 16})
local lmb_sprite = core.create_sprite({x = 0.897, y = 0.9299, width = 118, height = 16})
local huds = {}

local keys = {"up", "left", "down", "right", "jump", "aux1", "sneak"}

local huddefs

local huddefs_54 = {
  up = {hud_elem_type = "image", position = hudpos, offset = {x = 0, y = 0}, text = "w_key.png", alignment = {x = 1, y = 1}, scale = {x = 2, y = 2}, number = 0xFFFFFF},
  left = {hud_elem_type = "image", position = hudpos, offset = {x = -33, y = 33}, text = "a_key.png", alignment = {x = 1, y = 1}, scale = {x = 2, y = 2}, number = 0xFFFFFF},
  down = {hud_elem_type = "image", position = hudpos, offset = {x = 0, y = 33}, text = "s_key.png", alignment = {x = 1, y = 1}, scale = {x = 2, y = 2}, number = 0xFFFFFF},
  right = {hud_elem_type = "image", position = hudpos, offset = {x = 33, y = 33}, text = "d_key.png", alignment = {x = 1, y = 1}, scale = {x = 2, y = 2}, number = 0xFFFFFF},
  jump = {hud_elem_type = "image", position = hudpos, offset = {x = -33, y = 99}, text = "space_key.png", alignment = {x = 1, y = 1}, scale = {x = 2, y = 2}, number = 0xFFFFFF},
  place = {hud_elem_type = "image", position = hudpos, offset = {x = 17, y = 66}, text = "rmb_key.png", alignment = {x = 1, y = 1}, scale = {x = 2, y = 2}, number = 0xFFFFFF},
  dig = {hud_elem_type = "image", position = hudpos, offset = {x = -33, y = 66}, text = "lmb_key.png", alignment = {x = 1, y = 1}, scale = {x = 2, y = 2}, number = 0xFFFFFF},
  aux1 = {hud_elem_type = "image", position = hudpos, offset = {x = 33, y = 0}, text = "e_key.png", alignment = {x = 1, y = 1}, scale = {x = 2, y = 2}, number = 0xFFFFFF},
  sneak = {hud_elem_type = "image", position = hudpos, offset = {x = -66, y = 33}, text = "shift_key.png", alignment = {x = 1, y = 1}, scale = {x = 2, y = 2}, number = 0xFFFFFF},
}
local huddefs_pre54 = {
  up = {hud_elem_type = "image", position = hudpos, offset = {x = 0, y = 0}, text = "w_key.png", alignment = {x = 1, y = 1}, scale = {x = 2, y = 2}, number = 0xFFFFFF},
  left = {hud_elem_type = "image", position = hudpos, offset = {x = -33, y = 33}, text = "a_key.png", alignment = {x = 1, y = 1}, scale = {x = 2, y = 2}, number = 0xFFFFFF},
  down = {hud_elem_type = "image", position = hudpos, offset = {x = 0, y = 33}, text = "s_key.png", alignment = {x = 1, y = 1}, scale = {x = 2, y = 2}, number = 0xFFFFFF},
  right = {hud_elem_type = "image", position = hudpos, offset = {x = 33, y = 33}, text = "d_key.png", alignment = {x = 1, y = 1}, scale = {x = 2, y = 2}, number = 0xFFFFFF},
  jump = {hud_elem_type = "image", position = hudpos, offset = {x = -33, y = 99}, text = "space_key.png", alignment = {x = 1, y = 1}, scale = {x = 2, y = 2}, number = 0xFFFFFF},
  RMB = {hud_elem_type = "image", position = hudpos, offset = {x = 17, y = 66}, text = "rmb_key.png", alignment = {x = 1, y = 1}, scale = {x = 2, y = 2}, number = 0xFFFFFF},
  LMB = {hud_elem_type = "image", position = hudpos, offset = {x = -33, y = 66}, text = "lmb_key.png", alignment = {x = 1, y = 1}, scale = {x = 2, y = 2}, number = 0xFFFFFF},
  aux1 = {hud_elem_type = "image", position = hudpos, offset = {x = 33, y = 0}, text = "e_key.png", alignment = {x = 1, y = 1}, scale = {x = 2, y = 2}, number = 0xFFFFFF},
  sneak = {hud_elem_type = "image", position = hudpos, offset = {x = -66, y = 33}, text = "shift_key.png", alignment = {x = 1, y = 1}, scale = {x = 2, y = 2}, number = 0xFFFFFF},
}
local image_press
local image_press_54 = {up = "w_key_press.png", left = "a_key_press.png", down = "s_key_press.png", right = "d_key_press.png", jump = "space_key_press.png", place = "rmb_key_press.png", dig = "lmb_key_press.png", aux1 = "e_key_press.png", sneak = "shift_key_press.png"}
local image_press_pre54 = {up = "w_key_press.png", left = "a_key_press.png", down = "s_key_press.png", right = "d_key_press.png", jump = "space_key_press.png", RMB = "rmb_key_press.png", LMB = "lmb_key_press.png", aux1 = "e_key_press.png", sneak = "shift_key_press.png"}
local image_normal
local image_normal_54 = {up = "w_key.png", left = "a_key.png", down = "s_key.png", right = "d_key.png", jump = "space_key.png", place = "rmb_key.png", dig = "lmb_key.png", aux1 = "e_key.png", sneak = "shift_key.png"}
local image_normal_pre54 = {up = "w_key.png", left = "a_key.png", down = "s_key.png", right = "d_key.png", jump = "space_key.png", RMB = "rmb_key.png", LMB = "lmb_key.png", aux1 = "e_key.png", sneak = "shift_key.png"}

local before = {}
local version = ""
local function getversion()
    local player = minetest.localplayer
    if player:get_control().place == nil then
        version = "pre5.4"
    else
        version = "5.4"
    end
end

local function check()
    if not minetest.localplayer then
        minetest.after(0, check)
        return
    end
    getversion()
    if version == "pre5.4" then
        table.insert(keys, "RMB")
        table.insert(keys, "LMB")
        huddefs = huddefs_pre54
        image_press = image_press_pre54
        image_normal = image_normal_pre54
    else
        table.insert(keys, "place")
        table.insert(keys, "dig")
        huddefs = huddefs_54
        image_press = image_press_54
        image_normal = image_normal_54
    end

    if not rmbcps then
        rmbcps = minetest.localplayer:hud_add({hud_elem_type = "text", position = hudpos, offset = {x = -33, y = 159}, text = "RMB CPS: 0", alignment = {x = 1, y = 1}, scale = {x = 1, y = 1}, number = 0xFFFFFF})
    end
    if not lmbcps then
        lmbcps = minetest.localplayer:hud_add({hud_elem_type = "text", position = hudpos, offset = {x = -33, y = 145}, text = "LMB CPS: 0", alignment = {x = 1, y = 1}, scale = {x = 1, y = 1}, number = 0xFFFFFF})
    end

    huds = {}
    for _, key in ipairs(keys) do
        if not huds[key] then
            huds[key] = minetest.localplayer:hud_add(huddefs[key])
        end
    end
end

minetest.after(0, check)

local timer = 0
minetest.register_globalstep(function(dtime)
    if not minetest.settings:get_bool("show_keys") then return end
    if not minetest.localplayer then return end

    local keys_x_n = minetest.settings:get("keys_x") or 0.925
    local keys_y_n = minetest.settings:get("keys_y") or 0.725
    for _, key in ipairs(keys) do
        if huds[key] then
            minetest.localplayer:hud_change(huds[key], "position", {x = keys_x_n / screenW + 0.034, y = keys_y_n / screenH + 0.02})
        end
    end

    local x_r, y_r = core.get_position({id = rmb_sprite})
    local x_l, y_l = core.get_position({id = lmb_sprite})

    if core.is_moved(rmb_sprite) then
        minetest.localplayer:hud_change(rmbcps, "position", {x = x_r, y = y_r})
        minetest.localplayer:hud_change(rmbcps, "offset", {x = 0, y = 0})
    end

    if core.is_moved(lmb_sprite) then 
        minetest.localplayer:hud_change(lmbcps, "position", {x = x_l, y = y_l})
        minetest.localplayer:hud_change(lmbcps, "offset", {x = 0, y = 0})
    end

end)

local rmbclicks = 0
local rmbpress = false
local rmbtimer = 0
local function update_hud_positions()
    if not minetest.localplayer then return end

    local keys_x_n = minetest.settings:get("keys_x") or 0.925
    local keys_y_n = minetest.settings:get("keys_y") or 0.725

    for _, key in ipairs(keys) do
        if huds[key] then
            minetest.localplayer:hud_change(huds[key], "position", {x = keys_x_n / screenW + 0.034, y = keys_y_n / screenH + 0.02})
        end
    end

    if rmbcps then
        local x_r, y_r = core.get_position({id = rmb_sprite})
        minetest.localplayer:hud_change(rmbcps, "position", {x = x_r, y = y_r})
    end

    if lmbcps then
        local x_l, y_l = core.get_position({id = lmb_sprite})
        minetest.localplayer:hud_change(lmbcps, "position", {x = x_l, y = y_l})
    end
end

minetest.register_globalstep(function(dtime)
    if not minetest.settings:get_bool("show_keys") then return end
    update_hud_positions()
end)

minetest.register_globalstep(function(dtime)
    if not minetest.settings:get_bool("show_keys") then return end
    if not minetest.localplayer then return end
    local ctl = minetest.localplayer:get_control()
    if version == "pre5.4" then
        if ctl.RMB and not rmbpress then
            rmbclicks = rmbclicks + 1
        end
        rmbpress = ctl.RMB
    else
        if ctl.place and not rmbpress then
            rmbclicks = rmbclicks + 1
        end
        rmbpress = ctl.place
    end

    if rmbcps then
        minetest.localplayer:hud_change(rmbcps, "text", "RMB CPS: " .. rmbclicks)
    end

    rmbtimer = rmbtimer + dtime
    if rmbtimer >= 1 then
        rmbclicks = 0
        rmbtimer = 0
    end
end)

local lmbclicks = 0
local lmbpress = false
local lmbtimer = 0

minetest.register_globalstep(function(dtime)
    if not minetest.settings:get_bool("show_keys") then return end
    if not minetest.localplayer then return end
    local ctl = minetest.localplayer:get_control()
    if version == "pre5.4" then
        if ctl.LMB and not lmbpress then
            lmbclicks = lmbclicks + 1
        end
        lmbpress = ctl.LMB
    else
        if ctl.dig and not lmbpress then
            lmbclicks = lmbclicks + 1
        end
        lmbpress = ctl.dig
    end

    if lmbcps then
        minetest.localplayer:hud_change(lmbcps, "text", "LMB CPS: " .. lmbclicks)
    end

    lmbtimer = lmbtimer + dtime
    if lmbtimer >= 1 then
        lmbclicks = 0
        lmbtimer = 0
    end
end)

minetest.register_globalstep(function()
    if minetest.localplayer then
        if minetest.settings:get_bool("show_keys") then
            local player = minetest.localplayer
            local controls = player:get_control()
            if next(huds) == nil then  
                for _, key in ipairs(keys) do
                    huds[key] = player:hud_add(huddefs[key])
                end
            end
            for _, key in ipairs(keys) do
                if controls[key] and not before[key] then
                    if huds[key] then
                        player:hud_change(huds[key], "text", image_press[key])
                    end
                    before[key] = true
                elseif not controls[key] and before[key] then
                    if huds[key] then
                        player:hud_change(huds[key], "text", image_normal[key])
                    end
                    before[key] = false
                end
            end
        else
            for _, hud_id in pairs(huds) do
                minetest.localplayer:hud_remove(hud_id)
                huds[hud_id] = nil
            end
            minetest.localplayer:hud_change(rmbcps, "text", "")
            minetest.localplayer:hud_change(lmbcps, "text", "")
            huds = {}
            before = {}
        end
    end
end)
--[[
    GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007

    Made by Minetest-j45 -> https://github.com/Minetest-j45
]]--