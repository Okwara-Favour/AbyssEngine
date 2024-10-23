local At = require("Ats")
local message = At.getMessage()

Example = {}
Example.__index = Example

Entity = nil;
Count = 0
Position = Vec2:new(1.0, 1.0)

function Example:Start()
    local self = setmetatable({}, Example)
    --entity:getTransform().pos:totring()
    self.trea = "I am lua"
    --self.vec = Vec2:new(1.0, 1.0)
    --print(message)
    --print("Current Position:", self.entity:getTransform().pos:toString())
    --ConsoleText("Som")
    Entity = self.entity;
    At.setEntity(Entity)
    return self
end

function Example:Update()
    Count = Count + 1
    --print(count)

    --At.updateEntityPosition()

    if Listener:IsKeyPressed("Left") then
        self.entity:getTransform().pos.x = self.entity:getTransform().pos.x - Position.x
    end
    if Listener:IsKeyPressed("Right") then
        self.entity:getTransform().pos.x = self.entity:getTransform().pos.x + Position.x
    end
    if Listener:IsKeyPressed("Up") then
        self.entity:getTransform().pos.y = self.entity:getTransform().pos.y - Position.y
    end
    if Listener:IsKeyPressed("Down") then
        self.entity:getTransform().pos.y = self.entity:getTransform().pos.y + Position.y
    end
    --Entity:getTransform().pos.x = Entity:getTransform().pos.x + Position.x

    --print(Position.toString())
    return self
end

function Example:OnCollisionEnter(collidedEntity)
    print("enter", collidedEntity:getTransform().pos:toString())
end

function Example:OnCollisionExit(collidedEntity)
    print("exit", collidedEntity:getTransform().pos:toString())
end