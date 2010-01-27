-- functions that should be moved to a helper script?

function AddQuad(widget, quad, coords)
	_AddQuad(widget, quad[1], quad[2], quad[3], quad[4], coords[1], coords[2], coords[3], coords[4])
end

function AddWidget(name, hitbox, states, activestate)
	widget = _AddWidget(name, hitbox[1], hitbox[2], hitbox[3], hitbox[4])
	
	
	for i, w in pairs(states) do
		_AddState(widget, i)
	end
	
	_SetState(widget, activestate)
	_SetPosition(widget, hitbox[1], hitbox[2])
	
	return widget
end

all_widgets = {}

function GUIWidgets(widgets)
	for i, w in pairs(widgets) do
		widget = {instance = AddWidget(w.name, w.hitbox, w.states, w.activestate),
		          states = w.states}
		
		function widget.IsMouseOver(self)
			return _IsMouseOver(self.instance)
		end
		
		function widget.IsClicked(self)
			return _IsClicked(self.instance)
		end
		
		--all_widgets[#all_widgets+1] = widget
		all_widgets[w.name] = widget
	end
end

function DrawWidgets()
	for i, w in pairs(all_widgets) do
		activestate = _GetState(w.instance)
		w.states[activestate](w.instance)
	end
end

-- real gui user functions
function init()
	GUIWidgets({ { name = "Button1",
	               hitbox = {10, 10, 100, 40},
	               states = { idle   = function (instance) AddQuad(instance, {0, 0, 256, 256}, {0, 0, 256, 256}); end,
				              active = function (instance) print("in render function for active state"); end
				            },
				   activestate = "idle"
				 }
               }
              )
end

function update(delta)
	if (all_widgets.Button1:IsClicked()) then
		print("Clicked!")
	end
end
