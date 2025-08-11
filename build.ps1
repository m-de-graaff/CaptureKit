#!/usr/bin/env pwsh

param(
    [Parameter()]
    [ValidateSet("debug", "release", "default")]
    [string]$BuildType = "default",
    
    [Parameter()]
    [switch]$Clean,
    
    [Parameter()]
    [switch]$Test,
    
    [Parameter()]
    [switch]$Install
)

# Set error action preference
$ErrorActionPreference = "Stop"

# Colors for output
$Colors = @{
    Info = "Cyan"
    Success = "Green"
    Warning = "Yellow"
    Error = "Red"
}

function Write-ColorOutput {
    param(
        [string]$Message,
        [string]$Color = "White"
    )
    Write-Host $Message -ForegroundColor $Colors[$Color]
}

function Test-Command {
    param([string]$Command)
    try {
        Get-Command $Command -ErrorAction Stop | Out-Null
        return $true
    }
    catch {
        return $false
    }
}

# Check prerequisites
Write-ColorOutput "Checking prerequisites..." "Info"

$RequiredTools = @("cmake", "ninja")
$MissingTools = @()

foreach ($tool in $RequiredTools) {
    if (-not (Test-Command $tool)) {
        $MissingTools += $tool
    }
}

if ($MissingTools.Count -gt 0) {
    Write-ColorOutput "Missing required tools: $($MissingTools -join ', ')" "Error"
    Write-ColorOutput "Please install missing tools and try again." "Error"
    exit 1
}

Write-ColorOutput "All prerequisites found!" "Success"

# Set build directory
$BuildDir = "build\$BuildType"
$SourceDir = Get-Location

# Clean build if requested
if ($Clean -and (Test-Path $BuildDir)) {
    Write-ColorOutput "Cleaning build directory: $BuildDir" "Info"
    Remove-Item -Recurse -Force $BuildDir
}

# Create build directory
if (-not (Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Path $BuildDir -Force | Out-Null
}

# Configure
Write-ColorOutput "Configuring with preset: $BuildType" "Info"
Push-Location $BuildDir
try {
    $ConfigureResult = cmake --preset $BuildType $SourceDir
    if ($LASTEXITCODE -ne 0) {
        Write-ColorOutput "Configuration failed!" "Error"
        exit 1
    }
    Write-ColorOutput "Configuration successful!" "Success"
}
finally {
    Pop-Location
}

# Build
Write-ColorOutput "Building project..." "Info"
Push-Location $BuildDir
try {
    $BuildResult = cmake --build --preset $BuildType
    if ($LASTEXITCODE -ne 0) {
        Write-ColorOutput "Build failed!" "Error"
        exit 1
    }
    Write-ColorOutput "Build successful!" "Success"
}
finally {
    Pop-Location
}

# Run tests if requested
if ($Test) {
    Write-ColorOutput "Running tests..." "Info"
    Push-Location $BuildDir
    try {
        $TestResult = ctest --preset $BuildType
        if ($LASTEXITCODE -ne 0) {
            Write-ColorOutput "Tests failed!" "Error"
            exit 1
        }
        Write-ColorOutput "All tests passed!" "Success"
    }
    finally {
        Pop-Location
    }
}

# Install if requested
if ($Install) {
    Write-ColorOutput "Installing..." "Info"
    Push-Location $BuildDir
    try {
        $InstallResult = cmake --install .
        if ($LASTEXITCODE -ne 0) {
            Write-ColorOutput "Installation failed!" "Error"
            exit 1
        }
        Write-ColorOutput "Installation successful!" "Success"
    }
    finally {
        Pop-Location
    }
}

Write-ColorOutput "Build completed successfully!" "Success"
Write-ColorOutput "Build directory: $BuildDir" "Info"
