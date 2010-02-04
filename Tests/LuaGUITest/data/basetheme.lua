theme_texture = "data/guilook.png"

----------------------------------------------------
-- standard widgets

function NewSimpleButton(_name, _position, _size, _events)

	function render_button(instance, widget)
		size = widget.size
		
		if (widget:IsDown()) then
			AddQuad(instance, {0, 0, size.w, size.h}, {244, 0, 1, 255}); -- back
			
			AddQuad(instance, {-1, -1, 3, 3}, {0, 7, 3, 3}); -- top left corner
			AddQuad(instance, {size.w -2, -1, 3, 3}, {4, 7, 3, 3}); -- top right corner
			AddQuad(instance, {2, -1, size.w-4, 3}, {3, 7, 1, 3}); -- top border
			
			AddQuad(instance, {-1, size.h-2, 3, 3}, {0, 11, 3, 3}); -- bottom left corner
			AddQuad(instance, {size.w -2, size.h-2, 3, 3}, {4, 11, 3, 3}); -- bottom right corner
			AddQuad(instance, {2, size.h-2, size.w-4, 3}, {3, 11, 1, 3}); -- bottom border
			
			AddQuad(instance, {-1, 2, 3, size.h-4}, {0, 11, 3, 0}); -- left
			AddQuad(instance, {size.w -2, 2, 3, size.h-4}, {4, 11, 3, 0}); -- left
		else
			AddQuad(instance, {0, 0, size.w, size.h}, {254, 0, 1, 255}); -- back
			
			AddQuad(instance, {-1, -1, 3, 3}, {0, 0, 3, 3}); -- top left corner
			AddQuad(instance, {size.w -2, -1, 3, 3}, {4, 0, 3, 3}); -- top right corner
			AddQuad(instance, {2, -1, size.w-4, 3}, {3, 0, 1, 3}); -- top border
			
			AddQuad(instance, {-1, size.h-2, 3, 3}, {0, 4, 3, 3}); -- bottom left corner
			AddQuad(instance, {size.w -2, size.h-2, 3, 3}, {4, 4, 3, 3}); -- bottom right corner
			AddQuad(instance, {2, size.h-2, size.w-4, 3}, {3, 4, 1, 3}); -- bottom border
			
			AddQuad(instance, {-1, 2, 3, size.h-4}, {0, 4, 3, 0}); -- left
			AddQuad(instance, {size.w -2, 2, 3, size.h-4}, {4, 4, 3, 0}); -- left

		end

	end

	states = { "active", "inactive" }
	activestate = "active"
	widget = AddWidget(_name, {_position[1], _position[2], _size[1], _size[2]},
	                   states, activestate,
					   _events, render_button)
	
	return widget
end

function NewCheckbox(_name, _position, _events)

	function render_checkbox(instance, widget)
		size = widget.size
		
		if (widget:GetState() == "checked" or widget:IsDown()) then
			AddQuad(instance, {0, 0, size.w, size.h}, {244, 0, 1, 255}); -- back
			
			AddQuad(instance, {-1, -1, 3, 3}, {0, 7, 3, 3}); -- top left corner
			AddQuad(instance, {size.w -2, -1, 3, 3}, {4, 7, 3, 3}); -- top right corner
			AddQuad(instance, {2, -1, size.w-4, 3}, {3, 7, 1, 3}); -- top border
			
			AddQuad(instance, {-1, size.h-2, 3, 3}, {0, 11, 3, 3}); -- bottom left corner
			AddQuad(instance, {size.w -2, size.h-2, 3, 3}, {4, 11, 3, 3}); -- bottom right corner
			AddQuad(instance, {2, size.h-2, size.w-4, 3}, {3, 11, 1, 3}); -- bottom border
			
			AddQuad(instance, {-1, 2, 3, size.h-4}, {0, 11, 3, 0}); -- left
			AddQuad(instance, {size.w -2, 2, 3, size.h-4}, {4, 11, 3, 0}); -- left
			
			if (widget:GetState() == "checked" and not widget:IsDown()) then
				AddQuad(instance, {3, 3, 7, 7}, {0, 16, 7, 7}); -- checker
			end
		else
			AddQuad(instance, {0, 0, size.w, size.h}, {254, 0, 1, 255}); -- back
			
			AddQuad(instance, {-1, -1, 3, 3}, {0, 0, 3, 3}); -- top left corner
			AddQuad(instance, {size.w -2, -1, 3, 3}, {4, 0, 3, 3}); -- top right corner
			AddQuad(instance, {2, -1, size.w-4, 3}, {3, 0, 1, 3}); -- top border
			
			AddQuad(instance, {-1, size.h-2, 3, 3}, {0, 4, 3, 3}); -- bottom left corner
			AddQuad(instance, {size.w -2, size.h-2, 3, 3}, {4, 4, 3, 3}); -- bottom right corner
			AddQuad(instance, {2, size.h-2, size.w-4, 3}, {3, 4, 1, 3}); -- bottom border
			
			AddQuad(instance, {-1, 2, 3, size.h-4}, {0, 4, 3, 0}); -- left
			AddQuad(instance, {size.w -2, 2, 3, size.h-4}, {4, 4, 3, 0}); -- left

		end

	end

	states = { "unchecked", "checked" }
	activestate = "unchecked"
	_size = {12, 12}
	
	widget = AddWidget(_name, {_position[1], _position[2], _size[1], _size[2]},
	                   states, activestate,
					   _events, render_checkbox)
	
	old_onclick = widget.onClick
	function widget.onClick(self)
		
		if (old_onclick) then
			old_onclick(self)
		end
		
		if (self:GetState() == "checked") then
			self:SetState("unchecked")
		else
			self:SetState("checked")
		end
	end
	
	return widget
