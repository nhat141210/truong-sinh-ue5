[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$ToolName,

    [string]$ToolsetName = "",

    [string]$ArgumentsJson = "{}",

    [string]$ServerUrl = "http://127.0.0.1:8000/mcp"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$headers = @{
    Accept = "application/json, text/event-stream"
    "Content-Type" = "application/json"
}

$initializeRequest = @{
    jsonrpc = "2.0"
    id = 1
    method = "initialize"
    params = @{
        protocolVersion = "2025-11-25"
        capabilities = @{}
        clientInfo = @{
            name = "truong-sinh-windows-agent"
            version = "1.0"
        }
    }
} | ConvertTo-Json -Depth 20

$initializeResponse = Invoke-WebRequest -Uri $ServerUrl -Method Post -Headers $headers -Body $initializeRequest
$sessionId = [string]$initializeResponse.Headers["Mcp-Session-Id"]
if ([string]::IsNullOrWhiteSpace($sessionId)) {
    throw "Unreal MCP did not return an Mcp-Session-Id header."
}

$headers["Mcp-Session-Id"] = $sessionId

try {
    $initializedNotification = @{
        jsonrpc = "2.0"
        method = "notifications/initialized"
    } | ConvertTo-Json -Compress
    Invoke-WebRequest -Uri $ServerUrl -Method Post -Headers $headers -Body $initializedNotification | Out-Null

    $arguments = $ArgumentsJson | ConvertFrom-Json -AsHashtable
    if ([string]::IsNullOrWhiteSpace($ToolsetName)) {
        $mcpToolName = $ToolName
        $mcpArguments = $arguments
    }
    else {
        $mcpToolName = "call_tool"
        $mcpArguments = @{
            toolset_name = $ToolsetName
            tool_name = $ToolName
            arguments = $arguments
        }
    }

    $toolRequest = @{
        jsonrpc = "2.0"
        id = 2
        method = "tools/call"
        params = @{
            name = $mcpToolName
            arguments = $mcpArguments
        }
    } | ConvertTo-Json -Depth 100

    $response = Invoke-RestMethod -Uri $ServerUrl -Method Post -Headers $headers -Body $toolRequest
    $response | ConvertTo-Json -Depth 100
}
finally {
    try {
        Invoke-WebRequest -Uri $ServerUrl -Method Delete -Headers $headers | Out-Null
    }
    catch {
        Write-Verbose "Unreal MCP session cleanup failed: $($_.Exception.Message)"
    }
}
