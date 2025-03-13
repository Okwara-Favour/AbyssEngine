local At = require("Ats")
local message = At.getMessage()

Example = {}
Example.__index = Example

Entity = nil;

Example.ABYSS = {
    Velocity = Vec2:new(1.0, 1.0)
}

function Example:Start()
    --local self = setmetatable({}, Example)
    --entity:getTransform().pos:totring()
    self.trea = "I am lua"
    --self.vec = Vec2:new(1.0, 1.0)
    --print(message)
    --print("Current Velocity:", self.entity:getTransform().pos:toString())
    --ConsoleText("Som")
    Entity = self.entity;
    At.setEntity(Entity)
    return self
end

function Example:Update()
    --self.ABYSS.Count = self.ABYSS.Count + 1
    --print(count)

    --At.updateEntityVelocity()
    
    if Listener:IsKeyPressed("Left") then
        self.entity:getTransform().pos.x = self.entity:getTransform().pos.x - self.ABYSS.Velocity.x
    end
    if Listener:IsKeyPressed("Right") then
        self.entity:getTransform().pos.x = self.entity:getTransform().pos.x + self.ABYSS.Velocity.x
    end
    if Listener:IsKeyPressed("Up") then
        self.entity:getTransform().pos.y = self.entity:getTransform().pos.y - self.ABYSS.Velocity.y
    end
    if Listener:IsKeyPressed("Down") then
        self.entity:getTransform().pos.y = self.entity:getTransform().pos.y + self.ABYSS.Velocity.y
    end
    
    
    --Entity:getTransform().pos.x = Entity:getTransform().pos.x + self.ABYSS.Velocity.x
    self.entity:getTransform().angle = self.entity:getTransform().angle - 5
    --print(Velocity.toString())
    return self
end

function Example:OnCollisionEnter(collidedEntity)
    print("enter", collidedEntity:getTransform().pos:toString())
    return self
end

function Example:OnCollisionExit(collidedEntity)
    print("exit", collidedEntity:getTransform().pos:toString())
    return self
end