PlayerMove = {}
PlayerMove.__index = PlayerMove

PlayerMove.ABYSS = {
    Velocity = Vec2:new(3.0, 3.0),
    RotationSpeed = 1,
    CountCollision = 0
}


function PlayerMove:Start()
    local self = setmetatable({}, PlayerMove)
    self.camera = EManager:getEntityName("Camera")
    self.ABYSS.CountCollision = 4
    self.countat = 0
    return self
end

function PlayerMove:Update()
    self.cam = self.camera:getCamera().camera
    self.cameraPos = self.cam:getPosition()
    self.cameraSize = self.cam:getSize()


    self.topLeft = self.cameraPos - self.cameraSize / 2
    self.bottomRight = self.cameraPos + self.cameraSize / 2

    if Listener:IsKeyPressed("A") then
        self.entity:getTransform().pos.x = self.entity:getTransform().pos.x - self.ABYSS.Velocity.x
    end
    if Listener:IsKeyPressed("D") then
        self.entity:getTransform().pos.x = self.entity:getTransform().pos.x + self.ABYSS.Velocity.x
    end
    if Listener:IsKeyPressed("W") then
        self.entity:getTransform().pos.y = self.entity:getTransform().pos.y - self.ABYSS.Velocity.y
    end
    if Listener:IsKeyPressed("S") then
        self.entity:getTransform().pos.y = self.entity:getTransform().pos.y + self.ABYSS.Velocity.y
    end

    self.newPosition = self.entity:getTransform().pos
    self.playerSize = self.entity:getSize().size
    self.newPosition.x = math.max(self.topLeft.x + self.playerSize.x / 2, math.min(self.newPosition.x, self.bottomRight.x - self.playerSize.x / 2))
    self.newPosition.y = math.max(self.topLeft.y + self.playerSize.y / 2, math.min(self.newPosition.y, self.bottomRight.y - self.playerSize.y / 2))

    -- Apply the bounded position to the player
    self.entity:getTransform().pos = self.newPosition

    self.entity:getTransform().angle = self.entity:getTransform().angle + self.ABYSS.RotationSpeed
    return self
end

function PlayerMove:OnCollisionEnter(collidedEntity)
    self.ABYSS.CountCollision = self.ABYSS.CountCollision + 1
    --ConsoleText("abc " .. tostring(self.ABYSS.CountCollision))
    return self
end

function PlayerMove:OnCollisionExit(collidedEntity)
    return self
end