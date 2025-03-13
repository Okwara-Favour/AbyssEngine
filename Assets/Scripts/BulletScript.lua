BulletScript = {}
BulletScript.__index = BulletScript


BulletScript.ABYSS = {
    RotationSpeed = 12,
    Velocity = Vec2:new(5.0,0.0)
}


function BulletScript:Start()
    local self = setmetatable({}, BulletScript)
    self.destroyTimer = 300
    self.miniObstacle = GetPrefabByName("MiniObstacle")
    return self
end

function BulletScript:Update()
    self.entity:getTransform().pos = self.entity:getTransform().pos + self.ABYSS.Velocity
    self.entity:getTransform().angle = self.entity:getTransform().angle + self.ABYSS.RotationSpeed

    if (self.destroyTimer <= 0) then
        EManager:Destroy(self.entity)
    end
    self.destroyTimer = self.destroyTimer - 1

    return self
end

function BulletScript:OnCollisionEnter(collidedEntity)
    if collidedEntity:tag() == "Obstacle" then
        if string.find(collidedEntity:getName().name, "^Obstacle") then
            SpawnMiniObstacles(collidedEntity, self.miniObstacle) 
        end
        EManager:Destroy(collidedEntity)
        EManager:Destroy(self.entity)
    end
    --print("enter", collidedEntity:getTransform().pos:toString())
    return self
end

function BulletScript:OnCollisionExit(collidedEntity)
    if collidedEntity:tag() == "Obstacle" then
        --print("Collision End")
    end
    --print("exit", collidedEntity:getTransform().pos:toString())
    return self
end

function SpawnMiniObstacles(entity, miniObstacle)
    local miniObstacleCount = entity:getCircleRender().pointCount  -- Use pointCount for number of mini obstacles
    local basePos = entity:getTransform().pos  -- Position of the original obstacle
    local baseAngle = entity:getTransform().angle  -- Angle of the original obstacle

    local fillColor = entity:getCircleRender().fillColor  -- Fill color of the original obstacle
    local outlineColor = entity:getCircleRender().outlineColor  -- Outline color of the original obstacle
    local outlineThickness = entity:getCircleRender().outlineThickness  -- Outline thickness of the original obstacle

    for i = 1, miniObstacleCount do
        -- Calculate the angle for each mini obstacle based on the index
        local angleOffset = (360 / miniObstacleCount) * (i - 1)  -- Spread mini obstacles evenly
        local miniObstacleAngle = baseAngle + angleOffset

        local mini = EManager:Instantiate(miniObstacle)  -- Ensure you have a method to add the entity to the game
        -- Create a new mini obstacle
          -- Ensure this prefab is defined
        mini:getTransform().pos = basePos  -- Set the position
        mini:getTransform().angle = miniObstacleAngle  -- Set the angle

        -- Set the rendering properties
        mini:getCircleRender().fillColor = fillColor  -- Set fill color
        mini:getCircleRender().outlineColor = outlineColor  -- Set outline color
        mini:getCircleRender().outlineThickness = outlineThickness  -- Set outline thickness
        -- Add the mini obstacle to the entity manager or the game world
        
    end
end