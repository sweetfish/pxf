require("faker")

knugen = {}

require("knugen/card")

function pxf:Init()
	self.GameIdent = "Knugen"
	self.GameVersion = "1.0"

	pxf:add_console("GameIdent: ^4" .. self.GameIdent)
	pxf:add_console("GameVersion: ^4" .. self.GameVersion)

	screenw, screenh = pxf.graphics.getscreensize()
	pxf:add_console("Screen size: ^4" .. tostring(screenw) .. "x" .. tostring(screenh))

	self.GameSeed = 4546
	pxf:add_console("Game seed: ^4" .. self.GameSeed)
	initial_deal = knugen:gen_eight_stacks(self.GameSeed)

	decks = {knugen:new_tablau_deck(40, 142, initial_deal[1]),
	         knugen:new_tablau_deck(40 + 57,   142, initial_deal[2]),
	         knugen:new_tablau_deck(98 + 57,   142, initial_deal[3]),
	         knugen:new_tablau_deck(98 + 57*2, 142, initial_deal[4]),
	         knugen:new_tablau_deck(98 + 57*3, 142, initial_deal[5]),
	         knugen:new_tablau_deck(98 + 57*4, 142, initial_deal[6]),
	         knugen:new_tablau_deck(98 + 57*5, 142, initial_deal[7]),
	         knugen:new_tablau_deck(98 + 57*6, 142, initial_deal[8]),

	         knugen:new_foundation_deck(275,        54),
	         knugen:new_foundation_deck(275 + 57,   54),
	         knugen:new_foundation_deck(275 + 57*2, 54),
	         knugen:new_foundation_deck(275 + 57*3, 54),

	         knugen:new_cell_deck(32, 54),
	         knugen:new_cell_deck(32 + 57, 54),
	         knugen:new_cell_deck(32 + 57*2, 54),
	         knugen:new_cell_deck(32 + 57*3, 54),}

	draging_deck_info = nil

	last_mouse_drag = {drag = false, x1 = 0, y1 = 0, x2 = 0, y2 = 0, dx = 0, dy = 0}
end

function pxf:PreLoad()
	texture_map01 = pxf.resources.loadtexture("knugen_map01.png")
end

function pxf:Update(dt)
	--print("Time to UPDATE our game with '" .. tostring(dt) .. "'")
end

function pxf:Render()

	-- render background
	texture_map01:bind()
	pxf.graphics.translate(screenw / 2.0, screenh / 2.0)
	pxf.graphics.rotate(math.pi / 2)
	pxf.graphics.drawquad(0, 0,screenh,screenw, 0, 0, 1024, 683)
	--pxf.graphics.drawquad(screenw / 2.0, screenh / 2.0,screenw,screenh)

	pxf.graphics.translate(-screenh / 2.0, -screenw / 2.0)


  tx1 = last_mouse_drag.x1
  ty1 = last_mouse_drag.y1
  tx2 = last_mouse_drag.x2
  ty2 = last_mouse_drag.y2

	if (last_mouse_drag.drag) then
	  -- update current drag
	  if (draging_deck_info) then
	    draging_deck_info.new_deck.x = tx2
	    draging_deck_info.new_deck.y = ty2
    else
      -- start new drag
  	  for i=1,#decks do
        if decks[i]:hit_test(tx1, ty1) then
          local drag_deck = decks[i]:start_drag(ty1)
          if (drag_deck) then
            draging_deck_info = {from_deck = i, new_deck = knugen:new_deck(tx2,ty2, drag_deck)}
          end
          break
        end
  	  end
    end
  end

  for i=1,#decks do
    decks[i]:draw()
  end

  if (draging_deck_info) then
    pxf.graphics.setalpha(0.8)
    draging_deck_info.new_deck:draw()
  end

	-- render console
	pxf.graphics.loadidentity()
  pxf.graphics.translate(screenw / 2.0, screenh / 2.0)
  pxf.graphics.rotate(math.pi / 2)
  pxf.graphics.translate(-screenh / 2.0, -screenw / 2.0)
  pxf.console.cut_off_width = screenh
  pxf:draw_console(screenh, screenw)


	--pxf:add_console("Rendering frame: ^4" .. tostring(simple_framecount))

	-- make it crash:
  --pxf.graphics.drawquad(nil)

  -- reset mouse drag
  --last_mouse_drag.drag = false
end


function pxf:TextInput(str)
  pxf:add_console("> " .. str)
  ret = runstring("return " .. str)
  pxf:add_console("^4" .. tostring(ret))
end

function pxf:EventTap(x, y)
  pxf:add_console("Tap event, ^4x: " .. tostring(x) .. " y: " .. tostring(y))
  pxf:console_taptest(x, y)
end

function pxf:EventDoubleTap(x, y)
  pxf:add_console("Double tap event, ^4x: " .. tostring(x) .. " y: " .. tostring(y))
end

function pxf:EventDrag(x1, y1, x2, y2)
  last_mouse_drag.x1 = x1
  last_mouse_drag.y1 = y1
  last_mouse_drag.x2 = x2
  last_mouse_drag.y2 = y2
  last_mouse_drag.dx = x1-x2
  last_mouse_drag.dy = y1-y2
  last_mouse_drag.drag = true
  --pxf:add_console("Drag event, ^4(" .. tostring(x1) .. ", " .. tostring(y1) .. ") -> (" .. tostring(x2) .. ", " .. tostring(y2) .. "), delta: (" .. tostring(x1-x2) .. ", " .. tostring(y1-y2) .. ")")
end

function pxf:EventRelease(x, y)
  if (last_mouse_drag.drag and draging_deck_info) then
    last_mouse_drag.drag = false

    local ret = false
	  for i=1,#decks do
      if decks[i]:hit_test(tx1, ty1) then
        --draging_deck_info = {from_deck = i, new_deck = knugen:new_deck(tx2,ty2, decks[i]:start_drag(1))}
        ret = decks[i]:try_place(draging_deck_info.new_deck.cards)
        break
      end
	  end

	  decks[draging_deck_info.from_deck]:drag_finished(ret)
	  draging_deck_info = nil
  end

  --pxf:add_console("Release event, ^4x: " .. tostring(x) .. " y: " .. tostring(y))
end



