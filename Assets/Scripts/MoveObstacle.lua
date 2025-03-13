MoveObstacle = {}
MoveObstacle.__index = MoveObstacle

MoveObstacle.ABYSS = {
    RotationSpeed = 10,
    VelocityRangeX = Vec2:new(-2.0, -6.0)
}



function MoveObstacle:Start()
    --local self = setmetatable({}, MoveObstacle)
    self.camera = EManager:getEntityName("Camera")

    self.randomX = self.ABYSS.VelocityRangeX.x + (self.ABYSS.VelocityRangeX.y - self.ABYSS.VelocityRangeX.x) * math.random()
    
    self.player = EManager:getEntityName("Player")
    
    self.randomValue = math.random(1, 6)

    if self.randomValue == 3 then
        self:SetVelocityTowardsPlayer()
    else
        self.velocity = Vec2:new(self.randomX, 0)
    end

    return self
end

function MoveObstacle:Update()
    self.cam = self.camera:getCamera().camera
    self.cameraPos = self.cam:getPosition()
    self.cameraSize = self.cam:getSize()


    self.topLeft = self.cameraPos - self.cameraSize / 2
    self.bottomRight = self.cameraPos + self.cameraSize / 2

    if (self.entity:getTransform().pos.x + self.entity:getSize().size.x / 2) < self.topLeft.x then
        EManager:Destroy(self.entity)
    end

    if (self.entity:getTransform().pos.y + self.entity:getSize().size.y / 2) < self.topLeft.y then
        EManager:Destroy(self.entity)
    end
    if (self.entity:getTransform().pos.y - self.entity:getSize().size.y / 2) > self.bottomRight.y then
        EManager:Destroy(self.entity)
    end

    self.entity:getTransform().pos = self.entity:getTransform().pos + self.velocity
    self.entity:getTransform().angle = self.entity:getTransform().angle + self.ABYSS.RotationSpeed
    return self
end

function MoveObstacle:OnCollisionEnter(collidedEntity)
    return self
end

function MoveObstacle:OnCollisionExit(collidedEntity)
    return self
end

function MoveObstacle:SetVelocityTowardsPlayer()
    -- Calculate direction toward player and set velocity if player exists
    if self.player then
        local playerPos = self.player:getTransform().pos
        local obstaclePos = self.entity:getTransform().pos

        -- Calculate and normalize direction vector
        local direction = playerPos - obstaclePos
        direction:normalize()
        -- Set velocity with random speed
        local randomSpeed = self.ABYSS.VelocityRangeX.x + (self.ABYSS.VelocityRangeX.y - self.ABYSS.VelocityRangeX.x) * math.random()
        self.velocity = direction * math.abs(randomSpeed)
    end
end