local At = require("Ats")
local message = At.getMessage()

Example = {}
Example.__index = Example

function Example:Start(entity)
    local self = setmetatable({}, Example)
    --entity:getTransform().pos:totring()
    self.ent = entity
    self.count = 0
    self.trea = "I am lua"
    self.vec = Vec2:new(1.0, 1.0)
    --print(message)
    --print("Current Position:", entity:getTransform().pos:toString())

    return self
end

function Example:Update(entity)
    self.count = self.count + 1
    --editor:HandleError((self.count + ""))
    --entity:getTransform().pos = entity:getTransform().pos + self.vec
    if Listener:IsKeyPressed("Left") then
        entity:getTransform().pos.x = entity:getTransform().pos.x - self.vec.x
    end
    if Listener:IsKeyPressed("Right") then
        entity:getTransform().pos.x = entity:getTransform().pos.x + self.vec.x
    end
    --self.ent:getTransform().pos = self.ent:getTransform().pos + self.vec
    --print("Updated Position:", entity:getTransform().pos:toString())
    return self
end