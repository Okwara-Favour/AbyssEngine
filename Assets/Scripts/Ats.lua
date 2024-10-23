--require("Bts")

local entityRef = nil  -- Local variable to store the entity reference

-- Function to get a message
local function getMessage()
    return "ATS here"
end

-- Function to assign the entity to the local variable
local function setEntity(entity)
    entityRef = entity
end

-- Function to update the y position of the entity
local function updateEntityPosition()
    if entityRef then
        -- Update the y position of the entity (add some logic, e.g., moving it up by 1 unit)
        local transform = entityRef:getTransform()
        transform.pos.y = transform.pos.y + 1
        --print("Updated y position:", transform.pos.y)
    else
        --print("Entity not assigned!")
    end
end

-- Return the functions so they can be used in other scripts
return {
    getMessage = getMessage,
    setEntity = setEntity,
    updateEntityPosition = updateEntityPosition
}