end

function NewScroller(_name, _position, _size, _events)
	
	function render_slider(instance, widget)
		size =  {w = widget.size.w, h = widget.slider_height}
		pos_y = widget.slider_position
		
		if (widget:IsDown()) then
			AddQuad(instance, {0, pos_y, size.w, size.h}, {244, 0, 1, 255}); -- back
			
			AddQuad(instance, {-1, pos_y-1, 3, 3}, {0, 7, 3, 3}); -- top left corner
			AddQuad(instance, {size.w -2, pos_y-1, 3, 3}, {4, 7, 3, 3}); -- top right corner
			AddQuad(instance, {2, pos_y-1, size.w-4, 3}, {3, 7, 1, 3}); -- top border
			
			AddQuad(instance, {-1, pos_y+size.h-2, 3, 3}, {0, 11, 3, 3}); -- bottom left corner
			AddQuad(instance, {size.w -2, pos_y+size.h-2, 3, 3}, {4, 11, 3, 3}); -- bottom right corner
			AddQuad(instance, {2, pos_y+size.h-2, size.w-4, 3}, {3, 11, 1, 3}); -- bottom border
			
			AddQuad(instance, {-1, pos_y+2, 3, size.h-4}, {0, 11, 3, 0}); -- left
			AddQuad(instance, {size.w -2, pos_y+2, 3, size.h-4}, {4, 11, 3, 0}); -- left
		else
			AddQuad(instance, {0, pos_y, size.w, size.h}, {254, 0, 1, 255}); -- back
			
			AddQuad(instance, {-1, pos_y-1, 3, 3}, {0, 0, 3, 3}); -- top left corner
			AddQuad(instance, {size.w -2, pos_y-1, 3, 3}, {4, 0, 3, 3}); -- top right corner
			AddQuad(instance, {2, pos_y-1, size.w-4, 3}, {3, 0, 1, 3}); -- top border
			
			AddQuad(instance, {-1, pos_y+size.h-2, 3, 3}, {0, 4, 3, 3}); -- bottom left corner
			AddQuad(instance, {size.w -2, pos_y+size.h-2, 3, 3}, {4, 4, 3, 3}); -- bottom right corner
			AddQuad(instance, {2, pos_y+size.h-2, size.w-4, 3}, {3, 4, 1, 3}); -- bottom border
			
			AddQuad(instance, {-1, pos_y+2, 3, size.h-4}, {0, 4, 3, 0}); -- left
			AddQuad(instance, {size.w -2, pos_y+2, 3, size.h-4}, {4, 4, 3, 0}); -- left

		end

	end

	states = { "active", "inactive" }
	activestate = "active"
	widget = AddWidget(_name, {_position[1], _position[2], _size[1], _size[2]},
	                   states, activestate,
					   _events, render_slider)
	-- calculate slider size -> height = max(size[2] * 0.2, 20)
	widget.slider_height = math.max(_size[2] * 0.2, 20)
	
	-- save scroll position and default data
	widget.slider_position = 0
	widget.last_mouse_hit = nil
	widget.value = 0
	
	-- modify onUpdate
	old_onupdate = widget.onUpdate
	function widget.onUpdate(self)
		
		if (old_onupdate) then
			old_onupdate(self)
		end
		
		if (self:IsDraging()) then
			-- calc delta
			new_pos = self:GetMouseHit()
			if (self.last_mouse_hit) then
				-- continue to drag
				d = self.last_mouse_hit[2] - new_pos[2]
				self.slider_position = self.slider_position - d
			else
				-- start drag
				-- check if outside slider-button
				if (new_pos[2] < self.slider_position or new_pos[2] > self.slider_position + self.slider_height) then
					self.slider_position = new_pos[2] - self.slider_height / 2.0
				end
			end
			
			-- make sure we are inside our slider area
			if (self.slider_position < 0) then
				self.slider_position = 0
			elseif (self.slider_position + self.slider_height > self.size.h) then
				self.slider_position = self.size.h - self.slider_height
			end
			
			-- update value
			self.value = self.slider_position / (self.size.h - self.slider_height)
			print("value: " .. tostring(self.value))
			
			self.last_mouse_hit = self:GetMouseHit()
		else
			self.last_mouse_hit = nil
		end
	end
	
	return widget
end

