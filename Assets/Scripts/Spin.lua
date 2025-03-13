Spin = {}
Spin.__index = Spin


Spin.ABYSS = {
    RotationSpeed = 20
}

function Spin:Start()
    local self = setmetatable({}, Spin)
    return self
end

function Spin:Update()
    self.entity:getTransform().angle = self.entity:getTransform().angle + self.ABYSS.RotationSpeed
    return self
end

function Spin:OnCollisionEnter(collidedEntity)
    if collidedEntity:tag() == "Obstacle" then
        EManager:Destroy(collidedEntity)
        EManager:Destroy(self.entity)
    end
    
    return self
end

function Spin:OnCollisionExit(collidedEntity)
    if collidedEntity:tag() == "Obstacle" then
        --print("Collision End")
    end
    return self
end