MoveMiniObstacle = {}
MoveMiniObstacle.__index = MoveMiniObstacle

MoveMiniObstacle.ABYSS = {
    RotationSpeed = 2,
    VelocityRangeX = Vec2:new(-2.0, 2.0),
    VelocityRangeY = Vec2:new(-2.0, 2.0)
}



function MoveMiniObstacle:Start()
    --local self = setmetatable({}, MoveMiniObstacle)
    self.randomX = self.ABYSS.VelocityRangeX.x + (self.ABYSS.VelocityRangeX.y - self.ABYSS.VelocityRangeX.x) * math.random()
    self.randomY = self.ABYSS.VelocityRangeY.x + (self.ABYSS.VelocityRangeY.y - self.ABYSS.VelocityRangeY.x) * math.random()

    self.velocity = Vec2:new(self.randomX, self.randomY)

    self.destroyTimer = 200
    return self
end

function MoveMiniObstacle:Update()
    if (self.destroyTimer <= 0) then
        EManager:Destroy(self.entity)
    end

    self.miniVelocity = Vec2:new(math.cos(math.rad(self.entity:getTransform().angle)), math.sin(math.rad(self.entity:getTransform().angle)))  -- Velocity based on angle
    self.miniVelocity = self.miniVelocity * 2  -- Scale the velocity as needed, adjust the multiplier

    self.entity:getTransform().pos = self.entity:getTransform().pos + self.velocity
    self.entity:getTransform().angle = self.entity:getTransform().angle + self.ABYSS.RotationSpeed

    self.destroyTimer = self.destroyTimer - 1
    return self
end

function MoveMiniObstacle:OnCollisionEnter(collidedEntity)
    return self
end

function MoveMiniObstacle:OnCollisionExit(collidedEntity)
    return self
end
