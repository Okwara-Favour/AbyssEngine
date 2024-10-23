PlayerMove = {}
PlayerMove.__index = PlayerMove

Velocity = Vec2:new(1.0, 1.0)
RotationSpeed = 1

function PlayerMove:Start()
    local self = setmetatable({}, PlayerMove)
    return self
end

function PlayerMove:Update()
    if Listener:IsKeyPressed("Left") then
        self.entity:getTransform().pos.x = self.entity:getTransform().pos.x - Velocity.x
    end
    if Listener:IsKeyPressed("Right") then
        self.entity:getTransform().pos.x = self.entity:getTransform().pos.x + Velocity.x
    end
    if Listener:IsKeyPressed("Up") then
        self.entity:getTransform().pos.y = self.entity:getTransform().pos.y - Velocity.y
    end
    if Listener:IsKeyPressed("Down") then
        self.entity:getTransform().pos.y = self.entity:getTransform().pos.y + Velocity.y
    end

    self.entity:getTransform().angle = self.entity:getTransform().angle + RotationSpeed
    return self
end

function PlayerMove:OnCollisionEnter(collidedEntity)
    
end

function PlayerMove:OnCollisionExit(collidedEntity)
    
end