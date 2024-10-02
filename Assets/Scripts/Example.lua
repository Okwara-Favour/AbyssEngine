local At = require("Ats")
local message = At.getMessage()

Example = {}
Example.__index = Example

function Example:Start(entity)
    local self = setmetatable({}, Example)
    --entity:getTransform().pos:totring()
    self.ent = entity
    self.vec = Vec2:new(1.0, 1.0)
    print(message)
    print("Current Position:", entity:getTransform().pos:toString())

    return self
end

function Example:Update(entity)
    self.ent:getTransform().pos = self.ent:getTransform().pos + self.vec
    --print("Updated Position:", entity:getTransform().pos:toString())
end