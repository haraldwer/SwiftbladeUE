
-- https://heroiclabs.com/docs/nakama/server-framework/lua-runtime/function-reference/#Leaderboards

local nk = require("nakama")
nk.logger_info("Lua module loaded.")

local function CreateLeaderboard_Lua(context, payload)

	nk.logger_info("Creating leaderboard")
	local json = nk.json_decode(payload)
	local id = json.LeaderboardID
	local authoritative = false
	local sort = "desc"
	local operator = "best"
	local reset = "0 0 * * 1"
	local metadata = { data = "" }
	nk.leaderboard_create(id, authoritative, sort, operator, reset, metadata)

	return nk.json_encode({ 
		["payload"] = payload,
		["success"] = true
	})
end

nk.register_rpc(CreateLeaderboard_Lua, "CreateLeaderboard_Lua")