ObstacleSpawner = {}
ObstacleSpawner.__index = ObstacleSpawner

SpawnInterval = 30

function ObstacleSpawner:Start()
    local self = setmetatable({}, ObstacleSpawner)
    self.camera = EManager:getEntityName("Camera")
    self.obstacle = GetPrefabByName("Obstacle")
    self.countDown = SpawnInterval
    return self
end

function ObstacleSpawner:Update()
    self.cam = self.camera:getCamera().camera
    self.cameraPos = self.cam:getPosition()
    self.cameraSize = self.cam:getSize()


    self.topLeft = self.cameraPos - self.cameraSize / 2
    self.bottomRight = self.cameraPos + self.cameraSize / 2

    --print(topLeft:toString(), bottomRight:toString())
    self.objectX = self.entity:getTransform().pos.x
    self.obstacleSize = self.obstacle:getSize().size

    self.minY = self.topLeft.y + self.obstacleSize.y
    self.maxY = self.bottomRight.y - self.obstacleSize.y
    self.randomY = self.minY + (self.maxY - self.minY) * math.random()

    -- Set the obstacle's position
    self.obstaclePos = Vec2:new(self.objectX, self.randomY) -- Assuming Vec2 is a vector class
    -- Move the obstacle to the new position and instantiate it
    if self.countDown < 0 then
        self.Obs = EManager:Instantiate(self.obstacle)
        self.Obs:getTransform().pos = self.obstaclePos
        self.randomPointCount = math.random(3, 9)
        self.Obs:getCircleRender().pointCount = self.randomPointCount
        self.randomColorR = math.random(0, 255)
        self.randomColorG = math.random(0, 255)
        self.randomColorB = math.random(0, 255)
        self.Obs:getCircleRender().fillColor = Color:new(self.randomColorR, self.randomColorG, self.randomColorB, 255)
        self.Obs:getCircleRender().outlineColor = Color:new(self.randomColorG, self.randomColorB, self.randomColorR, 255)
        self.countDown = SpawnInterval
    end
    self.countDown = self.countDown - 1
    return self
end

function ObstacleSpawner:OnCollisionEnter(collidedEntity)
    
end

function ObstacleSpawner:OnCollisionExit(collidedEntity)
    
end