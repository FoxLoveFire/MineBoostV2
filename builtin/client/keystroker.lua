local screenW = minetest.settings:get("screen_width") or 1920
local screenH = minetest.settings:get("screen_height") or 1080
local hudpos = {x = 0.925, y = 0.725}

local rmb_sprite = core.create_sprite({x = 0.897, y = 0.931, width = 118, height = 16})
local lmb_sprite = core.create_sprite({x = 0.897, y = 0.9299, width = 118, height = 16})

local version = ""
local huds = {}
local keys = {"up", "left", "down", "right", "jump", "aux1", "sneak"}
local before = {}

-- Конфигурации для разных версий
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

local image_press_54 = {up = "w_key_press.png", left = "a_key_press.png", down = "s_key_press.png", right = "d_key_press.png", jump = "space_key_press.png", place = "rmb_key_press.png", dig = "lmb_key_press.png", aux1 = "e_key_press.png", sneak = "shift_key_press.png"}
local image_press_pre54 = {up = "w_key_press.png", left = "a_key_press.png", down = "s_key_press.png", right = "d_key_press.png", jump = "space_key_press.png", RMB = "rmb_key_press.png", LMB = "lmb_key_press.png", aux1 = "e_key_press.png", sneak = "shift_key_press.png"}

local image_normal_54 = {up = "w_key.png", left = "a_key.png", down = "s_key.png", right = "d_key.png", jump = "space_key.png", place = "rmb_key.png", dig = "lmb_key.png", aux1 = "e_key.png", sneak = "shift_key.png"}
local image_normal_pre54 = {up = "w_key.png", left = "a_key.png", down = "s_key.png", right = "d_key.png", jump = "space_key.png", RMB = "rmb_key.png", LMB = "lmb_key.png", aux1 = "e_key.png", sneak = "shift_key.png"}

local huddefs, image_press, image_normal
local rmbcps, lmbcps
local rmbclicks, lmbclicks = 0, 0
local rmbpress, lmbpress = false, false
local rmbtimer, lmbtimer = 0, 0
local huds_initialized = false

-- Определение версии
local function get_version()
    local player = minetest.localplayer
    if player and player:get_control().place == nil then
        version = "pre5.4"
        table.insert(keys, "RMB")
        table.insert(keys, "LMB")
        huddefs = huddefs_pre54
        image_press = image_press_pre54
        image_normal = image_normal_pre54
    else
        version = "5.4"
        table.insert(keys, "place")
        table.insert(keys, "dig")
        huddefs = huddefs_54
        image_press = image_press_54
        image_normal = image_normal_54
    end
end

local function initialize_huds()
    if not minetest.localplayer then return end
    
    get_version()

    for _, key in ipairs(keys) do
        if not huds[key] then
            huds[key] = minetest.localplayer:hud_add(huddefs[key])
        end
    end
    
    huds_initialized = true
end

local function remove_huds()
    if not minetest.localplayer then return end
    
    for key, hud_id in pairs(huds) do
        minetest.localplayer:hud_remove(hud_id)
        huds[key] = nil
    end
    

    before = {}
    huds_initialized = false
end

local function update_hud_positions()
    if not minetest.localplayer then return end

    local keys_x_n = minetest.settings:get("keys_x") or 0.925
    local keys_y_n = minetest.settings:get("keys_y") or 0.725

    local fixed_y_offset = 0.02

    for _, key in ipairs(keys) do
        if huds[key] then
            minetest.localplayer:hud_change(huds[key], "position", {x = keys_x_n / screenW + 0.034, y = keys_y_n / screenH - fixed_y_offset})
        end
    end
end

local function track_rmb_clicks(dtime)
    if not minetest.localplayer then return end
    
    local ctl = minetest.localplayer:get_control()
    local current_rmb = (version == "pre5.4") and ctl.RMB or ctl.place
    
    if current_rmb and not rmbpress then
        rmbclicks = rmbclicks + 1
    end
    rmbpress = current_rmb

    if rmbcps then
        minetest.localplayer:hud_change(rmbcps, "text", "RMB CPS: " .. rmbclicks)
    end

    rmbtimer = rmbtimer + dtime
    if rmbtimer >= 1 then
        rmbclicks = 0
        rmbtimer = 0
    end
end

local function track_lmb_clicks(dtime)
    if not minetest.localplayer then return end
    
    local ctl = minetest.localplayer:get_control()
    local current_lmb = (version == "pre5.4") and ctl.LMB or ctl.dig
    
    if current_lmb and not lmbpress then
        lmbclicks = lmbclicks + 1
    end
    lmbpress = current_lmb

    if lmbcps then
        minetest.localplayer:hud_change(lmbcps, "text", "LMB CPS: " .. lmbclicks)
    end

    lmbtimer = lmbtimer + dtime
    if lmbtimer >= 1 then
        lmbclicks = 0
        lmbtimer = 0
    end
end

local function update_key_states()
    if not minetest.localplayer then return end
    
    if minetest.settings:get_bool("show_keys") then
        local player = minetest.localplayer
        local controls = player:get_control()
        
        if not huds_initialized then
            initialize_huds()
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
        if huds_initialized then
            remove_huds()
        end
    end
end

-- Инициализация
minetest.after(0, function()
    if not minetest.localplayer then
        minetest.after(0, initialize_huds)
        return
    end
    if minetest.settings:get_bool("show_keys") then
        initialize_huds()
    end
end)

minetest.register_globalstep(function(dtime)
    if not minetest.localplayer then return end
    
    local show_keys = minetest.settings:get_bool("show_keys")
    
    if show_keys then
        update_hud_positions()
        track_rmb_clicks(dtime)
        track_lmb_clicks(dtime)
        update_key_states()
    else
        if huds_initialized then
            remove_huds()
        end
    end
end)

--[[
    GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    Made by Minetest-j45 -> https://github.com/Minetest-j45
]]--