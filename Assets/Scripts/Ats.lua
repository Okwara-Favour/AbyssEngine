require("Bts")

local function getMessage()
    return "ATS here"
end

-- Return the function so it can be called in other scripts
return {
    getMessage = getMessage
}