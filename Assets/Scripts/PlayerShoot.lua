PlayerShoot = {}
PlayerShoot.__index = PlayerShoot

--ShootRate = 100
PlayerShoot.ABYSS = {}

function PlayerShoot:Start()
    local self = setmetatable({}, PlayerShoot)
    self.bullet = GetPrefabByName("BulletA")
    --self.shootCountDown = ShootRate
    self.canShoot = true
    return self
end

function PlayerShoot:Update()
    if Listener:IsKeyPressed("M") and self.canShoot then
        self.canShoot = false
        if(self.bullet) then
            playerPos = self.entity:getTransform().pos;
            bullet = EManager:Instantiate(self.bullet)
            bullet:getTransform().pos = playerPos;
        end
    end

    if Listener:IsKeyReleased("M") and not self.canShoot then
        self.canShoot = true
    end
    return self
end

function PlayerShoot:OnCollisionEnter(collidedEntity)
    return self
end

function PlayerShoot:OnCollisionExit(collidedEntity)
    return self
end