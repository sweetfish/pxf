
pxf.CoreVersion = "0.1.0"

function pxf:CoreInit()
  pxf:init_console(15, 1000)

  -- Debug output
  pxf:add_console("LuaGame - Version ^4" .. self.CoreVersion)
  pxf:add_console("Instance: ^4" .. tostring(self.Instance))

  -- Test preload
  debug_font_texture = pxf.resources.loadtexture("debug_font.png")
end

-- Debugging console
function pxf:init_console(max_lines, cut_off_width)
  self.console = {buffer = {},
                  max_lines = max_lines,
                  current_input = 0,
                  cut_off_width = cut_off_width,
                  visible = false}
end

function d(str)
  pxf:add_console(str)
end

function pxf:add_console(str, skip_stdout)

  -- print to normal console
  if (skip_stdout == nil) then
    print(str)
  end

  -- move buffer up
  if self.console.current_input == self.console.max_lines then
    for i=1,(self.console.current_input - 1) do
      self.console.buffer[i] = self.console.buffer[i+1]
    end
  end

  -- add new str
  self.console.current_input = self.console.current_input + 1
  if self.console.current_input > self.console.max_lines then
    self.console.current_input = self.console.max_lines
  end

  -- Add up until max width
  line_w = #str*8 + 8
  if (line_w > self.console.cut_off_width) then
    cut_place = math.ceil((self.console.cut_off_width / line_w) * #str)
    s = string.sub(str, 1, cut_place)
    e = string.sub(str, cut_place + 1)
    self.console.buffer[self.console.current_input] = s
    pxf:add_console("^4 " .. e, true)
  else
    self.console.buffer[self.console.current_input] = str
  end
end

function pxf:console_taptest(x, y)

  --[[if (self.console.visible) then
    if (x > self.console.cut_off_width) then
      return false
    elseif (y > self.console.max_lines * 10 + 6) then
      return false
    end
  else
    if (x > self.console.cut_off_width) then
      return false
    elseif (y > 10) then
      return false
    end
  end]]

  if (y > 15) then
    return false
  end

  self.console.visible = not self.console.visible

  return true
end

function pxf:draw_console(screenw, screenh)
  --screenw, screenh = pxf.graphics.getscreensize()
  console_h = 10 * self.console.max_lines + 6

  if (not self.console.visible) then
    --[[counttext = "[" .. tostring(self.console.current_input) .. "," .. tostring(self.console.max_lines) .. "]"
    pxf:draw_font(counttext, screenw - #counttext*8, y)

    return]]
    pxf.graphics.setalpha(0.1)
  else

    -- bg
    pxf.graphics.setalpha(0.8)
    pxf.graphics.setcolor(0, 0, 0)
    pxf.graphics.drawquad(screenw / 2, (console_h / 2), screenw, console_h, 1, 1, 1, 1)
  end

  -- text
  x = 8
  --y = 26
  y = 8
  for i=1,(self.console.current_input) do
    pxf:draw_font(self.console.buffer[i], x, y+((i-1) * 10))
  end

  -- draw line count
  counttext = "[" .. tostring(self.console.current_input) .. "," .. tostring(self.console.max_lines) .. "]"
  pxf:draw_font(counttext, screenw - #counttext*8, y)
end

-- Font/text (using charmap) rendering
function pxf:draw_font(str, x, y)

  debug_font_texture:bind()
  pxf.graphics.setcolor(1, 1, 1) -- TODO: Should use some kind of getcolor before drawing, so it can be restored here
  pxf.graphics.translate(x, y)
	strlen = #str
	char_w = 8

	color_indicator = "^"
	change_color = false
	char_counter = 0

	for i=1,strlen do
	  -- calculate tex coords
	  index = string.byte(str, i)

	  s = math.fmod(index, 16) * 16
	  t = math.floor(index / 16) * 16

	  -- debug
	  --print(string.char(tostring(string.byte(str, i))) .. " -> " .. tostring(string.byte(str, i)))

	  -- change color?
	  if index == string.byte(color_indicator, 1) then
	    change_color = true
    else
      if change_color then

        -- Color indexes
        if string.char(tostring(string.byte(str, i))) == "0" then
          pxf.graphics.setcolor(0, 0, 0)
        elseif string.char(tostring(string.byte(str, i))) == "1" then
          pxf.graphics.setcolor(1, 0, 0)
        elseif string.char(tostring(string.byte(str, i))) == "2" then
          pxf.graphics.setcolor(0, 1, 0)
        elseif string.char(tostring(string.byte(str, i))) == "3" then
          pxf.graphics.setcolor(0, 0, 1)
        elseif string.char(tostring(string.byte(str, i))) == "4" then
          pxf.graphics.setcolor(1.0, 0.3, 0.3)
        else
          pxf.graphics.setcolor(1, 1, 1)
        end

        change_color = false
      else
  	    -- draw quad
  	    pxf.graphics.drawquad((char_counter)*char_w, 0, 16, 16, s, t, 16, 16)
  	    char_counter = char_counter + 1
	    end
    end
	end

	pxf.graphics.translate(-x, -y)
	pxf.graphics.setcolor(1, 1, 1) -- TODO: Should use some kind of getcolor before drawing, so it can be restored here
end

function pxf.graphics:newsprite(tex,cw,ch,freq)
	new_sprite = {}
	new_sprite.position = { x = 0, y = 0 }

	texture = tex
	cellwidth = cw
	cellheight = ch
	current_cell = 0

	tex_w,tex_h = texture:getsize()
	max_cellsw = tex_w / cw
	max_cellsh = tex_h / ch
	max_cells = max_cellsw * max_cellsh

	--print(max_cells)

	x0 = cellwidth * (current_cell % max_cellsw)
	y0 = cellheight * (current_cell / max_cellsh)

	--print(current_cell % max_cellsw)
	--print(current_cell / max_cellsh)

	function new_sprite:draw()
		texture:bind()

		pxf.graphics.drawquad(50, 150, cellwidth, cellheight, x0, y0, x0 + cellwidth, y0 + cellheight)
	end

	function new_sprite:update(dt)

	end

	return new_sprite
end
