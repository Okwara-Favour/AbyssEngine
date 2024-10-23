PlayerShoot = {}
PlayerShoot.__index = PlayerShoot

ShootRate = 10

function PlayerShoot:Start()
    local self = setmetatable({}, PlayerShoot)
    self.bullet = EManager:Copy(EManager:getEntityName("Bullet"))
    self.shootCountDown = ShootRate
    self.canShoot = true
    return self
end

function PlayerShoot:Update()
    if Listener:IsKeyPressed("Numpad0") and self.canShoot then
        self.canShoot = false
        if(self.bullet) then
            playerPos = self.entity:getTransform().pos;
            self.bullet:getTransform().pos = playerPos;
            bullet = EManager:Instantiate(self.bullet)
        end
    end

    if self.shootCountDown < 0 then
        self.canShoot = true
        self.shootCountDown = ShootRate
    end

    self.shootCountDown = self.shootCountDown - 1
    return self
end

function PlayerShoot:OnCollisionEnter(collidedEntity)
    
end

function PlayerShoot:OnCollisionExit(collidedEntity)
    
end