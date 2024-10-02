local At = require("Ats")

local message = At.getMessage()

-- Print the returned message
print(message)

editor:HandleError('test')
local entity = editor.entityManager:addEntity('enemy')
entity:addName('test1')
entity:addTransform(Vec2:new(), Vec2:new(1,1), 0)
entity:addSize(Vec2:new(3,3))
entity:addCircleShape()
local v = entity:getSize().size
v = v + 3;
print(v.x, v.y)