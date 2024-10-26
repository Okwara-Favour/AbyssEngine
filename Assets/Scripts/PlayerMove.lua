PlayerMove = {}
PlayerMove.__index = PlayerMove

Velocity = Vec2:new(3.0, 3.0)
RotationSpeed = 1

function PlayerMove:Start()
    local self = setmetatable({}, PlayerMove)
    self.camera = EManager:getEntityName("Camera")
    return self
end

function PlayerMove:Update()
    self.cam = self.camera:getCamera().camera
    self.cameraPos = self.cam:getPosition()
    self.cameraSize = self.cam:getSize()


    self.topLeft = self.cameraPos - self.cameraSize / 2
    self.bottomRight = self.cameraPos + self.cameraSize / 2

    if Listener:IsKeyPressed("A") then
        self.entity:getTransform().pos.x = self.entity:getTransform().pos.x - Velocity.x
    end
    if Listener:IsKeyPressed("D") then
        self.entity:getTransform().pos.x = self.entity:getTransform().pos.x + Velocity.x
    end
    if Listener:IsKeyPressed("W") then
        self.entity:getTransform().pos.y = self.entity:getTransform().pos.y - Velocity.y
    end
    if Listener:IsKeyPressed("S") then
        self.entity:getTransform().pos.y = self.entity:getTransform().pos.y + Velocity.y
    end

    self.newPosition = self.entity:getTransform().pos
    self.playerSize = self.entity:getSize().size
    self.newPosition.x = math.max(self.topLeft.x + self.playerSize.x / 2, math.min(self.newPosition.x, self.bottomRight.x - self.playerSize.x / 2))
    self.newPosition.y = math.max(self.topLeft.y + self.playerSize.y / 2, math.min(self.newPosition.y, self.bottomRight.y - self.playerSize.y / 2))

    -- Apply the bounded position to the player
    self.entity:getTransform().pos = self.newPosition

    self.entity:getTransform().angle = self.entity:getTransform().angle + RotationSpeed

    return self
end

function PlayerMove:OnCollisionEnter(collidedEntity)
    
end

function PlayerMove:OnCollisionExit(collidedEntity)
    
end