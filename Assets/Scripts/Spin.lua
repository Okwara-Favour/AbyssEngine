Spin = {}
Spin.__index = Spin


RotationSpeed = 20

function Spin:Start()
    local self = setmetatable({}, Spin)
    return self
end

function Spin:Update()
    self.entity:getTransform().angle = self.entity:getTransform().angle + RotationSpeed
    return self
end

function Spin:OnCollisionEnter(collidedEntity)
    if collidedEntity:tag() == "Obstacle" then
        EManager:Destroy(collidedEntity)
        EManager:Destroy(self.entity)
    end
    
    --print("enter", collidedEntity:getTransform().pos:toString())
end

function Spin:OnCollisionExit(collidedEntity)
    if collidedEntity:tag() == "Obstacle" then
        --print("Collision End")
    end
    --print("exit", collidedEntity:getTransform().pos:toString())
end