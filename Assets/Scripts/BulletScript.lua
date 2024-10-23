BulletScript = {}
BulletScript.__index = BulletScript


RotationSpeed = 2
Velocity = Vec2:new(2.0,0.0)

function BulletScript:Start()
    local self = setmetatable({}, BulletScript)
    self.destroyTimer = 300
    return self
end

function BulletScript:Update()
    self.entity:getTransform().pos = self.entity:getTransform().pos + Velocity
    --self.entity:getTransform().angle = self.entity:getTransform().angle + RotationSpeed

    if (self.destroyTimer <= 0) then
        EManager:Destroy(self.entity)
    end
    self.destroyTimer = self.destroyTimer - 1

    return self
end

function BulletScript:OnCollisionEnter(collidedEntity)
    if collidedEntity:tag() == "Obstacle" then
        if collidedEntity:hasCircleCollider() then
            print(collidedEntity:getCircleCollider().offset)
            print("Has circle")
        end
        if collidedEntity:hasBoxCollider() then
            print(collidedEntity:getBoxCollider().offset:toString())
            print("Has rectangle")
        end
        print("Collision Start", self.entity:id())
    end
    
    --print("enter", collidedEntity:getTransform().pos:toString())
end

function BulletScript:OnCollisionExit(collidedEntity)
    if collidedEntity:tag() == "Obstacle" then
        print("Collision End")
    end
    --print("exit", collidedEntity:getTransform().pos:toString())
end